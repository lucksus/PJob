#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include "PJobFile.h"
#include "scriptengine.h"

class Controller : public QObject
{
Q_OBJECT
public:
    static Controller& instance();
    ScriptEngine& script_engine();
    bool wants_shutdown();

public slots:
    void open_local_pjob_file(QString filename);
    void receive_pjob_file(QString base64string);
    void set_temp_dir(QString path);
    void set_parameter(QString name, double value);
    void set_application(QString app_name);
    void run_job();
    void exit();

    QStringList run_directories();


    void output(const QString& msg);

private:
    Controller();
    PJobFile *m_pjob_file;
    ScriptEngine *m_script_engine;
    QString m_temp_dir;
    QMap<QString,double> m_parameters;
    QString m_application;
    bool m_wants_shutdown;
};

#endif // CONTROLLER_H

