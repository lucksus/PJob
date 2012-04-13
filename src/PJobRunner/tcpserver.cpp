#include "tcpserver.h"
#include <QHostAddress>
#include "sessionthread.h"
#include <QtServiceBase>

TcpServer::TcpServer()
{
}

TcpServer& TcpServer::instance(){
    static TcpServer tcp_server;
    return tcp_server;
}

void TcpServer::clean_threads(){
    QList<int> to_delete;
    for(int i=0;i<m_threads.size();i++)
        if(m_threads[i]->isFinished()) to_delete.append(i);
    foreach(int index,to_delete)
        delete m_threads.takeAt(index);
}

void TcpServer::run(){
    m_server.listen(QHostAddress::Any, 23023);
    while(m_active){
        m_server.waitForNewConnection(1000);
        clean_threads();
        if(m_server.hasPendingConnections() == false) continue;
        QTcpSocket* connection = m_server.nextPendingConnection();
        QtServiceBase::instance()->logMessage(QString("Accepted connection from %1.").arg(connection->peerAddress().toString()));
        m_threads.append(new SessionThread(connection,&m_server));
        m_threads.last()->start();
    }
}


void TcpServer::set_active(bool active){
    m_active = active;
}
