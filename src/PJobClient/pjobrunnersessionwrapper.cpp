#include "pjobrunnersessionwrapper.h"
#include <QtCore/QRegExp>
#include <iostream>

PJobRunnerSessionWrapper::PJobRunnerSessionWrapper(QHostAddress hostname, long timeout)
    : m_peer(hostname)
{
    m_valid = false;
    m_socket.connectToHost(hostname, 23023);
    if(!m_socket.waitForConnected(timeout)) return;
    m_socket.write("hello\n");
    if(!m_socket.waitForBytesWritten(10000)) return;
    if(!m_socket.waitForReadyRead(10000)) return;
    QString hello_string;
    do{
        hello_string.append(m_socket.readAll());
    }while(m_socket.waitForReadyRead(1000) && hello_string.size() < 1024);

    if(hello_string.isEmpty()) return;

    QRegExp reg_exp("This is ([^\\s]*) \\(.*\\) version ([^\\s]*) running on ([^\\s]*) \\(([^\\s]*)\\)");

    if(reg_exp.indexIn(hello_string) == -1) return;
    m_version = reg_exp.cap(2);
    m_platform = reg_exp.cap(4);
    m_hostname = reg_exp.cap(3);
    m_valid = true;
}

PJobRunnerSessionWrapper::~PJobRunnerSessionWrapper(){
    if(m_socket.state() == QAbstractSocket::ConnectedState){
        m_socket.write("exit()");
        m_socket.waitForBytesWritten(100);
        m_socket.close();
        m_socket.waitForDisconnected(1000);
    }
}

bool PJobRunnerSessionWrapper::is_valid(){
    return m_valid;
}

QString PJobRunnerSessionWrapper::platform(){
    return m_platform;
}

QString PJobRunnerSessionWrapper::version(){
    return m_version;
}

QString PJobRunnerSessionWrapper::hostname(){
    return m_hostname;
}

bool PJobRunnerSessionWrapper::upload_pjobfile(const QByteArray& content){
    m_socket.write("prepare_push_connection()\n");
    if(!m_socket.waitForReadyRead(10000))return false;
    QString line = m_socket.readLine();
    quint32 port = line.toInt();

    std::cout << "Opening data connection to port " << port << "... ";
    QTcpSocket push_connection;
    push_connection.connectToHost(m_socket.peerAddress(), port);
    if(!push_connection.waitForConnected(10000))return false;
    std::cout << " done!" << std::endl;

    qint64 bytes_send = 0;
    qint64 all_bytes = content.size();
    const char* data = content.data();
    qint64 transfer_unit_size = 1024;

    while(bytes_send != all_bytes){
        qint64 bytes_written = push_connection.write(data, std::min(transfer_unit_size,all_bytes-bytes_send));
        data += bytes_written;
        bytes_send += bytes_written;
        push_connection.flush();
        push_connection.waitForBytesWritten();
        std::cout << "\r" << bytes_send * 100 / all_bytes << "% done...              ";
    }
    std::cout << std::endl;
    push_connection.close();
    push_connection.waitForDisconnected(10000);
    std::cout << content.size() << " bytes uploaded!" << std::endl;
    return true;
}

bool PJobRunnerSessionWrapper::download_results(QByteArray& data){
    m_socket.write("prepare_pull_connection_for_results()\n");
    if(!m_socket.waitForReadyRead(10000)) exit(0);
    QString line = m_socket.readLine();
    QString all = m_socket.readAll();
    quint32 port = line.toInt();
    std::cout << " from port " << port << "...";
    QTcpSocket pull_connection;
    pull_connection.connectToHost(m_socket.peerAddress(), port);
    if(!pull_connection.waitForConnected(10000)) exit(0);
    if(!pull_connection.waitForReadyRead(10000)) exit(0);

    while(true){
        if(pull_connection.state() == QTcpSocket::UnconnectedState){
            while(pull_connection.bytesAvailable())
                data.append(pull_connection.readAll());
            break;
        }
        if(pull_connection.waitForReadyRead(10)){
            data.append(pull_connection.readAll());
        }
    }
    return true;
}

bool PJobRunnerSessionWrapper::set_parameter(const QString& name, const double& value){
    return false;
}

bool PJobRunnerSessionWrapper::run_job(){
    m_socket.write("open_pjob_from_received_data()\n");
    if(!m_socket.waitForReadyRead(10000))exit(0);
    m_socket.write("run_job()\n");
    bool ok=false;
    bool want_exit=false;
    while(!want_exit){
        if(!m_socket.waitForReadyRead(10)) continue;
        QString line = m_socket.readAll();
        if(line.contains("Process exited normally.")){ ok = true; want_exit = true; }
        if(line.contains("Process crashed!")) want_exit = true;
        std::cout << line.toStdString() << std::endl;
    }

    while(m_socket.waitForReadyRead(1000)) std::cout << QString(m_socket.readAll()).toStdString();
    std::cout << std::endl;

    return ok;
}

bool PJobRunnerSessionWrapper::wait_for_job_finished(){
    return false;
}

QHostAddress PJobRunnerSessionWrapper::peer(){
    return m_peer;
}
