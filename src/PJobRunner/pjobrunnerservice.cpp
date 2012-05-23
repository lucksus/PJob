#include "pjobrunnerservice.h"
#include "tcpserver.h"
#include <boost/foreach.hpp>
#include "session.h"

PJobRunnerService::PJobRunnerService(int argc, char** argv) :
    QtService<QCoreApplication>(argc,argv,"PJobRunner")
{
    setServiceDescription("PCloud's worker deamon.");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtService<QCoreApplication>::CanBeSuspended);
    application()->setApplicationName("PJobRunner");
    application()->setOrganizationName("lucksus");
    application()->setOrganizationDomain("lucksus.eu");
    application()->setApplicationVersion("0.1");
    m_max_process_count = QThread::idealThreadCount();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(dispatch()));
    m_timer.start(200);
}

void PJobRunnerService::start(){
    TcpServer::instance().set_active(true);
    TcpServer::instance().start();
}

void PJobRunnerService::stop(){
    TcpServer::instance().set_active(false);
}

void PJobRunnerService::pause(){
    stop();
}

void PJobRunnerService::resume(){
    start();
}

void PJobRunnerService::processCommand(int code){

}

unsigned int PJobRunnerService::max_process_count(){
    return m_max_process_count;
}

void PJobRunnerService::set_max_process_count(unsigned int count){
    m_max_process_count = count;
}

unsigned int PJobRunnerService::number_queue_entries_for_peer(QHostAddress address){
    unsigned int count = 0;
    BOOST_FOREACH(Session* s, m_queue){
        if(s->peer() == address) count++;
    }
    return count;
}

void PJobRunnerService::enqueue(Session* s){
    m_queue.push_back(s);
}

void PJobRunnerService::dispatch(){
    QMutexLocker l(m_mutex);

}
