#include "controller.h"
#include <iostream>

Controller::Controller() : m_pjob_file(0), m_script_engine(0), m_wants_shutdown(false)
{
}


Controller& Controller::instance(){
    static Controller c;
    return c;
}

ScriptEngine& Controller::script_engine(){
    if(m_script_engine == 0) m_script_engine = new ScriptEngine();
    return *m_script_engine;
}

bool Controller::wants_shutdown(){
    return m_wants_shutdown;
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

void Controller::exit(){
    m_wants_shutdown = true;
}

QStringList Controller::run_directories(){
    return m_pjob_file->runDirectoryEntries();
}

void Controller::output(const QString& msg){
    std::cout << msg.toStdString() << std::endl;
}
