#pragma once
#include <QtCore/QObject>
#include <QtCore/QDir>
#include "PhotossJobSubmitStrategyProcessHolder.h"

class Job;
class PhotossJobSubmitStrategy : public QObject{
Q_OBJECT
public:
        PhotossJobSubmitStrategy(Job* j);
	virtual ~PhotossJobSubmitStrategy(void){};

	virtual void submit() = 0;
signals:
	void submited();
	void started();
	void failed();
	void finished(QString newRunDireectory);

protected:
	//! Gibt den Pfad zurck, zu dem kopiert wurde.
	QString copyJobToWorkingDirectoryWithoutRuns();
	QString mergeNewResultsFromWorkingDirectoryBack();
	QString temporaryPJobFileName();
	QDir workingDirectory();
        Job* m_job;

private:	
	static unsigned int m_jobDirectoryCounter;
	QDir m_workingDirectory;
	QString m_pjobFileName;
};
