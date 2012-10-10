#include "SettingsDialog.h"
#include "Settings.h"
#include <QtGui/QFileDialog>

SettingsDialog::SettingsDialog(void)
{
	ui.setupUi(this);
    ui.autoSaveToPjob->setChecked(Settings::getInstance().autoSaveToPjob());
    ui.internalResultsActivated->setChecked(Settings::getInstance().internal_results_activated());
	connect(this,SIGNAL(accepted()),this,SLOT(on_accepted()));
}


void SettingsDialog::on_accepted(){
    Settings::getInstance().setAutoSaveToPjob(ui.autoSaveToPjob->isChecked());
    Settings::getInstance().set_internal_results_activated(ui.internalResultsActivated->isChecked());
	Settings::getInstance().save();
}
