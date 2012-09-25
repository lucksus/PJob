#ifndef PJOBRUNNERSERVICE_H
#define PJOBRUNNERSERVICE_H
#include "qtservice.h"
#include "ticketdispatcher.h"
#include <QFile>
#include <QDir>
#include <QDateTime>

class PJobRunnerService;
class Timer : QObject{
Q_OBJECT
public:
    Timer(PJobRunnerService*);
private slots:
    void timeout();
private:
    PJobRunnerService* m_service;
    QTimer m_timer;
};


using namespace std;
class Session;
class PJobRunnerService : public QtService<QCoreApplication>
{
friend class ProcessCounter;
friend class Timer;
public:
    PJobRunnerService(int argc, char** argv);
    TicketDispatcher* ticket_dispatcher();
    void log(QString message,const MessageType& type = Information);
    static PJobRunnerService* instance();
    void save_user_file(QString name, const QByteArray& data);
    QByteArray read_user_file(QString name);
    void user_file_used(QString name);
    bool user_file_exists(QString name);

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

private slots:


private:
    TicketDispatcher* m_ticket_dispatcher;
    QFile m_log_file;
    QMap<QString, QDateTime> m_user_files;
    QMutex m_user_files_mutex;
    QMutex m_mutex_log_file;

    static QDir saved_user_files_dir();
    void clean_old_user_files();

    Timer m_timer;
};

#endif // PJOBRUNNERSERVICE_H
