#include "dataconnectionthread.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <assert.h>

DataReceiveThread::DataReceiveThread(unsigned int socket_descriptor):
    m_socket_descriptor(socket_descriptor), m_data(0)
{
}

QByteArray* DataReceiveThread::data() const{
    return m_data;
}

void DataReceiveThread::run(){
    QTcpSocket connection;
    connection.setSocketDescriptor(m_socket_descriptor);
    if(m_data) delete m_data;
    m_data = new QByteArray;
    while(connection.state() == QTcpSocket::ConnectedState){
        if(connection.waitForReadyRead(10)){
            m_data->append(connection.readAll());
        }
    }
    while(connection.bytesAvailable())
        m_data->append(connection.readAll());
}


DataPushThread::DataPushThread(unsigned int socket_descriptor, QByteArray* data):
    m_socket_descriptor(socket_descriptor), m_data(data)
{
}


void DataPushThread::run(){
    assert(m_data);
    assert(m_data->size() > 0);
    QTcpSocket connection;
    connection.setSocketDescriptor(m_socket_descriptor);

    qint64 bytes_send = 0;
    qint64 all_bytes = m_data->size();
    char* data = m_data->data();
    qint64 transfer_unit_size = 1024;

    while(bytes_send != all_bytes && connection.state() == QTcpSocket::ConnectedState){
        qint64 bytes_written = connection.write(data, std::min(transfer_unit_size,all_bytes-bytes_send));
        data += bytes_written;
        bytes_send += bytes_written;
        connection.flush();
        connection.waitForBytesWritten();
    }
}
