#pragma once
#include <vector>
#include <QtCore/QString>
#include <QtCore/QHash>
#include "ScriptFunctions.h"
using namespace std;

/*!
* Scattered data interpolation for given result values.
* Uses Hardy Multiquadric method to provide estimation for unknown values.
*
* Is initialized with the name of an existing result/pho file pair.
* Known values for this result are read from the Results object to calculate
* a Hardy Multiquadric (essentialy the alphas, i.e. m_alphas, see calculateAlphas()).
* After this a value for every parameter combination is provided by
* method getValue(QHash<QString,double>).
*/
class InterpolationFunction : public QObject
{
Q_OBJECT
public:
	InterpolationFunction(QString pjob_file, QString result);
	~InterpolationFunction();

	Q_INVOKABLE double getValue(QHash< QString, double >);
	Q_INVOKABLE QDoubleHash findMinimum();
	Q_INVOKABLE QDoubleHash findMaximum();

	QString pjob_file();
	QString result();

private:

	QString m_pjob_file;
	QString m_result;

	vector< vector<double> > interpolationPoints();
	vector< vector<double> > m_interpolationPoints;

	vector<double> interpolationValues();
	vector< double > m_interpolationValues;

	void calculateAlphas(vector<vector<double> > points, vector<double> values);
	vector< double > m_alphas;
	
	//! Fills m_parameterSorting 
	void buildParameterSorting();

	//! Map: Parametername -> Index (0...n)
	QHash< QString, unsigned int > m_parameterSorting;

	//! Parameter of Hardy Multiquadric method
	double m_mu;

	//! Parameter of Hardy Multiquadric method
	double m_d;

	//! Temporary
	double *m_xarr;

	//! Sets m_meanDistanceOfInterpolationPoints
	void calculateMeanDistance();
	double m_meanDistanceOfInterpolationPoints;
	
	
	double f(double* x);
	double R(unsigned int di, const vector<vector<double> >&, double* xj);
	double distance(vector<double>,vector<double>);

	static void remove_infs_and_nans(vector< vector<double> >& points, vector<double>& values);
};
