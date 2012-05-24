#ifndef PJOBRUNNERSERVICE_H
#define PJOBRUNNERSERVICE_H
#include "qtservice.h"
#include "ticketdispatcher.h"

using namespace std;
class Session;
class PJobRunnerService : public QtService<QCoreApplication>
{
friend class ProcessCounter;
public:
    PJobRunnerService(int argc, char** argv);
    TicketDispatcher* ticket_dispatcher();

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

private:
    TicketDispatcher* m_ticket_dispatcher;
};

#endif // PJOBRUNNERSERVICE_H
