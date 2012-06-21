#ifndef SESSIONTHREAD_H
#define SESSIONTHREAD_H
#include <QThread>
#include "session.h"
#include <QTcpSocket>
#include <QHostAddress>

class SessionThread : public QThread
{
    Q_OBJECT
public:
    SessionThread(int socket_descriptor);

protected:
    virtual void run();

private:
    int m_socket_descriptor;
};

#endif // SESSIONTHREAD_H
