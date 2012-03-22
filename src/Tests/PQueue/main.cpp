#include <QtTest/QtTest>
#include "TestPhotossJob.h"

int main(int argc, char** argv){
	TestPhotossJob testPhotossJob;
	QTest::qExec(&testPhotossJob, argc, argv);
}