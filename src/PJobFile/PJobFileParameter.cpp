#include "PJobFileParameter.h"

PJobFileParameter::PJobFileParameter()
: m_isVariation(false)
{
}


QString PJobFileParameter::name() const{
	return m_name;
}

void PJobFileParameter::setName(QString name){
	m_name = name;
}

double PJobFileParameter::value() const{
	return m_value;
}

void PJobFileParameter::setValue(double value){
	m_value = value;
	m_isVariation = false;
}

bool PJobFileParameter::isVariation() const{
	return m_isVariation;
}

double PJobFileParameter::minValue() const{
	return m_min;
}

double PJobFileParameter::maxValue() const{
	return m_max;
}

double PJobFileParameter::step() const{
	return m_step;
}

void PJobFileParameter::setVariation(double min, double max, double step){
	m_min = min;
	m_max = max;
	m_step = step;
	m_isVariation = true;
}