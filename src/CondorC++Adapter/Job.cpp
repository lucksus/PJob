#include "Job.h"
using namespace CondorAdapter;

Job::Job(string directory, string executable)
: m_id(-1), m_state(_notsubmited), m_directory(directory), m_executable(executable)
{

}

JobState Job::getState(){
	return m_state;
}

string Job::getDirectory(){
	return m_directory;
}

string Job::getExecutable(){
	return m_executable;
}

vector<string> Job::getFilesToBeTransfered(){
	return m_filesToBeTransfered;
}

void Job::addFileToBeTransfered(string filename){
	m_filesToBeTransfered.push_back(filename);
}

vector<string> Job::getArguments(){
	return m_arguments;
}

void Job::addArgument(string argument){
	m_arguments.push_back(argument);
}



void Job::eventForYou(Event e){

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
