#pragma once
#include "PJobResult.h"
#include <QtCore/QList>

class PJobResultFile
{
public:
	PJobResultFile();
        enum Type{SINGLE_VALUE, CSV};
	QString filename() const;
	void setFilename(QString filename);
        Type type() const;
        void setType(Type);
	
	QList<PJobResult> results() const;
	void addResult(PJobResult);
	void removeResult(QString resultName);
	
private:
	QString m_filename;
	QList<PJobResult> m_results;
        Type m_type;
};
