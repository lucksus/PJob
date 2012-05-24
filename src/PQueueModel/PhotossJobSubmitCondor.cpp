#include "PhotossJobSubmitCondor.h"
#include "PJobFile.h"
#include "CondorJob.h"
#include "Job.h"
#include "Settings.h"
#include <sstream>
#include "Logger.h"


PhotossJobSubmitCondor::PhotossJobSubmitCondor(Job* j)
:PhotossJobSubmitStrategy(j)
{
}

PhotossJobSubmitCondor::~PhotossJobSubmitCondor(void)
{
}


void PhotossJobSubmitCondor::submit(){
	m_path_to_workingcopy_pjobfile = copyJobToWorkingDirectoryWithoutRuns();
	m_condorJob = createJob();
	//m_workingDirectory = m_job->getDirectory().c_str();
	CondorAdapter::Condor::getInstance().setCondorDirectory(Settings::getInstance().condorDirectory().toStdString());
	try{
		CondorAdapter::Condor::getInstance().submit(m_condorJob);
		emit submited();
	}catch(QString error){
		QString message = "Could not submit job to Condor! condor_submit says:\n" + error;
		Logger::getInstance().error(message);
		emit failed();
	}
}

CondorAdapter::CondorJob* PhotossJobSubmitCondor::createJob(){
	stringstream photoss_exe;
	string photossDirectory = Settings::getInstance().photossDirectory().toStdString();
	photoss_exe << photossDirectory << "/Photoss.exe";

        CondorAdapter::CondorJob* j = new CondorAdapter::CondorJob(workingDirectory().absolutePath().toStdString(), photoss_exe.str());
	connect(j,SIGNAL(started()),this,SLOT(_started()));
	connect(j,SIGNAL(failed()),this,SLOT(_failed()));
	connect(j,SIGNAL(finished()),this,SLOT(_finished()));
	QDir photossDir(photossDirectory.c_str());
	QString entry;
	foreach(entry, photossDir.entryList()){
		QFileInfo info(photossDir.absoluteFilePath(entry));
		if(info.isDir()) continue;
		if(info.fileName() == "PQueue.exe") continue;
		if(info.fileName() == "PJobFileEditor.exe") continue;
		j->addFileToBeTransfered(photossDir.absoluteFilePath(entry).toStdString());
	}
	{
		//stringstream s;
		//s << photossDirectory << "/component_dlls/dev_kit.dll";
		//j->addFileToBeTransfered(s.str());
	}
	{
		stringstream s;
		s << m_path_to_workingcopy_pjobfile.toStdString();
		j->addFileToBeTransfered(s.str());
	}
	{
		stringstream s;
		s << temporaryPJobFileName().toStdString();
		if(Settings::getInstance().startPhotossWithoutMatlab())
			s << " -m none";

		QString parameter;
		foreach(parameter, m_job->parameters().keys()){
			s << " --param " << parameter.toStdString() << "=" << m_job->parameters()[parameter].toStdString();
		}

		j->addArgument(s.str());
	}
	return j;
}


void PhotossJobSubmitCondor::_started(){
	emit started();
}
void PhotossJobSubmitCondor::_failed(){
	emit failed();
}

void PhotossJobSubmitCondor::_finished(){
	try{
		QString latestRunDirectory = mergeNewResultsFromWorkingDirectoryBack();
		emit finished(latestRunDirectory);
	}catch(QString error){
		Logger::getInstance().error("No new run directory found after execution via Condor!!");
		_failed();
	}
	
}
