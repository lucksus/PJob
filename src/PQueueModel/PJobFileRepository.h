#pragma once
#include <QtCore/QHash>
#include <QtCore/QString>
#include "PJobFile.h"

class PJobFileRepository
{
public:
	static PJobFileRepository& getInstance();

	PJobFile* forFile(QString file);
	void close(QString file);
	void increaseCounter(QString file);
	void decreaseCounter(QString file);
private:
	PJobFileRepository(void);
	~PJobFileRepository(void);

	QHash< QString, PJobFile* > m_pjobFiles;
	QHash< QString, unsigned int > m_counter;
};
