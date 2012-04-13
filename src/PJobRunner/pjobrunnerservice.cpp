#include "pjobrunnerservice.h"
#include "tcpserver.h"

PJobRunnerService::PJobRunnerService(int argc, char** argv) :
    QtService<QCoreApplication>(argc,argv,"PJobRunner")
{
    setServiceDescription("PCloud's worker deamon.");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtService::CanBeSuspended);
    application()->setApplicationName("PJobRunner");
    application()->setOrganizationName("lucksus");
    application()->setOrganizationDomain("lucksus.eu");
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
