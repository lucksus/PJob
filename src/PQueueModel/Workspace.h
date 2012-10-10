#pragma once
#include <QtCore/QObject>
#include <QtCore/QList>
#include "Results.h"
#include "PJobFile.h"
#include "Job.h"
#include <QTimer>
#include "ParameterVariation.h"
#include <QThread>

using namespace std;
class PJobRunnerSessionThread;
class Workspace : public QObject
{
Q_OBJECT

public:
    static Workspace& getInstace(void);
    Results& getResults();

    //! Moves top queued job from m_jobsQueued to m_jobsRunning and returns it
    Job* get_next_queued_job_and_move_to_running();
    unsigned int thread_count();
    unsigned int enqueued_thread_count();
    ParameterVariation parameter_variation() const;
    void add_raw_results(QByteArray*);



public slots:
    void setPJobFile(PJobFile*);
    PJobFile* getPJobFile();
    void addJob(Job*);
    void removeJob(Job*);
    void setQueuePosition(Job*, unsigned int position);
    void start();
	void stop();
	bool isRunning();
	void import_results_from_pjobfile(QString file);
	void abort_progress(const QString& what);
    void save_pjobfile();
    void clearFinishedJobs();
    QList<Job*> failedJobs();
    QList<Job*> finishedJobs();
    QList<Job*> submittedJobs();
    QList<Job*> queuedJobs();
    void session_threads_update();
    QString pjob_file_signature();
    void prepare_runners_with_pjob_file();
    void start_parameter_variation(ParameterVariation);
    void write_raw_results();


signals:
    void jobAdded(Job*, unsigned int position);
    void jobRemoved(Job*);
    void jobMoved(Job*, unsigned int position);
    void started();
    void stopped();
    void progress(QString what, unsigned int percent);
    void pjobFileChanged(PJobFile*);


private slots:
    void jobStateChanged(Job*, Job::State);
    void session_finished();
    void delete_jobs();
    void upload_progress(unsigned int percent);
    void parameter_variation_update();

private:
    Workspace(void);
    ~Workspace(void);

    QMutex m_mutex;
    QTimer m_job_deleter_timer;
    QTimer m_session_thread_update_timer;

    PJobFile* m_pjob_file;
    QString m_pjob_file_signature;

    Results m_results;
    QList<Job*> m_jobsRunning;
    QList<Job*> m_jobsFinished;
    QList<Job*> m_jobsQueued;
    QList<Job*> m_jobsSubmited;
    QList<Job*> m_jobsFailed;
    QList<Job*> m_jobsToDelete;
    QMutex m_job_lists_mutex;
    bool m_running;
    unsigned int m_jobsAtOnce;

    void jobFinished(Job*);
    void jobStarted(Job*);
    void jobSubmited(Job*);
    void startNextJobInQueue();

    QSet<QString> m_progresses_to_abort;

    //QSet<PJobRunnerSessionThread*> m_session_threads;
    QMap<QHostAddress, QSet<PJobRunnerSessionThread*> >m_session_threads;
    void populate_session_threads();
    void clear_session_threads();
    unsigned int number_of_enqueued_sessions();
    void create_new_session_thread_for_host(QHostAddress);

    unsigned int m_prepared_hosts_count;

    ParameterVariation m_parameter_variation;
    bool m_do_parameter_variation;

    class ParameterVariationThread : public QThread{
    public:
        ParameterVariationThread(Workspace* w):m_workspace(w){}
    protected:
        virtual void run();
        Workspace* m_workspace;
    };
    ParameterVariationThread m_parameter_variation_thread;


    QList<QByteArray*> m_raw_results;
    QMutex m_mutex_raw_results;


    class RawResultSaverThread : public QThread{
    public:
        RawResultSaverThread(Workspace* w):m_workspace(w){}
    protected:
        virtual void run();
        Workspace* m_workspace;
    };
    RawResultSaverThread m_raw_resultsaver_thread;
};

