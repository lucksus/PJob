#pragma once
#include <QtCore/QObject>
#include <QtCore/QDir>
#include "PhotossJobSubmitStrategyProcessHolder.h"

class PhotossJob;
class PhotossJobSubmitStrategy : public QObject{
Q_OBJECT
public:
	PhotossJobSubmitStrategy(PhotossJob* j);
	virtual ~PhotossJobSubmitStrategy(void){};

	virtual void submit() = 0;
signals:
	void submited();
	void started();
	void failed();
	void finished(QString newRunDireectory);

protected:
	//! Gibt den Pfad zurück, zu dem kopiert wurde.
	QString copyJobToWorkingDirectoryWithoutRuns();
	QString mergeNewResultsFromWorkingDirectoryBack();
	QString temporaryPJobFileName();
	QDir workingDirectory();
	PhotossJob* m_job;

private:	
	static unsigned int m_jobDirectoryCounter;
	QDir m_workingDirectory;
	QString m_pjobFileName;
};
