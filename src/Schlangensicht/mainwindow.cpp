#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ResultLoaderAndSaver.h"
#include "Results.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->resultView->setModel(&m_result_model);
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
