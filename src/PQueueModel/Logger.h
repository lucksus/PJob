#pragma once
#include <QtCore/QObject>
#include <ostream>
#include "PhotossJob.h"

class Logger : public QObject
{
Q_OBJECT
public:
	static Logger& getInstance(void);

public slots:
	void jobStateChanged(PhotossJob*, PhotossJob::State);
	void jobResults(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
	void jobAdded(PhotossJob*, unsigned int position);
	void jobRemoved(PhotossJob*);
	void jobMoved(PhotossJob*, unsigned int position);
	void jobHasProblemsReadingResult(PhotossJob*, QString);
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
	QString toString(PhotossJob* j);
};
