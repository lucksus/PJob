#include <QtTest/QtTest>
#include "PhotossJob.h"

class TestPhotossJob: public QObject{
Q_OBJECT
private slots:
	void resultsFromFileSingleLine();
	void resultsFromFileDoubleLine();
};
