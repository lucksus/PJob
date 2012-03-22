#include "PhotossJobSubmitStrategyProcessHolder.h"
#include "PhotossJobSubmitStrategy.h"
#include <QtCore/QTimer>

PhotossJobSubmitStrategyProcessHolder& PhotossJobSubmitStrategyProcessHolder::getInstance(){
	static PhotossJobSubmitStrategyProcessHolder p;
	return p;
}

void PhotossJobSubmitStrategyProcessHolder::spawnProcess(QString command, QStringList args, QString workingDirectory, PhotossJobSubmitStrategy* j){
	QProcess* process = new QProcess();
	m_processes[process] = j;
	process->setWorkingDirectory(workingDirectory);
	connect(process,SIGNAL(started()),j,SLOT(_started()));
	connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cleanProcesses()));
	process->start(command,args);
	//process->waitForStarted();
}


void PhotossJobSubmitStrategyProcessHolder::cleanProcesses(){
	QProcess* p;
	foreach(p,m_processes.keys()){
		if( p->state() == QProcess::NotRunning ){
			QTimer::singleShot(1,m_processes[p],SLOT(_finished()));
			m_processes.remove(p);
			delete p;
		}

	}
}
