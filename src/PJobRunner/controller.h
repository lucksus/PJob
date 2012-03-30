#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include "PJobFile.h"

class Controller : public QObject
{
Q_OBJECT
public:
    static Controller& instance();
public slots:
    void open_local_pjob_file(QString filename);
    void receive_pjob_file(QString base64string);
    void set_temp_dir(QString path);
    void set_parameter(QString name, double value);
    void set_application(QString app_name);
    void run_job();

    QStringList run_directories();



private:
    Controller();
    PJobFile *m_pjob_file;
    QString m_temp_dir;
    QMap<QString,double> m_parameters;
    QString m_application;
};

#endif // CONTROLLER_H

