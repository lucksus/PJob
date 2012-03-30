#include "controller.h"

Controller::Controller()
{
}


Controller& Controller::instance(){
    static Controller c;
    return c;
}

void Controller::open_local_pjob_file(QString filename){
    if(m_pjob_file) delete m_pjob_file;
    m_pjob_file = new PJobFile(filename);
}

void Controller::receive_pjob_file(QString base64string){

}

void Controller::set_temp_dir(QString path){
    m_temp_dir = path;
}

void Controller::set_parameter(QString name, double value){
    m_parameters[name] = value;
}

void Controller::set_application(QString app_name){
    m_application = app_name;
}

void Controller::run_job(){

}

QStringList Controller::run_directories(){
    return m_pjob_file->runDirectoryEntries();
}
