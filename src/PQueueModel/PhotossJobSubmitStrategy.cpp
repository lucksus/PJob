#include "PhotossJobSubmitStrategy.h"
#include "Settings.h"
#include "PhotossJob.h"
#include <QtCore/QDateTime>
unsigned int PhotossJobSubmitStrategy::m_jobDirectoryCounter = 0;


PhotossJobSubmitStrategy::PhotossJobSubmitStrategy(Job* j)
:m_job(j)
{
	QDir workingDirectory(Settings::getInstance().workingDirectory());
	QString subDirectory = QDateTime::currentDateTime().toString("yyyyMMdd_hhmm_ss_zzz");
	workingDirectory.mkdir(subDirectory);
	workingDirectory.cd(subDirectory);
	m_jobDirectoryCounter++;
	m_workingDirectory = workingDirectory;

	QFileInfo pjobFile(m_job->pjobFile()->pjobFile());
	m_pjobFileName = pjobFile.fileName();
}

QString PhotossJobSubmitStrategy::copyJobToWorkingDirectoryWithoutRuns(){
	m_job->pjobFile()->copyWithoutRuns(m_workingDirectory.absoluteFilePath(m_pjobFileName));
	return m_workingDirectory.absoluteFilePath(m_pjobFileName);
}

QString PhotossJobSubmitStrategy::mergeNewResultsFromWorkingDirectoryBack(){
	PJobFile pjobFile(m_workingDirectory.absoluteFilePath(m_pjobFileName));
	m_job->pjobFile()->mergeRunsFrom(pjobFile);
	m_job->pjobFile()->save();
	return pjobFile.latestRunDirectory();
}

QString PhotossJobSubmitStrategy::temporaryPJobFileName(){
	return m_pjobFileName;
}

QDir PhotossJobSubmitStrategy::workingDirectory(){
	return m_workingDirectory;
}
