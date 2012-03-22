#pragma once
#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtCore/QVector>

class LatinHypercubeDesign
{
public:
	LatinHypercubeDesign(QHash< QString, QPair<double,double> > region, unsigned int sampleCount);
	QVector< QHash<QString, double> > getDesignPoints();

private:
	QVector< QHash<QString, double> > m_points;
	QVector<unsigned int> permute(QVector<unsigned int>);
	QVector<QVector<unsigned int> > createIndexMatrix(unsigned int dimensions, unsigned int samples);
	QVector< QHash<QString, double> > designToPoints(QVector< QVector<unsigned int> > design, QHash< QString, QPair<double,double> > region);
};
