#include "PQueueController.h"
#include <algorithm>
#include <QtCore/QMetaType>
#include "Logger.h"
#include "Scripter.h"
PQueueController::PQueueController(void)
: m_pjob_file(0), m_running(false)
{
	qRegisterMetaType< QHash<QString,double> >("QHash<QString,double>");
	qRegisterMetaType<QHash< QHash<QString,double>, QHash<QString,double> > >("QHash< QHash<QString,double>, QHash<QString,double> >");
        connect(this, SIGNAL(jobAdded(Job*,unsigned int)), &Logger::getInstance(), SLOT(jobAdded(Job*,unsigned int)));
        connect(this, SIGNAL(jobRemoved(Job*)), &Logger::getInstance(), SLOT(jobRemoved(Job*)));
        connect(this, SIGNAL(jobMoved(Job*,unsigned int)), &Logger::getInstance(), SLOT(jobMoved(Job*,unsigned int)));
	connect(this, SIGNAL(started()), &Logger::getInstance(), SLOT(started()));
	connect(this, SIGNAL(stopped()), &Logger::getInstance(), SLOT(stopped()));
	connect(&m_results, SIGNAL(newValueSet(QString , QString , QHash<QString,double> , double )),
		&Logger::getInstance(), SLOT(newValueSet(QString , QString , QHash<QString,double> , double )));
	Scripter::getInstance();
}

PQueueController::~PQueueController(void)
{
}

PQueueController& PQueueController::getInstace(void)
{
	static PQueueController p;
	return p;
}

Results& PQueueController::getResults(){
	return m_results;
}

void PQueueController::setPJobFile(PJobFile* p){
    m_pjob_file = p;
    emit pjobFileChanged(p);
}

void PQueueController::addJob(Job* j){
	m_jobsQueued.push_back(j);
        connect(j, SIGNAL(stateChanged(Job*, Job::State)), this, SLOT(jobStateChanged(Job*, Job::State)));
        connect(j, SIGNAL(stateChanged(Job*, Job::State)), &Logger::getInstance(), SLOT(jobStateChanged(Job*, Job::State)));
	connect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
		&m_results, SLOT(newValues(QHash< QHash<QString,double>, QHash<QString,double> > , QString )));
	connect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
		&Logger::getInstance(), SLOT(jobResults(QHash< QHash<QString,double>, QHash<QString,double> > , QString )));
        connect(j, SIGNAL(problemReadingResults(Job*,QString)), &Logger::getInstance(), SLOT(jobHasProblemsReadingResult(Job*,QString)));
	emit jobAdded(j, m_jobsQueued.indexOf(j));

        if(m_running && (m_jobsAtOnce == 0 || m_jobsRunning.size()+m_jobsSubmited.size() < static_cast<int>(m_jobsAtOnce))){
		j->submit();
	}
}

void PQueueController::removeJob(Job* j){
	m_jobsSubmited.removeOne(j);
	m_jobsRunning.removeOne(j);
	m_jobsFinished.removeOne(j);
	m_jobsQueued.removeOne(j);
        disconnect(j, SIGNAL(stateChanged(Job*, Job::State)), this, SLOT(jobStateChanged(Job*, Job::State)));
        disconnect(j, SIGNAL(stateChanged(Job*, Job::State)), &Logger::getInstance(), SLOT(jobStateChanged(Job*, Job::State)));
	disconnect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
		&m_results, SLOT(newValues(QHash< QHash<QString,double>, QHash<QString,double> > , QString )));
	disconnect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
		&Logger::getInstance(), SLOT(jobResults(QHash< QHash<QString,double>, QHash<QString,double> > , QString )));
	emit jobRemoved(j);
}

void PQueueController::setQueuePosition(Job* j, unsigned int position){
        if(static_cast<int>(position) >= m_jobsQueued.size()) position = m_jobsQueued.size()-1;
	m_jobsQueued.removeOne(j);
	m_jobsQueued.insert(position,j);
	emit jobMoved(j,position);
}

void PQueueController::start(unsigned int atOnce){
	m_running = true;
	m_jobsAtOnce = atOnce;
	emit started();
        Job* job;
	unsigned int submitCount=0;
	foreach(job, m_jobsQueued){
		if(atOnce > 0 && submitCount == atOnce)
			return;
		job->submit();
		submitCount++;
	}
	
}

void PQueueController::stop(){
	m_running = false;
	emit stopped();
}


void PQueueController::jobStateChanged(Job* job, Job::State state){
	switch(state){
                case Job::FINISHED:
			jobFinished(job);
			break;
                case Job::SUBMITED:
			jobSubmited(job);
			break;
                case Job::RUNNING:
			jobStarted(job);
			break;
                case Job::QUEUED:
			break;
	}
}

void PQueueController::jobSubmited(Job* j){
	m_jobsQueued.removeOne(j);
	m_jobsSubmited.push_back(j);
}

void PQueueController::jobStarted(Job* j){
	m_jobsQueued.removeOne(j);
	m_jobsSubmited.removeOne(j);
	m_jobsRunning.push_back(j);
}

void PQueueController::startNextJobInQueue(){
	if(m_jobsQueued.isEmpty())return;
        Job* job = m_jobsQueued.first();
	job->submit();
}

void PQueueController::jobFinished(Job* j){
	m_jobsRunning.removeOne(j);
	m_jobsFinished.push_back(j);
	if(m_running) startNextJobInQueue();
}


bool PQueueController::isRunning(){
	return m_running;
}

void PQueueController::import_results_from_pjobfile(QString filename)
{
	PJobFile file(filename);
	QStringList runList = file.runDirectoryEntries();
	
	//run_DATUM - Strings herausfiltern
	int i = 0;
	foreach(QString s, runList)
	{
		runList.replace(i++,s.section('/',0,1));
	}

	//doppelte Einträge verwerfen (jeder run soll logischerweise nur einmal geladen werden)
	runList.removeDuplicates();
	
	unsigned int size = runList.size();
	QString message = QString("Importing results from %1...").arg(filename);
	if(size > 10) emit progress(message,0);
	i = 0;
	foreach(QString s, runList)
	{
		//Results auslesen...
		QHash< QHash<QString,double>, QHash<QString,double> > result;
		try
		{
			result = file.getResultsForRun(s);
		}
		catch(QString s)
		{
			//Ursache z.B.: Dateien unvollständig => parametercombination.xml passt nicht zu den Ergebnissen)
			//=> überspringen
			continue;
		}

		//...und weiterleiten
		m_results.newValues(result,file.pjobFile());
		if(size > 10) emit progress(message,i++*100/size);
		if(m_progresses_to_abort.contains(message)){
			m_progresses_to_abort.remove(message);
			break;
		}
	}
	if(size > 10) emit progress(message,100);
}

void PQueueController::abort_progress(const QString& what){
	m_progresses_to_abort.insert(what);
}
