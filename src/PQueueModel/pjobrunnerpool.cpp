#include "pjobrunnerpool.h"
#include <assert.h>

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

QList<QHostAddress> PJobRunnerPool::known_pjob_runners(){
    return m_known_pjob_runners;
}

void PJobRunnerPool::start_search_local_network(){
    m_backup_list = m_known_pjob_runners;
    if(!m_scanner.isRunning())
        m_scanner.start();
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
    emit search_local_network_finished();
}

void PJobRunnerPool::scanner_is_probing(QHostAddress host){
    emit probing_host(host);
}

QString PJobRunnerPool::hostname(QHostAddress host){
    assert(m_info_sessions.contains(host));
    return m_info_sessions[host]->hostname();
}

QString PJobRunnerPool::platform(QHostAddress host){
    assert(m_info_sessions.contains(host));
    return m_info_sessions[host]->platform();
}
