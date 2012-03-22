#include "Scripter.h"
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include "ScriptFunctions.h"
#include "EngineThread.h"
#include "Settings.h"

Scripter::Scripter(void){
	readScriptDirectory();
}

Scripter& Scripter::getInstance(){
	static Scripter s;
	return s;
}

QScriptEngine* Scripter::createEngine(){
	QScriptEngine* engine = new QScriptEngine;
	addFunctionsToEngine(engine);
	engine->setProcessEventsInterval(10);
	return engine;
}

void Scripter::addOutputStringForEngine(QScriptEngine* engine, QString output){
	QString engineName;
	foreach(engineName,m_threads.keys())
		if(m_threads[engineName]->scriptEngine() == engine) break;
	
	m_outputs[engineName] += output;
	emit scriptOutput(output);
}

void Scripter::run(QString script, QString engineName){
	if(m_threads.contains(engineName)){
		if(m_threads[engineName]->isRunning()) throw QString("Engine already running!");
		m_threads[engineName]->setScript(script);
	}else{
		m_threads[engineName] = new EngineThread(script);
		connect(m_threads[engineName], SIGNAL(finishedScriptEvaluation(EngineThread*)), this, SLOT(engineThreadFinished(EngineThread*)));
	}
	
	emit engineStarted(engineName);
	m_threads[engineName]->start();
}

void Scripter::stop(QString engineName){
	if(!m_threads.contains(engineName))return;
	m_threads[engineName]->abort();
	//m_threads[engineName]->wait();
}

void Scripter::loadScriptFile(QString filename){
	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QString s = QString(file.readAll());
	file.close();
	m_scriptFiles[filename] = s;
	emit scriptFileLoaded(filename);
}

void Scripter::runLoadedScript(QString filename, QString prefix){
	run(prefix + m_scriptFiles[filename], filename);
}

void Scripter::engineThreadFinished(EngineThread* engineThread){
	QString engineName;
	foreach(engineName,m_threads.keys()){
		if(m_threads[engineName] == engineThread){
			QScriptValue result = m_threads[engineName]->result();

			QString output = m_outputs[engineName];
			m_outputs[engineName] = "";

			//if(output == "")
			output = result.toString();

					
			emit engineFinished(engineName,output);

			//delete m_threads[engineName];
			//m_threads.remove(engineName);
		}
	}
}

void Scripter::setProgress(QScriptEngine* engine, unsigned int percent){
	QString engineName;
	foreach(engineName,m_threads.keys()){
		if(m_threads[engineName]->scriptEngine() == engine){
			emit scriptProgessUpdated(engineName, percent);
		}
	}
}


void Scripter::readScriptDirectory(){
	QDir directory(Settings::getInstance().scriptDirectory());
	QStringList filter; filter << "*.js";
	QString file;
	foreach(file,directory.entryList(filter)){
		loadScriptFile(file);
	}
}

QStringList Scripter::loadedScripts(){
	return m_scriptFiles.keys();
}
