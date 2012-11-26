#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ResultModel.h"
#include "PJobFileLean.h"

class PlotWidget;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_actionImport_from_CSV_file_triggered();
    void on_actionOpen_PJob_file_triggered();
    void set_pjob_file(QString file_path);

    
private:
    Ui::MainWindow *ui;
    ResultModel m_result_model;
    PlotWidget* m_plotWidget;
    PJobFileLean* m_pjob_file;
};

#endif // MAINWINDOW_H
