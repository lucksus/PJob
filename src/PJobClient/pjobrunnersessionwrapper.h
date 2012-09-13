#ifndef PJOBRUNNERSESSIONWRAPPER_H
#define PJOBRUNNERSESSIONWRAPPER_H
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

class LostConnectionException{
public:
    LostConnectionException(std::string peer_name="", std::string m="") : m_message(m), m_peer_name(peer_name){}
    virtual const char* what() const throw(){
        std::string message = "Lost connection";
        if(!m_peer_name.empty()) message += std::string(" to ") + m_peer_name;
        if(!m_message.empty()) message += std::string(": ") + m_message;
        return message.c_str();
    }

private:
    std::string m_message, m_peer_name;
};

class PJobRunnerSessionWrapper : public QObject
{
Q_OBJECT
public:
    PJobRunnerSessionWrapper(QHostAddress hostname, long timeout = 10000);
    ~PJobRunnerSessionWrapper();

    bool is_valid();
    QString platform();
    QString version();
    QString hostname();

    bool upload_pjobfile(const QByteArray& data);
    bool download_results(QByteArray& data);
    bool set_parameter(const QString& name, const double& value);
    bool run_job();
    bool wait_for_job_finished();
    bool open_pjob_from_uploaded_data();
    bool open_pjob_from_user_file(QString name);
    bool save_user_file(QString name);

    QHostAddress peer();

    bool enqueue();
    bool wait_till_its_your_turn();

    int max_process_count();
    int process_count();

    void set_debug(bool);

signals:
    void job_std_out(QString);
    void job_error_out(QString);
    void debug_out(QString);

private:
    QTcpSocket m_socket;
    bool m_valid;
    QString m_platform, m_version, m_hostname;
    QHostAddress m_peer;
    bool m_debug_mode;

    void send(QString);
    void received(QString);
};

#endif // PJOBRUNNERSESSIONWRAPPER_H
