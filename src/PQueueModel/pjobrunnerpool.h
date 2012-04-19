#ifndef PJOBRUNNERPOOL_H
#define PJOBRUNNERPOOL_H
#include "pjobrunnersessionwrapper.h"
#include "pjobrunnernetworkscanner.h"

class PJobRunnerPool : public QObject
{
Q_OBJECT
public:
    static PJobRunnerPool& instance();
    QList<QHostAddress> known_pjob_runners();

public slots:
    void start_search_local_network();

signals:
    void found_new_pjob_runner(QHostAddress);
    void lost_pjob_runner(QHostAddress);
    void probing_host(QHostAddress);

private slots:
    void found_pjob_runner(PJobRunnerSessionWrapper*);
    void search_finished();
    void scanner_is_probing(QHostAddress);

private:
    PJobRunnerPool();
    PJobRunnerNetworkScanner m_scanner;
    QList<QHostAddress> m_known_pjob_runners, m_backup_list;
};

#endif // PJOBRUNNERPOOL_H
