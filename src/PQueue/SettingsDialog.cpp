#include "SettingsDialog.h"
#include "Settings.h"
#include <QtGui/QFileDialog>

SettingsDialog::SettingsDialog(void)
{
	ui.setupUi(this);
	ui.photossDirectory->setText(Settings::getInstance().photossDirectory());
	ui.condorDirectory->setText(Settings::getInstance().condorDirectory());
	ui.workingDirectory->setText(Settings::getInstance().workingDirectory());
	ui.useCondor->setChecked(Settings::getInstance().submitMethod() == Settings::USE_CONDOR);
	ui.startLocalPhotoss->setChecked(Settings::getInstance().submitMethod() == Settings::START_LOCAL_PHOTOSS);
	ui.startPhotossWithoutMatlab->setChecked(Settings::getInstance().startPhotossWithoutMatlab());
	connect(this,SIGNAL(accepted()),this,SLOT(on_accepted()));
}


void SettingsDialog::on_accepted(){
	Settings::getInstance().setPhotossDirectory(ui.photossDirectory->text());
	Settings::getInstance().setCondorDirectory(ui.condorDirectory->text());
	Settings::getInstance().setWorkingDirectory(ui.workingDirectory->text());
	if(ui.useCondor->isChecked())
		Settings::getInstance().setSubmitMethod(Settings::USE_CONDOR);
	else
		Settings::getInstance().setSubmitMethod(Settings::START_LOCAL_PHOTOSS);
	Settings::getInstance().setStartPhotossWithoutMatlab(ui.startPhotossWithoutMatlab->isChecked());
	Settings::getInstance().save();
}

void SettingsDialog::on_photossDirectoryBrowseButton_clicked(){
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select PHOTOSS directory"),
		ui.photossDirectory->text(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	ui.photossDirectory->setText(dir);
}

void SettingsDialog::on_condorDirectoryBrowseButton_clicked(){
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Condor directory"),
		ui.condorDirectory->text(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	ui.condorDirectory->setText(dir);
}

void SettingsDialog::on_workingDirectoryBrowseButton_clicked(){
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select working directory"),
		ui.workingDirectory->text(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	ui.workingDirectory->setText(dir);
}
