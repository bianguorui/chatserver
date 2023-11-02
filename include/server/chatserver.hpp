#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);
    // 启动
    void start();
private:
    // 上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr&);
    // 上报读写事件的回调函数
    void onMessage(const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp);

    TcpServer m_server; // 组合的muduo库，实现服务器的类对象
    EventLoop *m_loop;  // 指向事件循环的指针
};
#endif