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
//#include "Workspace.h"
#include <iostream>
#include <algorithm>
#include "PJobFileRepository.h"
#include "Workspace.h"


Job::Job(QHash<QString,QString> parameters, Workspace* workspace)
:m_workspace(workspace), m_parameters(parameters)
{
	QObject::moveToThread(Workspace::getInstace().thread());
}

Job::~Job(){
}

Workspace* Job::workspace(){
        return m_workspace;
}

QString Job::description(){
    QString result;
    QStringList list(m_parameters.keys());
    list.sort();
    foreach(QString key, list){
        result += m_parameters[key] + " ";
    }
    return result;
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
                m_workspace->getPJobFile()->checkIfRunIsProperlyFinished(runDirectory);
	}
	catch (QString e){
		emit problemReadingResults(this, e);
		failed();
		return;
	}
	
	//Wenn wir bis hier kommen, ist PHOTOSS durchgelaufen und es liegen alle Resultdateien vor..

	try{
                QHash< QHash<QString,double>, QHash<QString,double> > r = m_workspace->getPJobFile()->getResultsForRun(runDirectory);
                emit results(r, m_workspace->getPJobFile()->pjobFile());
                emit results(this, r, m_workspace->getPJobFile()->pjobFile());
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