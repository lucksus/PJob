#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ResultLoaderAndSaver.h"
#include "Results.h"
#include "PlotWidget.h"
#include <QFileDialog>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pjob_file(0)
{
    ui->setupUi(this);
    ui->resultView->setModel(&m_result_model);
    ui->resultView->setSortingEnabled(true);
    m_plotWidget = new PlotWidget;
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui->visualizationBox->layout());
    layout->insertWidget(0,m_plotWidget);
    connect(ui->showInterpolant, SIGNAL(stateChanged(int)), m_plotWidget, SLOT(showInterpolant(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionImport_from_CSV_file_triggered(){
    QString file = QFileDialog::getOpenFileName(this, tr("Select file for CSV import"),"","*.txt");
    if(file.isEmpty()) return;
    ResultLoaderAndSaver importer;
    connect(&importer, SIGNAL(parsed_csv_line(QHash<QString,double>,QHash<QString,double>)), &Results::getInstance(), SLOT(add_values(QHash<QString,double>,QHash<QString,double>)));
    importer.importFromCSV(file);
}

void MainWindow::on_actionOpen_PJob_file_triggered(){
    QString file = QFileDialog::getOpenFileName(this, tr("Select PJOB file "),"","*.pjob");
    if(file.isEmpty()) return;
    set_pjob_file(file);
}

void MainWindow::set_pjob_file(QString file_path){
    if(m_pjob_file) delete m_pjob_file;
    m_pjob_file = new PJobFileLean(file_path);
    ui->fileLabel->setText(file_path);

    foreach(PJobFileLean::ParameterCombination param_comb, m_pjob_file->parameter_combinations()){
        QHash<QString,double> results;
        foreach(PJobResultFile result_file, m_pjob_file->result_definitions()){
            if(result_file.type() != PJobResultFile::SINGLE_VALUE) continue;
            QByteArray *file = m_pjob_file->read_result_file(param_comb, result_file.filename());
            std::stringstream ss(file->constData());
            double d;
            ss >> d;
            if(d == 0)
                throw QString("NULL!!!");
            results[result_file.results().front().name()] = d;
            delete file;
        }
        QHash<QString,double> parameter_combinaton_hash;
        foreach(PJobFileParameter param, param_comb){
            parameter_combinaton_hash[param.name()] = param.value();
        }

        Results::getInstance().add_values(parameter_combinaton_hash, results);
    }
}
