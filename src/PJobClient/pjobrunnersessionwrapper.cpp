#include "pjobrunnersessionwrapper.h"
#include <QtCore/QRegExp>
#include <iostream>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

unsigned int s_standard_timeout = 20000;

PJobRunnerSessionWrapper::PJobRunnerSessionWrapper(QHostAddress hostname, long timeout)
    : m_peer(hostname)
{
    m_valid = false;
    m_socket.connectToHost(hostname, 23023);
    if(!m_socket.waitForConnected(timeout)) return;
    send("hello\n");
    if(!m_socket.waitForBytesWritten(s_standard_timeout)) return;
    if(!m_socket.waitForReadyRead(s_standard_timeout)) return;
    QString hello_string;
    unsigned int i=0;
    do{
        hello_string.append(m_socket.readAll());
        i++;
    }while((m_socket.state() == QAbstractSocket::ConnectedState) && m_socket.waitForReadyRead(1000) && (hello_string.size() < 1024) && (i < 10));
    received(hello_string);

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
        send("exit()\n");
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
    send("prepare_push_connection()\n");
    if(!m_socket.waitForReadyRead(s_standard_timeout))throw LostConnectionException(m_socket.peerName().toStdString(), "waiting for prepare_push_connection() reply.");
    QString line = m_socket.readLine();
    received(line);
    bool ok;
    quint32 port = line.toInt(&ok);
    if(! ok) return false;

    emit debug_out(QString("Opening data connection to port %1... ").arg(port));
    QTcpSocket push_connection;
    push_connection.connectToHost(m_socket.peerAddress(), port);
    if(!push_connection.waitForConnected(s_standard_timeout))throw LostConnectionException(push_connection.peerName().toStdString(), QString("Could not connect to port %1 for uploading.").arg(port).toStdString());

    qint64 bytes_send = 0;
    qint64 all_bytes = content.size();
    const char* data = content.data();
    qint64 transfer_unit_size = 1024*1024;

    while((bytes_send != all_bytes) && (push_connection.state() == QTcpSocket::ConnectedState)){
        push_connection.flush();
        qint64 bytes_written = push_connection.write(data, std::min(transfer_unit_size,all_bytes-bytes_send));
        data += bytes_written;
        bytes_send += bytes_written;
        push_connection.waitForBytesWritten(500);
        emit upload_progress(bytes_send * 100 / all_bytes);
    }
    if(bytes_send != all_bytes){
        emit debug_out(QString("Uploading pjobfile failed! Only %1 bytes out of %1 uploaded.").arg(bytes_send).arg(all_bytes));
        return false;
    }
    push_connection.close();
    push_connection.waitForDisconnected(s_standard_timeout);
    emit debug_out(QString("%1 bytes uploaded!").arg(content.size()));
    return true;
}

bool PJobRunnerSessionWrapper::download_results(QByteArray& data){
    send("prepare_pull_connection_for_results()\n");
    if(!m_socket.waitForReadyRead(s_standard_timeout))throw LostConnectionException(m_socket.peerName().toStdString(), "waiting for prepare_pull_connection_for_results() reply.");
    QString line = m_socket.readLine();
    received(line);
    QString all = m_socket.readAll();
    received(all);
    bool ok;
    quint32 port = line.toInt(&ok);
    if(! ok) return false;
    emit debug_out(QString("Pulling from port %1...").arg(port));
    QTcpSocket pull_connection;
    pull_connection.connectToHost(m_socket.peerAddress(), port);
    if(!pull_connection.waitForConnected(s_standard_timeout)) throw LostConnectionException(pull_connection.peerName().toStdString(), QString("Could not connect to port %1 for downloading.").arg(port).toStdString());;
    if(!pull_connection.waitForReadyRead(s_standard_timeout)) throw LostConnectionException(pull_connection.peerName().toStdString(), QString("Got nothing when waiting for data to download from port %1.").arg(port).toStdString());;

    while(true){
        if(pull_connection.state() == QTcpSocket::UnconnectedState){
            while(pull_connection.bytesAvailable())
                data.append(pull_connection.readAll());
            break;
        }
        if(pull_connection.waitForReadyRead(100)){
            data.append(pull_connection.readAll());
        }
    }
    return true;
}

bool PJobRunnerSessionWrapper::set_parameter(const QString& name, const double& value){
    send(QString("set_parameter(\"%1\",%2);\n").arg(name).arg(value));
    if(!m_socket.waitForReadyRead(s_standard_timeout))throw LostConnectionException(m_socket.peerName().toStdString(), QString("waiting for set_parameter() reply.").toStdString());
    return true;
}

bool PJobRunnerSessionWrapper::open_pjob_from_uploaded_data(){
    QString line;
    unsigned int i=0;
    do{
        if(i>=10){
            emit debug_out("Failed to open PJob file after 10 retries. Giving up...");
            return false;
        }
        if(i>0){
            QWaitCondition sleep;
            QMutex mutex;
            sleep.wait(&mutex, 500*(rand()%10));
        }
        if(line.isEmpty() || line.contains("Can't open pjob file!")){
            send("open_pjob_from_received_data()\n");
        }
        line = m_socket.readLine();
        if(line.isEmpty()){
            m_socket.waitForReadyRead(s_standard_timeout);
        }else{
            received(line);
        }
        i++;
    }while(!line.contains("pjob file opened from received data.") && m_socket.state() == QTcpSocket::ConnectedState);
    return true;
}

bool PJobRunnerSessionWrapper::run_job(){
    send("run_job()\n");
    if(!m_socket.waitForReadyRead(s_standard_timeout)) LostConnectionException(m_socket.peerName().toStdString(), "waiting for run_job() reply.");

    QByteArray buffer;
    do{
        buffer.append(m_socket.readAll());
        foreach(QByteArray line, buffer.split('\n')){
            received(line);
            if(line.contains("Starting process:"))
                return true;
            if(line.contains("Starting process:"))
                return true;
            if(line.contains("Can't"))
                return false;
            if(line.contains("ERROR!"))
                return false;
            if(line.contains("Process could not be started!"))
                return false;
        }
    }while(m_socket.waitForReadyRead(s_standard_timeout*5) && m_socket.state() == QTcpSocket::ConnectedState);
    return false;
}

bool PJobRunnerSessionWrapper::wait_for_job_finished(){
    bool ok=false;
    bool want_exit=false;
    while(!want_exit && m_socket.state() == QTcpSocket::ConnectedState){
        QString line = m_socket.readAll();
        if(line.isEmpty()){
            m_socket.waitForReadyRead(200);
            continue;
        }

        while(line.endsWith('\n') || line.endsWith('\r')) line.chop(1);
        received(line);
        emit job_std_out(line);
        if(line.contains("Process exited normally.")){ ok = true; want_exit = true; }
        if(line.contains("Process crashed!")) want_exit = true;
        if(line.contains("ERROR!")) want_exit = true;
        if(line.contains("Process could not be started!")) want_exit = true;
    }
    while(m_socket.waitForReadyRead(s_standard_timeout/60)){
        received(m_socket.readAll());
    }
    return ok;
}

QHostAddress PJobRunnerSessionWrapper::peer(){
    return m_peer;
}

bool PJobRunnerSessionWrapper::enqueue(){
    send("enqueue();\n");
    if(!m_socket.waitForReadyRead(s_standard_timeout)) return false;
    QString line = m_socket.readAll();
    received(line);
    if(line.contains("Successfully added to queue.")) return true;
    else return false;
}

bool PJobRunnerSessionWrapper::wait_till_its_your_turn(){
    QString buffer;
    while(m_socket.isReadable() && m_socket.state() == QAbstractSocket::ConnectedState){
        if(m_socket.waitForReadyRead(5000)){
            QByteArray read = m_socket.readAll();
            buffer.append(read);
            received(read);
            if(buffer.contains("It's your turn now! Go!")) return true;
        }
    }
    return false;
}

int PJobRunnerSessionWrapper::max_process_count(){
    send("max_process_count();\n");
    if(!m_socket.waitForReadyRead(s_standard_timeout)) throw LostConnectionException(m_socket.peerName().toStdString(), "waiting for max_process_count() reply.");
    bool ok;
    QString line = m_socket.readAll();
    received(line);
    int value = line.toInt(&ok);
    if(ok) return value;
    else throw QString("connection problem");
}

int PJobRunnerSessionWrapper::process_count(){
    send("process_count();\n");
    if(!m_socket.waitForReadyRead(s_standard_timeout)) throw LostConnectionException(m_socket.peerName().toStdString(), "waiting for process_count() reply.");
    bool ok;
    QString line = m_socket.readAll();
    received(line);
    int value = line.toInt(&ok);
    if(ok) return value;
    else throw QString("connection problem");
}

void PJobRunnerSessionWrapper::set_debug(bool b){
    m_debug_mode = b;
}

void PJobRunnerSessionWrapper::send(QString data){
    m_socket.write(data.toUtf8());
    if(!data.endsWith("\n")) data.append("\n");
    if(m_debug_mode) emit debug_out(QString(">>> %1").arg(data).arg(m_peer.toString()));
}

void PJobRunnerSessionWrapper::received(QString data){
    if(!data.endsWith("\n")) data.append("\n");
    if(m_debug_mode) emit debug_out(QString("%1").arg(data).arg(m_peer.toString()));
}

bool PJobRunnerSessionWrapper::open_pjob_from_user_file(QString name){
    send(QString("open_pjob_from_saved_file(\"%1\");\n").arg(name));
    if(!m_socket.waitForReadyRead(s_standard_timeout)) throw LostConnectionException(m_socket.peerName().toStdString(), "waiting for open_pjob_from_saved_file() reply.");
    QString line = m_socket.readAll();
    received(line);
    return line.contains("File found. Opening PJob");
}

bool PJobRunnerSessionWrapper::save_user_file(QString name){
    send(QString("save_received_data(\"%1\")\n").arg(name));
    if(!m_socket.waitForReadyRead(500)) throw LostConnectionException(m_socket.peerName().toStdString(), "waiting for save_received_data() reply.");
    QString line = m_socket.readAll();
    received(line);
    return line.contains("Received data successfully saved");
}
