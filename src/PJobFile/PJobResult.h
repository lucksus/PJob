#pragma once
#include <QtCore/QString>

class PJobResult
{
public:
	PJobResult();
	QString name() const;
	void setName(QString);

	QString unit() const;
	void setUnit(QString);

private:
	QString m_name, m_unit;
};
