#include <QtTest/QtTest>

class TestPJobFile: public QObject{
Q_OBJECT
private slots:
	void readParameterDefinitions();
	void addParameterDefintion();
	void removeParameterDefinition();

	void readParameterCombination();
	void writeParameterCombination();

	void readResultDefinitions();
	void writeResultDefintions();

private:
	void createTestParameterdefinitionFile(QString filename);
	void createTestParametercombinationFile(QString filename);
	void createTestResultDefinitions(QString filename);
};