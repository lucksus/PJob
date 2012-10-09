#pragma once
#include "PJobFile.h"

class PhotossJobFile: public PJobFile{
public:
	PhotossJobFile(QString path);
	~PhotossJobFile();
	QString newRun(QList<PJobFileParameter> parametercombination, QString runDescription);

private:
	QString m_temporaryDirectory;
	void open();
	void close();
	void tempDirToPJobFile();
	QDir tempDir() const;
	void save();
};