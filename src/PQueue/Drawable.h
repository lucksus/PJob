#pragma once
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QHash>
#include "Workspace.h"

class Drawable
{
public:
        Drawable(){}
        virtual ~Drawable(){}
	Drawable(QString phoFile, QString result, QHash<QString, double> parameters, QString xachsis, QString yachsis);

	void setMinMax(double min, double max);
	void draw();

protected:

	virtual void drawImplementation(QSet< QHash<QString,double> > combinationsInPlane, double minx, double maxx, double miny, double maxy) = 0;

	double getValue(QHash<QString,double>);
	double getGLHeight(QHash<QString,double>);
	double getGLHeight(double);
	double getGLValueX(QHash<QString,double>);
	double getGLValueY(QHash<QString,double>);

	QString m_phoFile;
	QString m_result;
	QString m_xachsis;
	QString m_yachsis;
	QHash<QString,double> m_parameters;
	double m_min;
	double m_max;
	double m_minx,m_maxx,m_miny,m_maxy;
};
