#pragma once
#include "PhotossJobSubmitStrategy.h"
#include <QtCore/QHash>
#include <QtCore/QProcess>

class PhotossJobSubmitLocal : public PhotossJobSubmitStrategy{
Q_OBJECT
public:
	PhotossJobSubmitLocal(Job*);
	~PhotossJobSubmitLocal(void);

	void submit();

signals:
	void wantsProcess(QString, QStringList, QString, PhotossJobSubmitStrategy*);

public slots:
	void _started();
	void _finished();
};


