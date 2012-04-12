#ifndef DATACONNECTIONTHREAD_H
#define DATACONNECTIONTHREAD_H

#include <QThread>
class QTcpServer;
class DataConnection : public QThread
{
public:
    explicit DataConnection(QByteArray& data_destination, QObject *parent = 0);
    ~DataConnection();
    quint32 open_data_port();
protected:
    QByteArray& m_data;
    QTcpServer* m_server;
    bool m_want_exit;
    static quint32 s_port;
};


class DataReceiveConnection : public DataConnection
{
public:
    explicit DataReceiveConnection(QByteArray& data_destination, QObject *parent = 0);
    bool data_received();
protected:
    void run();

private:
    bool m_data_received;
};

class DataPushConnection : public DataConnection{
public:
    explicit DataPushConnection(QByteArray& data_destination, QObject *parent = 0);
protected:
    void run();
};

#endif // DATACONNECTIONTHREAD_H
