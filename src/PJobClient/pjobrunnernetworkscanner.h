#ifndef PJOBRUNNERNETWORKSCANNER_H
#define PJOBRUNNERNETWORKSCANNER_H
#include <QtCore/QThread>
#include <QtNetwork/QHostAddress>

class PJobRunnerSessionWrapper;
class PJobRunnerNetworkScanner : public QThread
{
Q_OBJECT
public:
    PJobRunnerNetworkScanner();
    static QList<PJobRunnerSessionWrapper*> do_blocking_scan();

protected:
    virtual void run();

signals:
    void found_pjob_runner(PJobRunnerSessionWrapper*);
    void finished_scanning();
    void probing_host(QHostAddress);

private:
    quint32 m_port;
    void found(PJobRunnerSessionWrapper*);
    static QList<PJobRunnerSessionWrapper*> s_found_sessions;
    void scan();
    static bool s_blocking_scan;
};

#endif // PJOBRUNNERNETWORKSCANNER_H
