#ifndef TICKETDISPATCHER_H
#define TICKETDISPATCHER_H
#include <list>
#include <set>
#include <QMutex>
#include <QTimer>
#include <QtNetwork/QHostAddress>
#include <QObject>
using namespace std;

class Session;
class TicketDispatcher : public QObject{
Q_OBJECT
public:
    TicketDispatcher();
    unsigned int max_process_count();
    void set_max_process_count(unsigned int);
    unsigned int running_processes();
    unsigned int number_queue_entries_for_peer(QHostAddress);
    //! Returns true if session was added to queue, false if session was already in queue
    bool enqueue(Session*);
    void finished_turn(Session*);
    void remove_session(Session*);

private:
    //! Number of allowed simultaniously running processes.
    unsigned int m_max_process_count;
    list<Session*> m_queue;
    set<Session*> m_active_sessions;
    QMutex m_mutex;
    QTimer m_timer;

private slots:
    void dispatch();
};

#endif // TICKETDISPATCHER_H
