#ifndef JOBOUTPUTWIDGET_H
#define JOBOUTPUTWIDGET_H

#include <QWidget>

namespace Ui {
    class JobOutputWidget;
}
class Job;
class JobOutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JobOutputWidget(Job* job, QWidget *parent = 0);
    ~JobOutputWidget();

    Job* job();

private slots:
    void std_out(QString);
    void err_out(QString);
    void connection_debug(QString);

private:
    Ui::JobOutputWidget *ui;
    Job* m_job;
};

#endif // JOBOUTPUTWIDGET_H
