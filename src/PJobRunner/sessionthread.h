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
    SessionThread(QTcpSocket* connection, QObject* parent);

protected:
    virtual void run();

private:
    QTcpSocket* m_connection;
    QHostAddress m_peer_adress;
};

#endif // SESSIONTHREAD_H
