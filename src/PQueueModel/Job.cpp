#include "Job.h"
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <sstream>
#include <iostream>
#include <limits>
#include "Settings.h"
//#include "PQueueController.h"
#include <iostream>
#include <algorithm>
#include "PJobFileRepository.h"
#include "PQueueController.h"


Job::Job(QHash<QString,QString> parameters)
:m_parameters(parameters)
{
	QObject::moveToThread(PQueueController::getInstace().thread());
}

Job::~Job(){
}

PJobFile* Job::pjobFile(){
	return m_pjobFile;
}

QString Job::description(){
	return m_pjobFile->pjobFile()+": ";
}

QHash<QString,QString> Job::parameters(){
	return m_parameters;
}

void Job::submited(){
	m_state = SUBMITED;
	emit stateChanged(this,m_state);
}

void Job::started(){
	m_state = RUNNING;
	emit stateChanged(this,m_state);
}

void Job::failed(){
	m_state = FAILED;
	emit stateChanged(this,m_state);
}

void Job::finished(){
	m_state = FINISHED;
	emit stateChanged(this,m_state);
}

void Job::process_finished_run(QString runDirectory){
	try{
		m_pjobFile->checkIfRunIsProperlyFinished(runDirectory);
	}
	catch (QString e){
		emit problemReadingResults(this, e);
		failed();
		return;
	}
	
	//Wenn wir bis hier kommen, ist PHOTOSS durchgelaufen und es liegen alle Resultdateien vor..

	try{
		QHash< QHash<QString,double>, QHash<QString,double> > r = m_pjobFile->getResultsForRun(runDirectory);
		emit results(r, m_pjobFile->pjobFile());
		emit results(this, r, m_pjobFile->pjobFile());
	}catch(QString s){
		emit problemReadingResults(this, s);
	}
	finished();

	//Because of this we have to make sure
	//this slot is always called from the main (=GUI) thread
	//which is (hopefully) never blocked.
	m_waitConditionJobState.wakeAll();
}

void Job::waitUntilFinished(){
	while(m_state != FINISHED){
		m_mutex.lock();
		m_waitConditionJobState.wait(&m_mutex);
		m_mutex.unlock();
	};
}


void Job::std_out(QString s){
    m_std_out.append("\n");
    m_std_out.append(s);
}

void Job::err_out(QString s){
    m_err_out.append("\n");
    m_err_out.append(s);
}
