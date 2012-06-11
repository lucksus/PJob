#include "PJobRunnerSessionThread.h"
#include "pjobrunnersessionwrapper.h"
#include "PQueueController.h"
#include <memory>

PJobRunnerSessionThread::PJobRunnerSessionThread(QHostAddress address, PQueueController* workspace)
    : m_peer(address), m_workspace(workspace)
{
}

void PJobRunnerSessionThread::run(){
    std::auto_ptr<PJobRunnerSessionWrapper> session(new PJobRunnerSessionWrapper(m_peer));
    session->enqueue();
    session->wait_till_its_your_turn();
    Job* job = m_workspace->startNextQueuedJob();
    if(!job) return;
    job->submited();
    PJobFile* pjob_file = m_workspace->getPJobFile();
    if(!pjob_file) return;
    QByteArray* raw = pjob_file->raw_without_results();
    session->upload_pjobfile(*raw);
    delete raw;
    foreach(QString parameter, job->parameters().keys()){
        bool ok;
        double d = job->parameters()[parameter].toDouble(&ok);
        if(ok) session->set_parameter(parameter, d);
    }
    connect(session.get(), SIGNAL(job_std_out(QString)), job, SLOT(err_out(QString)));
    connect(session.get(), SIGNAL(job_error_out(QString)), job, SLOT(err_out(QString)));
    if(session->run_job()){
        job->started();
        if(!session->wait_for_job_finished())
            job->failed();
        else{
            //Job is finished. Get results.
            QByteArray results;
            session->download_results(results);
            QString run_name = PJobFile::name_of_first_run_in_raw_bytes(results);
            pjob_file->add_raw_files(results);
            job->process_finished_run(run_name);
            job->finished();
        }
    }else job->failed();
    disconnect(session.get(), SIGNAL(job_std_out(QString)), job, SLOT(err_out(QString)));
    disconnect(session.get(), SIGNAL(job_error_out(QString)), job, SLOT(err_out(QString)));
}
