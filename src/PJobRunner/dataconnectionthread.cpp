#include "dataconnectionthread.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

DataConnectionThread::DataConnectionThread(QByteArray& data_destination, QObject *parent) :
    QThread(parent), m_data_destination(data_destination), m_server(0), m_data_received(false), m_want_exit(false)
{
}

DataConnectionThread::~DataConnectionThread(){
    m_want_exit = true;
    wait();
    if(m_server) delete m_server;
}

quint32 DataConnectionThread::open_data_port(){
    quint32 port = 23023 + 1;
    m_server = new QTcpServer;
    while(!m_server->listen(QHostAddress::Any, port) && port < 32000) port++;
    if(!m_server->isListening()) throw QString("No free port found!");
    return port;
}

void DataConnectionThread::run(){
    if(!m_server->isListening()) return;
    QTcpSocket* connection = 0;

    while(!m_want_exit){
        if(!connection){
            m_server->waitForNewConnection(10);
            if(m_server->hasPendingConnections() == false) continue;
            connection = m_server->nextPendingConnection();
            connection->setReadBufferSize(0);
        }
        if(connection->state() == QTcpSocket::UnconnectedState){
            while(connection->bytesAvailable())
                m_data_destination.append(connection->readAll());
            m_data_received = true;
            break;
        }
        if(connection->waitForReadyRead(10)){
            m_data_destination.append(connection->readAll());
        }
    }
}

bool DataConnectionThread::data_received(){
    return m_data_received;
}
