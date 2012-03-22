#include <QtTest/QtTest>
#include "TestPJobFile.h"

int main(int argc, char** argv){
	TestPJobFile testPJobFile;
	QTest::qExec(&testPJobFile, argc, argv);
}