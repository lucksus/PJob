#ifndef DATACONNECTIONSERVER_H
#define DATACONNECTIONSERVER_H

#include <QTcpServer>
#include <QMutex>

class DataConnectionServer : public QTcpServer
{
Q_OBJECT
public:
    explicit DataConnectionServer(QObject *parent = 0);
    unsigned int serve_data(const QByteArray& data);
    unsigned int serve_data(QByteArray* data);
    unsigned int receive_data();
    QByteArray* received_data() const;
    bool has_received_data() const;

signals:
    void transmission_finished();
    
protected:
    virtual void incomingConnection(int handle);

private:
    QThread* m_worker_thread;
    QByteArray *m_data;
    static quint32 s_port;
    static QMutex m_port_mutex;
    enum Mode{Serve, Receive};
    Mode m_mode;
    bool m_has_received_data;

    quint32 open_data_port();

private slots:
    void thread_finished();

};

#endif // DATACONNECTIONSERVER_H
