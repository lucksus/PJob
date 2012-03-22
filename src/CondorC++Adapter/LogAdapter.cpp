#include "LogAdapter.h"
#include <sstream>
#include <assert.h>

using namespace CondorAdapter;

LogAdapter::LogAdapter(string logfile)
: m_filename(logfile), m_filePosition(0)
{
}

vector<Event> LogAdapter::checkForNewEvents(){
	ifstream file(m_filename.c_str());
	unsigned int MAX_BUFFER=300;
	char* buffer = new char[MAX_BUFFER];
	
	for(unsigned int i=0;i<m_filePosition;++i)
		file.getline(buffer,MAX_BUFFER);
	
	vector<Event> newEvents;
	while(file.getline(buffer,MAX_BUFFER)){
		++m_filePosition;
		if('\t' == buffer[0]) continue;
		if('.' == buffer[0]) continue;
		if(' ' == buffer[0]) continue;
		stringstream s;
		s.str(string(buffer));
		Event ev;
		s >> ev;
		assert(ev <= 27); //There are 28 Condor-Events
		newEvents.push_back(ev);
	}
	delete buffer;

	return newEvents;
}
