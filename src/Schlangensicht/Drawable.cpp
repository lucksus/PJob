#include "Drawable.h"
#include "Results.h"

Drawable::Drawable(QString phoFile, QString result, QHash<QString, double> parameters, QString xachsis, QString yachsis)
:m_phoFile(phoFile), m_result(result), m_xachsis(xachsis), m_yachsis(yachsis), m_parameters(parameters){}

void Drawable::setMinMax(double min, double max){
	m_min = min;
	m_max = max;
}

void Drawable::draw(){
	QSet< QHash<QString,double> > combinationsInPlane;

	if(m_phoFile.size()){
        //combinationsInPlane = Workspace::getInstace().getResults().combinationsInPlane(m_phoFile,m_xachsis,m_yachsis,m_parameters);
        /*QSet< QHash<QString,double> > allCombinations = Workspace::getInstace().getResults().parameterCombinationsFor(m_phoFile);
		m_minx = Results::minParameterValue(allCombinations,m_xachsis);
		m_maxx = Results::maxParameterValue(allCombinations,m_xachsis);
		m_miny = Results::minParameterValue(allCombinations,m_yachsis);
        m_maxy = Results::maxParameterValue(allCombinations,m_yachsis);*/
	}

	drawImplementation(combinationsInPlane,m_minx,m_maxx,m_miny,m_maxy);
}


double Drawable::getValue(QHash<QString,double> parameterCombination){
    return 0;//Workspace::getInstace().getResults().getValue(m_phoFile,m_result,parameterCombination);
}

double Drawable::getGLHeight(QHash<QString,double> parameterCombination){
	return (getValue(parameterCombination)-m_min)/(m_max-m_min) * 75;
}

double Drawable::getGLHeight(double value){
	return (value-m_min)/(m_max-m_min) * 75;
}

double Drawable::getGLValueX(QHash<QString,double> parameterCombination){
	double x = parameterCombination[m_xachsis];
	if(m_maxx==m_minx){m_maxx+=1;m_minx-=1;}
	return (x-m_minx)/(m_maxx-m_minx)*100 - 50;
}

double Drawable::getGLValueY(QHash<QString,double> parameterCombination){
	double y = parameterCombination[m_yachsis];
	if(m_maxy==m_miny){m_maxy+=1;m_miny-=1;}
	return -( (y-m_miny)/(m_maxy-m_miny)*100 - 50 );
}
