#include "Logger.h"
#include <iostream>
#include "Workspace.h"

Logger::Logger(void)
{
}

Logger& Logger::getInstance(void){
	static Logger l;
	return l;
}

void Logger::jobStateChanged(Job* job, Job::State state){
	QString s;
	switch(state){
		case Job::FINISHED:
			s.append("FINISHED: ");
			break;
		case Job::SUBMITED:
			s.append("SUBMITED: ");
			break;
		case Job::RUNNING:
			s.append("RUNNING: ");
			break;
		case Job::QUEUED:
			s.append("QUEUED: ");
			break;
	}
	s.append(toString(job));
	log(s);
}

void Logger::jobResults(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile){
	log(QString("GOT RESULTS: from PHO-File: %1").arg(phoFile));
	QHash<QString,double> parameters;
	foreach(parameters, values.keys()){
		QString parameter;
		QString s;
		foreach(parameter, parameters.keys()){
			s.append(parameter);
			s.append("=");
			s.append(QString("%1").arg(parameters[parameter]));
			s.append(" ");
		}
		log(QString("\t%1").arg(s));

		QHash<QString,double> results = values[parameters];
		QString resultName;
		QString r;
		foreach(resultName, results.keys()){
			r.append(resultName);
			r.append("=");
			r.append(QString("%1").arg(results[resultName]));
			r.append(" ");
		}
		log(QString("\t\t-->%1").arg(r));
	}
}

void Logger::jobAdded(Job* job, unsigned int position){
	log(QString("ADDED: at pos %1: %2").arg(position).arg(toString(job)));

}

void Logger::jobRemoved(Job* job){
	log(QString("REMOVED: %1").arg(toString(job)));
}

void Logger::jobMoved(Job* job, unsigned int position){
	log(QString("MOVED: to pos %1: %2").arg(position).arg(toString(job)));
}

void Logger::jobHasProblemsReadingResult(Job* j, QString s){
	log(QString("!!!PROBLEM READING RESULT:"));
	log(QString("%1: %2").arg(toString(j)).arg(s));
}

void Logger::started(){
	log(QString("---QUEUE STARTED---"));
}

void Logger::stopped(){
	log(QString("---QUEUE STOPPED---"));
}


//std::ostream& Logger::out(){
//	return std::cout;
//}

QString Logger::toString(Job* j){
	//QString parameters;
	//QString parameter;
	//foreach(parameter,j->parameters().keys()){
	//	parameters.append(parameter);
	//	parameters.append(":");
	//	parameters.append(j->parameters()[parameter]);
	//	parameters.append(" ");
	//}
        return QString("Job (%1, %2)").arg(j->workspace()->getPJobFile()->pjobFile()).arg(j->description());
}

//void Logger::log(std::string s){
//	emit text(s.c_str());
//}

void Logger::log(QString s){
	s.append("\n");
	//std::cout << s.toStdString();
	emit text(s);
}

void Logger::scriptPrint(QString text){
	log(QString("Script output: %1").arg(text));
}

void Logger::error(const QString& text){
    log(QString("ERROR: %1").arg(text));
}

void Logger::debug(const QString& text){
    log(QString("DEBUG: %1").arg(text));
}
