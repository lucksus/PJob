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
#include "PhotossJobSubmitStrategy.h"
using namespace std;


class PhotossJob : public QObject
{
Q_OBJECT

public:
	PhotossJob(QString pjobFile, QHash<QString,QString> parameters);
	~PhotossJob();

	 
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

	static QHash< QString,QVector<double> > readGlobalVariablesFromPHOFile(QString file);

public slots:
	void submit();

signals:
	void stateChanged(PhotossJob*, PhotossJob::State);
	void results(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
	void results(PhotossJob* job, QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
	void problemReadingResults(PhotossJob*, QString);

private slots:
	void submited();
	void started();
	void failed();
	void finished();
	void process_finished_run(QString runDirectory);

private:
	
	PJobFile* m_pjobFile;
	QString m_pjobFileAbsoulutePath;
	//QString m_phoFile;
	//QString m_scriptFile;
	QString m_workingDirectory;
	QHash<QString, QString> m_parameters;
	State m_state;
	PhotossJobSubmitStrategy* m_submitStrategy;
	void createSubmitStrategy();
	
	QMutex m_mutex;
	QWaitCondition m_waitConditionJobState;
};


