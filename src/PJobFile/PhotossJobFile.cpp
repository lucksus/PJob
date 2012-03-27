#include <QtCore/QDateTime>
#include "FileHelperFunctions.h"
#include "PhotossJobFile.h"

PhotossJobFile::PhotossJobFile(QString pjobFile):PJobFile(pjobFile)
{
	//Temporäres Verzeichnis erstellen und Dateien dorthin entpacken
	QFileInfo fileInfo(pjobFile);
	m_temporaryDirectory = fileInfo.baseName() + "_temp_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmm_ss");
	this->open();
}

PhotossJobFile::~PhotossJobFile()
{
	this->close();
}

QString PhotossJobFile::newRun(QList<PJobFileParameter> parametercombination, QString runDescription)
{
	parametercombination = makeParameterCombinationValid(parametercombination);

	if(runDescription == "")
		runDescription = QDateTime::currentDateTime().toString("yyyyMMdd_hhmm_ss_zzz");
	QDir runsDirectory(tempDir().absoluteFilePath("Runs"));
	if(!runsDirectory.exists())
		tempDir().mkdir("Runs");
	QString runDirectory = "run_" + runDescription;	

	QFileInfo fileInfoNewDirectory(runsDirectory.absoluteFilePath(runDirectory));
	while(fileInfoNewDirectory.exists()){
		runDirectory += "_";
		fileInfoNewDirectory = QFileInfo(runsDirectory.absoluteFilePath(runDirectory));
	}

	runsDirectory.mkdir(runDirectory);
	QDir resourcesDirectory(tempDir().absoluteFilePath("Resources"));
	QFileInfo entry;
	foreach(entry,resourcesDirectory.entryInfoList()){
		if(entry.fileName() == "." || entry.fileName() == "..") continue;
		if(entry.isDir()){
			QDir runDir(runsDirectory.absoluteFilePath(runDirectory));
			runDir.mkdir(entry.fileName());
			copyDirectory(entry.absoluteFilePath(),runsDirectory.absoluteFilePath(runDirectory)+"/"+entry.fileName());
		}
		else
			QFile::copy(entry.absoluteFilePath(), runsDirectory.absoluteFilePath(runDirectory)+"/"+entry.fileName());
	}

	QDir::setCurrent(runsDirectory.absoluteFilePath(runDirectory));

	writeParameterCombinationForRun(parametercombination, runDirectory);
	
	return runsDirectory.absoluteFilePath(runDirectory);
}

void PhotossJobFile::open()
{
	QFileInfo fileInfo(m_pjobFile);
	QString workingDir = fileInfo.absolutePath();
	QDir dir(workingDir);
	rmDirectory(workingDir + "/" + m_temporaryDirectory);
	try
	{
		m_data->extract(dir.absoluteFilePath(m_temporaryDirectory)+'/',"Resources/");
	}
	catch(ReadFileError e)
	{
		//do something
	}
}

void PhotossJobFile::close()
{
	if(m_saveAutomatically) 
		this->save();
	QFileInfo fileInfo(m_pjobFile);
	QDir::setCurrent(fileInfo.absolutePath());
	rmDirectory(fileInfo.absolutePath() + "/" + m_temporaryDirectory);
}

void PhotossJobFile::tempDirToPJobFile()
{
	QFileInfo fileInfo(m_pjobFile);
	QString runDir = fileInfo.absolutePath() + '/' + m_temporaryDirectory;
	QDir dir(runDir);

	//Es muss nur das Runs-Verzeichnis gespeichert werden (am Rest ändert sich nichts)
	dir.cd("Runs");
	foreach(QString s, dir.entryList(QDir::Dirs))
	{
		QDir current(s);
		foreach(QFileInfo entry, current.entryInfoList(QDir::Files))
		{
			//Wenn die Datei nicht im Resources Verzeichnis existiert einfach hinzufügen
			if(!(m_data->contains("Resources/" + entry.fileName())))
			{
				try
				{
					m_data->appendFile(entry.absoluteFilePath(),current.absolutePath().section('/',-2) + '/' + entry.fileName() );
				}
				catch(PJobFileError e)
				{
					//do something
				}
			}
			else
			{
				//Sonst Vergleich ob die Dateien identisch sind
				QFile file(entry.absoluteFilePath());
				if(!(file.open(QFile::ReadOnly)))
					throw QString("Couldn't read file from temporary directory!");

				//Wenn Dateien unterschiedlich sind wird kopiert, sonst passiert gar nichts
				if(file.readAll() != m_data->readFile("Resources/" + entry.fileName()))
					try
					{
						m_data->appendFile(entry.absoluteFilePath(),current.absolutePath().section('/',-2) + '/' + entry.fileName() );
					}
					catch(PJobFileError e)
					{
						//do something
					}
				file.close();
			}	
		}
	}
	try
	{
		m_data->flush();
	}
	catch (WriteFileError e)
	{
		//do something
	}
}

QDir PhotossJobFile::tempDir() const
{
	QFileInfo fileInfo(m_pjobFile);
	return QDir(fileInfo.absolutePath() + "/" + m_temporaryDirectory);
}

void PhotossJobFile::save()
{
	tempDirToPJobFile();
}
