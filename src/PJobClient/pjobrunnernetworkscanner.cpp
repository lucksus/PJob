#include "pjobrunnernetworkscanner.h"
#include <QtNetwork>
#include "pjobrunnersessionwrapper.h"

PJobRunnerNetworkScanner::PJobRunnerNetworkScanner()
    : m_port(23023)
{
}

void PJobRunnerNetworkScanner::run(){
    foreach(const QNetworkInterface& interface, QNetworkInterface::allInterfaces()){
        foreach(const QNetworkAddressEntry& address_entry, interface.addressEntries()){
            quint32 netmask = address_entry.netmask().toIPv4Address();
            quint32 address_to_try = netmask+1;
            quint32 inv_netmask = ~netmask;
            while((address_to_try | inv_netmask) == inv_netmask){
                PJobRunnerSessionWrapper* session = new PJobRunnerSessionWrapper(QHostAddress(address_to_try));
                if(session->is_valid()) emit found_pjob_runner(session);
                else delete session;
                address_to_try++;
            }
        }
    }
    emit finished_scanning();
}
