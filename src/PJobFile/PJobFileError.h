#pragma once
#include <QtCore/QString>

class PJobFileError
{
public:
	PJobFileError(QString msg):m_msg(msg){}
	~PJobFileError(){}
	QString msg() 
	{
		return m_msg;
	}

private:
	QString m_msg;
};

class ReadFileError:public PJobFileError
{
public:
	ReadFileError(QString msg):PJobFileError(msg){}
	~ReadFileError(){}
};

class WriteFileError:public PJobFileError
{
public:
	WriteFileError(QString msg):PJobFileError(msg){}
	~WriteFileError(){}
};

class FileSystemError:public PJobFileError
{
public:
	FileSystemError(QString msg):PJobFileError(msg){}
	~FileSystemError(){}
};

class RawDataError:public PJobFileError
{
public:
	RawDataError(QString msg):PJobFileError(msg){}
	~RawDataError(){}
};