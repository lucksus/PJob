#include "dataconnectionserver.h"
#include "dataconnectionthread.h"
#include <assert.h>

quint32 DataConnectionServer::s_port = 23023;
QMutex DataConnectionServer::m_port_mutex;

DataConnectionServer::DataConnectionServer(QObject *parent) :
    QTcpServer(parent), m_worker_thread(0), m_data(0), m_has_received_data(false)
{
}

quint32 DataConnectionServer::open_data_port(){
   QMutexLocker locker(&m_port_mutex);
   quint32 port = s_port + 1;
   while(!listen(QHostAddress::Any, port) && port < 32000) port++;
   if(!isListening()) throw QString("No free port found!");
   s_port = port;
   return port;
}

unsigned int DataConnectionServer::serve_data(const QByteArray& data){
    QByteArray *ptr = new QByteArray(data);
    try{
        return serve_data(ptr);
    }catch(QString s){
        delete ptr;
        throw(s);
    }
}

unsigned int DataConnectionServer::serve_data(QByteArray *data){
    if(isListening()) throw QString("DataConnectionServer is already listening!");
    if(m_data) delete m_data;
    m_data = data;
    m_mode = Serve;
    return open_data_port();
}

unsigned int DataConnectionServer::receive_data(){
    if(isListening()) throw QString("DataConnectionServer is already listening!");
    m_mode = Receive;
    m_has_received_data = false;
    return open_data_port();
}

QByteArray* DataConnectionServer::received_data() const{
    return m_data;
}

bool DataConnectionServer::has_received_data() const{
    return m_has_received_data;
}

void DataConnectionServer::incomingConnection(int handle){
    assert(m_worker_thread == 0);
    close(); //only first connection gets handled
    switch(m_mode){
    case Serve:
        m_worker_thread = new DataPushThread(handle, m_data);
        break;
    case Receive:
        m_worker_thread = new DataReceiveThread(handle);
        break;
    default:
        assert(false);
    }
    connect(m_worker_thread, SIGNAL(finished()), this, SLOT(thread_finished()));
    m_worker_thread->start();

}

void DataConnectionServer::thread_finished(){
    if(m_mode == Serve){
        delete m_worker_thread;
        delete m_data;
        emit transmission_finished();
        return;
    }

    assert(dynamic_cast<DataReceiveThread*>(m_worker_thread) != 0);
    DataReceiveThread* thread = dynamic_cast<DataReceiveThread*>(m_worker_thread);
    m_data = thread->data();
    emit transmission_finished();
}
