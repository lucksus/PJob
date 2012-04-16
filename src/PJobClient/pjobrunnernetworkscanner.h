#ifndef PJOBRUNNERNETWORKSCANNER_H
#define PJOBRUNNERNETWORKSCANNER_H
#include <QtCore/QThread>

class PJobRunnerSessionWrapper;
class PJobRunnerNetworkScanner : public QThread
{
Q_OBJECT
public:
    PJobRunnerNetworkScanner();

protected:
    virtual void run();

signals:
    void found_pjob_runner(PJobRunnerSessionWrapper*);
    void finished_scanning();

private:
    quint32 m_port;
};

#endif // PJOBRUNNERNETWORKSCANNER_H
