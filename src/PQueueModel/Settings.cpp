#include "Settings.h"
#include <QtCore/QSettings>

Settings::Settings(void)
{
	load();
	m_scriptDirectory = ".";
}

Settings& Settings::getInstance(){
	static Settings s;
	return s;
}

bool Settings::autoSaveToPjob(){
    return m_autoSaveToPjob;
}

void Settings::setAutoSaveToPjob(bool auto_save){
    m_autoSaveToPjob = auto_save;
}

bool Settings::internal_results_activated(){
    return m_internal_results_activated;
}

void Settings::set_internal_results_activated(bool b){
    m_internal_results_activated = b;
}

void Settings::setScriptDirectory(QString directory){
        m_scriptDirectory = directory;
}

QString Settings::scriptDirectory(){
   return m_scriptDirectory;
}

void Settings::save(){
	QSettings settings("HFT", "PQueue");
	settings.beginGroup("main");
        settings.setValue("autoSaveToPjob", m_autoSaveToPjob);
        settings.setValue("internal_results_activated", m_internal_results_activated);
	settings.endGroup();
}

void Settings::load(){
	QSettings settings("HFT", "PQueue");
	settings.beginGroup("main");
        m_autoSaveToPjob = settings.value("autoSaveToPjob", true).toBool();
        m_internal_results_activated = settings.value("internal_results_activated", false).toBool();
	settings.endGroup();
}
