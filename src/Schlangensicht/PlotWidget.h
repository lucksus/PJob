#pragma once
#include <QtOpenGL/QGLWidget>
#include <QtCore/QHash>
class Drawable;
class Box;
class ResultVisualization;
class InterpolationFunctionVisualization;

class PlotWidget : public QGLWidget
{
Q_OBJECT
public:
	PlotWidget(void);

public slots:
	void show(QString phoFile, QString result, QHash<QString,double> parameters, QString xachsis, QString yachsis);
	void showInterpolant(int);

protected:
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

	void paintGL();
	void resizeGL(int width, int height);
	void initializeGL();


private:
	float m_cameraAngleXRotation;
	float m_cameraAngleYRotation;
	float m_cameraDistance;

	QList<Drawable*> m_drawables;

	Box* m_box;
	ResultVisualization* m_resultVisualization;
	InterpolationFunctionVisualization* m_interpolantVisualization;

	bool m_mousedown;
	int m_oldMouseX;
	int m_oldMouseY;

	bool m_haveDisplayList;
	int m_displayList;
	bool m_showInterpolant;

	void removeDisplayList();
	
};
