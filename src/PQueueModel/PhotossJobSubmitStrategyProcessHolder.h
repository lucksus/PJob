#pragma once
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QProcess>
class PhotossJobSubmitStrategy;

/*!
* This Singleton's purpose is to assure that QProcess objects (used to spawn PHOTOSS processes)
* always live in the main (=GUI) thread.
* Therefore PhotossJobProcessHolder::getInstance() is called in PQueueController's constructor
* to make sure this object is created in the main thread also.
*
* PhotossJob objects may live in the main thread or in a thread belonging to a script engine.
* By calling spawnProcess() via Qt's signal/slot system it is assured that
* this function is processed by the main thread no matter which thread the calling
* PhotossJob lives in.
*/
class PhotossJobSubmitStrategyProcessHolder : public QObject{
	Q_OBJECT
public:
	static PhotossJobSubmitStrategyProcessHolder& getInstance();

	public slots:
		void spawnProcess(QString command, QStringList args, QString workingDirectory, PhotossJobSubmitStrategy* j);

	private slots:
		void cleanProcesses();

private:
	PhotossJobSubmitStrategyProcessHolder(){}
	QHash<QProcess*,PhotossJobSubmitStrategy*> m_processes;
};
