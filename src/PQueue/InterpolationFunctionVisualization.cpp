#include "InterpolationFunctionVisualization.h"
#include <QtOpenGL/QGLWidget>

InterpolationFunctionVisualization::InterpolationFunctionVisualization(QString phoFile, QString result, QHash<QString, double> parameters, QString xachsis, QString yachsis)
: Drawable(phoFile,result,parameters,xachsis,yachsis)
{
	m_interpolant = PQueueController::getInstace().getResults().interpolation_function(phoFile,result);
}

InterpolationFunctionVisualization::~InterpolationFunctionVisualization(void)
{
}


void InterpolationFunctionVisualization::drawImplementation(QSet< QHash<QString,double> > combinationsInPlane, double minx, double maxx, double miny, double maxy){
	if(maxx==minx){maxx+=1;minx-=1;}
	if(maxy==miny){maxy+=1;miny-=1;}
	double diffX = (maxx-minx)/20;
	double diffY = (maxy-miny)/20;
	for(int i=0;i<2;i++)
	for(double x = minx; x<=maxx-diffX/2; x+=diffX){
		for(double y = miny; y<=maxy-diffY/2; y+=diffY){
			QHash<QString,double> parameters = m_parameters;

			parameters[m_xachsis] = x;
			parameters[m_yachsis] = y;
			float hl[3];
			hl[0] = getGLValueX(parameters);
			hl[1] = getGLHeight(m_interpolant->getValue(parameters));
			hl[2] = getGLValueY(parameters);

			parameters[m_xachsis] = x+diffX;
			parameters[m_yachsis] = y;
			float hr[3];
			hr[0] = getGLValueX(parameters);
			hr[1] = getGLHeight(m_interpolant->getValue(parameters));
			hr[2] = getGLValueY(parameters);
	
			parameters[m_xachsis] = x;
			parameters[m_yachsis] = y+diffY;
			float vl[3];
			vl[0] = getGLValueX(parameters);
			vl[1] = getGLHeight(m_interpolant->getValue(parameters));
			vl[2] = getGLValueY(parameters);

			parameters[m_xachsis] = x+diffX;
			parameters[m_yachsis] = y+diffY;
			float vr[3];
			vr[0] = getGLValueX(parameters);
			vr[1] = getGLHeight(m_interpolant->getValue(parameters));
			vr[2] = getGLValueY(parameters);

			glDisable(GL_CULL_FACE);
			if(0 == i){
				glDepthMask(false);

				glBegin(GL_QUADS);
				glColor4f(0.6f,0.6f,0.f,0.5);
				glVertex3fv(vl);
				glVertex3fv(vr);
				glVertex3fv(hr);
				glVertex3fv(hl);
				glEnd();
			}else{
				glDepthMask(true);

				//glLineWidth(3.f);
				glBegin(GL_LINES);
				glColor4f(0.f,0.f,0.f,0.5f);
				glVertex3fv(vl);
				glVertex3fv(vr);
				glVertex3fv(hr);
				glVertex3fv(hl);
				glVertex3fv(vl);
				glVertex3fv(hl);
				glVertex3fv(vr);
				glVertex3fv(hr);
				glEnd();
				//glLineWidth(1.f);
			}

			

			glEnable(GL_CULL_FACE);
		}
	}
}
