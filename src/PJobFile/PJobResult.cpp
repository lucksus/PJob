#include "PJobResult.h"

PJobResult::PJobResult()
{
}

QString PJobResult::name() const{
	return m_name;
}

void PJobResult::setName(QString name){
	m_name = name;
}

QString PJobResult::unit() const{
	return m_unit;
}

void PJobResult::setUnit(QString unit){
	m_unit = unit;
}