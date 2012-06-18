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
	settings.endGroup();
}

void Settings::load(){
	QSettings settings("HFT", "PQueue");
	settings.beginGroup("main");
        m_autoSaveToPjob = settings.value("autoSaveToPjob", true).toBool();
	settings.endGroup();
}
