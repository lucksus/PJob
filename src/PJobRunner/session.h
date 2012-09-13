#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QObject>
#include "PJobFile.h"
#include "scriptengine.h"
#include <QHostAddress>
#include <QTimer>

class QTcpSocket;
class DataReceiveConnection;
class DataPushConnection;
class Session : public QObject
{
Q_OBJECT
public:
    Session(QTcpSocket* socket = 0);
    ~Session();
    static Session& global_instance();
    ScriptEngine& script_engine();
    bool wants_shutdown();
    void set_temp_dir(QString path);
    QString hello();

    void give_turn();
    void finish_turn();
    QHostAddress peer();
    void update();

public slots:
    QString platform();
    void open_local_pjob_file(QString filename);
    quint32 prepare_push_connection();
    void open_pjob_from_received_data();
    void set_parameter(QString name, double value);
    void set_application(QString app_name);
    void run_job();
    void exit();
    const QByteArray& received_data();
    void write_received_data_to_file(QString);
    quint32 prepare_pull_connection_for_results();
    void enqueue();
    QStringList run_directories();
    unsigned int max_process_count();
    unsigned int process_count();
    void save_received_data(QString name);
    void open_pjob_from_saved_file(QString name);

    void output(const QString& msg);

private slots:
    void turn_timeout();
    void fetch_received_data_from_connection_server();

private:
    PJobFile *m_pjob_file;
    ScriptEngine *m_script_engine;
    QString m_temp_dir;
    QMap<QString,double> m_parameters;
    QString m_application;
    bool m_wants_shutdown;
    QTcpSocket* m_socket;
    QByteArray m_received_data;
    DataReceiveConnection* m_data_receive_connection;
    DataPushConnection* m_data_push_connection;

    QStringList create_commandline_arguments_for_app(const PJobFileApplication&);
    QList<PJobFileParameter> parameters_as_pjobfileparameters();

    QMutex m_mutex_received_data;


    bool m_has_turn, m_got_turn;
    bool m_has_running_process;
    QTimer m_turn_timeout;
    bool m_renew_turn;
    bool m_data_received;
};

#endif // CONTROLLER_H

