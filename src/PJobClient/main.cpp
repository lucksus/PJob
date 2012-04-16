#include <QtCore/QCoreApplication>
#include <iostream>
#include <QtCore/QFile>
#include <QTcpSocket>
#include <algorithm>
#include "PJobFile.h"
#include "pjobrunnersessionwrapper.h"
#include <QtNetwork/QHostAddress>

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

    QHostAddress host(argv[1]);
    PJobRunnerSessionWrapper session(host);
    if(!session.is_valid()){
        std::cout << "Could not connect to PJobRunner on host " << argv[1] << "!" << std::endl;
        usage();
    }

    std::cout << "Connection established!" << std::endl;

    session.upload_pjobfile(file.readAll());

    std::cout << "Running job..." << std::endl;
    if(!session.run_job()){
        std::cout << "Job application crashed!" << std::endl << "Not pulling results..." << std::endl;
        exit(0);
    }else std::cout << "Running job done!" << std::endl;


    QByteArray results;
    std::cout << "Pulling results...";
    session.download_results(results);
    std::cout << " done!" << std::endl;

    std::cout << "Saving results to local PJob file...";
    PJobFile pjob_file(file_path);
    pjob_file.add_raw_files(results);
    pjob_file.save();
    std::cout << " done!" << std::endl;

    return 0;
}
