#ifndef PJOBRUNNERSESSIONTHREAD_H
#define PJOBRUNNERSESSIONTHREAD_H
#include <QThread>
#include <QtNetwork/QHostAddress>

class PJobRunnerSessionWrapper;
class PQueueController;
class PJobRunnerSessionThread : public QThread
{
Q_OBJECT
public:
    PJobRunnerSessionThread(QHostAddress, PQueueController*);
    virtual void run();

private:
    QHostAddress m_peer;
    PQueueController *m_workspace;
};

#endif // PJOBRUNNERSESSIONTHREAD_H
