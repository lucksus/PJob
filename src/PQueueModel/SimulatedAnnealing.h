#pragma once
#include "InterpolationFunction.h"
#include <vector>

class SimulatedAnnealing{
public:
	SimulatedAnnealing(InterpolationFunction*);

	QHash< QString, double > findMinimum();
	QHash< QString, double > findMaximum();

private:
	InterpolationFunction* m_interpolant;

	QHash< QString, double > find();

	QHash< QString, double > generateTry(const QHash< QString, double >&);
	QHash< QString, double > generateStartPosition();
	bool withProbEToTheMinus(const float &x);

	bool better(double newValue, double oldValue);

	bool m_searchMinimum;


	unsigned int m_maxRuns;
	double m_startTemp;
	double m_FAC;
};
