#include <QtCore/QCoreApplication>
#include "session.h"
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include "tcpserver.h"
#include <QtCore/QDir>
#include "pjobrunnerservice.h"

int main(int argc, char *argv[])
{
    PJobRunnerService service(argc,argv);
    Session::global_instance().set_temp_dir(QDir::temp().absolutePath() + "/PJobRunner_temp");
    return service.exec();


/*
    if(argc == 2){
        if(QString(argv[1]) == "server"){
            TcpServer::instance().startup();
            return 0;
        }
        QFileInfo file_info(argv[1]);
        if(file_info.exists() && file_info.isFile()){
            std::cout << QString("Interpreting file %1...").arg(file_info.absolutePath()).toStdString() << std::endl;
            QFile file(file_info.absoluteFilePath());
            file.open(QFile::ReadOnly);
            Session::global_instance().script_engine().evaluate(file.readAll());

            return 0;
        }
    }

    if(argc > 1) std::cout << "Interpreting command line arguments..." << std::endl;
    for(int i=1;i<argc;i++){
        Session::global_instance().output(argv[i]);
        Session::global_instance().script_engine().evaluate(argv[i]);
    }

    char buffer[1024];

    std::cout << "Entering interactive mode..." << std::endl;
    while(!Session::global_instance().wants_shutdown()){

        std::cin.getline(buffer,1024);
        Session::global_instance().script_engine().evaluate(buffer);
    }
    std::cout << "Exiting..." << std::endl;

    return 0;
    */
}
