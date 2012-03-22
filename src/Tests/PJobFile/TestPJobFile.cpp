#include "TestPJobFile.h"
#include "PJobFile.h"

void TestPJobFile::createTestParameterdefinitionFile(QString filename){
	QString xmlFile;
	xmlFile.append("<parameterdefinitions>\n");
		xmlFile.append("\t<parameter>\n");
			xmlFile.append("\t\t<name>length</name>\n");
			xmlFile.append("\t\t<defaultValue>100</defaultValue>\n");
		xmlFile.append("\t</parameter>\n");
		xmlFile.append("\t<parameter>\n");
			xmlFile.append("\t\t<name>power</name>\n");
			xmlFile.append("\t\t<defaultValue>5</defaultValue>\n");
			xmlFile.append("\t\t<min>1</min>\n");
			xmlFile.append("\t\t<max>15</max>\n");
		xmlFile.append("\t</parameter>\n");
	xmlFile.append("</parameterdefinitions>\n");

	QFile testFile(filename);
	testFile.open(QIODevice::WriteOnly);
	testFile.write(xmlFile.toLocal8Bit());
	testFile.close();
}

void TestPJobFile::createTestParametercombinationFile(QString filename){
	QString xmlFile;
	xmlFile.append("<parametercombination>\n");
		xmlFile.append("\t<parameter>\n");
			xmlFile.append("\t\t<name>length</name>\n");
			xmlFile.append("\t\t<value>100</value>\n");
		xmlFile.append("\t</parameter>\n");
		xmlFile.append("\t<parameter>\n");
			xmlFile.append("\t\t<name>power</name>\n");
			xmlFile.append("\t\t<variation>\n");
				xmlFile.append("\t\t\t<min>1</min>\n");
				xmlFile.append("\t\t\t<max>15</max>\n");
				xmlFile.append("\t\t\t<step>1</step>\n");
			xmlFile.append("\t\t</variation>\n");
		xmlFile.append("\t</parameter>\n");
	xmlFile.append("</parametercombination>\n");

	QFile testFile(filename);
	testFile.open(QIODevice::WriteOnly);
	testFile.write(xmlFile.toLocal8Bit());
	testFile.close();
}

void TestPJobFile::createTestResultDefinitions(QString filename){
	QString xmlFile;
	xmlFile.append("<resultdefinitions>\n");
		xmlFile.append("\t<resultFile>\n");
			xmlFile.append("\t\t<fileName>ParameterVariation.txt</fileName>\n");
			xmlFile.append("\t\t<format>PHOTOSS_CSV</format>\n");
			xmlFile.append("\t\t<result>\n");
				xmlFile.append("\t\t\t<name>EOP</name>\n");
			xmlFile.append("\t\t</result>\n");
			xmlFile.append("\t\t<result>\n");
				xmlFile.append("\t\t\t<name>Q</name>\n");
				xmlFile.append("\t\t\t<unit>dB</unit>\n");
			xmlFile.append("\t\t</result>\n");
		xmlFile.append("\t</resultFile>\n");
	xmlFile.append("</resultdefinitions>\n");
	
	QFile testFile(filename);
	testFile.open(QIODevice::WriteOnly);
	testFile.write(xmlFile.toLocal8Bit());
	testFile.close();
}

void TestPJobFile::readParameterDefinitions(){
	createTestParameterdefinitionFile("testReadParameterDefinitions.xml");
	QList<PJobFileParameterDefinition> parameters = PJobFile::readParameterDefinitions("testReadParameterDefinitions.xml");
	QFile::remove("testReadParameterDefinitions.xml");

	QCOMPARE(parameters.size(), 2);

	PJobFileParameterDefinition p1 = parameters.front();
	parameters.pop_front();
	PJobFileParameterDefinition p2 = parameters.front();

	QCOMPARE(p1.name(), QString("length"));
	QCOMPARE(p1.defaultValue(), 100.);
	QCOMPARE(p1.hasMinValue(), false);
	QCOMPARE(p1.hasMaxValue(), false);

	QCOMPARE(p2.name(), QString("power"));
	QCOMPARE(p2.defaultValue(), 5.);
	QCOMPARE(p2.hasMinValue(), true);
	QCOMPARE(p2.hasMaxValue(), true);
	QCOMPARE(p2.minValue(), 1.);
	QCOMPARE(p2.maxValue(), 15.);
}

void TestPJobFile::addParameterDefintion(){
	createTestParameterdefinitionFile("testAddParameterDefinition.xml");
	PJobFileParameterDefinition p;
	p.setName("iterations");
	p.setDefaultValue(3);
	p.setMinValue(1);
	PJobFile::addParameterDefinition(p,"testAddParameterDefinition.xml");
	QList<PJobFileParameterDefinition> parameters = PJobFile::readParameterDefinitions("testAddParameterDefinition.xml");
	QFile::remove("testAddParameterDefinition.xml");

	QCOMPARE(parameters.size(), 3);
	
	bool found = false;
	PJobFileParameterDefinition def;
	foreach(def,parameters){
		if(def.name() != p.name()) continue;
		found = true;
		QCOMPARE(def.defaultValue(),p.defaultValue());
		QCOMPARE(def.hasMinValue(),p.hasMinValue());
		QCOMPARE(def.hasMaxValue(),p.hasMaxValue());
		QCOMPARE(def.minValue(),p.minValue());
	}

	QCOMPARE(found, true);
}

void TestPJobFile::removeParameterDefinition(){
	createTestParameterdefinitionFile("testRemoveParameterDefinition.xml");
	PJobFile::removeParameterDefinition("length","testRemoveParameterDefinition.xml");
	QList<PJobFileParameterDefinition> parameters = PJobFile::readParameterDefinitions("testRemoveParameterDefinition.xml");
	QFile::remove("testRemoveParameterDefinition.xml");

	QCOMPARE(parameters.size(), 1);

	PJobFileParameterDefinition p2 = parameters.front();

	QCOMPARE(p2.name(), QString("power"));
	QCOMPARE(p2.defaultValue(), 5.);
	QCOMPARE(p2.hasMinValue(), true);
	QCOMPARE(p2.hasMaxValue(), true);
	QCOMPARE(p2.minValue(), 1.);
	QCOMPARE(p2.maxValue(), 15.);
}


void TestPJobFile::readParameterCombination(){
	createTestParametercombinationFile("testReadParameterCombination.xml");
	QList<PJobFileParameter> parameters = PJobFile::readParameterCombination("testReadParameterCombination.xml");
	QFile::remove("testReadParameterCombination.xml");

	QCOMPARE(parameters.size(), 2);

	PJobFileParameter p1 = parameters.front();
	parameters.pop_front();
	PJobFileParameter p2 = parameters.front();

	QCOMPARE(p1.name(), QString("length"));
	QCOMPARE(p1.isVariation(), false);
	QCOMPARE(p1.value(), 100.);

	QCOMPARE(p2.name(), QString("power"));
	QCOMPARE(p2.isVariation(), true);
	QCOMPARE(p2.minValue(), 1.);
	QCOMPARE(p2.maxValue(), 15.);
	QCOMPARE(p2.step(), 1.);
}


void TestPJobFile::writeParameterCombination(){
	QList<PJobFileParameter> parameters;

	PJobFileParameter p;
	p.setName("length");
	p.setValue(100.);
	parameters << p;
	p.setName("power");
	p.setVariation(1.,15.,1.);
	parameters << p;

	PJobFile::writeParameterCombination(parameters, "testReadParameterCombination.xml");
	parameters = PJobFile::readParameterCombination("testReadParameterCombination.xml");

	QFile::remove("testReadParameterCombination.xml");

	QCOMPARE(parameters.size(), 2);

	PJobFileParameter p1 = parameters.front();
	parameters.pop_front();
	PJobFileParameter p2 = parameters.front();

	QCOMPARE(p1.name(), QString("length"));
	QCOMPARE(p1.isVariation(), false);
	QCOMPARE(p1.value(), 100.);

	QCOMPARE(p2.name(), QString("power"));
	QCOMPARE(p2.isVariation(), true);
	QCOMPARE(p2.minValue(), 1.);
	QCOMPARE(p2.maxValue(), 15.);
	QCOMPARE(p2.step(), 1.);
}

void TestPJobFile::readResultDefinitions(){
	createTestResultDefinitions("testReadResultDefinitions.xml");
	QList<PJobResultFile> resultFiles = PJobFile::readResultDefintions("testReadResultDefinitions.xml");
	QFile::remove("testReadResultDefinitions.xml");
	
	QCOMPARE(resultFiles.size(), 1);

	PJobResultFile p = resultFiles.front();
	QList<PJobResult> results = p.results();

	QCOMPARE(p.filename(), QString("ParameterVariation.txt"));
	QCOMPARE(results.size(), 2);

	PJobResult r1  = results.front();
	results.pop_front();
	PJobResult r2 = results.front();

	QCOMPARE(r1.name(), QString("EOP"));
	QCOMPARE(r1.unit(), QString(""));
	QCOMPARE(r2.name(), QString("Q"));
	QCOMPARE(r2.unit(), QString("dB"));
}

void TestPJobFile::writeResultDefintions(){
	PJobResultFile file;
	file.setFilename("ParameterVariation.txt");
	PJobResult result;
	result.setName("EOP");
	file.addResult(result);
	result.setName("Q");
	result.setUnit("dB");
	file.addResult(result);

	QList<PJobResultFile> resultFiles;
	resultFiles << file;
	PJobFile::writeResultDefinitions(resultFiles,"testWriteResultDefinitions.xml");
	resultFiles = PJobFile::readResultDefintions("testWriteResultDefinitions.xml");

	QCOMPARE(resultFiles.size(), 1);

	PJobResultFile p = resultFiles.front();
	QList<PJobResult> results = p.results();

	QCOMPARE(p.filename(), QString("ParameterVariation.txt"));
	QCOMPARE(results.size(), 2);

	PJobResult r1  = results.front();
	results.pop_front();
	PJobResult r2 = results.front();

	QCOMPARE(r1.name(), QString("EOP"));
	QCOMPARE(r1.unit(), QString(""));
	QCOMPARE(r2.name(), QString("Q"));
	QCOMPARE(r2.unit(), QString("dB"));
}