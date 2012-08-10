#pragma once
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QProcess>
#include <fstream>
#include <QtCore/QMetaType>
#include <QtNetwork/QHostAddress>

using namespace std;

class Workspace;
class PJobRunnerSessionWrapper;
class Job : public QObject
{
Q_OBJECT
friend class PJobRunnerSessionThread;
public:
    Job();
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

    /*!
    * Blocks the calling thread (which must not be the main (=GUI) thread!)
    * until this job either running, finished or failed.
    */
    Q_INVOKABLE void waitUntilRunning();

    /*!
     * If the job is failed, sets it status back to queued and emits signal.
     */
    Q_INVOKABLE void requeue();

    Workspace* workspace() const;
    QString std_out() const;
    QString err_out() const;
    QString connection_debug() const;

    QHostAddress peer() const;
    State state() const {return m_state;}

signals:
    void stateChanged(Job*, Job::State);
	void results(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
    void results(Job* job, QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
    void problemReadingResults(Job*, QString);
    void std_out(QString);
    void err_out(QString);
    void connection_debug(QString);

private slots:
	void submited();
	void started();
	void failed();
	void finished();
	void process_finished_run(QString runDirectory);
    void got_std_out(QString);
    void got_err_out(QString);
    void got_connection_debug(QString);

private:
    Workspace* m_workspace;
    QHash<QString, QString> m_parameters;
    State m_state;
    QString m_std_out;
    QString m_err_out;
    QString m_connection_debug;
    PJobRunnerSessionWrapper* m_session;

public:
    QMutex m_mutex_deletable;
};

Q_DECLARE_METATYPE(Job*);
Q_DECLARE_METATYPE(QList<Job*>);


