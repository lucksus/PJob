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
    virtual ~PJobRunnerSessionThread();
    virtual void run();

private:
    QHostAddress m_peer;
    PJobRunnerSessionWrapper *m_session;
    PQueueController *m_workspace;
};

#endif // PJOBRUNNERSESSIONTHREAD_H
