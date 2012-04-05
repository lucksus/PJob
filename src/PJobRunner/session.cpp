#include "session.h"
#include <iostream>
#include <QtCore/QProcess>
#include <QtCore/QFile>
#include <QTcpSocket>

Session::Session(QTcpSocket* socket) : m_pjob_file(0), m_script_engine(0), m_wants_shutdown(false), m_socket(socket)
{
}


Session& Session::global_instance(){
    static Session c;
    return c;
}

ScriptEngine& Session::script_engine(){
    if(m_script_engine == 0) m_script_engine = new ScriptEngine(this);
    return *m_script_engine;
}

bool Session::wants_shutdown(){
    return m_wants_shutdown;
}

void Session::open_local_pjob_file(QString filename){
    if(m_pjob_file) delete m_pjob_file;
    m_pjob_file = new PJobFile(filename);
    output(QString("File %1 opened!").arg(filename));
    m_application = m_pjob_file->defaultApplication();
    foreach(PJobFileParameterDefinition d, m_pjob_file->parameterDefinitions()){
        m_parameters[d.name()] = d.defaultValue();
    }
}

void Session::receive_pjob_file(QString base64string){

}

void Session::set_temp_dir(QString path){
    m_temp_dir = path;
}

void Session::set_parameter(QString name, double value){
    m_parameters[name] = value;
}

void Session::set_application(QString app_name){
    m_application = app_name;
}

bool removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void Session::run_job(){
    PJobFileApplication app = m_pjob_file->applicationByName(m_application);
#ifdef Q_WS_WIN
    if(app.platform != PJobFileApplication::Win32  && app.platform != PJobFileApplication::Win64){
        output(QString("Can't run %1. No Application not build for Windows!").arg(app.name));
        return;
    }
#endif
#ifdef Q_WS_MAC
    if(app.platform != PJobFileApplication::MacOSX){
        output(QString("Can't run %1. No Application not build for MacOSX!").arg(app.name));
        return;
    }
#endif
#ifdef Q_WS_X11
    if(app.platform != PJobFileApplication::Linux){
        output(QString("Can't run %1. No Application not build for Unix-like Systems!").arg(app.name));
        return;
    }
#endif

    QString temp_dir = QFileInfo(m_temp_dir).absoluteFilePath();

    output(QString("Clearing temporary directory %1").arg(temp_dir));
    removeDir(temp_dir);

    QString resources_directory = QString("%1/Resources/").arg(temp_dir);
    m_pjob_file->export_application(app.name, temp_dir);
    m_pjob_file->export_resources(temp_dir);
    QString executable = temp_dir + "/" + app.name + "/" + app.executable;
    QFile::setPermissions(executable, QFile::ExeUser);
    QProcess process;
    process.setWorkingDirectory(resources_directory);
    output(QString("Starting process: %1").arg(executable));
    output(QString("With arguments:"));
    foreach(QString arg, create_commandline_arguments_for_app(app)){
        output(arg);
    }

    process.start(executable, create_commandline_arguments_for_app(app));
    process.waitForStarted(-1);
    output("-------------------");
    output("Process std output:");
    output("-------------------");
    do{
        output(process.readAllStandardOutput());
    }while(!process.waitForFinished(100));

    switch(process.exitStatus()){
    case QProcess::NormalExit:
        output("Process exited normally.");
        m_pjob_file->import_run_directory(resources_directory);
        output("Created files imported into run directory.");
        break;
    case QProcess::CrashExit:
        output("Process crashed!");
        switch(process.error()){
        case QProcess::FailedToStart:
            output("Process failed to start!");
            break;
        case QProcess::Crashed:
            output("Process crashed some time after starting successfully!");
            break;
        default:
            break;
        }
        output("---------------------");
        output("Process error output:");
        output("---------------------");
        output(process.readAllStandardError());
    }
    m_pjob_file->save();
}

QStringList Session::create_commandline_arguments_for_app(const PJobFileApplication& app){
    QStringList params;
    QMapIterator<QString, double> it(m_parameters);
    while(it.hasNext()){
        it.next();
        QString arguments = app.parameter_pattern;
        arguments.replace(QString("<param>"), it.key());
        arguments.replace(QString("<value>"), QString::number(it.value()));
        params.append(arguments);
    }
    QStringList result = app.arguments.split(" ");
    int params_index = result.indexOf("<parameters>");
    result.removeAt(params_index);
    while(!params.empty())
        result.insert(params_index, params.takeLast());
    return result;
}



void Session::exit(){
    m_wants_shutdown = true;
}

QStringList Session::run_directories(){
    return m_pjob_file->runDirectoryEntries();
}

void Session::output(const QString& msg){
    if(m_socket) m_socket->write((msg + "\n").toAscii());
    else std::cout << msg.toStdString() << std::endl;
}
