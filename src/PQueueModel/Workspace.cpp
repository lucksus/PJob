#include "Workspace.h"
#include <algorithm>
#include <QtCore/QMetaType>
#include "Logger.h"
#include "Scripter.h"
#include "pjobrunnerpool.h"
#include "PJobRunnerSessionThread.h"
#include <QDateTime>

Workspace::Workspace(void)
    : m_pjob_file(0), m_job_lists_mutex(QMutex::Recursive), m_running(false), m_parameter_variation_thread(this)
{
    qRegisterMetaType< QHash<QString,double> >("QHash<QString,double>");
    qRegisterMetaType<QHash< QHash<QString,double>, QHash<QString,double> > >("QHash< QHash<QString,double>, QHash<QString,double> >");
    connect(this, SIGNAL(jobAdded(Job*,unsigned int)), &Logger::getInstance(), SLOT(jobAdded(Job*,unsigned int)));
    connect(this, SIGNAL(jobRemoved(Job*)), &Logger::getInstance(), SLOT(jobRemoved(Job*)));
    connect(this, SIGNAL(jobMoved(Job*,unsigned int)), &Logger::getInstance(), SLOT(jobMoved(Job*,unsigned int)));
    connect(this, SIGNAL(started()), &Logger::getInstance(), SLOT(started()));
    connect(this, SIGNAL(stopped()), &Logger::getInstance(), SLOT(stopped()));
    connect(&m_results, SIGNAL(newValueSet(QString , QString , QHash<QString,double> , double )), &Logger::getInstance(), SLOT(newValueSet(QString , QString , QHash<QString,double> , double )));
    Scripter::getInstance();
    connect(&m_job_deleter_timer, SIGNAL(timeout()), this, SLOT(delete_jobs()));
    connect(&m_session_thread_update_timer, SIGNAL(timeout()), this, SLOT(session_threads_update()));
    m_job_deleter_timer.start(500);
    m_session_thread_update_timer.start(500);
    qRegisterMetaType<Job::State>("Job::State");
}

Workspace::~Workspace(void)
{
}

Workspace& Workspace::getInstace(void)
{
    static Workspace p;
    return p;
}

Results& Workspace::getResults(){
	return m_results;
}

void Workspace::setPJobFile(PJobFile* p){
    m_pjob_file = p;
    ParameterVariation pv;
    foreach(PJobFileParameterDefinition def, Workspace::getInstace().getPJobFile()->parameterDefinitions()){
        pv.add_parameter(def.name(), def.defaultValue(), def.defaultValue(), 1);
    }
    m_parameter_variation = pv;

    QFileInfo info(p->pjobFile());
    m_pjob_file_signature = QString("%1__%2").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmm_ss_zzz")).arg(info.baseName());
    emit pjobFileChanged(p);
}

PJobFile* Workspace::getPJobFile(){
    return m_pjob_file;
}

void Workspace::addJob(Job* j){
    m_job_lists_mutex.lock();
	m_jobsQueued.push_back(j);
    m_job_lists_mutex.unlock();
        connect(j, SIGNAL(stateChanged(Job*, Job::State)), this, SLOT(jobStateChanged(Job*, Job::State)), Qt::QueuedConnection);
        connect(j, SIGNAL(stateChanged(Job*, Job::State)), &Logger::getInstance(), SLOT(jobStateChanged(Job*, Job::State)), Qt::QueuedConnection);
	connect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
                &m_results, SLOT(newValues(QHash< QHash<QString,double>, QHash<QString,double> > , QString )), Qt::QueuedConnection);
	connect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
                &Logger::getInstance(), SLOT(jobResults(QHash< QHash<QString,double>, QHash<QString,double> > , QString )), Qt::QueuedConnection);
        connect(j, SIGNAL(problemReadingResults(Job*,QString)), &Logger::getInstance(), SLOT(jobHasProblemsReadingResult(Job*,QString)), Qt::QueuedConnection);
	emit jobAdded(j, m_jobsQueued.indexOf(j));
}

void Workspace::removeJob(Job* j){
    m_job_lists_mutex.lock();
	m_jobsSubmited.removeOne(j);
	m_jobsRunning.removeOne(j);
	m_jobsFinished.removeOne(j);
	m_jobsQueued.removeOne(j);
    m_job_lists_mutex.unlock();
        disconnect(j, SIGNAL(stateChanged(Job*, Job::State)), this, SLOT(jobStateChanged(Job*, Job::State)));
        disconnect(j, SIGNAL(stateChanged(Job*, Job::State)), &Logger::getInstance(), SLOT(jobStateChanged(Job*, Job::State)));
	disconnect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
		&m_results, SLOT(newValues(QHash< QHash<QString,double>, QHash<QString,double> > , QString )));
	disconnect(j, SIGNAL(results(QHash< QHash<QString,double>, QHash<QString,double> > , QString )),
		&Logger::getInstance(), SLOT(jobResults(QHash< QHash<QString,double>, QHash<QString,double> > , QString )));
	emit jobRemoved(j);
}

void Workspace::setQueuePosition(Job* j, unsigned int position){
    QMutexLocker locker(&m_job_lists_mutex);
    if(static_cast<int>(position) >= m_jobsQueued.size()) position = m_jobsQueued.size()-1;
	m_jobsQueued.removeOne(j);
	m_jobsQueued.insert(position,j);
	emit jobMoved(j,position);
}

void Workspace::start(){
    if(m_running) return;
    m_running = true;
    prepare_runners_with_pjob_file();
    populate_session_threads();
    session_threads_update();
    emit started();
}

void Workspace::prepare_runners_with_pjob_file(){
    QString progress_title = "Uploading PJob file to pjob runners...";
    emit progress(progress_title, 0);
    unsigned int i = 0;
    m_prepared_hosts_count = 0;
    foreach(QHostAddress host, PJobRunnerPool::instance().known_pjob_runners()){
        PJobRunnerSessionWrapper session(host);
        connect(&session, SIGNAL(upload_progress(uint)), this, SLOT(upload_progress(uint)));
        if(! session.open_pjob_from_user_file(m_pjob_file_signature) ){
            QByteArray *raw = getPJobFile()->raw_without_results();
            bool ok = false;
            while(!ok){
                if(!session.upload_pjobfile(*raw)) continue;
                while(!ok){
                    try{
                        ok = session.save_user_file(m_pjob_file_signature);
                    }catch(LostConnectionException e){}
                }
            }
            delete raw;
        }
        m_prepared_hosts_count++;
    }
    emit progress(progress_title, 100);
}

void Workspace::upload_progress(unsigned int percent){
    PJobRunnerSessionWrapper* session = dynamic_cast<PJobRunnerSessionWrapper*>(sender());
    if(!session) return;
    QString progress_title = "Uploading PJob file to pjob runners...";
    unsigned int overall_percent = m_prepared_hosts_count * 100 / PJobRunnerPool::instance().known_pjob_runners().size();
    unsigned int this_percent = percent*0.99 / PJobRunnerPool::instance().known_pjob_runners().size();
    emit progress(progress_title, overall_percent + this_percent);

}

void Workspace::session_threads_update(){
    if(m_running){
        foreach(QHostAddress host, m_session_threads.keys()){
            foreach(PJobRunnerSessionThread* thread, m_session_threads[host]){
                if(!thread->isRunning()) thread->start();
            }
        }
    }else{
        clear_session_threads();
    }
}

void Workspace::stop(){
    if(!m_running) return;
	m_running = false;
    m_parameter_variation_thread.quit();
	emit stopped();
}


void Workspace::jobStateChanged(Job* job, Job::State state){
    QMutexLocker locker(&m_job_lists_mutex);
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
            m_jobsFailed.removeOne(job);
            m_jobsFinished.removeOne(job);
            m_jobsQueued.push_back(job);
			break;
    case Job::FAILED:
            m_jobsRunning.removeOne(job);
            m_jobsSubmited.removeOne(job);
            m_jobsQueued.removeOne(job);
            m_jobsFailed.push_back(job);
	}
}

void Workspace::jobSubmited(Job* j){
    QMutexLocker locker(&m_job_lists_mutex);
	m_jobsQueued.removeOne(j);
	m_jobsSubmited.push_back(j);
}

void Workspace::jobStarted(Job* j){
    QMutexLocker locker(&m_job_lists_mutex);
	m_jobsQueued.removeOne(j);
	m_jobsSubmited.removeOne(j);
	m_jobsRunning.push_back(j);
}

void Workspace::jobFinished(Job* j){
    QMutexLocker locker(&m_job_lists_mutex);
	m_jobsRunning.removeOne(j);
	m_jobsFinished.push_back(j);
}


bool Workspace::isRunning(){
	return m_running;
}

void Workspace::import_results_from_pjobfile(QString filename)
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
                catch(ReadFileError e)
                {
                        //s.o.
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

void Workspace::abort_progress(const QString& what){
	m_progresses_to_abort.insert(what);
}

Job* Workspace::get_next_queued_job_and_move_to_running(){
    QMutexLocker locker(&m_job_lists_mutex);
    if(m_jobsQueued.isEmpty()) return 0;
    Job* job = m_jobsQueued.takeFirst();
    m_jobsRunning.push_back(job);
    return job;
}

unsigned int Workspace::thread_count(){
    unsigned int session_thread_count = 0;
    foreach(QHostAddress host, m_session_threads.keys()){
        session_thread_count += m_session_threads[host].size();
    }
    return session_thread_count;
}

unsigned int Workspace::enqueued_thread_count(){
    unsigned int count = 0;
    foreach(QHostAddress host, m_session_threads.keys()){
        foreach(PJobRunnerSessionThread* thread, m_session_threads[host]){
            if(thread->is_enqueued()) count++;
        }
    }
    return count;
}

void Workspace::session_finished(){
    if(!m_running) return;
    QObject* sender_object = sender();
    PJobRunnerSessionThread* thread = dynamic_cast<PJobRunnerSessionThread*>(sender_object);
    if(!thread) return;
    unsigned int max_thread_count = PJobRunnerPool::instance().max_thread_count();
    unsigned int session_thread_count = thread_count();

    if(max_thread_count < session_thread_count){
        foreach(QHostAddress host, m_session_threads.keys()){
            m_session_threads[host].remove(thread);
        }
        delete thread;
        return;
    }
    if(max_thread_count > session_thread_count){
        foreach(QHostAddress host, m_session_threads.keys()){
            for(unsigned int i=m_session_threads[host].size(); i < PJobRunnerPool::instance().max_thread_count_for_host(host); i++)
                create_new_session_thread_for_host(host);
        }
    }

    if(m_jobsQueued.size()<=0) return;
    thread->start();
}

void Workspace::create_new_session_thread_for_host(QHostAddress host){
    PJobRunnerSessionThread* thread = new PJobRunnerSessionThread(host, this);
    connect(thread, SIGNAL(finished()), this, SLOT(session_finished()), Qt::QueuedConnection);
    m_session_threads[host].insert(thread);
}

void Workspace::populate_session_threads(){
    clear_session_threads();
    foreach(QHostAddress host, PJobRunnerPool::instance().known_pjob_runners()){
        try{
            unsigned int thread_count = PJobRunnerPool::instance().max_thread_count_for_host(host);
            for(unsigned int i=0; i<thread_count; i++){
                create_new_session_thread_for_host(host);
            }
        }catch(LostConnectionException e){
            PJobRunnerPool::instance().remove(host);
        }
    }
}

void Workspace::clear_session_threads(){
    foreach(QHostAddress host, m_session_threads.keys())
        foreach(PJobRunnerSessionThread* thread, m_session_threads[host]){
            if(thread->isRunning()) continue;
            m_session_threads[host].remove(thread);
            delete thread;
            if(m_session_threads[host].isEmpty()) m_session_threads.remove(host);
        }
}

unsigned int Workspace::number_of_enqueued_sessions(){
    unsigned int count = 0;
    foreach(QHostAddress host, m_session_threads.keys())
        foreach(PJobRunnerSessionThread* thread, m_session_threads[host]){
            if(thread->is_enqueued()) count++;
        }
    return count;
}

void Workspace::save_pjobfile(){
    m_pjob_file->save();
}

void Workspace::clearFinishedJobs(){
    QMutexLocker locker(&m_job_lists_mutex);
    while(m_jobsFinished.size() > 0)
        m_jobsToDelete.append(m_jobsFinished.takeFirst());
}

QList<Job*> Workspace::failedJobs(){
    QMutexLocker locker(&m_job_lists_mutex);
    return m_jobsFailed;
}

QList<Job*> Workspace::finishedJobs(){
    QMutexLocker locker(&m_job_lists_mutex);
    return m_jobsFinished;
}

QList<Job*> Workspace::submittedJobs(){
    QMutexLocker locker(&m_job_lists_mutex);
    return m_jobsSubmited;
}

QList<Job*> Workspace::queuedJobs(){
    QMutexLocker locker(&m_job_lists_mutex);
    return m_jobsQueued;
}

void Workspace::delete_jobs(){
    QMutexLocker locker(&m_job_lists_mutex);
    QList<Job*> jobs_remained;
    QList<Job*> seen_jobs;
    while(m_jobsToDelete.size() > 0){
        Job *j = m_jobsToDelete.takeFirst();
        if(seen_jobs.contains(j)) continue;
        seen_jobs.append(j);
        if(! j->m_mutex_deletable.tryLock(5)){
            jobs_remained.append(j);
            continue;
        }
        removeJob(j);
        j->m_mutex_deletable.unlock();
        delete j;
    }

    m_jobsToDelete = jobs_remained;
}

QString Workspace::pjob_file_signature(){
    return m_pjob_file_signature;
}

void Workspace::start_parameter_variation(ParameterVariation pv){
    m_parameter_variation = pv;
    m_parameter_variation.reset();
    start();
    m_parameter_variation_thread.start();
}

void Workspace::parameter_variation_update(){
    clearFinishedJobs();
    unsigned int max_thread_count = PJobRunnerPool::instance().max_thread_count();
    unsigned int thread_count = PJobRunnerPool::instance().thread_count();
    int threads_to_start = max_thread_count - thread_count - submittedJobs().length() - queuedJobs().length();
    if(threads_to_start  <= 0) return;
    QList<Job*> failed_jobs = failedJobs();
    for(unsigned int i=0;i<threads_to_start;i++){
        Job* job;
        if(failed_jobs.length() > 0){
            job = failed_jobs.front();
            failed_jobs.pop_front();
            job->requeue();
        }else{
            if(!m_parameter_variation.index_valid()) break;
            QHash<QString,double> parameters = m_parameter_variation.parameter_combination();
            m_parameter_variation.next();
            QHash<QString,QString> parameters_string;
            foreach(QString name, parameters.keys()){
                parameters_string[name] = QString("%1").arg(parameters[name]);
            }
            job = new Job(parameters_string,this);
            addJob(job);
        }
    }
}


ParameterVariation Workspace::parameter_variation() const{
    return m_parameter_variation;
}

void Workspace::ParameterVariationThread::run(){
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), m_workspace, SLOT(parameter_variation_update()), Qt::DirectConnection);
    timer.start(1000);
    exec();
}
