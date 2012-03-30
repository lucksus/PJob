#include <QtCore/QCoreApplication>
#include "controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    for(int i=1;i<argc;i++){
        Controller::instance().output(argv[i]);
        Controller::instance().script_engine().evaluate(argv[i]);
    }

    char buffer[1024];

    while(!Controller::instance().wants_shutdown()){
        std::cin.getline(buffer,1024);
        Controller::instance().script_engine().evaluate(buffer);
    }
    std::cout << "Exiting..." << std::endl;
}
