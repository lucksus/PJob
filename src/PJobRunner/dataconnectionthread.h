#ifndef DATACONNECTIONTHREAD_H
#define DATACONNECTIONTHREAD_H

#include <QThread>

class DataReceiveThread : public QThread
{
public:
    explicit DataReceiveThread(unsigned int socket_descriptor);
    QByteArray* data() const;
protected:
    void run();

private:
    QByteArray* m_data;
    unsigned int m_socket_descriptor;
};

class DataPushThread : public QThread
{
public:
    explicit DataPushThread(unsigned int socket_descriptor, QByteArray* data);
protected:
    void run();
private:
    QByteArray* m_data;
    unsigned int m_socket_descriptor;
};

#endif // DATACONNECTIONTHREAD_H
