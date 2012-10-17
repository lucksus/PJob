#pragma once
#include "Drawable.h"
#include <QtCore/QString>
#include <QtCore/QHash>

class ResultVisualization :	public Drawable
{
public:
	ResultVisualization(QString phoFile, QString result, QHash<QString, double> parameters, QString xachsis, QString yachsis);
protected:
	void drawImplementation(QSet< QHash<QString,double> > combinationsInPlane, double minx, double maxx, double miny, double maxy);
private:
	void drawPillar(double height, double color);
	
};
