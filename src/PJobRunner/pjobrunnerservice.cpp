#include "pjobrunnerservice.h"
#include "tcpserver.h"
#include <boost/foreach.hpp>
#include <QDate>
#include <QTime>

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
    m_log_file.setFileName(QCoreApplication::applicationDirPath()+"/log.txt");
    m_log_file.open(QIODevice::WriteOnly | QIODevice::Append);
}

void PJobRunnerService::stop(){
    TcpServer::instance().set_active(false);
    if(m_ticket_dispatcher) delete m_ticket_dispatcher;
    m_ticket_dispatcher = 0;
    m_log_file.close();
}

void PJobRunnerService::pause(){
    stop();
}

void PJobRunnerService::resume(){
    start();
}

void PJobRunnerService::processCommand(int code){

}

void PJobRunnerService::log(QString message,const MessageType &type){
    switch(type){
    case Success:
        message.prepend("SUCCESS: ");
        break;
    case Error:
        message.prepend("ERROR: ");
        break;
    case Warning:
        message.prepend("WARNING: ");
        break;
    case Information:
    default:
        break;
    }
    message.append("\n");
    message.prepend(QTime::currentTime().toString("hh:mm:ss: "));
    message.prepend(QDate::currentDate().toString("yyyymmdd_"));
    m_log_file.write(message.toStdString().c_str());
    m_log_file.flush();
}

PJobRunnerService* PJobRunnerService::instance(){
    return dynamic_cast<PJobRunnerService*>(QtServiceBase::instance());
}


