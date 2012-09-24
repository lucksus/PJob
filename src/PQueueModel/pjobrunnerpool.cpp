#include "pjobrunnerpool.h"
#include <assert.h>
#include <iostream>

PJobRunnerPool::PJobRunnerPool()
{
    connect(&m_scanner, SIGNAL(found_pjob_runner(PJobRunnerSessionWrapper*)), this, SLOT(found_pjob_runner(PJobRunnerSessionWrapper*)));
    connect(&m_scanner, SIGNAL(finished_scanning()), this, SLOT(search_finished()));
    connect(&m_scanner, SIGNAL(probing_host(QHostAddress)), this, SLOT(scanner_is_probing(QHostAddress)));
}

PJobRunnerPool& PJobRunnerPool::instance(){
    static PJobRunnerPool pool;
    return pool;
}

QList<QHostAddress> PJobRunnerPool::known_pjob_runners() const{
    return m_known_pjob_runners;
}

void PJobRunnerPool::start_search_network(const QNetworkInterface& i){
    m_backup_list = m_known_pjob_runners;
    if(!m_scanner.isRunning()){
        m_scanner.set_network_interface(i);
        emit network_scan_started();
        m_scanner.start();
    }
}

void PJobRunnerPool::stop_search_network(){
    m_scanner.stop_scan();
}

void PJobRunnerPool::found_pjob_runner(PJobRunnerSessionWrapper* session){
    QHostAddress new_peer = session->peer();
    if(!m_info_sessions.contains(new_peer)){
        m_info_sessions[new_peer] = session;
    }else delete session;
    if(!m_known_pjob_runners.contains(new_peer)){
        m_known_pjob_runners.append(new_peer);
        emit found_new_pjob_runner(new_peer);
    }
}

void PJobRunnerPool::search_finished(){
    foreach(QHostAddress host, m_backup_list){
        if(!m_known_pjob_runners.contains(host))
            emit lost_pjob_runner(host);
    }
    emit network_scan_finished();
}

void PJobRunnerPool::scanner_is_probing(QHostAddress host){
    emit probing_host(host);
}

QString PJobRunnerPool::hostname(QHostAddress host) const{
    assert(m_info_sessions.contains(host));
    return m_info_sessions[host]->hostname();
}

QString PJobRunnerPool::platform(QHostAddress host) const{
    assert(m_info_sessions.contains(host));
    return m_info_sessions[host]->platform();
}

unsigned int PJobRunnerPool::max_thread_count() const{
    unsigned int count = 0;
    foreach(QHostAddress host, m_known_pjob_runners){
        try{
            PJobRunnerSessionWrapper session(host);
            count += session.max_process_count();
        }catch(...){
            std::cout << "Ups!" << std::endl;
        }
    }
    return count;
}

bool PJobRunnerPool::is_scanning(){
    return m_scanner.isRunning();
}



unsigned int PJobRunnerPool::max_thread_count_for_host(QHostAddress host) const{
    QHash<QHostAddress, PJobRunnerSessionWrapper*>::const_iterator it = m_info_sessions.find(host);
    if(it == m_info_sessions.end()){
        try{
            PJobRunnerSessionWrapper session(host);
            return session.max_process_count();
        }catch(...){
            std::cout << "Ups2!" << std::endl;
        }
    }
    return it.value()->max_process_count();
}

unsigned int PJobRunnerPool::thread_count_for_host(QHostAddress host) const{
    unsigned int retries = 0;
    while(retries < 3){
        try{
            QHash<QHostAddress, PJobRunnerSessionWrapper*>::const_iterator it = m_info_sessions.find(host);
            if(it == m_info_sessions.end()){
                PJobRunnerSessionWrapper session(host);
                return session.process_count();
            }
            return it.value()->process_count();
        }catch(LostConnectionException e){
            retries++;
        }catch(QString s){
            retries++;
        }

    }
    return 0;
}

unsigned int PJobRunnerPool::thread_count() const{
    unsigned int count=0;
    foreach(QHostAddress host, known_pjob_runners()){
        count += thread_count_for_host(host);
    }
    return count;
}

void PJobRunnerPool::remove(QHostAddress host){
    m_known_pjob_runners.removeOne(host);
    m_info_sessions.remove(host);
}
