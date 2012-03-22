#ifndef LOG_ADAPTER_H
#define LOG_ADAPTER_H
#include <string>
#include <fstream>
#include <vector>
#include "Condor.h"

namespace CondorAdapter{
using namespace std;

//! Reads Condor events from Condor job log files
/*!
* A instance incorporates one job log file.
* It memorizes how many lines have been read before. So every call to checkForNewEvents()
* returns just the events that have been added to the file since the last call to checkForNewEvents().
*/
class LogAdapter{
public:
	//! Constructor
	/*!
	* \param logfile File to watch for Condor events.
	*/
	LogAdapter(string logfile);

	//! Returns new Condor events
	/*!
	* See Condor documentation for event codes.
	* http://www.cs.wisc.edu/condor/manual/v6.8/2_6Managing_Job.html#SECTION00366000000000000000
	*/
	vector<Event> checkForNewEvents();

private:
	string m_filename;
	unsigned int m_filePosition;
};

}
#endif
