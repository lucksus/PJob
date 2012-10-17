#include "NonEquidistantSlider.h"
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QFontMetrics>

NonEquidistantSlider::NonEquidistantSlider(QWidget *parent)
: QWidget(parent)
{
	m_leftMargin = 5;
	m_rightMargin = 15;
	m_topMargin = 2;
	m_markerHeight = 5;
	m_ticHeight = 5;
}

double NonEquidistantSlider::value(){
	return m_value;
}

QList<double> NonEquidistantSlider::values(){
	return m_values;
}


void NonEquidistantSlider::setValue(double v){
	if(m_values.count(v)){
		m_value = v;
		emit valueChanged(m_value);
	}
}

void NonEquidistantSlider::setValues(QList<double> vs){
	m_values = vs;
	qSort(m_values);
	if(!m_values.empty())
		m_value = m_values.first();
}

void NonEquidistantSlider::addValue(double v){
	m_values.append(v);
	qSort(m_values);
}

void NonEquidistantSlider::removeValue(double v){
	m_values.removeOne(v);
}

void NonEquidistantSlider::paintEvent(QPaintEvent*){
	QPainter painter(this);

	int lineY = m_topMargin+m_markerHeight+m_ticHeight/2;
	painter.drawLine(m_leftMargin,lineY,width()-m_rightMargin,lineY);

	
	qSort(m_values);
	int lastDrawnX=-100;
	double v;
	bool firstTime=true;
	foreach(v, m_values){
		int x = xForValue(v);
		painter.drawLine(x,lineY-m_ticHeight/2,x,lineY+m_ticHeight/2);
		
		QString s = QString("%1").arg(v);
		int width = QFontMetrics(QFont()).width(s);
		if(firstTime){
			painter.drawLine(x,lineY-(m_ticHeight+1)/2,x,lineY+(m_ticHeight+3)/2);
			x -= 5;
			painter.drawText(x,lineY+m_ticHeight/2+10,s);
			lastDrawnX = x + width;
			firstTime = false;
		}else{
			if(x-width/2 <= lastDrawnX + 10) continue;
			painter.drawLine(x,lineY-(m_ticHeight+1)/2,x,lineY+(m_ticHeight+3)/2);
			painter.drawText(x-width/2,lineY+m_ticHeight/2+10,s);
			lastDrawnX = x + width/2;
		}
	}

	painter.translate(xForValue(m_value),0);
	painter.setBrush(QColor("red"));
	static const QPoint marker[3] = {
		QPoint(-3, m_topMargin),
		QPoint(3, m_topMargin),
		QPoint(0, m_topMargin+m_markerHeight)
	};
	if(isEnabled())
		painter.drawConvexPolygon(marker,3);

}


int NonEquidistantSlider::xForValue(double d){
	double min = m_values.first();
	double max = m_values.last();
	int length = width() - m_leftMargin - m_rightMargin;
	if(min == max)
		if(d == min)
			return length/2 + m_leftMargin;
		
	double a = (d-min)/(max-min);
        int pos = static_cast<int>(a*length);
	return pos+m_leftMargin;
}

double NonEquidistantSlider::valueForX(int x){
	x -= m_leftMargin;
	int length = width() - m_leftMargin - m_rightMargin;
	double a = static_cast<double>(x) / length;
	double min = m_values.first();
	double max = m_values.last();
	return a*(max-min) + min;
}

double NonEquidistantSlider::getNearestValue(double v){
	double distance = qAbs(v-m_values.first());
	double foundValue = m_values.first();
	double d;
	foreach(d,m_values){
		double newDistance = qAbs(v-d);
		if(newDistance < distance){
			distance = newDistance;
			foundValue = d;
		}
	}

	return foundValue;
}

void NonEquidistantSlider::changeValue(double v){
	if(v != m_value){
		m_value = v;
		emit valueChanged(m_value);
		update();
	}
}

void NonEquidistantSlider::mouseMoveEvent(QMouseEvent *e){
	changeValue(getNearestValue(valueForX(e->x())));
}

void NonEquidistantSlider::mousePressEvent(QMouseEvent *e){
	changeValue(getNearestValue(valueForX(e->x())));
}

void NonEquidistantSlider::wheelEvent(QWheelEvent *e){
	int index = m_values.indexOf(m_value);
	if(e->delta() > 0) index++;
	else index--;
	if(index >= m_values.size()) index = m_values.size()-1;
	if(index <= 0) index = 0;
	changeValue(m_values[index]);
}
