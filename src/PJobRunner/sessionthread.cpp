#include "sessionthread.h"
#include "session.h"
#include <QHostAddress>
#include <QtServiceBase>

SessionThread::SessionThread(QTcpSocket* connection, QObject *parent) :
    QThread(parent), m_connection(connection)
{
    m_peer_adress = m_connection->peerAddress();
}

void SessionThread::run(){
    Session* session = new Session(m_connection);
    while(m_connection->state() == QAbstractSocket::ConnectedState){
        while(m_connection->state() == QAbstractSocket::ConnectedState && !m_connection->waitForReadyRead(10)) QCoreApplication::processEvents();
        QByteArray line = m_connection->readLine(1024*1024*500);
        if(!line.isEmpty()) session->script_engine().evaluate(line);
        if(session->wants_shutdown()) m_connection->disconnectFromHost();
    }
    delete session;
    QtServiceBase::instance()->logMessage(QString("Connection to %1 closed.").arg(m_peer_adress.toString()));
    delete m_connection;
    m_connection = 0;
}
