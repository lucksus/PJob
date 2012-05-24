#pragma once
#include "PhotossJobSubmitStrategy.h"
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include "Condor.h"

class PhotossJobSubmitCondor : public PhotossJobSubmitStrategy{
Q_OBJECT
public:
        PhotossJobSubmitCondor(Job*);
	~PhotossJobSubmitCondor(void);

	void submit();

private slots:
	void _started();
	void _failed();
	void _finished();

private:
	
        CondorAdapter::CondorJob* m_condorJob;
        CondorAdapter::CondorJob* createJob();
	QString m_path_to_workingcopy_pjobfile;

};
