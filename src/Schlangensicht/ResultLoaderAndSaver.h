#pragma once
#include "hash.h"
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <limits>

//***************Unser CSV-Format***************//
//Unsere Dateien beginnen mit einem HEADER der Länge n+1, 
//wobei n gleich der Anzahl an verschiedenen .pjob-Dateien entspricht
//Im Anschluss werden die Jeweiligen Parametervariationen der Dateien eingetragen.
//
//Die Sortierung orientiert sich an der Zeile n+1
//
//Wenn die Results zweier oder mehrerer unterschiedlicher .pjob-Dateien den gleichen Namen haben,
//werden sie in einer einzigen Zeile zusammengefasst. Das hat den Vorteil, dass man beispielsweise
//ähnliche Simulationen leichter miteinander vergleichen kann!
//
//Die Trennzeichen jeweiligen für Zeilen und Spalten sind dabei dynamisch zuweisbar!
//Diese lassen sich mittels der Variablen m_seperators definieren.
//Standardmäßig verwenden wir Tabluator (hier durch <=> gekennzeichnet)
//für Spalten- und Zeilenumbruch "\n" für Zeilenwechsel
//
//Man beachte die Trennung von Parametern und Results durch ZWEI Tabulatoren in den ersten n Zeilen
//(NICHT in Zeile n+1 !!!)
//
//Leerzeichen und Tabulatoren zu Beginn der Zeilen wurden hier nur der Lesbarkeit halber hinzugefügt!

//		%C:/datei_1.pjob <=> param_d1_1 <=> param_d1_2<=> <=> result_d1_1 <=> result_d1_2
//		%C:/datei_i.pjob <=> param_di_1 <=> <=> result_di_1 <=> result_di_2 <=> result_di_3
//		%C:/datei_n.pjob <=> param_dn_1 <=> <=> result_dn_1
//		%PJobFile <=> param_x <=> param_x2 <=> ...usw... <=> param_w <=> result_y <=> result_y2 <=> ...usw... <=>result
//		%C:/datei_1.pjob <=> <=> ...usw... <=> param_wert_d1_1_#1 <=> <=> ...usw... <=> param_wert_d1_2_#1 <=> <=> ... <=> result_wert_d1_1_#1 <=> <=> ...usw... <=> result_wert_d1_2_#1 <=> <=>...usw...
//		%C:/datei_1.pjob <=> <=> ...usw... <=> param_wert_d1_1_#2 <=> <=> ...usw... <=> param_wert_d1_2_#2 <=> <=> ... <=> result_wert_d1_1_#2 <=> <=> ...usw... <=> result_wert_d1_2_#2 <=> <=>...usw...
//		%C:/datei_2.pjob <=> <=> ...usw ... usw ... usw ... <=> param_wert_di_1_#1 <=> <=> ... <=> result_wert_di_1_#1 <=> <=> ...usw... <=> result_wert_di_2_#1 <=> <=>...usw...
//		...usw...
//		...usw...

//Das Ende der Datei enthält keine Leerzeile!
//**********************************************//
class ResultLoaderAndSaver : public QObject
{
Q_OBJECT
public:
    ResultLoaderAndSaver(QString columnSep = QString::null, QString rowSep = QString::null, QString unit1Sep = QString::null, QString unit2Sep = QString::null);
	void setSeperators(QString columnSep, QString rowSep, QString unit1Sep, QString unit2Sep);
	~ResultLoaderAndSaver();

	void exportToCSV(QString outputFile, QStringList pjobFiles,QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > const &results);
    void importFromCSV(QString inputFile);

signals:
    void parsed_csv_line(QHash<QString,double> parameters, QHash<QString,double> results);

private:
	struct CSVSeperators
	{
		QString column, row, unit1, unit2;
	};
	
	CSVSeperators m_seperators;

	void writeLine(QTextStream& outStream, std::pair<QStringList,QStringList> header, QHash<QString,double> parameters, QHash<QString,double> results);
	void writeHeader(QTextStream& outStream, std::pair<QStringList,QStringList> header);
	std::pair<QStringList,QStringList> headerForResult(QString,QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > const &n_results);

	QHash<QString, std::pair<QStringList,QStringList> > parseHeaderLines(QByteArray& data);
	QStringList parseLine(QByteArray& data);
	std::pair<QString,QStringList> parseValueLine(QByteArray& data);
};
