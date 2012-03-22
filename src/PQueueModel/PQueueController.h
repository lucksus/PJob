#pragma once
#include <QtCore/QObject>
#include <QtCore/QList>
#include "PhotossJob.h"
#include "Results.h"
using namespace std;

class PQueueController : public QObject
{
Q_OBJECT

public:
	static PQueueController& getInstace(void);
	Results& getResults();

public slots:
	void addJob(PhotossJob*);
	void removeJob(PhotossJob*);
	void setQueuePosition(PhotossJob*, unsigned int position);
	void start(unsigned int atOnce=0);
	void stop();
	bool isRunning();
	void import_results_from_pjobfile(QString file);

	void abort_progress(const QString& what);


signals:
	void jobAdded(PhotossJob*, unsigned int position);
	void jobRemoved(PhotossJob*);
	void jobMoved(PhotossJob*, unsigned int position);
	void started();
	void stopped();
	void progress(QString what, unsigned int percent);


private slots:
	void jobStateChanged(PhotossJob*, PhotossJob::State);

private:
	PQueueController(void);
	~PQueueController(void);

	Results m_results;
	QList<PhotossJob*> m_jobsRunning;
	QList<PhotossJob*> m_jobsFinished;
	QList<PhotossJob*> m_jobsQueued;
	QList<PhotossJob*> m_jobsSubmited;
	bool m_running;
	unsigned int m_jobsAtOnce;

	void jobFinished(PhotossJob*);
	void jobStarted(PhotossJob*);
	void jobSubmited(PhotossJob*);
	void startNextJobInQueue();

	QSet<QString> m_progresses_to_abort;
};

