#ifndef PJOBRUNNERSESSIONWRAPPER_H
#define PJOBRUNNERSESSIONWRAPPER_H
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

class PJobRunnerSessionWrapper : public QObject
{
Q_OBJECT
public:
    PJobRunnerSessionWrapper(QHostAddress hostname, long timeout = 10000);
    bool is_valid();
    QString platform();
    QString version();

    bool upload_pjobfile(const QByteArray& data);
    bool download_results(QByteArray& data);
    bool set_parameter(const QString& name, const double& value);
    bool run_job();
    bool wait_for_job_finished();

    QHostAddress peer();

signals:
    void job_std_out(QString);
    void job_error_out(QString);

private:
    QTcpSocket m_socket;
    bool m_valid;
    QString m_platform, m_version;
    QHostAddress m_peer;
};

#endif // PJOBRUNNERSESSIONWRAPPER_H
