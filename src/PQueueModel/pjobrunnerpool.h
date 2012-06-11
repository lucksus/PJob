#ifndef PJOBRUNNERPOOL_H
#define PJOBRUNNERPOOL_H
#include "pjobrunnersessionwrapper.h"
#include "pjobrunnernetworkscanner.h"

class PJobRunnerPool : public QObject
{
Q_OBJECT
public:
    static PJobRunnerPool& instance();
    //! Gives a List of all hosts of this pool running a pjob runner
    QList<QHostAddress> known_pjob_runners() const;

    //! Returns number of threads for given pjob runner
    unsigned int thread_count(QHostAddress) const;

    //! Calculates the number of threads of the entire pool
    unsigned int max_thread_count() const;


public slots:
    void start_search_local_network();
    QString hostname(QHostAddress) const;
    QString platform(QHostAddress) const;

signals:
    void found_new_pjob_runner(QHostAddress);
    void lost_pjob_runner(QHostAddress);
    void probing_host(QHostAddress);
    void search_local_network_finished();

private slots:
    void found_pjob_runner(PJobRunnerSessionWrapper*);
    void search_finished();
    void scanner_is_probing(QHostAddress);

private:
    PJobRunnerPool();
    PJobRunnerNetworkScanner m_scanner;
    QList<QHostAddress> m_known_pjob_runners, m_backup_list;
    QHash<QHostAddress, PJobRunnerSessionWrapper*> m_info_sessions;
};

#endif // PJOBRUNNERPOOL_H
