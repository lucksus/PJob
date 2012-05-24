#ifndef CONDOR_H
#define CONDOR_H
#include <string>
#include <map>
#include <QThread>
#include <QMutex>

namespace CondorAdapter{
using namespace std;
typedef int Event;
class LogAdapter;
class CondorJob;

//! Singleton object which incorporates Condor and all its supported features (like submit, remove..)
/*!
* This class is a singleton. The static function getInstance() provides access to the object.
* At present the only supported feature is to submit a Job. Therefore a Job object has to be
* constructed and passed to function submit(Job*). When reading an condor event (from a condor log file
* by using LogAdapter) this object calls eventForYou(Event) on the Job object (wich in turn changes the
* Job object's state an emits a signal).
*
* This class inherits QThread so function run() runs in its own thread. run() checks every m_interval seconds
* all jobs logfiles for new Condor events and passes them to the associated Job object.
*/
class Condor : public QThread{
public:
	static Condor& getInstance();

	//! Generates Condor job file and executes "condor_submit"
	/*!
	* Calls writeJobFile(string,string,Job*) to create a Condor job file
	* submits that job to Condor by executing "condor_submit" and spawns
	* a thread for run() if it does not exist yet.
	*/
        void submit(CondorJob*);
	
	//! Sets Condor install directory
	/*!
	* In order to call "condor_submit" we need to know where this executable resides.
	* Its path is expected to be $(directory)/bin/$(condor_submit).
	* $(directory) defaults to "C:/condor" and can be set with this method.
	*/
	void setCondorDirectory(string directory);

	//! Sets filename of "condor_submit" executable
	/*!
	* In order to call "condor_submit" we need to know where this executable resides.
	* Its path is expected to be $(directory)/bin/$(condor_submit).
	* $(condor_submit) defaults to "condor_submit.exe" and can be set with this method.
	*/
	void setCondorSubmitCommand(string command);

	//! Time in seconds between two consecutive log file checks
	void setEventReadingInterval(unsigned int seconds);

	//! Stops the log file reading thread
	/*!
	* Call this function when your application quits.
	*/
	void stopThread();

protected:
	//! Thread's main-function
	/*!
	* Gets events from log file and passes them to Job objects.
	*/
	virtual void run();

private:
	Condor();
	~Condor();
        void writeJobFile(string directory, string filename, CondorJob*);

        map<CondorJob*,LogAdapter*> m_logAdapters;
	QMutex m_mutex;
	bool m_stopRequested;
	bool m_noJobs;

	unsigned int m_interval;
	string m_condorDirectory;
	string m_condorSubmit;

	
};

}
#endif
