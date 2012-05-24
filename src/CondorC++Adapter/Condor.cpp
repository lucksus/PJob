#include "Condor.h"
#include "CondorJob.h"
#include "LogAdapter.h"
#include <QMutexLocker>
#include <QProcess>
#include <QDir>
#include <QString>
#include <QFile>
#include <vector>
#include <sstream>
#include <iostream>
using namespace CondorAdapter;

Condor::Condor()
: m_stopRequested(false), m_noJobs(true), m_interval(1), m_condorDirectory("C:/condor"), m_condorSubmit("condor_submit.exe")
{
}

Condor::~Condor(){
	stopThread();
}


Condor& Condor::getInstance(){
	static Condor c;
	return c;
}

void Condor::submit(CondorJob* job){
	stringstream jobFileName; jobFileName << job->getDirectory() << "/job.txt";
	stringstream logFileName; logFileName << job->getDirectory() << "/log.txt";
	stringstream condorSubmit; condorSubmit << m_condorDirectory << "/bin/" << m_condorSubmit;
	QDir dir(job->getDirectory().c_str());
	if(! dir.exists())
		dir.mkpath(job->getDirectory().c_str());
	else{
		QString jobPath = dir.filePath("job.txt");
		QString logPath = dir.filePath("log.txt");
		QFile::remove(jobPath);
		QFile::remove(logPath);
	}
	writeJobFile(job->getDirectory(), "job.txt", job);

	QString program = condorSubmit.str().c_str();
	QStringList arguments;
	arguments << "job.txt";
	QProcess cs(this);
	cs.setWorkingDirectory(job->getDirectory().c_str());
	cs.start(program, arguments); 
	cs.waitForFinished(60000);	//wait 60 seconds

	if(cs.exitCode() != 0){
		QString error = cs.readAllStandardError();
		throw error;
	}

	QMutexLocker locker(&m_mutex);
	m_logAdapters[job] = new LogAdapter(logFileName.str());
	m_noJobs = false;
	if(!isRunning())
		start();

}

void Condor::writeJobFile(string directory, string filename, CondorJob* job){
	stringstream stream;
	stream << directory << "/" << filename;
	cout << stream.str();
	fstream jobFile(stream.str().c_str(), ios::out);
	jobFile << "EXECUTABLE = " << job->getExecutable() << endl;
	jobFile << "UNIVERSE = vanilla" << endl
			<< "SHOULD_TRANSFER_FILES = YES" << endl
			<< "WHEN_TO_TRANSFER_OUTPUT = ON_EXIT" << endl
			<< "REQUIREMENTS = (Arch == \"INTEL\" || Arch == \"X86_64\") && (OpSys == \"WINNT60\" || OpSys == \"WINNT61\" || OpSys == \"WINNT51\")" << endl;

	vector<string> files = job->getFilesToBeTransfered();
	if(files.size() > 0){
		jobFile << "TRANSFER_INPUT_FILES = " << files[0];
		for(unsigned int i=1;i<files.size();++i)
			jobFile << ", \\" << endl << files[i];
	}
	jobFile << endl;

	vector<string> arguments = job->getArguments();
	if(arguments.size() > 0){
		jobFile << "ARGUMENTS = " << arguments[0];
		for(unsigned int i=1;i<arguments.size();++i)
			jobFile << " " << arguments[i];
	}
	jobFile << endl;

	jobFile << "ERROR = err.txt" << endl;
	jobFile << "OUTPUT = out.txt" << endl;
	jobFile << "LOG = log.txt" << endl;
	jobFile << "QUEUE" << endl;
}


void Condor::stopThread(){
	m_stopRequested = true;
}


void Condor::run(){
	m_stopRequested = false;
	while(!m_stopRequested && !m_noJobs){
		QMutexLocker locker(&m_mutex);
			vector<map<CondorJob*,LogAdapter*>::iterator> eraseList;
			map<CondorJob*,LogAdapter*>::iterator it;
			//there is one LogAdapter for every running job:
			for(it=m_logAdapters.begin();it!=m_logAdapters.end();++it){
				CondorJob* job = it->first;
				LogAdapter* logAdapter = it->second;

				//-----------------------------------------------------
				//get new events from log file through LogAdapter:
				vector<Event> events = logAdapter->checkForNewEvents();
				//-----------------------------------------------------

				//for every new event call eventForYou on the Job object:
				for(unsigned int i=0;i<events.size();++i){
					//---------------------------
					job->eventForYou(events[i]);
					//---------------------------
					if(__CONDORCPPADAPTER_TERMINATED == events[i])
						eraseList.push_back(it);
				}
			}

			//erase all finished jobs from collection:
			for(unsigned int i=0;i<eraseList.size();++i)
				m_logAdapters.erase(eraseList[i]);
			//if collection is empty this thread can be closed:
			if(m_logAdapters.size() == 0)
				m_noJobs = true;

		locker.unlock();
		sleep(m_interval);
	}
}

void Condor::setCondorDirectory(string directory){
	m_condorDirectory = directory;
}
