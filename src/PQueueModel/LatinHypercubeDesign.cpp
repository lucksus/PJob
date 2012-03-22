#include "LatinHypercubeDesign.h"
#include "RandomGenerator.h"
#include <QtCore/QStringList>
#define PERMUTATION_COUNT 100

LatinHypercubeDesign::LatinHypercubeDesign(QHash< QString, QPair<double,double> > region, unsigned int sampleCount)
{
	QVector<QVector<unsigned int> > design = createIndexMatrix(region.size(),sampleCount);
	for(int i=0;i<design.size();++i)
                for(unsigned int j=0;j<PERMUTATION_COUNT*sampleCount;++j)
			design[i] = permute(design[i]);
	m_points = designToPoints(design, region);
}

QVector<QVector<unsigned int> > LatinHypercubeDesign::createIndexMatrix(unsigned int dimensions, unsigned int samples){
	QVector< QVector< unsigned int > > indices;
	indices.resize(dimensions);
	for(int i=0;i<indices.size();++i){
		indices[i].resize(samples);
		for(int j=0;j<indices[i].size();++j)
			indices[i][j]=j;
	}
	return indices;
}

QVector<unsigned int> LatinHypercubeDesign::permute(QVector<unsigned int> in){
	unsigned int x = static_cast<unsigned int>(PQueueMath::RandomGenerator::uniform(0,in.size()));
	unsigned int y = static_cast<unsigned int>(PQueueMath::RandomGenerator::uniform(0,in.size()));
	unsigned int temp = in[x];
	in[x] = in[y];
	in[y] = temp;
	return in;
}

QVector< QHash<QString, double> > LatinHypercubeDesign::designToPoints(QVector< QVector<unsigned int> > design, QHash< QString, QPair<double,double> > region){
	QVector< QHash<QString, double> > result;
	QStringList parameters = region.keys();
	qSort(parameters);
	result.resize(design[0].size());
	for(int i=0;i<design[0].size();++i){
		QString p;
		int j=0;
		foreach(p, parameters){
			result[i][p] = region[p].first + (static_cast<double>(design[j][i])/design[0].size() * (region[p].second - region[p].first));
			++j;
		}
	}
	return result;
}

QVector< QHash<QString, double> > LatinHypercubeDesign::getDesignPoints(){
	return m_points;
}
