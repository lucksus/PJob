#include "PJobFileFormat.h"
#include <iostream>

class PJobOutput: QObject
{
	Q_OBJECT
public:
	PJobOutput(PJobFileFormat *file);
	~PJobOutput();
	void setVerbose(bool mode);
	static void printAbout();
	void printDetail(std::string command);

public slots:
	void print(QString text);

private:
	PJobFileFormat *m_pjobFile;
};