#include "Box.h"
#include <QtOpenGL/qgl.h>
#include <limits>
#include <sstream>
#include <assert.h>
#include "PlotWidget.h"
#include "Workspace.h"
#ifdef Q_OS_WIN
#include <gl/glu.h>
#endif
#ifdef Q_OS_UNIX
#ifdef Q_OS_MACX
#include <glu.h>
#else
#include <GL/glu.h>
#endif
#endif

using namespace std;

Box::Box(PlotWidget* parent, double min, double max)
: m_min(min), m_max(max), m_parent(parent)
{
}

Box::~Box(void)
{
}

void Box::setResultName(QString resultName){
	m_resultName = resultName;
}

void Box::setDimensions(double min, double max){
	m_min = min;
	m_max = max;
}

void Box::drawImplementation(QSet< QHash<QString,double> >,double,double,double,double){
	//Rahmen:
	glColor3f(0.f,0.f,0.f);
	glBegin(GL_LINES);
	glVertex3f(XMIN, 0.f, ZMIN);
	glVertex3f(XMAX, 0.f, ZMIN);
	glVertex3f(XMIN, 0.f, ZMAX);
	glVertex3f(XMAX, 0.f, ZMAX);
	glVertex3f(XMIN, HEIGHT, ZMIN);
	glVertex3f(XMAX, HEIGHT, ZMIN);
	glVertex3f(XMIN, HEIGHT, ZMAX);
	glVertex3f(XMAX, HEIGHT, ZMAX);

	glVertex3f(XMIN, 0.f, ZMIN);
	glVertex3f(XMIN, 0.f, ZMAX);
	glVertex3f(XMAX, 0.f, ZMIN);
	glVertex3f(XMAX, 0.f, ZMAX);
	glVertex3f(XMIN, HEIGHT, ZMIN);
	glVertex3f(XMIN, HEIGHT, ZMAX);
	glVertex3f(XMAX, HEIGHT, ZMIN);
	glVertex3f(XMAX, HEIGHT, ZMAX);

	glVertex3f(XMIN, 0.f, ZMAX);
	glVertex3f(XMIN, HEIGHT, ZMAX);
	glVertex3f(XMAX, 0.f, ZMAX);
	glVertex3f(XMAX, HEIGHT, ZMAX);
	glVertex3f(XMIN, 0.f, ZMIN);
	glVertex3f(XMIN, HEIGHT, ZMIN);
	glVertex3f(XMAX, 0.f, ZMIN);
	glVertex3f(XMAX, HEIGHT, ZMIN);
	glEnd();

	glDisable(GL_CULL_FACE);

	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_QUADS);
	glVertex3f(XMIN, 0.f, ZMIN);
	glVertex3f(XMIN, 0.f, ZMAX);
	glVertex3f(XMAX, 0.f, ZMAX);
	glVertex3f(XMAX, 0.f, ZMIN);
	glEnd();

	glEnable(GL_CULL_FACE);



	//Beschriftung:
	QFont font;
	font.setPixelSize(11);
	font.setBold(false);

	glColor3f(0.0f,0.0f,0.0f);
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetIntegerv (GL_VIEWPORT, viewport);
	glGetDoublev (GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev (GL_PROJECTION_MATRIX, projection);
	GLdouble x1,x2,x3;	//camera position
	GLdouble z1,z2,z3;	//direction vector

	int h = m_parent->QWidget::height();



	GLdouble a1,a2,a3;
	GLdouble b1,b2,b3;
	GLdouble c1,c2,c3;
	GLdouble d1,d2,d3;
	gluProject(XMIN,0.f,ZMIN, modelview, projection, viewport, &a1, &a2, &a3);
	gluProject(XMIN,0.f,ZMAX, modelview, projection, viewport, &b1, &b2, &b3);
	gluProject(XMAX,0.f,ZMIN, modelview, projection, viewport, &c1, &c2, &c3);
	gluProject(XMAX,0.f,ZMAX, modelview, projection, viewport, &d1, &d2, &d3);
	float x,z,x_dir,z_dir;
	if(a1>b1)
		if(c1>d1)
			if(a1>c1){
				x=XMIN;z=ZMIN;x_dir=-1;z_dir=1;
			}else{
				x=XMAX;z=ZMIN;x_dir=1;z_dir=1;
			}
		else
			if(a1>d1){
				x=XMIN;z=ZMIN;x_dir=-1;z_dir=1;
			}else{
				x=XMAX;z=ZMAX;x_dir=1;z_dir=-1;
			}
	else
		if(c1>d1)
			if(b1>c1){
				x=XMIN;z=ZMAX;x_dir=-1;z_dir=-1;
			}else{
				x=XMAX;z=ZMIN;x_dir=1;z_dir=1;
			}
		else
			if(b1>d1){
				x=XMIN;z=ZMAX;x_dir=-1;z_dir=-1;
			}else{
				x=XMAX;z=ZMAX;x_dir=1;z_dir=-1;
			}


	//Funktionswerte:
	glColor3f(0.0f,0.0f,0.0f);
	gluProject(x+15*x_dir, HEIGHT / 2, z+15*z_dir, modelview, projection, viewport, &x1, &x2, &x3);
        m_parent->renderText(static_cast<int>(x1),static_cast<int>(h-x2),m_resultName,font);

	if(m_max>m_min && m_max != numeric_limits<float>::infinity() && m_max != -numeric_limits<float>::infinity()
		&& m_min != numeric_limits<float>::infinity() && m_min != -numeric_limits<float>::infinity())
		for(float f=m_min;f<=m_max;f+=(m_max-m_min)/5){
			gluProject(x+5*x_dir, HEIGHT*((f-m_min)/(m_max-m_min))-5, z+5*z_dir, modelview, projection, viewport, &x1, &x2, &x3);
			std::stringstream s;
			s.precision(3);
			s << f;
			glColor3f(0.f,0.f,0.f);
                        m_parent->renderText(static_cast<int>(x1),static_cast<int>(h-x2),s.str().c_str(),font);
			glColor3f(0.4f,0.4f,0.4f);
			glBegin(GL_LINES);
			glVertex3f(x+3*x_dir, HEIGHT*((f-m_min)/(m_max-m_min)), z+3*z_dir);
			glVertex3f(x+1*x_dir, HEIGHT*((f-m_min)/(m_max-m_min)), z+1*z_dir);
			glEnd();
		}

	//X-Achsenbeschriftung:
	glColor3f(0.0f,0.0f,0.0f);
	gluProject(XMAX / 2, -25.f, ZMIN + 10.f, modelview, projection, viewport, &x1, &x2, &x3);
        m_parent->renderText(static_cast<int>(x1),static_cast<int>(h-x2),m_xachsis,font);

	QList<double> xachsisValues = Workspace::getInstace().getResults().valuesFor(m_phoFile, m_xachsis);
	if(!xachsisValues.isEmpty()){
		qSort(xachsisValues);
		double xmin = xachsisValues.first();
		double xmax = xachsisValues.last();
		double d;
		foreach(d,xachsisValues){
			double x = XMIN + (XMAX - XMIN)*(d-xmin)/(xmax-xmin);
			if(xmin==xmax) x = XMIN + (XMAX-XMIN)/2;
			gluProject(x, -10.f, ZMIN, modelview, projection, viewport, &x1, &x2, &x3);
			std::stringstream s;
			s.precision(3);
			s << d;
			glColor3f(0.f,0.f,0.f);
                        m_parent->renderText(static_cast<int>(x1),static_cast<int>(h-x2),s.str().c_str(),font);
			glColor3f(0.4f,0.4f,0.4f);
			glBegin(GL_LINES);
			glVertex3f(x, -3.f, ZMIN);
			glVertex3f(x, -6.f, ZMIN);
			glEnd();
		}
	}

	//Z-Achsenbeschriftung:
	gluProject(XMIN - 10.f, -25.f, ZMAX / 2, modelview, projection, viewport, &z1, &z2, &z3);
	glColor3f(0.f,0.f,0.f);
        m_parent->renderText(static_cast<int>(z1),static_cast<int>(h-z2),m_yachsis,font);

	QList<double> yachsisValues = Workspace::getInstace().getResults().valuesFor(m_phoFile, m_yachsis);
	if(!yachsisValues.isEmpty()){
		qSort(yachsisValues);
		double ymin = yachsisValues.first();
		double ymax = yachsisValues.last();
		double d;
		foreach(d,yachsisValues){
			double z = ZMIN-(-ZMAX+ZMIN)*((d-ymin)/(ymax-ymin));
			if(ymin==ymax) z = ZMIN + (ZMAX-ZMIN)/2;
			gluProject(XMIN, -15.f, z, modelview, projection, viewport, &x1, &x2, &x3);
			std::stringstream s;
			s.precision(3);
			s << d;
			glColor3f(0.f,0.f,0.f);
                        m_parent->renderText(static_cast<int>(x1),static_cast<int>(h-x2),s.str().c_str(),font);
			glColor3f(0.4f,0.4f,0.4f);
			glBegin(GL_LINES);
			glVertex3f(XMIN, -3.f, z);
			glVertex3f(XMIN, -8.f, z);
			glEnd();
		}
	}
}

void Box::setAchsisNames(QString xachsis, QString yachsis){
	m_xachsis = xachsis;
	m_yachsis = yachsis;
}

void Box::setPhoFile(QString phoFile){
	m_phoFile = phoFile;
}
