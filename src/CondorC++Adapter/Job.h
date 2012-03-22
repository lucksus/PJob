#ifndef JOB_H
#define JOB_H
#include <string>
#include <vector>
#include <QObject>
#include "Condor.h"

//Condor event codes:
#define __CONDORCPPADAPTER_SUBMITED 0 
#define __CONDORCPPADAPTER_EXECUTING 1 
#define __CONDORCPPADAPTER_ERROR_IN_EXECUTABLE 2 
#define __CONDORCPPADAPTER_CHEKCPOINTED 3 
#define __CONDORCPPADAPTER_EVICTED 4
#define __CONDORCPPADAPTER_TERMINATED 5
#define __CONDORCPPADAPTER_SHADOW_EXCEPTION 7
#define __CONDORCPPADAPTER_SUSPENDED 10
#define __CONDORCPPADAPTER_UNSUSPENDED 11
//See Condor documentation for a complete list:
//http://www.cs.wisc.edu/condor/manual/v6.8/2_6Managing_Job.html#SECTION00366000000000000000

namespace CondorAdapter{
using namespace std;

enum JobState {_notsubmited, _submited, _running, _suspended, _finished};


//! A class for Condor Jobs
/*!
* For every job you submit to Condor you need an object of this class.
* Every jobs needs a working directory and an executable. Both must be
* provided to the constructor. Additional files which need to be transfered
* and arguments for the executable can be added by using Job's methods.
* To submit a job pass the object to Condor::submit(Job*).
* The Condor object calls eventForYou(Event) (that's why class Condor is a friend)
* to inform the job about events. This method updates the job's state and
* emits a corresponding signal.
*
* States:
* A job's initial state is _notsubmited. After submition it is _submited.
* When execution begins started() is emited and the state changes to _running.
* A running job can be suspended and unsuspended what would make the Job
* object emit suspended() and unsuspended() accordingly and set the state
* to _supended and back again to _running.
* When the job terminates the Job object emits finished() and changes its state
* to _finished.
* 
*/
class Job : public QObject{
Q_OBJECT
friend class Condor;
public:
	//! Constructor
	/*!
	* A Condor job needs a working directory for log files and result files
	* and an application which should be run.
	* \param directory Working directory
	* \param executable Application to be run on Condor
	*/
	Job(string directory, string executable);

	//! Returns the Jobs state
	/*!
	* Can be one of: _notsubmited, _submited, _running, _suspended, _finished.
	*/
	JobState getState();

	//! Path to job's working directory
	/*!
	* Called by Condor when job is being submited
	*/
	string getDirectory();

	//! Path to job's application
	/*!
	* Called by Condor when job is being submited
	*/
	string getExecutable();

	//! Paths to files which need to be transfered together with executable
	/*!
	* Called by Condor when job is being submited
	*/
	vector<string> getFilesToBeTransfered();

	//! Adds a file which needs to be transfered on execution
	void addFileToBeTransfered(string filename);

	//! Arguments to be passed to application on execution
	/*!
	* Called by Condor when job is being submited
	*/
	vector<string> getArguments();

	//! Adds an arguments which needs to be passed to application on execution
	void addArgument(string argument);

signals:
	void started();
	void failed();
	void suspended();
	void unsuspended();
	void finished();
	void finished(Job*);

private:
	int m_id;
	JobState m_state;
	string m_directory;
	string m_executable;
	vector<string> m_arguments;
	vector<string> m_filesToBeTransfered;
	void eventForYou(Event);
};

}
#endif
