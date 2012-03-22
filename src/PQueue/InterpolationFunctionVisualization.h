#pragma once
#include "Drawable.h"
#include "InterpolationFunction.h"
class InterpolationFunctionVisualization : public Drawable
{
public:
	InterpolationFunctionVisualization(QString phoFile, QString result, QHash<QString, double> parameters, QString xachsis, QString yachsis);
	~InterpolationFunctionVisualization(void);

protected:
	virtual void drawImplementation(QSet< QHash<QString,double> > combinationsInPlane, double minx, double maxx, double miny, double maxy);

private:
	InterpolationFunction* m_interpolant;
};
