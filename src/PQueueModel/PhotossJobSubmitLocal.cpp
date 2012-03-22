#include "PhotossJobSubmitLocal.h"
#include <QtCore/QFileInfo>
#include "PhotossJob.h"
#include "Settings.h"
#include <QtCore/QTimer>
#include <QtCore/QDir>

PhotossJobSubmitLocal::PhotossJobSubmitLocal(PhotossJob* j)
:PhotossJobSubmitStrategy(j)
{
	//This assures that slots are processed by the main (=GUI) thread
	//even if this object was created from a different (=script engine) thread.
	//moveToThread(PQueueController::getInstace().thread());
	connect(this, SIGNAL(wantsProcess(QString,QStringList,QString,PhotossJobSubmitStrategy*)),
		&PhotossJobSubmitStrategyProcessHolder::getInstance(), SLOT(spawnProcess(QString,QStringList,QString,PhotossJobSubmitStrategy*)));
}

PhotossJobSubmitLocal::~PhotossJobSubmitLocal(void)
{
}


void PhotossJobSubmitLocal::submit(){
	copyJobToWorkingDirectoryWithoutRuns();
	QFileInfo pjobFileInfo = QFileInfo(workingDirectory().absoluteFilePath(temporaryPJobFileName()));
	QString workingDirectory = pjobFileInfo.absolutePath();

	QStringList args;
	args << QDir::toNativeSeparators(pjobFileInfo.absoluteFilePath());
	if(Settings::getInstance().startPhotossWithoutMatlab()){
		args << "-m"; args << "none";
	}
	if(Settings::getInstance().runPhotossMinimized()){
		args << "--minimized";
	}
	
	QString p;
	foreach(p,m_job->parameters().keys())
		args << "--param" << QString("%1=%2").arg(p).arg(m_job->parameters()[p]);
	
	emit wantsProcess(Settings::getInstance().photossDirectory().append("/Photoss.exe"),args,workingDirectory,this);
}

void PhotossJobSubmitLocal::_started(){
	emit started();
}

void PhotossJobSubmitLocal::_finished(){
	QString latestRunDirectory = mergeNewResultsFromWorkingDirectoryBack();
	emit finished(latestRunDirectory);
}

