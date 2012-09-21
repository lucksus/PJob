#include "InterpolationFunction.h"
#include <QtGlobal>
#include <math.h>
#include <QtCore/QStringList>
#include "Workspace.h"
#include "SimulatedAnnealing.h"
#include <set>
#include <limits>
#include <boost/foreach.hpp>

#ifndef Q_OS_WIN
//Optimierer: min_a (A*a - y)
extern "C"{
	int sgels_(char *trans, int *m, int *n, int * nrhs,
		float *a, int *lda, float *b, int *ldb, float *work, 
		int *lwork, int *info);

	int dgels_(char *trans, int *m, int *n, int * nrhs,
		double *a, int *lda, double *b, int *ldb, 
		double *work, int *lwork, int *info);
}
#endif


InterpolationFunction::InterpolationFunction(QString pjob_file, QString result)
: m_pjob_file(pjob_file), m_result(result), m_mu(0.5), m_d(0)
{
	buildParameterSorting();
	m_interpolationPoints = interpolationPoints();
	m_interpolationValues = interpolationValues();
	Q_ASSERT(m_interpolationPoints.size() == m_interpolationValues.size());
	remove_infs_and_nans(m_interpolationPoints, m_interpolationValues);
	Q_ASSERT(m_interpolationPoints.size() == m_interpolationValues.size());
	BOOST_FOREACH(std::vector<double>& point, m_interpolationPoints){
		Q_ASSERT(point.size() == m_interpolationPoints[0].size());
	}

	m_xarr = new double[m_interpolationPoints[0].size()];

	calculateMeanDistance();
	calculateAlphas(m_interpolationPoints,m_interpolationValues);
}

void InterpolationFunction::buildParameterSorting(){
	QSet< QHash<QString,double> > parameterCombinations = Workspace::getInstace().getResults().parameterCombinationsFor(m_pjob_file);
	QSet< QString > parameterNames;
	QHash<QString,double> combination;
	foreach(combination, parameterCombinations){
		QString parameterName;
		foreach(parameterName, combination.keys())
			parameterNames.insert(parameterName);
	}
	QStringList sortedParameterNames = parameterNames.toList();
	qSort(sortedParameterNames);
	for(int i=0;i<sortedParameterNames.size();++i)
		m_parameterSorting[sortedParameterNames.at(i)] = i;
}

vector< vector<double> > InterpolationFunction::interpolationPoints(){
	vector< vector<double> > result;
	QSet< QHash<QString,double> > parameterCombinations = Workspace::getInstace().getResults().parameterCombinationsFor(m_pjob_file);
	QHash<QString,double> combination;
	foreach(combination, parameterCombinations){
		QString parameterName;
		vector<double> parameterValues;
		parameterValues.resize(combination.size());
		foreach(parameterName, combination.keys())
			parameterValues[m_parameterSorting[parameterName]] = combination[parameterName];
		result.push_back(parameterValues);
	}
	return result;
}

vector<double> InterpolationFunction::interpolationValues(){
	vector<double> result;
	QSet< QHash<QString,double> > parameterCombinations = Workspace::getInstace().getResults().parameterCombinationsFor(m_pjob_file);
	QHash<QString,double> combination;
	foreach(combination, parameterCombinations){
		result.push_back(Workspace::getInstace().getResults().getValue(m_pjob_file,m_result,combination));
	}
	return result;
}

InterpolationFunction::~InterpolationFunction(){
	delete[] m_xarr;
}

double InterpolationFunction::getValue(QHash< QString, double > parameters){
	if(m_interpolationPoints.size() == 0) return 0;
    Q_ASSERT(parameters.size() == static_cast<int>(m_interpolationPoints[0].size()));
	QString parameterName;
	foreach(parameterName, parameters.keys())
	m_xarr[m_parameterSorting[parameterName]] = parameters[parameterName];

	return f(m_xarr);
}

double InterpolationFunction::f(double* x){
	//if(!m_hasSolution) return 0;

		double sum=0;
//		//x skalieren:
//		Data* main = Schlangensicht::getInstance().getMainData();
//		for(unsigned int i=0; i < m_scaledPoints[0].size(); ++i){
//			x[i] = (x[i] - main->getMinForVariable(main->getVariables()[i])) /
//				(main->getMaxForVariable(main->getVariables()[i]) - main->getMinForVariable(main->getVariables()[i]));
//		}

		for(unsigned int i=0; i < m_interpolationPoints.size(); ++i)
			sum += m_alphas[i] * R(i,m_interpolationPoints, x);

//		//Ausgabe skalieren:
//		sum = m_interpolationValues->getMin() + sum*(m_interpolationValues->getMax() - m_interpolationValues->getMin());
		return sum;
}


void InterpolationFunction::calculateAlphas(vector<vector<double> > points, vector<double> values){
	unsigned int size = points.size();
	int* pivot = new int[size];
	double* b = new double[size];
	double* AT = new double[size*size];

	for(unsigned int i=0;i<size;++i)
		b[i] = values[i];

	unsigned int dim = points[0].size();
	double* arr = new double[dim];

	// to call a Fortran routine from C we
	// have to transform the matrix
	// c: spaltenweise
	// fortran: zeilenweise
	for (unsigned int i=0; i<size; i++)
		for(unsigned int j=0; j<size; j++){
			for(unsigned int x=0; x<dim; x++)
				arr[x] = points[j][x];
			AT[j+size*i]=R(i,points,arr);	
		}
		delete[] arr;

		int info;


		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		//---------------------- LAPACK: min_a (A*a - y) ----------------------------
		//INPUT: A in AT, y in b
		//OUTPUT: a in b
		int m,n;
		m=n=size;
		int nrhs=1;
		int lwork=m*n;
		double* work2 = new double[m*n + m*n];
                int lwork2=2*m*n;
#ifndef Q_OS_WIN
                dgels_("N",&m,&n,&nrhs,AT,&m,b,&m,work2,&lwork2,&info);
#endif
		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------

		m_alphas.resize(size);
        for(unsigned int i=0;i<size;++i){
#ifndef Q_OS_WIN
			if(0 == info){//Ergebnis nun in b
				m_alphas[i] = b[i];
            }else
#endif
				m_alphas[i] = 0;
		}

		delete[] pivot;
		delete[] b;
		delete[] AT;
		delete[] work2;
}



double InterpolationFunction::R(unsigned int di, const vector<vector<double> >& points, double* xj){
	float distance_sq = 0;
	float result_sq;

	for(unsigned int i=0; i < points[0].size(); ++i)
		distance_sq += (points[di][i]-xj[i])*(points[di][i]-xj[i]);

	result_sq = distance_sq + m_d*m_meanDistanceOfInterpolationPoints*m_d*m_meanDistanceOfInterpolationPoints;

	return pow(double(result_sq), double(m_mu));
}

double InterpolationFunction::distance(vector<double> x,vector<double> y){
	float sum=0;
	for(unsigned int i=0; i < x.size(); ++i)
		sum += (x[i]-y[i])*(x[i]-y[i]);
	return sqrt(sum);
}

void InterpolationFunction::calculateMeanDistance(){
	unsigned int size = m_interpolationPoints.size();
	float meanDistance=0;
	for(unsigned int i=0;i<size;++i){
		float min;
		bool firstTime = true;
		for(unsigned int j=0;j<size;++j)
			if(j!=i){
				float d = distance(m_interpolationPoints[i],m_interpolationPoints[j]);
				if(firstTime || d<min){
					min = d;
					firstTime = false;
				}
			}
			meanDistance += min;
	}

	m_meanDistanceOfInterpolationPoints = meanDistance / size;
}


QString InterpolationFunction::pjob_file(){
	return m_pjob_file;
}

QString InterpolationFunction::result(){
	return m_result;
}

QDoubleHash InterpolationFunction::findMinimum(){
	SimulatedAnnealing sa(this);
	return sa.findMinimum();
}

QDoubleHash InterpolationFunction::findMaximum(){
	SimulatedAnnealing sa(this);
	return sa.findMaximum();
}

void InterpolationFunction::remove_infs_and_nans(vector< vector<double> >& points, vector<double>& values){
	set<unsigned int> indices_to_remove;
	for(unsigned int i=0;i<values.size();i++){
		if((values[i] != values[i]) || (values[i] == numeric_limits<double>::infinity())) indices_to_remove.insert(i);
	}

	vector< vector<double> > new_points;
	vector<double> new_values;

	for(unsigned int i=0;i<values.size();i++){
		if(indices_to_remove.count(i)) continue;
		new_points.push_back(points[i]);
		new_values.push_back(values[i]);
	}

	points = new_points;
	values = new_values;
}
