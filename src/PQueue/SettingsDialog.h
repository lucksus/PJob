#pragma once
#include <QtGui/QDialog>
#include "ui_SettingsDialog.h"

class SettingsDialog : public QDialog
{
Q_OBJECT
public:
	SettingsDialog(void);
private slots:
	void on_accepted();
	void on_photossDirectoryBrowseButton_clicked();
	void on_condorDirectoryBrowseButton_clicked();
	void on_workingDirectoryBrowseButton_clicked();

private:
	Ui::SettingsDialog ui;
};
