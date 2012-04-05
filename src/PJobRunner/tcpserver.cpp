#include "tcpserver.h"
#include <QHostAddress>
#include "sessionthread.h"

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

void TcpServer::startup(){
    m_server.listen(QHostAddress::Any, 23023);
    while(true){
        m_server.waitForNewConnection(1000);
        clean_threads();
        if(m_server.hasPendingConnections() == false) continue;
        QTcpSocket* connection = m_server.nextPendingConnection();
        std::cout << "Accepted connection from " << connection->peerAddress().toString().toStdString() << std::endl;
        m_threads.append(new SessionThread(connection,&m_server));
        m_threads.last()->start();
    }
}
