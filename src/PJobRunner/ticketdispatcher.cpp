#include "ticketdispatcher.h"
#include "session.h"
#include <assert.h>
#include <QThread>
#include <boost/foreach.hpp>

TicketDispatcher::TicketDispatcher(){
    m_max_process_count = QThread::idealThreadCount();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(dispatch()));
    m_timer.start(5000);
}

unsigned int TicketDispatcher::max_process_count(){
    return m_max_process_count;
}

void TicketDispatcher::set_max_process_count(unsigned int count){
    m_max_process_count = count;
}

unsigned int TicketDispatcher::number_queue_entries_for_peer(QHostAddress address){
    unsigned int count = 0;
    BOOST_FOREACH(Session* s, m_queue){
        if(s->peer() == address) count++;
    }
    BOOST_FOREACH(Session* s, m_active_sessions){
        if(s->peer() == address) count++;
    }
    return count;
}

bool TicketDispatcher::enqueue(Session* s){
    BOOST_FOREACH(Session* it, m_queue){
        if(s == it) return false;
    }
    m_queue.push_back(s);
    return true;
}

void TicketDispatcher::finished_turn(Session* s){
    QMutexLocker l(&m_mutex);
    assert(m_active_sessions.count(s) == 1);
    m_active_sessions.erase(s);
}

void TicketDispatcher::dispatch(){
    QMutexLocker l(&m_mutex);
    while((m_active_sessions.size() < m_max_process_count) && !m_queue.empty()){
        Session* s = m_queue.front(); m_queue.pop_front();
        s->give_turn();
        m_active_sessions.insert(s);
    }
}

void TicketDispatcher::remove_session(Session* s){
    m_queue.remove(s);
    m_active_sessions.erase(s);
}
