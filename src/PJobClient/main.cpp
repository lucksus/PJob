#include <QtCore/QCoreApplication>
#include <iostream>
#include <QtCore/QFile>
#include <QTcpSocket>
#include <algorithm>

void usage(){
    std::cout << "Usage: PJobClient <name of host> <port> <path to pjob file>" << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    if(argc != 4) usage();

    //QCoreApplication a(argc, argv);

    QFile pjob_file(argv[3]);
    if(!pjob_file.open(QIODevice::ReadOnly)){
        std::cout << "Could not open " << argv[2] << "!" << std::endl;
        usage();
    }

    bool ok;
    int port = QString(argv[2]).toInt(&ok);
    if(!ok){
        std::cout << argv[2] << " is not a valid port number!" << std::cout;
        usage();
    }

    QTcpSocket connection;
    connection.connectToHost(argv[1], port);
    if(!connection.waitForConnected(5000)){
        std::cout << "Could not connect to port " << port << " on host " << argv[1] << "!" << std::endl;
        usage();
    }

    std::cout << "Connection established!" << std::endl;

    QByteArray content = pjob_file.readAll();
    qint64 bytes_send = 0;
    qint64 all_bytes = content.size();
    char* data = content.data();
    qint64 transfer_unit_size = 1024;

    while(bytes_send != all_bytes){
        qint64 bytes_written = connection.write(data, std::min(transfer_unit_size,all_bytes-bytes_send));
        data += bytes_written;
        bytes_send += bytes_written;
        connection.flush();
        connection.waitForBytesWritten();
        //std::cout << "\r" << bytes_send * 100 / all_bytes << "% done...              ";
        std::cout << bytes_send << " bytes send." << std::endl;
    }
    std::cout << std::endl;

    connection.close();

    std::cout << content.size() << " bytes uploaded!" << std::endl;
    return 0;
}
