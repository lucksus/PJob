#ifndef PJOBRUNNERSESSIONTHREAD_H
#define PJOBRUNNERSESSIONTHREAD_H
#include <QThread>
#include <QtNetwork/QHostAddress>

class PJobRunnerSessionWrapper;
class Workspace;
class PJobRunnerSessionThread : public QThread
{
Q_OBJECT
public:
    PJobRunnerSessionThread(QHostAddress, Workspace*);
    virtual void run();
    bool is_enqueued();

private:
    QHostAddress m_peer;
    Workspace *m_workspace;
    bool m_enqueued;
};

#endif // PJOBRUNNERSESSIONTHREAD_H
