#include "PJobFileParameterDefinition.h"

PJobFileParameterDefinition::PJobFileParameterDefinition()
:m_hasUnit(false), m_hasMin(false), m_hasMax(false){

}

PJobFileParameterDefinition::PJobFileParameterDefinition(QString name, double defaultValue)
:m_name(name), m_defaultValue(defaultValue), m_hasUnit(false), m_hasMin(false), m_hasMax(false)
{
	PJobFileParameterDefinition();
}

void PJobFileParameterDefinition::setName(QString name){
	m_name = name;
}

QString PJobFileParameterDefinition::name() const{
	return m_name;
}

void PJobFileParameterDefinition::setDefaultValue(double defaultValue){
	m_defaultValue = defaultValue;
}

double PJobFileParameterDefinition::defaultValue() const{
	return m_defaultValue;
}

bool PJobFileParameterDefinition::hasUnit() const{
	return m_hasUnit;
}

void PJobFileParameterDefinition::setUnit(QString unit){
	m_unit = unit;
	m_hasUnit = true;
}

QString PJobFileParameterDefinition::unit() const{
	return m_unit;
}

bool PJobFileParameterDefinition::hasMinValue() const{
	return m_hasMin;
}

void PJobFileParameterDefinition::setMinValue(double min){
	m_min = min;
	m_hasMin = true;
}

double PJobFileParameterDefinition::minValue() const{
	return m_min;
}

bool PJobFileParameterDefinition::hasMaxValue() const{
	return m_hasMax;
}

void PJobFileParameterDefinition::setMaxValue(double max){
	m_max = max;
	m_hasMax = true;
}

double PJobFileParameterDefinition::maxValue() const{
	return m_max;
}