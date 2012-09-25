#include "pjobrunnerservice.h"
#include "tcpserver.h"
#include <boost/foreach.hpp>
#include <QDate>
#include <QTime>

Timer::Timer(PJobRunnerService* service) : m_service(service){
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.start(1000*60*5);
}

void Timer::timeout(){
    m_service->clean_old_user_files();
}


PJobRunnerService::PJobRunnerService(int argc, char** argv) :
    QtService<QCoreApplication>(argc,argv,"PJobRunner"), m_ticket_dispatcher(0),
    m_user_files_mutex(QMutex::Recursive), m_mutex_log_file(QMutex::Recursive), m_timer(this)
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
    message.prepend(QDate::currentDate().toString("yyyyMMdd_"));
    QMutexLocker l(&m_mutex_log_file);
    m_log_file.write(message.toStdString().c_str());
    m_log_file.flush();
}

PJobRunnerService* PJobRunnerService::instance(){
    return dynamic_cast<PJobRunnerService*>(QtServiceBase::instance());
}

QDir PJobRunnerService::saved_user_files_dir(){
    QDir temp = QDir::temp();
    QString saved_files_dir = "PJobServer_saved_user_files";
    if(!temp.exists(saved_files_dir)) temp.mkdir(saved_files_dir);
    temp.cd(saved_files_dir);
    return temp;
}

void PJobRunnerService::user_file_used(QString name){
    m_user_files[name] = QDateTime::currentDateTime();
}

void PJobRunnerService::clean_old_user_files(){
    QMutexLocker locker(&m_user_files_mutex);
    QStringList deleted;
    foreach(QString filename, m_user_files.keys()){
        if(m_user_files[filename].daysTo(QDateTime::currentDateTime()) > 0){
            QDir temp = saved_user_files_dir();
            temp.remove(filename);
            deleted.append(filename);
        }
    }

    foreach(QString filename, deleted){
        m_user_files.remove(filename);
    }
}

bool PJobRunnerService::user_file_exists(QString name){
    return m_user_files.contains(name);
}

void PJobRunnerService::save_user_file(QString name, const QByteArray& data){
    QDir temp = saved_user_files_dir();
    QFile file(temp.absoluteFilePath(name));
    if(! file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QString message = QString("Error! Could not write to file %1!").arg(temp.absoluteFilePath(name));
        log(message, PJobRunnerService::Error);
        throw(message);
    }

    file.write(data);
    file.close();
    user_file_used(name);
}

QByteArray PJobRunnerService::read_user_file(QString name){
    if(! user_file_exists(name)) throw QString("Can't open user file \"%1\"! No such file.").arg(name);
    QDir temp = saved_user_files_dir();
    QString file_path = temp.absoluteFilePath(name);
    if(! temp.exists(name)){
        QString message = QString("Error! File \"%1\" is in list of user files but does not exist!").arg(file_path);
        log(message);
        m_user_files.remove(name);
        throw message;
    }
    QFile file(file_path);
    if(! file.open(QIODevice::ReadOnly)){
        QString message = QString("Error! Can't open file \"%1\"!").arg(file_path);
        log(message);
        m_user_files.remove(name);
        throw message;
    }
    user_file_used(name);
    return file.readAll();
}
