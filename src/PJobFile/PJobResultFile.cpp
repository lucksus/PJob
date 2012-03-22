#include "PJobResultFile.h"

PJobResultFile::PJobResultFile(void)
{
    m_type = PJobResultFile::SINGLE_VALUE;
}

QString PJobResultFile::filename() const{
	return m_filename;
}

void PJobResultFile::setFilename(QString filename){
	m_filename = filename;
}

QList<PJobResult> PJobResultFile::results() const{
	return m_results;
}

void PJobResultFile::addResult(PJobResult r){
	m_results.append(r);
}

void PJobResultFile::removeResult(QString resultName){
	QList<PJobResult>::iterator it;
	for(it = m_results.begin();it != m_results.end(); ++it)
		if(it->name() == resultName){
			m_results.erase(it);
			return;
		}
}

PJobResultFile::Type PJobResultFile::type() const{
    return m_type;
}
void PJobResultFile::setType(PJobResultFile::Type type){
    m_type = type;
}
