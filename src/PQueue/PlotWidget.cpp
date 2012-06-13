#include "PlotWidget.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include "Drawable.h"
#include "Box.h"
#include "ResultVisualization.h"
#include "InterpolationFunctionVisualization.h"
#include "PQueueController.h"
#ifdef Q_OS_WIN
#include <gl/glu.h>
#endif

PlotWidget::PlotWidget(void)
: m_cameraAngleXRotation(28), m_cameraAngleYRotation(40), m_cameraDistance(200), m_mousedown(false), m_haveDisplayList(false)
{
	m_resultVisualization = 0;
	m_interpolantVisualization = 0;
	m_showInterpolant = false;
	m_box = new Box(this, 0, 20);
	m_drawables.append(m_box);
	setMouseTracking(true);
}


void PlotWidget::mouseMoveEvent(QMouseEvent *e){
	if(m_mousedown){
		int deltaX = e->x() - m_oldMouseX;
		int deltaY = -(e->y() - m_oldMouseY);
		m_oldMouseX = e->x();
		m_oldMouseY = e->y();
		m_cameraAngleXRotation -= deltaY;
		m_cameraAngleYRotation += deltaX;
		if(m_cameraAngleXRotation >  90) m_cameraAngleXRotation =  90;
		if(m_cameraAngleXRotation < -90) m_cameraAngleXRotation = -90;
		updateGL();
	}
}

void PlotWidget::mousePressEvent(QMouseEvent* e){
	m_mousedown = true;
	m_oldMouseX = e->x();
	m_oldMouseY = e->y();
}

void PlotWidget::mouseReleaseEvent(QMouseEvent*){
	m_mousedown = false;
}

void PlotWidget::wheelEvent(QWheelEvent *e){
	m_cameraDistance -= e->delta()/10.f;
	if(m_cameraDistance<70)m_cameraDistance=70;
	updateGL();
}

void PlotWidget::paintGL(){
	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//----------------------------------------------------
	//Set up projection matrix
	//----------------------------------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80., 1., 0.1, 20000.);

	//----------------------------------------------------
	//Set up modelview matrix
	//----------------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.,0.,0.-m_cameraDistance);
	glRotatef(m_cameraAngleXRotation, 1., 0., 0.);
	glRotatef(m_cameraAngleYRotation, 0., 1., 0.);

	m_box->draw();
	if(!m_haveDisplayList && m_resultVisualization){
		m_displayList = glGenLists (1);
		glNewList(m_displayList, GL_COMPILE);
		m_resultVisualization->draw();
		if(m_showInterpolant && m_interpolantVisualization)
			m_interpolantVisualization->draw();
		glEndList();
		m_haveDisplayList = true;
	}
	glCallList(m_displayList);
}

void PlotWidget::removeDisplayList(){
	glDeleteLists(m_displayList,1);
	m_haveDisplayList=false;
}

void PlotWidget::resizeGL(int width, int height){
	glViewport(0,0,width,height);
}

void PlotWidget::initializeGL(){
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POLYGON_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void PlotWidget::show(QString phoFile, QString result, QHash<QString,double> parameters, QString xachsis, QString yachsis){
	m_drawables.removeOne(m_resultVisualization);
	if(m_resultVisualization) delete m_resultVisualization;
	if(m_interpolantVisualization) delete m_interpolantVisualization;
	m_resultVisualization = new ResultVisualization(phoFile,result,parameters,xachsis,yachsis);
	m_interpolantVisualization = new InterpolationFunctionVisualization(phoFile,result,parameters,xachsis,yachsis);
	m_drawables.append(m_resultVisualization);
	m_drawables.append(m_interpolantVisualization);
	m_box->setResultName(result);
	m_box->setAchsisNames(xachsis, yachsis);
	m_box->setPhoFile(phoFile);
	double min = Workspace::getInstace().getResults().getMinimumValue(phoFile,result,true);
	double max = Workspace::getInstace().getResults().getMaximumValue(phoFile,result,true);
	m_box->setDimensions(min,max);
	m_resultVisualization->setMinMax(min,max);
	m_interpolantVisualization->setMinMax(min,max);
	removeDisplayList();
	updateGL();
}

void PlotWidget::showInterpolant(int i){
	m_showInterpolant = i>0;
	removeDisplayList();
	updateGL();
}
