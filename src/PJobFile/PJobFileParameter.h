#pragma once
#include <QtCore/QString>

class PJobFileParameter
{
public:
	PJobFileParameter();
	
	QString name() const;
	void setName(QString name);

	double value() const;
	void setValue(double value);

	bool isVariation() const;
	double minValue() const;
	double maxValue() const;
	double step() const;
	void setVariation(double min, double max, double step);

private:
	QString m_name;
	double m_value, m_min, m_max, m_step;
	bool m_isVariation;
};
