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

void Settings::setCondorDirectory(QString directory)
{
	m_condorDirectory = directory;
}

QString Settings::condorDirectory(void)
{
	return m_condorDirectory;
}

void Settings::setPhotossDirectory(QString directory)
{
	m_photossDirectory = directory;
}

QString Settings::photossDirectory(void)
{
	return m_photossDirectory;
}

void Settings::setWorkingDirectory(QString directory)
{
	m_workingDirectory = directory;
}

QString Settings::workingDirectory(void)
{
	return m_workingDirectory;
}

void Settings::setSubmitMethod(SubmitMethod method){
	m_submitMethod = method;
}

Settings::SubmitMethod Settings::submitMethod(){
	return m_submitMethod;
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
	settings.setValue("condorDirectory", m_condorDirectory);
	settings.setValue("photossDirectory", m_photossDirectory);
	settings.setValue("workingDirectory", m_workingDirectory);
	settings.setValue("submitMethod", m_submitMethod);
	settings.setValue("startPhotossWithoutMatlab", m_startPhotossWithoutMatlab);
	settings.setValue("runPhotossMinimized", m_startPhotossMinimized);
	settings.endGroup();
}

void Settings::load(){
	QSettings settings("HFT", "PQueue");
	settings.beginGroup("main");
	m_condorDirectory = settings.value("condorDirectory","C:\\condor").toString();
	m_photossDirectory = settings.value("photossDirectory","C:\\Programme\\PHOTOSS").toString();
	m_workingDirectory = settings.value("workingDirectory","C:\\PQueueWorkingDir").toString();
	m_submitMethod = static_cast<Settings::SubmitMethod>(settings.value("submitMethod").toInt());
	m_startPhotossWithoutMatlab = settings.value("startPhotossWithoutMatlab",false).toBool();
	m_startPhotossMinimized = settings.value("runPhotossMinimized",true).toBool();
	settings.endGroup();
}

void Settings::setStartPhotossWithoutMatlab(bool b){
	m_startPhotossWithoutMatlab = b;
}

bool Settings::startPhotossWithoutMatlab(){
	return m_startPhotossWithoutMatlab;
}
void Settings::setRunPhotossMinimized(bool b){
	m_startPhotossMinimized = b;
}

bool Settings::runPhotossMinimized(){
	return m_startPhotossMinimized;
}