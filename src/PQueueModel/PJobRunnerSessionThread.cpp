#include "PJobRunnerSessionThread.h"
#include "pjobrunnersessionwrapper.h"
#include "PQueueController.h"

PJobRunnerSessionThread::PJobRunnerSessionThread(QHostAddress address, PQueueController* workspace)
    : m_peer(address), m_workspace(workspace)
{
}

void PJobRunnerSessionThread::run(){
    m_session = new PJobRunnerSessionWrapper(m_peer);
    m_session->enqueue();
    m_session->wait_till_its_your_turn();
    Job* job = m_workspace->startNextJobInQueue();
    if(!job) return;
    job->submited();
    PJobFile* pjob_file = m_workspace->getPJobFile();
    if(!pjob_file) return;
    QByteArray* raw = pjob_file->raw_without_results();
    m_session->upload_pjobfile(*raw);
    delete raw;
    foreach(QString parameter, job->parameters().keys()){
        bool ok;
        double d = job->parameters()[parameter].toDouble(&ok);
        if(ok) m_session->set_parameter(parameter, d);
    }
    connect(m_session, SIGNAL(job_std_out(QString)), job, SLOT(err_out(QString)));
    connect(m_session, SIGNAL(job_error_out(QString)), job, SLOT(err_out(QString)));
    if(m_session->run_job()){
        job->started();
        if(!m_session->wait_for_job_finished())
            job->failed();
        else{
            //Job is finished. Get results.
            QByteArray results;
            m_session->download_results(results);
            job->finished();
        }
    }else job->failed();
    disconnect(m_session, SIGNAL(job_std_out(QString)), job, SLOT(err_out(QString)));
    disconnect(m_session, SIGNAL(job_error_out(QString)), job, SLOT(err_out(QString)));
}
