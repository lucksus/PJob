#pragma once
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QProcess>
#include <fstream>
using namespace std;

class Workspace;
class Job : public QObject
{
Q_OBJECT
friend class PJobRunnerSessionThread;
public:
        Job(QHash<QString,QString> parameters, Workspace*);
        ~Job();

	QString description();
	QHash<QString,QString> parameters();

	enum State{QUEUED, SUBMITED, RUNNING, FINISHED, FAILED};

	/*! 
	* Blocks the calling thread (which must not be the main (=GUI) thread!)
	* until this job is finished.
	*/
	Q_INVOKABLE void waitUntilFinished();

        Workspace* workspace() const;
        QString std_out() const;

signals:
        void stateChanged(Job*, Job::State);
	void results(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
        void results(Job* job, QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
        void problemReadingResults(Job*, QString);
        void std_out(QString);
        void err_out(QString);

private slots:
	void submited();
	void started();
	void failed();
	void finished();
	void process_finished_run(QString runDirectory);
        void got_std_out(QString);
        void got_err_out(QString);

private:
        Workspace* m_workspace;
	QHash<QString, QString> m_parameters;
	State m_state;
        QString m_std_out;
        QString m_err_out;
	
	QMutex m_mutex;
	QWaitCondition m_waitConditionJobState;
};


