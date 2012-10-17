#pragma once
#include "Drawable.h"
#include <QtCore/QString>
class PlotWidget;
#define XMIN -50
#define XMAX 50
#define ZMIN 50
#define ZMAX -50
#define HEIGHT 75

class Box :	public Drawable
{
public:
	Box(PlotWidget* parent, double min, double max);
	virtual ~Box(void);

	void setPhoFile(QString);
	void setResultName(QString);
	void setAchsisNames(QString xachsis, QString yachsis);
	void setDimensions(double min, double max);

	void drawImplementation(QSet< QHash<QString,double> > combinationsInPlane, double minx, double maxx, double miny, double maxy);

private:
	unsigned int m_xsize;
	unsigned int m_zsize;
	float m_min;
	float m_max;
	PlotWidget *m_parent;
	QString m_resultName;
	QString m_xachsis;
	QString m_yachsis;
	QString m_phoFile;
};
