#pragma once
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QProcess>
#include <fstream>
#include "PJobFile.h"
using namespace std;


class Job : public QObject
{
Q_OBJECT
friend class PJobRunnerSessionThread;
public:
        Job(QString pjobFile, QHash<QString,QString> parameters);
        ~Job();

	 
	PJobFile* pjobFile();
	QString mainScript();
	QString description();
	void addResource(QString path);
	//QString phoFile();
	QHash<QString,QString> parameters();


	enum State{QUEUED, SUBMITED, RUNNING, FINISHED, FAILED};

	/*! 
	* Blocks the calling thread (which must not be the main (=GUI) thread!)
	* until this job is finished.
	*/
	Q_INVOKABLE void waitUntilFinished();

public slots:
	void submit();

signals:
        void stateChanged(Job*, Job::State);
	void results(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
        void results(Job* job, QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
        void problemReadingResults(Job*, QString);

private slots:
	void submited();
	void started();
	void failed();
	void finished();
	void process_finished_run(QString runDirectory);
        void std_out(QString);
        void err_out(QString);

private:
	
	PJobFile* m_pjobFile;
	QString m_pjobFileAbsoulutePath;
	//QString m_phoFile;
	//QString m_scriptFile;
	QString m_workingDirectory;
	QHash<QString, QString> m_parameters;
	State m_state;
	
	QMutex m_mutex;
	QWaitCondition m_waitConditionJobState;
};


