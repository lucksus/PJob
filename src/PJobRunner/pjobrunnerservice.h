#ifndef PJOBRUNNERSERVICE_H
#define PJOBRUNNERSERVICE_H
#include <list>
#include "qtservice.h"
#include <QMutex>
#include <QTimer>
#include <QtNetwork/QHostAddress>

using namespace std;
class Session;
class ProcessCounter;
class PJobRunnerService : public QtService<QCoreApplication>
{
friend class ProcessCounter;
public:
    PJobRunnerService(int argc, char** argv);

    unsigned int max_process_count();
    void set_max_process_count(unsigned int);
    unsigned int running_processes();
    unsigned int number_queue_entries_for_peer(QHostAddress);
    void enqueue(Session*);

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

private:
    //! Number of allowed simultaniously running processes.
    unsigned int m_max_process_count;
    list<Session*> m_queue;
    QMutex m_mutex;
    QTimer m_timer;

private slots:
    void dispatch();
};

class ProcessCounter{
public:
    ProcessCounter(){
        PJobRunnerService* service = dynamic_cast<PJobRunnerService*>(QtServiceBase::instance());
        QMutexLocker l(service->m_mutex);
        service->m_max_process_count++;
    }
    ~ProcessCounter(){
        PJobRunnerService* service = dynamic_cast<PJobRunnerService*>(QtServiceBase::instance());
        QMutexLocker l(service->m_mutex);
        service->m_max_process_count--;
    }

    static bool
};

#endif // PJOBRUNNERSERVICE_H
