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
    ui->textEdit->setText(job->std_out());
    setWindowTitle(QString("Job output: %1").arg(job->description()));
}

JobOutputWidget::~JobOutputWidget()
{
    delete ui;
}

void JobOutputWidget::std_out(QString s){
    ui->textEdit->append(s);
}

void JobOutputWidget::err_out(QString s){
    ui->textEdit->append(QString("ERROR: %1").arg(s));
}

Job* JobOutputWidget::job(){
    return m_job;
}
