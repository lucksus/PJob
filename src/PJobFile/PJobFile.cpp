#include "PJobFile.h"
#include "PJobFileXMLFunctions.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QProcess>
#include <QtCore/QHash>
#include "FileHelperFunctions.h"
#include <sstream>
#include <limits>
#include <fstream>

PJobFile::PJobFile(QString pjobFile)
: m_saveAutomatically(false)
{
	QFileInfo fileInfo(pjobFile);
	m_pjobFile = fileInfo.absoluteFilePath();
        bool create_new = false;
        if(!fileInfo.exists())
            create_new = true;

	m_data = new PJobFileFormat(m_pjobFile);

        if(create_new){
            create();
            save();
        }
}

PJobFile::~PJobFile(){
	if(m_saveAutomatically) 
		this->save();
	delete m_data;
}

void PJobFile::addResource(QString path){
	try
	{
		m_data->appendFolder(path,"Resources/");
	}
	catch(PJobFileError e)
	{
		//do something
	}

	if(m_saveAutomatically)
		this->save();
	emit changed();
}

void PJobFile::addResource(const QByteArray& content, const QString& internal_file_name){
	m_data->appendFile(content, QString("Resources/%1").arg(internal_file_name));
	if(m_saveAutomatically)
		this->save();
	emit changed();
}

QStringList PJobFile::runDirectoryEntries() const{
	QStringList runDirs;
	foreach(QString entry ,m_data->content())
	{
		if(entry.startsWith("Runs/"))
			runDirs.append(entry);
	}
	return runDirs;
}

QString PJobFile::latestRunDirectory() const{
	QStringList runs = this->runDirectoryEntries();
	if(runs.empty()) throw QString("No run directory found.");
	return (runs.last()).section("/",0,1);
}

QString PJobFile::pjobFile() const{
	return m_pjobFile;
}

QString PJobFile::mainPscript() const{
	return QString(m_data->readFile("Resources/main.pscript"));
}

void PJobFile::setMainPscript(QString script){
	QByteArray a;
	a.append(script);
	try{
		m_data->appendFile(a,"Resources/main.pscript");
	}
	catch(PJobFileError e)
	{
		//do something
	}

	if(m_saveAutomatically) 
		this->save();
	emit changed();
}

PJobFileFormat* PJobFile::getPJobFile(){
	return m_data;
}

void PJobFile::create(){
	writeResultDefinitions(QList<PJobResultFile>());
	writeParameterDefinitions(QList<PJobFileParameterDefinition>());
    writeBinaries(QList<PJobFileBinary>());
	try
	{
		m_data->appendFile(QByteArray(), "Resources/main.pscript");
	}
	catch(PJobFileError e)
	{
		//do something
	}
	if(m_saveAutomatically) 
		this->save();
}

QList<PJobFileParameterDefinition> PJobFile::parameterDefinitions() const{
	return PJobFileXMLFunctions::readParameterDefinitions(m_data->readFile("parameterdefinitions.xml"));
}

void PJobFile::addParameterDefinition(const PJobFileParameterDefinition& def){
	QByteArray xmlFile = PJobFileXMLFunctions::addParameterDefinition(def, m_data->readFile("parameterdefinitions.xml"));
	try
	{
		m_data->appendFile(xmlFile, "parameterdefinitions.xml");
	}
	catch(PJobFileError e)
	{
		//do something
	}
	if(m_saveAutomatically)
		this->save();
	emit changed();
}

void PJobFile::removeParameterDefinition(QString parameterName){
	QByteArray xmlFile = PJobFileXMLFunctions::removeParameterDefinition(parameterName, m_data->readFile("parameterdefinitions.xml"));
	try
	{
		m_data->appendFile(xmlFile, "parameterdefinitions.xml");
	}
	catch (PJobFileError e)
	{
		//do something
	}
	if(m_saveAutomatically)
		this->save();
	emit changed();
}

void PJobFile::writeParameterDefinitions(QList<PJobFileParameterDefinition> definitions){
	try
	{
		m_data->appendFile(PJobFileXMLFunctions::writeParameterDefinitions(definitions), "parameterdefinitions.xml");
	}
	catch (PJobFileError e)
	{
		//do something
	}
	if(m_saveAutomatically)
		this->save();
	emit changed();
}

QList<PJobFileParameter> PJobFile::readParameterCombinationForRun( QString run ){
	QFileInfo fileInfo(run);
	return PJobFileXMLFunctions::readParameterCombination(m_data->readFile("Runs/" + fileInfo.fileName() + "/parametercombination.xml"));
}

void PJobFile::writeParameterCombinationForRun( QList<PJobFileParameter> parameters, QString run ){
	QString xmlPath = QString("Runs/%1/parametercombination.xml").arg(run);
	try
	{
		m_data->appendFile(PJobFileXMLFunctions::writeParameterCombination(parameters), xmlPath);
	}
	catch(PJobFileError e)
	{
		//do something
	}
	if(m_saveAutomatically)
		this->save();
}

QList<PJobResultFile> PJobFile::readResultDefinitions(){
	return PJobFileXMLFunctions::readResultDefinitions(m_data->readFile("resultdefinitions.xml"));
}

void PJobFile::writeResultDefinitions(QList<PJobResultFile> resultFiles){
	try
	{
		m_data->appendFile(PJobFileXMLFunctions::writeResultDefinitions(resultFiles), "resultdefinitions.xml");
	}
	catch(PJobFileError e)
	{
		//do something
	}
	if(m_saveAutomatically) 
		this->save();
	emit changed();
}

QList<PJobFileBinary> PJobFile::binaries() const{
    return PJobFileXMLFunctions::readBinaries(m_data->readFile("binaries.xml"));
}

void PJobFile::writeBinaries(const QList<PJobFileBinary>& binaries){
    m_data->appendFile(PJobFileXMLFunctions::writeBinaries(binaries), "binaries.xml");
    if(m_saveAutomatically) save();
    emit changed();
}

void PJobFile::save(){
	try
	{
		m_data->flush();
	}
	catch (WriteFileError e)
	{
		//do something
	}
}

void PJobFile::setSaveAutomatically(bool s){
	m_saveAutomatically = s;
}

QList<PJobFileParameter> PJobFile::makeParameterCombinationValid(const QList<PJobFileParameter>& parametercombination) const{
	QList<PJobFileParameter> result;
	QList<PJobFileParameterDefinition> parameterdefinitions = parameterDefinitions();
	PJobFileParameterDefinition def;
	foreach(def,parameterdefinitions){
		PJobFileParameter parameter;
		bool found = false;
		PJobFileParameter p;
		foreach(p,parametercombination){
			if(p.name() != def.name())continue;
			if(p.isVariation()){
				double min = p.minValue();
				double max = p.minValue();
				if(min < def.minValue() && def.hasMinValue()) min = def.minValue();
				if(max < def.minValue() && def.hasMinValue()) max = def.minValue();
				if(min > def.maxValue() && def.hasMaxValue()) min = def.maxValue();
				if(max > def.maxValue() && def.hasMaxValue()) max = def.maxValue();
				p.setVariation(min,max,p.step());
			}else{
				if(p.value() < def.minValue() && def.hasMinValue()) p.setValue(def.minValue());
				if(p.value() > def.maxValue() && def.hasMaxValue()) p.setValue(def.maxValue());
			}
			found = true;
			parameter = p;
		}
		if(!found){
			parameter.setName(def.name());
			parameter.setValue(def.defaultValue());
		}
		result << parameter;
	}
	return result;
}

QStringList PJobFile::readHeader( istream& file )
{
	string line;
	getline(file, line);
	stringstream line_stream;
	line_stream.str(line);
	//Das "%" wegschmeissen:
	char c;
	line_stream.get(c);

	//Kommentare und whitespaces entfernen
	do{
		line_stream.get(c);
		if(c!='%' && c!=' ' && c!='\t') line_stream.putback(c);
	}while(c=='%' || c==' ' || c=='\t');

	QStringList header;

	//Variabelnnamen einlesen
	while(!line_stream.eof()){
		string s;
		char buf[512];
		line_stream.getline(buf,512,'\t');
		s = string(buf);
		s = s.substr(0,s.find(","));
		if(s!= "")header.append(s.c_str());
	}

	while(header.back()=="" || header.back()=="\n" || header.back()=="\t" || header.back()=="\r")header.pop_back();
	return header;
}

pair< QStringList, QStringList > PJobFile::tellParametersAndResultsApart(QStringList header, QStringList knowResults){
	pair< QStringList, QStringList > result;
	foreach(QString s, header){
		if(knowResults.count(s) > 0)
			result.second.append(s);
		else
			result.first.append(s);
	}
	return result;
}

QHash<QString,double> PJobFile::readValues(istream& input, QStringList header){
	
	//Kommentare ignorieren:
	char c;
	input.get(c);
	if(c != '%') input.putback(c);
	else{
		char c[500];
		input.getline(c,500,'\n');
		return QHash<QString,double>();
	}
	//----------------------

	if(input.eof()) return QHash<QString,double>();

	streamsize oldPrecision = input.precision(20);
	QHash<QString,double> result;
	for(int i=0; i < header.size(); i++){
		string s;
		double f;
		input >> s;
		if(input.eof()) return QHash<QString,double>();
		if(s == "1.#QNAN")
			f = std::numeric_limits<double>::quiet_NaN();
		else if(s == "1.#INF")
			f = std::numeric_limits<double>::infinity();
		else{
			stringstream ss;
			ss << s;
			ss >> f;
		}
		result[header[i]] = f;
	}
	input.precision(oldPrecision);
	return result;
}

QList< QHash< QString, double > > PJobFile::resultsFromFile(istream& input){
	//---------------
	//--Header-------
	//---------------
	QStringList header;
	header = readHeader(input);
	if(header.isEmpty()) 
		throw QString("Unexpected format");

	//pair<QStringList,QStringList> parameters_and_results = tellParametersAndResultsApart(header, knowResults);
	//QStringList parameters = parameters_and_results.first;
	//QStringList results = parameters_and_results.second;

	QList< QHash< QString, double > > allResults;

	//---------------
	//--Werte--------
	//---------------
	QHash< QString, double > results;
	do{
		//QHash<QString,double> parameterCombination = readValues(input,parameters);
		//if(parameterCombination.isEmpty()) continue;
		results = readValues(input,header);
		if(results.isEmpty()) continue;
		allResults.append(results);
	}while(!input.eof());

	return allResults;
}

QHash< QHash<QString,double>, QHash<QString,double> > PJobFile::getResultsForRun(QString run){
	QHash< QHash<QString,double>, QHash<QString,double> > returnTmp;
	QList<PJobFileParameter> parameters = readParameterCombinationForRun(run);
	QHash<QString, PJobFileParameter> variationParmeters;
	QHash<QString,double> parameterCombination;
	PJobFileParameter p;
	foreach(p,parameters)
		if(p.isVariation())
			variationParmeters[p.name()] = p;
		else
			parameterCombination[p.name()] = p.value();

	QHash<QString,double> results;

	//resultdefinitions.xml parsen
	QList<PJobResultFile> resultFileDefinitions = readResultDefinitions();
	foreach(PJobResultFile resultFile, resultFileDefinitions)
	{
		//nur ein einziger Result pro Resultfile (neuer Code)
		if(resultFile.type() == PJobResultFile::SINGLE_VALUE)
		{
			//Wert auslesen (stets nur 1 vorhanden) der entsprechenden Parametercombination und hinzufügen
			QByteArray parsed = m_data->readFile(run + '/' + resultFile.filename()).simplified();
			QString name = resultFile.results().first().name();
			if(resultFile.results().first().unit() != "")
			name += "[" + resultFile.results().first().unit() + "]";

			//INF- und NaN-Werte abfragen
			if(parsed == "1.#INF")
			{
				results[name]= std::numeric_limits<double>::infinity();
			}
			else if(parsed == "1.#QNAN")
				results[name]= std::numeric_limits<double>::quiet_NaN();
			else{
				QString help = parsed;
				std::stringstream ss(help.toStdString());
				double d;
				ss >> d;
				results[name]=d;
				//ACHTUNG!!
				//TODO:
				//Nico: Falls die Datei mehrere Resultwerte enthält, wird hier immer der ERSTE genommen!
				//Das habe ich so gemacht, damit die Beispiel-Sim, die wir übermorgen (16.11.2011) in München zeigen wollen, funktioniert.
				//Sinnvoller wäre eigentlich, den letzen Wert zu nehmen, oder sich einen Mechanismus zu überlegen, mehrere Werte einlesen zu können...
			}
		}
	

		/******/
		//beliebig viele Results pro Resultfile (gaaanz viel alter Code -> funktioniert bisher nicht)
		if(resultFile.type() == PJobResultFile::CSV)
		{
			QStringList resultNames;
			PJobResult r;
			foreach(r,resultFile.results()){
				QString s = r.name();
				resultNames << s;
			}

			QHash<QString,double> params = parameterCombination;
			
			QString name;
			QHash<QString,double> row;
			foreach(row,readResultFilePHOTOSS_CSV(run + "/" + resultFile.filename())){
				foreach(name,row.keys()){
					double d = row[name];
					if(variationParmeters.contains(name))
						params[name] = d;
					else if(resultNames.contains(name))
						results[name] = d;
				}
				returnTmp[params] = results;
			}
		}
	}
	/********/
	if(parameterCombination.size() == parameters.size())
		returnTmp[parameterCombination] = results;
	return returnTmp;
}

QList< QHash< QString, double > > PJobFile::readResultFilePHOTOSS_CSV(QString resultFile){
	if(!m_data->contains(resultFile))
		throw QString("Result file %1 does not exist").arg(resultFile);
        istringstream sstream(m_data->readFile(resultFile).data());
        istream& stream = sstream;
        return resultsFromFile(stream);
}

void PJobFile::copyWithoutRuns(QString path)
{
	//Es wird zunächst eine leere .pjob-Datei im Zielverzeichnis erzeugt
	PJobFileFormat copy(path);

	//Anschließend werden alle Einträge bis auf die Runs kopiert
	foreach(QString s, m_data->content())
	{
		if(!s.startsWith("Runs/"))
			copy.appendRaw(m_data->readRaw(s));
	}
	//Änderungen übernehmen
	copy.flush();
}

void PJobFile::mergeRunsFrom(const PJobFile& otherPJob)
{
	foreach(QString run,otherPJob.runDirectoryEntries())
	{
		QByteArray neuer_run = otherPJob.m_data->readRaw(run);

		bool fine = false;

		//Wenn der Run noch nicht existiert wird er hinzugefügt (Standardfall)
		try
		{
			fine = m_data->appendRaw(neuer_run);
		}
		catch(RawDataError e)
		{
			//do something
		}
		
		//Ansonsten muss geguckt werden, ob die Runs binär unterschiedlich sind und der neue Run ggf. umbenannt werden
		//(Äußerst seltener, eher theoretischer, Fall!!)
			if((!fine) && (neuer_run != m_data->readRaw(run)))
			{
				//Es wird solange der letzte char des Datei-Strings geändert, bis sich die Dateinamen unterscheiden (ggf. wird angehängt)
				int len = neuer_run.indexOf('\n') - 1;
				int n = 0, slen = 1;
				QString neuer_string = QString(neuer_run.left(len+1));
				QString s = "";
				
				//Umbennen bis Dateinamen unterschiedlich
				do
				{
					s = (QString(n++));
					neuer_string.replace(len,slen,s);
					slen = s.length();
				}while(m_data->contains(neuer_string));

				//Hinzufügen
				QByteArray b;
				b.append(neuer_string);
				neuer_run.replace(0,len+slen,b);
				try
				{
					bool fine = m_data->appendRaw(neuer_run);
				}
				catch(RawDataError e)
				{
					//do something
				}
			}
	}
}

void PJobFile::checkIfRunIsProperlyFinished(QString run){
	if(m_data->content().contains(run+"/backup_log.txt"))
		throw QString(m_data->readFile(run+"/backup_log.txt"));
	foreach(PJobResultFile file, readResultDefinitions()){
		if( ! m_data->content().contains(run+"/"+file.filename()))
			throw QString("Result file "+file.filename()+" is missing!");
	}
}

void PJobFile::remove_all_runs(){
	QString what = "Removing all run directories...";
	QStringList runs = runDirectoryEntries();
	m_progresses_to_abort.remove(what);
	for(int i=0; i<runs.size(); i++){
		m_data->remove(runs.at(i));
		emit progress(what, i*100/runs.size());
		if(m_progresses_to_abort.contains(what)) break;
	}
	emit progress(what,100);
	emit changed();
}

void PJobFile::abort_progress(const QString &p){
	m_progresses_to_abort.insert(p);
}
