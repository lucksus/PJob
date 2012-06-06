#include "Job.h"
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <sstream>
#include <iostream>
#include <limits>
#include "Settings.h"
//#include "PQueueController.h"
#include "Tokenizer.h"
#include <iostream>
#include <algorithm>
#include "PhotossJobSubmitCondor.h"
#include "PhotossJobSubmitLocal.h"
#include "PJobFileRepository.h"
#include "PQueueController.h"


Job::Job(QString pjobFile, QHash<QString,QString> parameters)
:m_parameters(parameters), m_submitStrategy(0)
{
	QFileInfo fileInfo(pjobFile);
	m_pjobFileAbsoulutePath = fileInfo.absoluteFilePath();
	m_pjobFile = PJobFileRepository::getInstance().forFile(m_pjobFileAbsoulutePath);
	PJobFileRepository::getInstance().increaseCounter(m_pjobFileAbsoulutePath);
	QObject::moveToThread(PQueueController::getInstace().thread());
}

Job::~Job(){
	PJobFileRepository::getInstance().decreaseCounter(m_pjobFileAbsoulutePath);
}

PJobFile* Job::pjobFile(){
	return m_pjobFile;
}

QString Job::mainScript(){
	return m_pjobFile->mainPscript();
}

QString Job::description(){
	return m_pjobFile->pjobFile()+": ";
}

void Job::addResource(QString path){
	m_pjobFile->addResource(path);
}

QHash<QString,QString> Job::parameters(){
	return m_parameters;
}

void Job::submit(){
	createSubmitStrategy();
	m_submitStrategy->submit();
}

void Job::createSubmitStrategy(){
	if(m_submitStrategy) delete m_submitStrategy;

	if(Settings::getInstance().submitMethod() == Settings::START_LOCAL_PHOTOSS){
		m_submitStrategy = new PhotossJobSubmitLocal(this);
	}

	if(Settings::getInstance().submitMethod() == Settings::USE_CONDOR){
		m_submitStrategy = new PhotossJobSubmitCondor(this);
	}

	connect(m_submitStrategy, SIGNAL(submited()), this, SLOT(submited()));
	connect(m_submitStrategy, SIGNAL(started()), this, SLOT(started()));
	connect(m_submitStrategy, SIGNAL(failed()), this, SLOT(failed()));
	connect(m_submitStrategy, SIGNAL(finished(QString)), this, SLOT(process_finished_run(QString)));
}


void Job::submited(){
	m_state = SUBMITED;
	emit stateChanged(this,m_state);
}

void Job::started(){
	m_state = RUNNING;
	emit stateChanged(this,m_state);
}

void Job::failed(){
	m_state = FAILED;
	emit stateChanged(this,m_state);
}

void Job::finished(){
	m_state = FINISHED;
	emit stateChanged(this,m_state);
}

void Job::process_finished_run(QString runDirectory){
	try{
		m_pjobFile->checkIfRunIsProperlyFinished(runDirectory);
	}
	catch (QString e){
		emit problemReadingResults(this, e);
		failed();
		return;
	}
	
	//Wenn wir bis hier kommen, ist PHOTOSS durchgelaufen und es liegen alle Resultdateien vor..

	try{
		QHash< QHash<QString,double>, QHash<QString,double> > r = m_pjobFile->getResultsForRun(runDirectory);
		emit results(r, m_pjobFile->pjobFile());
		emit results(this, r, m_pjobFile->pjobFile());
	}catch(QString s){
		emit problemReadingResults(this, s);
	}
	finished();

	//Because of this we have to make sure
	//this slot is always called from the main (=GUI) thread
	//which is (hopefully) never blocked.
	m_waitConditionJobState.wakeAll();
}

void Job::waitUntilFinished(){
	while(m_state != FINISHED){
		m_mutex.lock();
		m_waitConditionJobState.wait(&m_mutex);
		m_mutex.unlock();
	};
}




QHash< QString,QVector<double> > Job::readGlobalVariablesFromPHOFile(QString file){
	QHash< QString,QVector<double> > result;

	std::ifstream phoFile(file.toStdString().c_str());
	std::string parameterVariation;
	std::string globalVariables;
	std::vector<token> variables;
	FindString("ParameterVariation{", "}ParameterVariation", phoFile, parameterVariation);
	std::stringstream stream(parameterVariation);
	FindString("GlobalVariables{","}GlobalVariables",stream,globalVariables);
	FindTokens("(#","#)",globalVariables,variables);
	for(unsigned int i=0;i<variables.size();++i){
		std::vector<token> tokens;
		FindTokens("(#","#)", variables[i].getValue(), tokens);
		std::string name;
		std::string values;
		std::string variation_type;
		ReadToken(tokens,"Variation_type", variation_type);
		if(variation_type != "Variation") continue;

		ReadToken(tokens,"Name",name);
		ReadToken(tokens,"Variation_vector",values);
		std::stringstream valuesStream(values);
		std::vector<double> variationValues;
		while(true){
			double d;
			valuesStream >> d;
			if(valuesStream.eof()) break;
			variationValues.push_back(d);
		}
		sort(variationValues.begin(),variationValues.end());

		result[QString::fromStdString(name)] = QVector<double>::fromStdVector(variationValues);
	}

	return result;
}
