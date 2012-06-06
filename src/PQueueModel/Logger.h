#pragma once
#include <QtCore/QObject>
#include <ostream>
#include "Job.h"

class Logger : public QObject
{
Q_OBJECT
public:
	static Logger& getInstance(void);

public slots:
	void jobStateChanged(Job*, Job::State);
	void jobResults(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
	void jobAdded(Job*, unsigned int position);
	void jobRemoved(Job*);
	void jobMoved(Job*, unsigned int position);
	void jobHasProblemsReadingResult(Job*, QString);
	void started();
	void stopped();
	void scriptPrint(QString text);

	void error(QString text);
	
signals:
	void text(QString);

private:
	Logger(void);
	//std::ostream& out();
	//void log(std::string);
	void log(QString);
	QString toString(Job* j);
};
