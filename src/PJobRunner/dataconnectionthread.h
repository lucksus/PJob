#ifndef DATACONNECTIONTHREAD_H
#define DATACONNECTIONTHREAD_H

#include <QThread>
class QTcpServer;
class DataConnectionThread : public QThread
{
    Q_OBJECT
public:
    explicit DataConnectionThread(QByteArray& data_destination, QObject *parent = 0);
    ~DataConnectionThread();
    quint32 open_data_port();
    bool data_received();

protected:
    void run();


private:
    QByteArray& m_data_destination;
    QTcpServer* m_server;
    bool m_data_received;
    bool m_want_exit;
};

#endif // DATACONNECTIONTHREAD_H
