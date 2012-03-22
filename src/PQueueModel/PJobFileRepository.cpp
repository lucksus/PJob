#include "PJobFileRepository.h"

PJobFileRepository::PJobFileRepository(void)
{
}

PJobFileRepository::~PJobFileRepository(void)
{
	PJobFile* p;
	foreach(p,m_pjobFiles.values()) delete p;
}


PJobFileRepository& PJobFileRepository::getInstance(){
	static PJobFileRepository p;
	return p;
}

PJobFile* PJobFileRepository::forFile(QString file){
	if(!m_pjobFiles.contains(file)){
		m_pjobFiles[file] = new PJobFile(file);
		m_counter[file] = 0;
	}
	return m_pjobFiles[file];
}

void PJobFileRepository::close(QString file){
	delete m_pjobFiles[file];
	m_pjobFiles.remove(file);
};

void PJobFileRepository::increaseCounter(QString file){
	m_counter[file]++;
}

void PJobFileRepository::decreaseCounter(QString file){
	m_counter[file]--;
	if(m_counter[file] == 0) close(file);
}
