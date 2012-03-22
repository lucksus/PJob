#include "ResultLoaderAndSaver.h"

ResultLoaderAndSaver::ResultLoaderAndSaver(QString columnSep, QString rowSep, QString unit1Sep, QString unit2Sep)
{
	setSeperators(columnSep,rowSep,unit1Sep,unit2Sep);
}

ResultLoaderAndSaver::~ResultLoaderAndSaver()
{
}

void ResultLoaderAndSaver::setSeperators(QString columnSep, QString rowSep, QString unit1Sep, QString unit2Sep)
{
	//Spaltenseparator
	if(columnSep != NULL)
		m_seperators.column = columnSep;
	else if(m_seperators.column == NULL)
		m_seperators.column = '\t';

	//Zeilenseparator
	if(rowSep != NULL)
		m_seperators.row = rowSep;
	else if(m_seperators.row == NULL)
		m_seperators.row = '\n';

	//Einheitenseparator links
	if(unit1Sep != NULL)
		m_seperators.unit1 = unit1Sep;
	else if(m_seperators.unit1 == NULL)
		m_seperators.unit1 = ',';

	//Einheitenseparator rechts
	if(unit2Sep != NULL)
		m_seperators.unit2 = unit2Sep;
	else if(m_seperators.unit2 == NULL)
		m_seperators.unit2 = "";
}

QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > ResultLoaderAndSaver::importFromCSV(QString inputFile)
{
	//Datei Öffnen
	QFile file(inputFile);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QByteArray data = file.readAll(); 

	//Header parsen
	QHash< QString,std::pair<QStringList,QStringList> > headerLines = parseHeaderLines(data);

	data.remove(0,9);
	QStringList varList = parseLine(data);

	//Bevor es weitergeht kann bereits auf Konsistenz geprüft werden!

	//FEHLT NOCH
	
	//Nun könnten die Werte ausgelesen werden:
	QList< std::pair<QString,QStringList> > values;
	
	//über alle Zeilen iterieren
	while(!data.isEmpty())
	{
		//Kommentare ignorieren
		if(data.startsWith('%'))
		{
			int row = data.indexOf(m_seperators.row);
			data.remove(0,data.indexOf(row+1));
			continue;
		}
		
		//Parsen
		values.append(parseValueLine(data));
	}

	//Nun wurde das gesamte File geparst! Jetzt müssen die Werte nur noch richtig zusammengebastelt werden.
	//
	//header(1...n):	QHash<QString, std::pair<QStringList,QStringList> > headerLines;
	//header(n+1):		QStringList varList;
	//Werte:			QList< std::pair<QString,QStringList> > values;
	//
	//verpacken zu:
	//
	//QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > m_results;
	//
	//klingt leichter als es ist...
	
	int n = varList.length();
	QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > valuesForFile;

	//über die einzelnen Wertezeilen iterieren
	std::pair<QString,QStringList> line;
	foreach(line, values)
	{
		QHash<QString,double> parameterCombination;
		QHash<QString,double> resultCombination;
		QString jobFile = line.first;

		//über die Spalten iterieren
		for(int i=0;i<n;i++)
		{
			QString name = varList.at(i);
			QString valueString = line.second.at(i);
			double value;

			//INF- und NaN-Werte abfragen
			if(valueString == "inf")
				value = std::numeric_limits<double>::infinity();
			else if(valueString == "nan")
				value = std::numeric_limits<double>::quiet_NaN();
			else
				value = valueString.toDouble();

			//Fehlerfälle noch berücksichtigen!!!

			//wenn die Variable der Spalte bekannt ist => result oder parameter?
			if(headerLines[jobFile].first.contains(name))
				parameterCombination[name] = value;
			else if(headerLines[jobFile].second.contains(name))
				resultCombination[name] = value;
		}
		//nun wird die Zeile zu den results hinzugefügt
		valuesForFile[jobFile].insert(parameterCombination,resultCombination);
	}
	return valuesForFile;
}

void ResultLoaderAndSaver::exportToCSV(QString outputFile, QStringList pjobFiles,QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > const &results)
{
	//nichts zu tun
	if(pjobFiles.isEmpty()) 
		return;

	QFile file(outputFile);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
	QTextStream out(&file);
	pjobFiles.sort();
	std::pair<QStringList,QStringList> allColumnsList;

	//die ersten n Header-Zeilen schreiben
	foreach(QString jobFile, pjobFiles)
	{
		out << '%' << jobFile;

		//Mergen fü die n+1. Zeile
		std::pair<QStringList,QStringList> header = headerForResult(jobFile,results);
		allColumnsList.first.append(header.first);
		allColumnsList.second.append(header.second);

		//Parameter auflisten (aktuelle Zeile)
		foreach(QString s,header.first)
			out << m_seperators.column << s;
		
		//Zwischentab
		out << m_seperators.column;
		
		//Results auflisten (aktuelle Zeile)
		foreach(QString s,header.second)
			out << m_seperators.column << s;

		out << m_seperators.row;
	}
	
	//gleichnamige Parameter/Results zusammenführen, aschließend sortieren
	//kann auf Wunsch später auskommentiert werden => keine anderen Anpassungen nötig
	allColumnsList.first.removeDuplicates();
	allColumnsList.second.removeDuplicates();
	allColumnsList.first.sort();
	allColumnsList.second.sort();

	//n+1. Zeile schreiben
	writeHeader(out,allColumnsList);

	//Werte schreiben
	foreach(QString jobFile, pjobFiles)
	{
		QHash<QString,double> parameterCombination;
		foreach(parameterCombination, results[jobFile].keys())
		{
			//Anm.:jobFile enthält absolute Pfade
			//auf Wunsch kann hier jobFile.section('/',-1,-1) übergeben werden 
			//um nur den Dateinamen zu behalten!
			out << jobFile;
			writeLine(out, allColumnsList, parameterCombination, results[jobFile][parameterCombination]);
		}
	}
}

//PRIVATE:

QHash<QString, std::pair<QStringList,QStringList> > ResultLoaderAndSaver::parseHeaderLines(QByteArray& data)
{
	QHash<QString, std::pair<QStringList,QStringList> > returnTmp;
	
	//solange noch Headerzeilen anstehen (1...n)    //n+1 wird abgefangen
	while(data.startsWith('%'))
	{
		//wenn es sich um die letzte Headerzeile handelt, startet diese mit "PJobFile" + Spalten-Trennzeichen
		if(data.left(10)=="%PJobFile" + m_seperators.column)
			//Da dieser Fall bei validen Dateien am Ende _immer_ auftritt => Behandlung außerhalb der Schleife
			break;

		//Kommentarzeichen entfernen
		data.remove(0,1);
		
		QStringList line = parseLine(data);
		int middle = line.indexOf("");

		//Liste splitten
		returnTmp[line.first()].first=line.mid(1,middle-1);
		returnTmp[line.first()].second=line.mid(middle+1);

		foreach(QString s, line)
			s;
	}

	return returnTmp;
}

QStringList ResultLoaderAndSaver::parseLine(QByteArray &data)
{
	QStringList returnTmp;
	int col, row = data.indexOf(m_seperators.row);

	while(row>0)
	{
		if(data.left(m_seperators.column.length())==m_seperators.column)
			data.remove(0,1);
		col = data.indexOf(m_seperators.column);
		row = data.indexOf(m_seperators.row);
		
		//parsen
		QString parsed;
		if((col<row) && (col>=0))
		{
			//noch weitere Strings
			parsed = data.left(col);
			data.remove(0,col);
		}
		else
		{
			//letzter String
			parsed = data.left(row);
			data.remove(0,row);
			row = 0;
		}
		
		returnTmp.append(parsed);
	}
	data.remove(0,1);
	return returnTmp;
}

std::pair<QString,QStringList> ResultLoaderAndSaver::parseValueLine(QByteArray &data)
{
	std::pair<QString,QStringList> returnTmp;
	int col = data.indexOf(m_seperators.column);

	returnTmp.first = data.left(col);
	data.remove(0,col);
	
	returnTmp.second=parseLine(data);

	return returnTmp;
}

std::pair<QStringList,QStringList> ResultLoaderAndSaver::headerForResult(QString result,QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > const &n_results){
	QSet<QString> parameters;
	QSet<QString> results;

	QHash<QString,double> parameterCombination;
	foreach(parameterCombination, n_results[result].keys()){
		QString parameter;
		foreach(parameter, parameterCombination.keys())
			parameters.insert(parameter);
		QString resultName;
		foreach(resultName,  n_results[result][parameterCombination].keys())
			results.insert(resultName);
	}

	std::pair<QStringList,QStringList> p(parameters.toList(),results.toList());
	p.first.sort();
	p.second.sort();
	return p;
}

void ResultLoaderAndSaver::writeHeader(QTextStream& outStream, std::pair<QStringList,QStringList> header){
	outStream << "%PJobFile";
	QString p;
	foreach(p, header.first)
		outStream << m_seperators.column << p;
	foreach(p, header.second)
		outStream << m_seperators.column << p.replace(',',m_seperators.unit1) << m_seperators.unit2;
	outStream << m_seperators.row;
}

void ResultLoaderAndSaver::writeLine(QTextStream& outStream, std::pair<QStringList,QStringList> header, QHash<QString,double> parameters, QHash<QString,double> results){
	foreach(QString p, header.first)
		if(parameters[p])
			outStream << m_seperators.column << parameters[p];
		else
			outStream << m_seperators.column;
	foreach(QString r, header.second)
		if(results[r])
			outStream << m_seperators.column << results[r];
		else
			outStream << m_seperators.column;
	outStream << m_seperators.row;
}
