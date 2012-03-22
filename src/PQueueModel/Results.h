#pragma once
#include "ResultLoaderAndSaver.h"
#include "InterpolationFunction.h"
#include <QtCore/QMap>
using namespace std;

class Results : public QObject
{
Q_OBJECT

public:
        Results(QString columnSep = QString::null, QString rowSep = QString::null, QString unit1Sep = QString::null, QString unit2Sep = QString::null);

	double getValue(QString phoFile, QString result, QHash<QString,double> parameters) const;
	QStringList results() const;
	QSet< QHash<QString,double> > parameterCombinations() const;
	QSet< QHash<QString,double> > parameterCombinationsFor(QString phoFile) const;
	QSet<QString> parametersFor(QString phoFile) const;
	QSet<QString> resultsFor(QString phoFile) const;
	QList<double> valuesFor(QString phoFile, QString parameter) const;
	QStringList phoFiles() const;
	double getMinimumValue(QString phoFile, QString result, bool exclude_inf = false);
	double getMaximumValue(QString phoFile, QString result, bool exclude_inf = false);
	QSet< QHash<QString,double> > combinationsInPlane(QString phoFile, QString xachsis, QString yachsis, const QHash<QString,double> otherParams);

	static double minParameterValue(QSet< QHash<QString,double> > parameterCombinations, QString parameterName);
	static double maxParameterValue(QSet< QHash<QString,double> > parameterCombinations, QString parameterName);

	void setSeperators(QString columnSep = NULL, QString rowSep = NULL, QString unit1Sep = NULL, QString unit2Sep = NULL);

	InterpolationFunction* interpolation_function(QString pjob_file, QString result);

public slots:
	void newValues(QHash< QHash<QString,double>, QHash<QString,double> > values, QString phoFile);
	void exportToCSV(QString outputFile, QStringList pjobFiles);
	QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > importFromCSV(QString inputFile);

signals:
	void newValueSet(QString phoFile, QString result, QHash<QString,double> parameters, double value);
	void newValuesSet(QString phoFile);

private:
	/*!
	* PJobFile ->
	*	( ParameterKombination -> (ResultName->ResultWert) )
	*/

	QHash<
		QString,
		QHash< QHash<QString,double>, QHash<QString,double> >
		>
		m_results;

	QMap<std::pair<QString,QString>, InterpolationFunction*> m_interpolation_functions_cache;
	void clear_interpolation_function_cache();

	ResultLoaderAndSaver* m_ResultLoaderAndSaver;
};
