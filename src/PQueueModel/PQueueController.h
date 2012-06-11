#pragma once
#include <QtCore/QObject>
#include <QtCore/QList>
#include "Job.h"
#include "Results.h"
#include "PJobFile.h"

using namespace std;
class PJobRunnerSessionThread;
class PQueueController : public QObject
{
Q_OBJECT

public:
	static PQueueController& getInstace(void);
	Results& getResults();

        //! Moves top queued job from m_jobsQueued to m_jobsRunning and returns it
        Job* startNextQueuedJob();


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

private:
	PQueueController(void);
	~PQueueController(void);

        QMutex m_mutex;

        PJobFile* m_pjob_file;

	Results m_results;
        QList<Job*> m_jobsRunning;
        QList<Job*> m_jobsFinished;
        QList<Job*> m_jobsQueued;
        QList<Job*> m_jobsSubmited;
	bool m_running;
	unsigned int m_jobsAtOnce;

        void jobFinished(Job*);
        void jobStarted(Job*);
        void jobSubmited(Job*);
	void startNextJobInQueue();

	QSet<QString> m_progresses_to_abort;

        QSet<PJobRunnerSessionThread*> m_session_threads;
        void populate_session_threads();
        void clear_session_threads();
};

