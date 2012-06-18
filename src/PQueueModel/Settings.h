#pragma once
#include <QtCore/QObject>

class Settings : public QObject
{
Q_OBJECT
Q_PROPERTY(bool autoSaveToPjob READ autoSaveToPjob WRITE setAutoSaveToPjob)
public:
	static Settings& getInstance();

        bool autoSaveToPjob();
        void setAutoSaveToPjob(bool);

	void setScriptDirectory(QString directory);
	QString scriptDirectory();

	void save();
	void load();

private:
	Settings(void);
        bool m_autoSaveToPjob;
	QString m_scriptDirectory;

};
