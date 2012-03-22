#pragma once
#include <QtCore/QString>

class PJobFileParameterDefinition
{
public:
	PJobFileParameterDefinition();
	PJobFileParameterDefinition(QString name, double defaultValue);
	void setName(QString name);
	QString name() const;

	void setDefaultValue(double defaultValue);
	double defaultValue() const;

	bool hasUnit() const;
	void setUnit(QString unit);
	QString unit() const;

	bool hasMinValue() const;
	void setMinValue(double);
	double minValue() const;

	bool hasMaxValue() const;
	void setMaxValue(double);
	double maxValue() const;

private:
	QString m_name, m_unit;
	double m_defaultValue, m_min, m_max;
	bool m_hasUnit, m_hasMin, m_hasMax;
};


inline bool operator==(const PJobFileParameterDefinition& p1, const PJobFileParameterDefinition& p2){
	bool r =  p1.name()==p2.name() && p1.defaultValue()==p2.defaultValue() &&
		p1.hasUnit()==p2.hasUnit() && p1.hasMinValue()==p2.hasMinValue() && p1.hasMaxValue()==p2.hasMaxValue();
	if(p1.hasUnit()) r = r && p1.unit() == p2.unit();
	if(p1.hasMaxValue()) r = r && p1.maxValue() == p2.maxValue();
	if(p1.hasMinValue()) r = r && p1.minValue() == p2.minValue();
	return r;
}
