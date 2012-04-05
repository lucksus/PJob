#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QtNetwork/QTcpServer>

class SessionThread;
class TcpServer
{
public:
    static TcpServer& instance();
    void startup();
    void clean_threads();

private:
    TcpServer();
    QTcpServer m_server;
    QList<SessionThread*> m_threads;
};

#endif // TCPSERVER_H
