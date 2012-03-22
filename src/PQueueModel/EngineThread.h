#pragma once
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>
#include "ScriptFunctions.h"


class Evaluater : public QObject{
Q_OBJECT
public:
	Evaluater(QScriptEngine* e, QString s):m_engine(e),m_script(s){
	}

	QScriptValue result(){
		return m_result;
	}

public slots:
	void evaluate(){
		m_result = m_engine->evaluate(m_script);
		QThread::currentThread()->quit();
	}

private:
	QScriptEngine* m_engine;
	QString m_script;
	QScriptValue m_result;
};


//! Thread containing a QScriptEngine
class EngineThread : public QThread{
Q_OBJECT
public:
	EngineThread(QString s):m_engine(0),m_script(s){
		connect(this,SIGNAL(finished()),this,SLOT(threadFinished()));
	}

	virtual void run(){
		if(!m_engine){
			m_engine = new QScriptEngine;
			addFunctionsToEngine(m_engine);
		}
		m_engine->moveToThread(this);
		//m_result = m_engine->evaluate(m_script);
		
		Evaluater e(m_engine,m_script);
		QTimer::singleShot(1,&e,SLOT(evaluate()));
		exec();
		m_result = e.result();
	}

	void abort(){
		m_engine->abortEvaluation();
	}

	QScriptValue result(){
		return m_result;
	}

	void setScript(QString script){
		m_script = script;
	}

	const QScriptEngine* const scriptEngine(){
		return m_engine;
	}

signals:
	void finishedScriptEvaluation(EngineThread*);

private slots:
	void threadFinished(){
		emit finishedScriptEvaluation(this);
	}

private:
	QScriptEngine* m_engine;
	QString m_script;
	QScriptValue m_result;
};
