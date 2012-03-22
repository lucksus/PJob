#pragma once
#include <QtScript/QScriptEngine>
#include <QtCore/QHash>
#include <QtCore/QThread>

class EngineThread;

class Scripter : public QObject
{
Q_OBJECT
public:
	static Scripter& getInstance();

	void run(QString program, QString engineName="console");
	void stop(QString engineName);
	void loadScriptFile(QString filename);
	void runLoadedScript(QString filename, QString prefix = "");

	void addOutputStringForEngine(QScriptEngine* engine, QString output);
	void setProgress(QScriptEngine* engine, unsigned int percent);

	QStringList loadedScripts();

signals:
	void scriptFileLoaded(QString file);
	void engineFinished(QString engineName, QString output);
	void scriptOutput(QString text);
	void engineStarted(QString engineName);
	void scriptProgessUpdated(QString engineName, unsigned int percent);

private slots:
	void engineThreadFinished(EngineThread*);

private:

	Scripter(void);
	QHash<QString, QString> m_scriptFiles;
	//QHash<QString, QScriptEngine*> m_engines;
	//QHash<QScriptEngine*, QString> m_outputs;
	//QHash<QScriptEngine*, EngineThread*> m_threads;
	QHash<QString, EngineThread*> m_threads;
	QHash<QString, QString> m_outputs;

	QScriptEngine* createEngine();

	void readScriptDirectory();
};
