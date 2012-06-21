#include "sessionthread.h"
#include "session.h"
#include <QHostAddress>
#include <QtServiceBase>

SessionThread::SessionThread(int socket_descriptor) :
    m_socket_descriptor(socket_descriptor)
{
}

void SessionThread::run(){
    QTcpSocket connection;
    if(!connection.setSocketDescriptor(m_socket_descriptor)){
        QtServiceBase::instance()->logMessage(QString("Could not set socket descriptor: %1").arg(connection.errorString()));
        return;
    }
    QtServiceBase::instance()->logMessage(QString("Accepted connection from %1. Starting new session..").arg(connection.peerAddress().toString()));
    Session* session = new Session(&connection);
    while(connection.state() == QAbstractSocket::ConnectedState){
        while(connection.state() == QAbstractSocket::ConnectedState && !connection.waitForReadyRead(10)) session->update();
        QByteArray line = connection.readAll();
        if(!line.isEmpty()) session->script_engine().evaluate(line);
        if(session->wants_shutdown())
            connection.disconnectFromHost();
    }
    delete session;
    QtServiceBase::instance()->logMessage(QString("Connection to %1 closed.").arg(connection.peerAddress().toString()));
}
