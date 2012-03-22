#include "Results.h"
#include <QtCore/QStringList>
#include <QtCore/QFile>

Results::Results(QString columnSep, QString rowSep, QString unit1Sep, QString unit2Sep)
{
	m_ResultLoaderAndSaver = new ResultLoaderAndSaver(columnSep,rowSep,unit1Sep,unit2Sep);
}

void Results::setSeperators(QString columnSep, QString rowSep, QString unit1Sep, QString unit2Sep)
{
	m_ResultLoaderAndSaver->setSeperators(columnSep,rowSep,unit1Sep,unit2Sep);
}

double Results::getValue(QString phoFile, QString result, QHash<QString,double> parameters) const{
	return m_results[phoFile][parameters][result];
}

QStringList Results::results() const{
	QStringList results;
	QHash< QHash<QString,double>, QHash<QString,double> > values;
	QHash<QString,double> resultValues;
	QString result;
	foreach(values, m_results.values())
		foreach(resultValues, values.values())
			foreach(result, resultValues.keys())
				results.append(result);
	return results;
}

QSet< QHash<QString,double> > Results::parameterCombinations() const{
	QSet< QHash<QString,double> > combinations;
	QHash< QHash<QString,double>, QHash<QString,double> > values;
	QHash<QString,double> combination;
	foreach(values, m_results.values())
		foreach(combination, values.keys())
			combinations.insert(combination);
	return combinations;
}

QSet< QHash<QString,double> > Results::parameterCombinationsFor(QString phoFile) const{
	QSet< QHash<QString,double> > combinations;
	QHash<QString,double> combination;
	foreach(combination, m_results[phoFile].keys())
		combinations.insert(combination);
	return combinations;
}

QSet<QString> Results::parametersFor(QString phoFile) const{
	QSet<QString> parameters;
	QString key;
	QHash<QString,double> parameterCombination;
	foreach(parameterCombination,parameterCombinationsFor(phoFile))
		foreach(key, parameterCombination.keys())
			parameters.insert(key);
	return parameters;
}

QSet<QString> Results::resultsFor(QString phoFile) const{
	QHash<QString,double> paramCombination;
	QSet<QString> resultNames;
	QString resultName;
	foreach(paramCombination, parameterCombinationsFor(phoFile))
		foreach(resultName, m_results[phoFile][paramCombination].keys())
			resultNames.insert(resultName);
	return resultNames;
}

QList<double> Results::valuesFor(QString phoFile, QString parameter) const{
	QList<double> values;
	QHash<QString,double> combination;
	foreach(combination,parameterCombinationsFor(phoFile))
		values.append(combination[parameter]);
	return values;
}

QStringList Results::phoFiles() const{
	return QStringList(m_results.keys());
}

void Results::newValues(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile){
	QHash<QString,double> parameterCombination;
	foreach(parameterCombination, values.keys()){
		QHash<QString,double> results = values[parameterCombination];
		m_results[phoFile][parameterCombination] = results;
		QString resultName;
		foreach(resultName, results.keys())
			emit newValueSet(phoFile, resultName, parameterCombination, results[resultName]);
	}
	emit newValuesSet(phoFile);
	clear_interpolation_function_cache();
}

QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > Results::importFromCSV(QString inputFile)
{
	QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > returnTmp = m_ResultLoaderAndSaver->importFromCSV(inputFile);

	//die Results müssen vor Rückgabe noch bekanntgegeben werden!
	foreach(QString jobFile, returnTmp.keys())
		newValues(returnTmp[jobFile],jobFile);
	return returnTmp;
}

void Results::exportToCSV(QString outputFile, QStringList pjobFiles)
{
	m_ResultLoaderAndSaver->exportToCSV(outputFile, pjobFiles, m_results);
}

double Results::getMinimumValue(QString phoFile, QString result, bool exclude_inf){
	QList<QHash<QString,double> > vs = m_results[phoFile].values();
	double min = vs.first()[result];
	QHash<QString,double> v;
	foreach(v,vs){
		double d = v[result];
		if(exclude_inf && (d==-numeric_limits<double>::infinity())) continue;
		if(d < min) min = d;
	}
	return min;
}

double Results::getMaximumValue(QString phoFile, QString result, bool exclude_inf){
	QList<QHash<QString,double> > vs = m_results[phoFile].values();
	double max = vs.first()[result];
	QHash<QString,double> v;
	foreach(v,vs){
		double d = v[result];
		if(exclude_inf && (d==numeric_limits<double>::infinity())) continue;
		if(d > max) max = d;
	}
	return max;
}


QSet< QHash<QString,double> > Results::combinationsInPlane(QString phoFile, QString xachsis, QString yachsis, const QHash<QString,double> otherParams){
	QSet< QHash<QString,double> > combinations = parameterCombinationsFor(phoFile);
	QSet< QHash<QString,double> > combinationsInPlane;
	QHash<QString,double> c;
	foreach(c,combinations){
		QString parameter;
		QHash<QString,double> newCombination;
		bool ok = true;
		foreach(parameter,c.keys()){
			if(parameter == xachsis || parameter == yachsis)continue;
			if(c[parameter] != otherParams[parameter]) ok = false;
		}
		if(ok) combinationsInPlane.insert(c);
	}
	return combinationsInPlane;
}

double Results::minParameterValue(QSet< QHash<QString,double> > parameterCombinations, QString parameterName){
	double min;
	bool firstTime=true;
	QHash<QString,double> c;
	foreach(c,parameterCombinations){
		if(firstTime){
			min = c[parameterName];
			firstTime=false;
		}else{
			if(c[parameterName] < min) min = c[parameterName];
		}
	}
	return min;
}

double Results::maxParameterValue(QSet< QHash<QString,double> > parameterCombinations, QString parameterName){
	double max;
	bool firstTime=true;
	QHash<QString,double> c;
	foreach(c,parameterCombinations){
		if(firstTime){
			max = c[parameterName];
			firstTime=false;
		}else{
			if(c[parameterName] > max) max = c[parameterName];
		}
	}
	return max;
}

InterpolationFunction* Results::interpolation_function(QString pjob_file, QString result){
	std::pair<QString,QString> p(pjob_file,result);
	if(m_interpolation_functions_cache.find(p) == m_interpolation_functions_cache.end()) m_interpolation_functions_cache[p] = new InterpolationFunction(pjob_file, result);
	return m_interpolation_functions_cache[p];
}

void Results::clear_interpolation_function_cache(){
	std::pair<QString,QString> p;
	foreach(p, m_interpolation_functions_cache.keys()){
		delete m_interpolation_functions_cache[p];
	}
	m_interpolation_functions_cache.clear();
}
