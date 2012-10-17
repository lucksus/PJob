#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ResultModel.h"

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
    
private:
    Ui::MainWindow *ui;
    ResultModel m_result_model;
};

#endif // MAINWINDOW_H
