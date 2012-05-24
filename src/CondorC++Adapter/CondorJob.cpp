#include "CondorJob.h"
using namespace CondorAdapter;

CondorJob::CondorJob(string directory, string executable)
: m_id(-1), m_state(_notsubmited), m_directory(directory), m_executable(executable)
{

}

JobState CondorJob::getState(){
	return m_state;
}

string CondorJob::getDirectory(){
	return m_directory;
}

string CondorJob::getExecutable(){
	return m_executable;
}

vector<string> CondorJob::getFilesToBeTransfered(){
	return m_filesToBeTransfered;
}

void CondorJob::addFileToBeTransfered(string filename){
	m_filesToBeTransfered.push_back(filename);
}

vector<string> CondorJob::getArguments(){
	return m_arguments;
}

void CondorJob::addArgument(string argument){
	m_arguments.push_back(argument);
}



void CondorJob::eventForYou(Event e){

	switch(e){
		case __CONDORCPPADAPTER_SUBMITED:
			m_state = _submited;
			break;
		case __CONDORCPPADAPTER_EXECUTING:
			m_state = _running;
			emit started();
			break;
		case __CONDORCPPADAPTER_TERMINATED:
			m_state = _finished;
			emit finished();
			emit finished(this);
			break;
		case __CONDORCPPADAPTER_SHADOW_EXCEPTION:
			m_state = _submited;
			emit failed();
			break;
		case __CONDORCPPADAPTER_SUSPENDED:
			m_state = _suspended;
			emit suspended();
			break;
		case __CONDORCPPADAPTER_UNSUSPENDED:
			m_state = _running;
			emit unsuspended();
			break;

	}
}
