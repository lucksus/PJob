#pragma once
#include <QtGui/QWidget>

class NonEquidistantSlider : public QWidget
{
Q_OBJECT
Q_PROPERTY(double value READ value WRITE setValue)
Q_PROPERTY(QList<double> values READ values WRITE setValues)
public:
	NonEquidistantSlider(QWidget *parent = 0);
	double value();
	QList<double> values();


public slots:
	void setValue(double);
	void setValues(QList<double>);
	void addValue(double);
	void removeValue(double);

signals:
	void valueChanged(double);


protected:
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);

private:
	double m_value;
	QList<double> m_values;

	int xForValue(double);
	double valueForX(int);
	double getNearestValue(double);
	void changeValue(double);

	int m_leftMargin;
	int m_rightMargin;
	int m_topMargin;
	int m_markerHeight;
	int m_ticHeight;

};
