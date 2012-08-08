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

    bool is_scanning();


public slots:
    void start_search_network(const QNetworkInterface&);
    void stop_search_network();
    QString hostname(QHostAddress) const;
    QString platform(QHostAddress) const;

    //! Returns number of cores for given pjob runner
    unsigned int max_thread_count_for_host(QHostAddress) const;

    //! Returns number of running threads for given pjob runner
    unsigned int thread_count_for_host(QHostAddress) const;

    //! Calculates the number of threads of the entire pool
    unsigned int max_thread_count() const;

    unsigned int thread_count() const;

signals:
    void found_new_pjob_runner(QHostAddress);
    void lost_pjob_runner(QHostAddress);
    void probing_host(QHostAddress);
    void network_scan_finished();
    void network_scan_started();

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
