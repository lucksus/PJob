#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QtNetwork/QTcpServer>

class SessionThread;
class TcpServer : public QTcpServer
{
public:
    static TcpServer& instance();
    void set_active(bool);

protected:
    virtual void incomingConnection(int socketDescriptor);

private:
    TcpServer();
};

#endif // TCPSERVER_H
