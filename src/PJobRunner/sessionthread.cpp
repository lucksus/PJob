#include "sessionthread.h"
#include "session.h"
#include <QHostAddress>
#include <QtServiceBase>
#include "pjobrunnerservice.h"
#include <exception>
#ifdef Q_OS_UNIX
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

SessionThread::SessionThread(int socket_descriptor) :
    m_socket_descriptor(socket_descriptor)
{
#ifdef Q_OS_UNIX
    // We expect write failures to occur but we want to handle them where
    // the error occurs rather than in a SIGPIPE handler.
    signal(SIGPIPE, SIG_IGN);
    //from: http://stackoverflow.com/questions/108183/how-to-prevent-sigpipes-or-handle-them-properly
#endif
}

void SessionThread::run(){
    QTcpSocket connection;
    if(!connection.setSocketDescriptor(m_socket_descriptor)){
        PJobRunnerService::instance()->log(QString("Could not set socket descriptor: %1").arg(connection.errorString()));
        return;
    }
    PJobRunnerService::instance()->log(QString("Accepted connection from %1. Starting new session..").arg(connection.peerAddress().toString()));
    Session* session = new Session(&connection);
    QByteArray receive_buffer;
    while(connection.state() == QAbstractSocket::ConnectedState){
        while(connection.state() == QAbstractSocket::ConnectedState && !connection.waitForReadyRead(10)) session->update();
        bool error = false;
        receive_buffer.append(connection.readAll());
        QList<QByteArray> lines = receive_buffer.split('\n');
        if(lines.size() >= 2){
            foreach(QByteArray line, lines){
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
                    PJobRunnerService::instance()->log(message, PJobRunnerService::Error);
                    try{
                        session->output(message);
                    }catch(...){}
                    error = true;
                }
            }
            receive_buffer.clear();
        }

        if(session->wants_shutdown() || error)
            connection.disconnectFromHost();
    }
    delete session;
    PJobRunnerService::instance()->log(QString("Connection to %1 closed.").arg(connection.peerAddress().toString()));
}
