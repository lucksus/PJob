#include "FileHelperFunctions.h"

void copyDirectory(QString source, QString dest){
	QDir dir(source);
	QDir destDir(dest);
	if(!destDir.exists()){
		QString dirName = destDir.dirName();
		destDir.cdUp();
		destDir.mkpath(dirName);
		destDir.cd(dirName);
	}

	QFileInfo entry;
	foreach(entry,dir.entryInfoList()){
		if(entry.fileName() == "." || entry.fileName() == "..") continue;
		if(entry.isDir()){
			destDir.mkdir(entry.fileName());
			copyDirectory(entry.absoluteFilePath(), destDir.filePath(entry.fileName()));
		}else
			QFile::copy(entry.absoluteFilePath(), destDir.filePath(entry.fileName()));
	}
}

void rmDirectory(QString d){
	QDir dir(d);
	if(!dir.exists())return;
	QFileInfo entry;
	foreach(entry, dir.entryInfoList()){
		if(entry.fileName() == "." || entry.fileName() == "..") continue;
		if(entry.isDir()){
			rmDirectory(entry.absoluteFilePath());
		}else
			dir.remove(entry.fileName());
	}

	QString name = dir.dirName();
	dir.cdUp();
	dir.rmdir(name);
}