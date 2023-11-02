/*
muduo网络库给用户提供了两个主要的类
TcpServer:用于编写服务器程序的
TcpClient:用于编写客户端程序的
epoll + 线程池
好处：能够把网络IO代码和业务代码进行区分
                    用户的连接和断开     用户的可读写事件
//用了网络库的目的就是为了我们在写代码时不需要关心网络模块的代码
*/
#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop时间循环对象的指针
3.名却Tcpserver需要哪些参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数中，注册处理连接的回调函数和处理读写时间的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配IO线程和worker线程
*/
class ChatServer
{
public:
    ChatServer(EventLoop *loop,               // 事件循环
               const InetAddress &listenAddr, // IP地址
               const string &nameArg)         // 服务器的名字
        : m_server(loop, listenAddr, nameArg), m_loop(loop)
    {
        // 给服务器注册用户连接的创建和回调
        m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1)); //_1参数占位符
        // 给服务器创建读写事件回调
        m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量 1个IO线程 3个worker线程
        m_server.setThreadNum(4);
    }

    // 开启事件循环
    void start()
    {
        m_server.start();
    }

private:
    // 专门处理用户连接创建和断开  epoll
    void onConnection(const TcpConnectionPtr &conn)
    {
        // peerAddress()获取对端地址
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown(); //close(fd);
            // m_loop->quit();
        }
    }

    // 专门处理用户读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,                  // 缓冲区
                   Timestamp time)               // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv date:" << buf << " time:" << time.toString() << endl;
        conn->send(buf);
    }
    TcpServer m_server; // #1
    EventLoop *m_loop;  // epoll
};

int main()
{
    EventLoop loop; //epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop(); //epoll_wait以阻塞的方式等待新用户连接，已连接用户的读写事件等

    return 0;
}