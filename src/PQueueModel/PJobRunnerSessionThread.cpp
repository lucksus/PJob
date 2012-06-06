#include "PJobRunnerSessionThread.h"
#include "pjobrunnersessionwrapper.h"
#include "PQueueController.h"

PJobRunnerSessionThread::PJobRunnerSessionThread(QHostAddress address)
    : m_peer(address)
{
}

void PJobRunnerSessionThread::run(){
    m_session = new PJobRunnerSessionWrapper(m_peer);
    m_session->enqueue();
    m_session->wait_till_its_your_turn();
    Job* job = m_workspace->startNextJobInQueue();
    PJobFile* pjob_file = m_workspace->getPJobFile();

}
