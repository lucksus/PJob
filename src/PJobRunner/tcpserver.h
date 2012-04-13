#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QtNetwork/QTcpServer>
#include <QtCore/QThread>

class SessionThread;
class TcpServer : public QThread
{
public:
    static TcpServer& instance();
    void set_active(bool);

protected:
    virtual void run();

private:
    TcpServer();
    QTcpServer m_server;
    QList<SessionThread*> m_threads;

    bool m_active;

    void clean_threads();
};

#endif // TCPSERVER_H
