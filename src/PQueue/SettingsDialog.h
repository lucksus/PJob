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

private:
	Ui::SettingsDialog ui;
};
