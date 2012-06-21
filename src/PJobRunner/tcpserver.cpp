#include "tcpserver.h"
#include <QHostAddress>
#include "sessionthread.h"
#include <QtServiceBase>
#include <QtNetwork/QTcpServer>

TcpServer::TcpServer()
{
}

TcpServer& TcpServer::instance(){
    static TcpServer tcp_server;
    return tcp_server;
}

void TcpServer::incomingConnection(int socketDescriptor){
    SessionThread* session = new SessionThread(socketDescriptor);
    connect(session, SIGNAL(finished()), session, SLOT(deleteLater()));
    session->start();
}

void TcpServer::set_active(bool active){
    if(active) listen(QHostAddress::Any, 23023);
    else close();
}
