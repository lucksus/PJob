#include "sessionthread.h"
#include "session.h"
#include <QHostAddress>
#include <QtServiceBase>
#include "pjobrunnerservice.h"
#include <exception>

SessionThread::SessionThread(int socket_descriptor) :
    m_socket_descriptor(socket_descriptor)
{
}

void SessionThread::run(){
    QTcpSocket connection;
    if(!connection.setSocketDescriptor(m_socket_descriptor)){
        PJobRunnerService::instance()->log(QString("Could not set socket descriptor: %1").arg(connection.errorString()));
        return;
    }
    PJobRunnerService::instance()->log(QString("Accepted connection from %1. Starting new session..").arg(connection.peerAddress().toString()));
    Session* session = new Session(&connection);
    while(connection.state() == QAbstractSocket::ConnectedState){
        while(connection.state() == QAbstractSocket::ConnectedState && !connection.waitForReadyRead(10)) session->update();
        QByteArray line = connection.readAll();
        bool error = false;
        try{
            if(!line.isEmpty()) session->script_engine().evaluate(line);
        }catch(QString str){
            session->output(str);
            PJobRunnerService::instance()->log(str, PJobRunnerService::Error);
        }catch(PJobFileError e){
            session->output(e.msg());
            PJobRunnerService::instance()->log(e.msg(), PJobRunnerService::Error);
        }catch(std::exception e){
            session->output(e.what());
            PJobRunnerService::instance()->log(e.what(), PJobRunnerService::Error);
        }catch(...){
            QString message = "Unhandled exception caught! Closing Session...";
            session->output(message);
            PJobRunnerService::instance()->log(message, PJobRunnerService::Error);
            error = true;
        }

        if(session->wants_shutdown() || error)
            connection.disconnectFromHost();
    }
    delete session;
    PJobRunnerService::instance()->log(QString("Connection to %1 closed.").arg(connection.peerAddress().toString()));
}
