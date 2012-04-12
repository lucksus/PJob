#include <QtCore/QCoreApplication>
#include <iostream>
#include <QtCore/QFile>
#include <QTcpSocket>
#include <algorithm>
#include "PJobFile.h"

void usage(){
    std::cout << "Usage: PJobClient <host> <pjob file>" << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    if(argc != 3) usage();

    //QCoreApplication a(argc, argv);
    QString file_path = argv[2];

    QFile file(argv[2]);
    if(!file.open(QIODevice::ReadOnly)){
        std::cout << "Could not open " << argv[2] << "!" << std::endl;
        usage();
    }

    QTcpSocket connection;
    connection.connectToHost(argv[1], 23023);
    if(!connection.waitForConnected(5000)){
        std::cout << "Could not connect to PJobRunner on host " << argv[1] << "!" << std::endl;
        usage();
    }

    std::cout << "Connection established!" << std::endl;

    connection.write("prepare_push_connection()\n");
    while(!connection.waitForReadyRead()) ;
    QString line = connection.readLine();
    quint32 port = line.toInt();

    std::cout << "Opening data connection to port " << port << "... ";
    QTcpSocket push_connection;
    push_connection.connectToHost(argv[1], port);
    if(!push_connection.waitForConnected(10000))
        exit(0);
    std::cout << " done!" << std::endl;

    QByteArray content = file.readAll();
    file.close();
    qint64 bytes_send = 0;
    qint64 all_bytes = content.size();
    char* data = content.data();
    qint64 transfer_unit_size = 1024;

    while(bytes_send != all_bytes){
        qint64 bytes_written = push_connection.write(data, std::min(transfer_unit_size,all_bytes-bytes_send));
        data += bytes_written;
        bytes_send += bytes_written;
        push_connection.flush();
        push_connection.waitForBytesWritten();
        std::cout << "\r" << bytes_send * 100 / all_bytes << "% done...              ";
        //std::cout << bytes_send << " bytes send." << std::endl;
    }
    std::cout << std::endl;
    push_connection.close();
    push_connection.waitForDisconnected(10000);
    std::cout << content.size() << " bytes uploaded!" << std::endl;

    std::cout << "Running job..." << std::endl;

    connection.write("open_pjob_from_received_data()\n");
    if(!connection.waitForReadyRead(10000))exit(0);
    connection.write("run_job()\n");
    bool ok=false;
    while(!ok){
        if(!connection.waitForReadyRead(10)) continue;
        QString line = connection.readAll();
        if(line.contains("undefined")) ok = true;
        std::cout << line.toStdString() << std::endl;
    }

    std::cout << "Pulling results...";
    connection.write("prepare_pull_connection_for_results()\n");
    if(!connection.waitForReadyRead(10000)) exit(0);
    line = connection.readLine();
    QString all = connection.readAll();
    port = line.toInt();
    std::cout << " from port " << port << "...";
    QTcpSocket pull_connection;
    pull_connection.connectToHost(argv[1], port);
    if(!pull_connection.waitForConnected(10000)) exit(0);
    if(!pull_connection.waitForReadyRead(10000)) exit(0);
    QByteArray results;

    while(true){
        if(pull_connection.state() == QTcpSocket::UnconnectedState){
            while(pull_connection.bytesAvailable())
                results.append(pull_connection.readAll());
            break;
        }
        if(pull_connection.waitForReadyRead(10)){
            results.append(pull_connection.readAll());
        }
    }

    std::cout << " done!" << std::endl;

    std::cout << "Saving results to local PJob file...";

    PJobFile pjob_file(file_path);
    pjob_file.add_raw_files(results);
    pjob_file.save();
    std::cout << " done!" << std::endl;



    return 0;
}
