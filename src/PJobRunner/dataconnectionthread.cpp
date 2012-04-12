#include "dataconnectionthread.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

quint32 DataConnection::s_port = 23023;

DataConnection::DataConnection(QByteArray& data, QObject *parent) :
    QThread(parent), m_data(data), m_server(0), m_want_exit(false)
{
}

DataReceiveConnection::DataReceiveConnection(QByteArray &data, QObject *parent):
    DataConnection(data, parent), m_data_received(false)
{
}

DataPushConnection::DataPushConnection(QByteArray &data, QObject *parent):
    DataConnection(data, parent)
{
}

DataConnection::~DataConnection(){
    m_want_exit = true;
    wait();
    if(m_server) delete m_server;
}

quint32 DataConnection::open_data_port(){
    quint32 port = s_port + 1;
    m_server = new QTcpServer;
    while(!m_server->listen(QHostAddress::Any, port) && port < 32000) port++;
    if(!m_server->isListening()) throw QString("No free port found!");
    s_port = port;
    return port;
}

void DataReceiveConnection::run(){
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
                m_data.append(connection->readAll());
            m_data_received = true;
            break;
        }
        if(connection->waitForReadyRead(10)){
            m_data.append(connection->readAll());
        }
    }
}

bool DataReceiveConnection::data_received(){
    return m_data_received;
}

void DataPushConnection::run(){
    if(!m_server->isListening()) return;
    QTcpSocket* connection = 0;

    while(!m_want_exit){
        if(!connection){
            m_server->waitForNewConnection(10);
            if(m_server->hasPendingConnections() == false) continue;
            connection = m_server->nextPendingConnection();
            connection->setReadBufferSize(0);
        }

        qint64 bytes_send = 0;
        qint64 all_bytes = m_data.size();
        char* data = m_data.data();
        qint64 transfer_unit_size = 1024;

        while(bytes_send != all_bytes){
            qint64 bytes_written = connection->write(data, std::min(transfer_unit_size,all_bytes-bytes_send));
            data += bytes_written;
            bytes_send += bytes_written;
            connection->flush();
            connection->waitForBytesWritten();
        }

        connection->close();
        m_want_exit = true;
    }
}
