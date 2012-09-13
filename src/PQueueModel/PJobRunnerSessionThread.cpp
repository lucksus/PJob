#include "PJobRunnerSessionThread.h"
#include "pjobrunnersessionwrapper.h"
#include "Workspace.h"
#include <memory>
#include "Logger.h"

PJobRunnerSessionThread::PJobRunnerSessionThread(QHostAddress address, Workspace* workspace)
    : m_peer(address), m_workspace(workspace)
{
    m_enqueued = false;
}

void PJobRunnerSessionThread::run(){
    m_enqueued = false;
    std::auto_ptr<PJobRunnerSessionWrapper> session(new PJobRunnerSessionWrapper(m_peer));
    session->set_debug(true);
    connect(session.get(), SIGNAL(debug_out(QString)), &Logger::getInstance(), SLOT(debug(QString)));
    session->enqueue();
    m_enqueued = true;
    if(!session->wait_till_its_your_turn()) return;
    m_enqueued = false;
    Job* job = m_workspace->get_next_queued_job_and_move_to_running();
    if(!job) return;
    QMutexLocker lock(&job->m_mutex_deletable);
    job->m_session = session.get();
    connect(session.get(), SIGNAL(debug_out(QString)), job, SLOT(got_connection_debug(QString)));
    job->submited();
    PJobFile* pjob_file = m_workspace->getPJobFile();
    if(!pjob_file) return;
    QByteArray* raw = pjob_file->raw_without_results();

    unsigned int tries=0;
    bool ok=true;
    do{
        try{
            tries++;
            ok = session->upload_pjobfile(*raw);
        }catch(LostConnectionException e){
            ok=false;
            job->got_connection_debug(e.what());
        }
    }while(!ok && tries <10);
    delete raw;
    if(!ok){
        job->failed();
        job->m_session = 0;
        return;
    }

    try{
        foreach(QString parameter, job->parameters().keys()){
            bool ok;
            double d = job->parameters()[parameter].toDouble(&ok);
            if(ok) session->set_parameter(parameter, d);
        }
        connect(session.get(), SIGNAL(job_std_out(QString)), job, SLOT(got_std_out(QString)));
        connect(session.get(), SIGNAL(job_error_out(QString)), job, SLOT(got_err_out(QString)));
        if(session->run_job()){
            job->started();
            if(!session->wait_for_job_finished())
                job->failed();
            else{
                //Job is finished. Get results.
                QByteArray results;
                session->download_results(results);
                try{
                    QString run_name = PJobFile::name_of_first_run_in_raw_bytes(results);
                    pjob_file->add_raw_files(results);
                    job->process_finished_run(run_name);
                    job->finished();
                }catch(QString s){
                    job->failed();
                }
            }
        }else job->failed();
    }catch(LostConnectionException e){
        job->failed();
        job->got_connection_debug(e.what());
    }

    job->m_session = 0;
    disconnect(session.get(), SIGNAL(job_std_out(QString)), job, SLOT(got_std_out(QString)));
    disconnect(session.get(), SIGNAL(job_error_out(QString)), job, SLOT(got_err_out(QString)));
    disconnect(session.get(), SIGNAL(debug_out(QString)), job, SLOT(got_connection_debug(QString)));
}

bool PJobRunnerSessionThread::is_enqueued(){
    return m_enqueued;
}
