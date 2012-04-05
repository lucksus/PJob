#include "sessionthread.h"
#include "session.h"
#include <QHostAddress>

SessionThread::SessionThread(QTcpSocket* connection, QObject *parent) :
    QThread(parent), m_connection(connection)
{
}

void SessionThread::run(){
    Session* session = new Session(m_connection);
    while(m_connection->state() == QAbstractSocket::ConnectedState){
        while(!m_connection->waitForReadyRead()) sleep(1);
        QByteArray line = m_connection->readLine(1024*1024*500);
        if(!line.isEmpty()) session->script_engine().evaluate(line);
        if(session->wants_shutdown()) m_connection->disconnectFromHost();
    }
    delete session;
    std::cout << "Connection to " << m_connection->peerAddress().toString().toStdString() << std::endl;
    delete m_connection;
    m_connection = 0;
}
