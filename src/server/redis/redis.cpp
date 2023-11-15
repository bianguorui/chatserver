#include "redis.hpp"
#include <iostream>
using namespace std;

Redis::Redis()
    : m_publish_context(nullptr), m_subscribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (m_publish_context != nullptr)
    {
        redisFree(m_publish_context);
    }

    if (m_subscribe_context != nullptr)
    {
        redisFree(m_subscribe_context);
    }
}

bool Redis::connect()
{
    // 负责publish发布消息的上下文连接
    m_publish_context = redisConnect("127.0.0.1", 6379);

    if (m_publish_context == nullptr)
    {
        cerr << "connect redis failed! --publish" << endl;
        return false;
    }

    // 负责subscribe订阅消息的上下文连接
    m_subscribe_context = redisConnect("127.0.0.1", 6379);

    if (m_subscribe_context == nullptr)
    {
        cerr << "connect redis failed! --subscribe" << endl;
        return false;
    }

    // 在单独的线程中，监听通道中的事件，有消息给业务层进行上报
    thread t([&]()
             { observer_channel_message(); });
    t.detach();

    cout << "connect redis-server success!" << endl;

    return true;
}

// 向redis指定的channel通道发布消息
bool Redis::publish(int channel, string message)
{
    redisReply *reply = (redisReply *)redisCommand(m_publish_context, "PUBLISH %d %s", channel);
    if (reply == nullptr)
    {
        cerr << "publish command failed!" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的channel通道订阅消息
bool Redis::subsribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->m_subscribe_context, "SUBSRIBE %d", channel))
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->m_subscribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }

    return true;
}

// 向redis指定的channel通道取消订阅消息
bool Redis::unsubsribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->m_subscribe_context, "UNSUBSRIBE %d", channel))
    {
        cerr << "unsubsribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->m_subscribe_context, &done))
        {
            cerr << "unsubsribe command failed!" << endl;
            return false;
        }
    }

    return false;
}

// 在独立线程中，接受订阅通道中的消息
void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->m_subscribe_context, (void**)&reply))
    {
        // 订阅收到的消息是一个带三元素的数组
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            // 给业务层上报通道上发生的消息
            m_notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }

        freeReplyObject(reply);
    }

    cerr << "------------observer_channel_message quit--------------" << endl;
}

// 初始化向业务层上报通道消息的回调对象
void Redis::init_notify_handler(function<void(int, string)> fn)
{
    this->m_notify_message_handler = fn;
}