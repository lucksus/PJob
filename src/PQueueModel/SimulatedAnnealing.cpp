#include "SimulatedAnnealing.h"
#include "RandomGenerator.h"
#include <QtCore/QSet>
#include <QtCore/QHash>
#include "PQueueController.h"

SimulatedAnnealing::SimulatedAnnealing(InterpolationFunction* i)
: m_interpolant(i), m_maxRuns(25), m_startTemp(5),  m_FAC(0.5f)
{}

QHash< QString, double > SimulatedAnnealing::findMinimum(){
	m_searchMinimum = true;
	return find();
}
QHash< QString, double > SimulatedAnnealing::findMaximum(){
	m_searchMinimum = false;
	return find();
}

QHash< QString, double > SimulatedAnnealing::find(){
	QHash< QString, double > posBest;
	QHash< QString, double > pos = generateStartPosition();
	posBest = pos;
	float temperature = m_startTemp;
	float f = m_interpolant->getValue(pos);
	float fBest = f;

	bool newPosition = true;
	//------------------------------------------
	//Temperature-Loop
	while(newPosition){
		newPosition = false;
		unsigned int runsWithoutNewPosition = 1;
		pos = posBest;
		f = fBest;
		//--------------------------------------
		//Pertubation-Loop
		while(runsWithoutNewPosition < m_maxRuns && temperature!=0){
			QHash< QString, double > posTry;
			bool ok;
			do{
				ok=true;
				posTry = generateTry(pos);
			}while(!ok);
			float fTry = m_interpolant->getValue(posTry);
			if(better(fTry,f)){
				pos = posTry;
				f = fTry;
				newPosition = true;
			}else if((fTry != f) && withProbEToTheMinus((fTry-f)/temperature)){
				pos = posTry;
				f = fTry;
				newPosition = true;
			}
			if(better(fTry,fBest)){
				posBest = posTry;
				fBest = fTry;
				runsWithoutNewPosition = 1;
				
			}else{
				++runsWithoutNewPosition;
			}
		}
		//Pertubation-Loop
		//--------------------------------------
		temperature *= m_FAC;
	}
	//Temperature-Loop
	//------------------------------------------
	return posBest;
}



QHash< QString, double > SimulatedAnnealing::generateTry(const QHash< QString, double >& oldPos){
	QHash< QString, double > result(oldPos);
	QString parameter;
	foreach(parameter, result.keys()){
		QSet< QHash<QString,double> > combinations = PQueueController::getInstace().getResults().parameterCombinationsFor(m_interpolant->pjob_file());
		double max = PQueueController::getInstace().getResults().maxParameterValue(combinations,parameter);
		double min = PQueueController::getInstace().getResults().minParameterValue(combinations,parameter);
		float stepSize = (max - min)/1000;
		float step = static_cast<float>(PQueueMath::RandomGenerator::uniform(0.f,2.f*stepSize));
		result[parameter] += step- stepSize;
	}
	return result;
}

QHash< QString, double > SimulatedAnnealing::generateStartPosition(){
	QHash< QString, double > result;
	QString parameter;
	foreach(parameter, PQueueController::getInstace().getResults().parametersFor(m_interpolant->pjob_file())){
		QSet< QHash<QString,double> > combinations = PQueueController::getInstace().getResults().parameterCombinationsFor(m_interpolant->pjob_file());
		double max = PQueueController::getInstace().getResults().maxParameterValue(combinations,parameter);
		double min = PQueueController::getInstace().getResults().minParameterValue(combinations,parameter);
		result[parameter] = static_cast<float>(PQueueMath::RandomGenerator::uniform(max, min)); 
	}
	return result;
}

bool SimulatedAnnealing::withProbEToTheMinus(const float &x){
	float a = exp(-x);
	return PQueueMath::RandomGenerator::uniform(0,1) <= a;
}

bool SimulatedAnnealing::better(double newValue, double oldValue){
	if(m_searchMinimum)
		return newValue < oldValue;
	else
		return newValue > oldValue;
}
