#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis服务器
    bool connect();

    // 向redis指定的channel通道发布消息
    bool publish(int channel, string message);

    // 向redis指定的channel通道订阅消息
    bool subscribe(int channel);

    // 向redis指定的channel通道取消订阅消息
    bool unsubscribe(int channel);

    // 在独立线程中，接受订阅通道中的消息
    void observer_channel_message();

    // 初始化向业务层上报通道消息的回调对象
    void init_notify_handler(function<void(int, string)> fn);

private:
    redisContext *m_publish_context;

    redisContext *m_subscribe_context;

    function<void(int, string)> m_notify_message_handler;
};

#endif
