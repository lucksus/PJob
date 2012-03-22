#pragma once
#include <QtCore/QObject>

class Settings : public QObject
{
Q_OBJECT
Q_PROPERTY(QString condorDirectory READ condorDirectory WRITE setCondorDirectory)
Q_PROPERTY(QString photossDirectory READ photossDirectory WRITE setPhotossDirectory)
Q_PROPERTY(QString workingDirectory READ workingDirectory WRITE setWorkingDirectory)
public:
	static Settings& getInstance();
	enum SubmitMethod{USE_CONDOR, START_LOCAL_PHOTOSS};

	void setCondorDirectory(QString directory);
	QString condorDirectory(void);
	void setPhotossDirectory(QString directory);
	QString photossDirectory(void);
	void setWorkingDirectory(QString directory);
	QString workingDirectory(void);
	void setSubmitMethod(SubmitMethod method);
	SubmitMethod submitMethod();
	void setRunPhotossMinimized(bool b);
	bool runPhotossMinimized();
	void setStartPhotossWithoutMatlab(bool b);
	bool startPhotossWithoutMatlab();

	void setScriptDirectory(QString directory);
	QString scriptDirectory();

	void save();
	void load();

private:
	Settings(void);

	QString m_condorDirectory;
	QString m_photossDirectory;
	QString m_workingDirectory;
	SubmitMethod m_submitMethod;
	bool m_startPhotossMinimized;
	bool m_startPhotossWithoutMatlab;

	QString m_scriptDirectory;

};
