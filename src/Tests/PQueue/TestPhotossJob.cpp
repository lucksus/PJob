#include "TestPhotossJob.h"
#include <sstream>
#include <iostream>

void TestPhotossJob::resultsFromFileSingleLine(){
	QStringList parameters;
	parameters << "length" << "power" << "d";
	QHash< QHash<QString,double>, QHash<QString,double> > results;
	QHash<QString,double> parameterCombination;

	stringstream stream;
	stream << "%length,[Unit]	power,[Unit]	d,[Unit]	Eye opening	Eye opening penalty	Q_1	" << std::endl;
	stream << "% 	 	 	EyeAnalyzer	EyeAnalyzer	EyeAnalyzer" << std::endl;
	stream << "100	5	15	1	2	1.830163265666469" << std::endl;
	
	//results = PhotossJob::resultsFromFile(stream,parameters);
	
	parameterCombination["length"] = 100;
	parameterCombination["power"]  = 5;
	parameterCombination["d"]      = 15;

	QCOMPARE(results.keys().size(), 1);
	QCOMPARE(results.count(parameterCombination),1);
	QCOMPARE(results[parameterCombination].size(),3);
	QCOMPARE(results[parameterCombination].count("Eye opening"),1);
	QCOMPARE(results[parameterCombination].count("Eye opening penalty"),1);
	QCOMPARE(results[parameterCombination].count("Q_1"),1);
	QCOMPARE(results[parameterCombination]["Eye opening"],1.);
	QCOMPARE(results[parameterCombination]["Eye opening penalty"],2.);
	QCOMPARE(results[parameterCombination]["Q_1"],1.830163265666469);
}

void TestPhotossJob::resultsFromFileDoubleLine(){
	QStringList parameters;
	parameters << "length" << "power" << "d";
	QHash< QHash<QString,double>, QHash<QString,double> > results;
	QHash<QString,double> parameterCombination1,parameterCombination2;

	stringstream stream;
	stream << "%length,[Unit]	power,[Unit]	d,[Unit]	Eye opening	Eye opening penalty	Q_1	" << std::endl;
	stream << "% 	 	 	EyeAnalyzer	EyeAnalyzer	EyeAnalyzer" << std::endl;
	stream << "0.1	0.2	0.3	1000	2000	3000" << std::endl;
	stream << "1	1	1	1500	2500	3500" << std::endl;

	//results = PhotossJob::resultsFromFile(stream,parameters);

	parameterCombination1["length"] = 0.1;
	parameterCombination1["power"]  = 0.2;
	parameterCombination1["d"]      = 0.3;

	parameterCombination2["length"] = 1;
	parameterCombination2["power"]  = 1;
	parameterCombination2["d"]      = 1;

	QCOMPARE(results.keys().size(), 2);
	QCOMPARE(results.keys().count(parameterCombination1),1);
	QCOMPARE(results.keys().count(parameterCombination2),1);
	QCOMPARE(results[parameterCombination1].size(),3);
	QCOMPARE(results[parameterCombination1].count("Eye opening"),1);
	QCOMPARE(results[parameterCombination1].count("Eye opening penalty"),1);
	QCOMPARE(results[parameterCombination1].count("Q_1"),1);
	QCOMPARE(results[parameterCombination1]["Eye opening"],1000.);
	QCOMPARE(results[parameterCombination1]["Eye opening penalty"],2000.);
	QCOMPARE(results[parameterCombination1]["Q_1"],3000.);
	QCOMPARE(results[parameterCombination2].size(),3);
	QCOMPARE(results[parameterCombination2].count("Eye opening"),1);
	QCOMPARE(results[parameterCombination2].count("Eye opening penalty"),1);
	QCOMPARE(results[parameterCombination2].count("Q_1"),1);
	QCOMPARE(results[parameterCombination2]["Eye opening"],1500.);
	QCOMPARE(results[parameterCombination2]["Eye opening penalty"],2500.);
	QCOMPARE(results[parameterCombination2]["Q_1"],3500.);
}
