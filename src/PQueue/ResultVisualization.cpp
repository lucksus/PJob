#include "ResultVisualization.h"
#include <QtCore/QSet>
#include <QtOpenGL/qgl.h>
#include "PQueueController.h"

ResultVisualization::ResultVisualization(QString phoFile, QString result, QHash<QString, double> parameters, QString xachsis, QString yachsis)
:Drawable(phoFile,result,parameters,xachsis,yachsis)
{
}

void ResultVisualization::drawImplementation(QSet< QHash<QString,double> > combinationsInPlane, double minx, double maxx, double miny, double maxy){
	QHash<QString,double> c;
	foreach(c,combinationsInPlane){
		QHash<QString,double> parameters = m_parameters;
		parameters[m_xachsis] = c[m_xachsis];
		parameters[m_yachsis] = c[m_yachsis];
		double value = PQueueController::getInstace().getResults().getValue(m_phoFile, m_result, parameters);
		double color = (value-m_min)/(m_max-m_min);
		if(m_max == m_min) color = 0.5;
		double height = color*75;
		double x=0,z=0;
		//Division durch Null abfangen!
		if(maxx != minx)
			x = (c[m_xachsis]-minx)/(maxx-minx)*100 -50;
		if(maxy != miny)
			z = (c[m_yachsis]-miny)/(maxy-miny)*100 -50;

		glPushMatrix();
		glTranslated(x,0,-z);
		drawPillar(height,color);
		glPopMatrix();
	}
}


void ResultVisualization::drawPillar(double height, double color){

	float pillarSizeX = 1;
	float pillarSizeY = 1;

	float lvu[3] = {-pillarSizeX/2,0,pillarSizeY/2};
	float lvo[3] = {-pillarSizeX/2,height,pillarSizeY/2};
	float rvu[3] = {pillarSizeX/2,0,pillarSizeY/2};
	float rvo[3] = {pillarSizeX/2,height,pillarSizeY/2};
	float lhu[3] = {-pillarSizeX/2,0,-pillarSizeY/2};
	float lho[3] = {-pillarSizeX/2,height,-pillarSizeY/2};
	float rhu[3] = {pillarSizeX/2,0,-pillarSizeY/2};
	float rho[3] = {pillarSizeX/2,height,-pillarSizeY/2};
	glDisable(GL_CULL_FACE);

	glBegin(GL_QUADS);
	glColor3f(1.f,1.f,1.f);
	glVertex3fv(lvu);
	glVertex3fv(rvu);
	glColor3d(color,0.f,1-color);
	glVertex3fv(rvo);
	glVertex3fv(lvo);

	glColor3f(1.f,1.f,1.f);
	glVertex3fv(rvu);
	glVertex3fv(rhu);
	glColor3d(color,0.f,1-color);
	glVertex3fv(rho);
	glVertex3fv(rvo);

	glColor3f(1.f,1.f,1.f);
	glVertex3fv(lhu);
	glVertex3fv(lvu);
	glColor3d(color,0.f,1-color);
	glVertex3fv(lvo);
	glVertex3fv(lho);


	glColor3d(color,0.f,1-color);
	glVertex3fv(rhu);
	glVertex3fv(lhu);
	glVertex3fv(lho);
	glVertex3fv(rho);

	glColor3d(color,0.f,1-color);
	glVertex3fv(lvo);
	glVertex3fv(rvo);
	glVertex3fv(rho);
	glVertex3fv(lho);

	glEnd();
}

