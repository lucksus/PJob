#include "JobOutputWidget.h"
#include "ui_JobOutputWidget.h"
#include "Job.h"

JobOutputWidget::JobOutputWidget(Job* job, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JobOutputWidget),
    m_job(job)
{
    ui->setupUi(this);
    connect(job, SIGNAL(std_out(QString)), this, SLOT(std_out(QString)), Qt::QueuedConnection);
    connect(job, SIGNAL(err_out(QString)), this, SLOT(err_out(QString)), Qt::QueuedConnection);
    connect(job, SIGNAL(connection_debug(QString)), this, SLOT(connection_debug(QString)), Qt::QueuedConnection);
    ui->stdOut->setText(job->std_out());
    ui->errOut->setText(job->err_out());
    ui->connectionDebug->setText(job->connection_debug());
    setWindowTitle(QString("Job output: %1").arg(job->description()));
    QStringList header;
    header.append("Parameter");
    header.append("Value");
    ui->parametersTableWidget->setHorizontalHeaderLabels(header);
    ui->errOut->setTextColor(QColor(255,0,0));
    ui->connectionDebug->setTextColor(QColor(0,0,255));
}

JobOutputWidget::~JobOutputWidget()
{
    delete ui;
}

void JobOutputWidget::std_out(QString s){
    ui->stdOut->append(s);
}

void JobOutputWidget::err_out(QString s){
    ui->errOut->append(s);
}

void JobOutputWidget::connection_debug(QString s){
    ui->connectionDebug->append(s);
}

Job* JobOutputWidget::job(){
    return m_job;
}
