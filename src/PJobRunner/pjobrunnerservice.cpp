#include "pjobrunnerservice.h"
#include "tcpserver.h"
#include <boost/foreach.hpp>

PJobRunnerService::PJobRunnerService(int argc, char** argv) :
    QtService<QCoreApplication>(argc,argv,"PJobRunner"), m_ticket_dispatcher(0)
{
    setServiceDescription("PCloud's worker deamon.");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtService<QCoreApplication>::CanBeSuspended);
    application()->setApplicationName("PJobRunner");
    application()->setOrganizationName("lucksus");
    application()->setOrganizationDomain("lucksus.eu");
    application()->setApplicationVersion("0.1");

}

TicketDispatcher* PJobRunnerService::ticket_dispatcher(){
    return m_ticket_dispatcher;
}

void PJobRunnerService::start(){
    m_ticket_dispatcher = new TicketDispatcher;
    TcpServer::instance().set_active(true);
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
