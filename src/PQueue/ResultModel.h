#pragma once
#include <QtCore/QAbstractTableModel>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include "Results.h"

//! Model Klasse, die Results in einem QTableView darstellt.
/*!
 * Implementiert Funktionen aus der Abstrakten Oberklasse QAbstractTableModel.
 * Für weitere Informationen bitte den Qt Model/View Programming Guide lesen.
 *
 * Das Model stellt die Ergebnisse einer einzigen PJob Datei aus m_resultSpace dar.
 * initialize(QString pjobFile) initialisiert das Modell mit dem Übergebenen Run. Die
 * Daten werden in den privaten Member Variablen zwischengespeichert. update(QString pjobFile)
 * lädt die Daten erneut, falls der übergeben PJob mit dem momentan geladenen übereinstimmt.
 * purge() löscht die Daten des Models.
 *
 * Das Signal modelInitialized() kann genutzt werden, um zum Beispiel nach einer
 * Initialisierung des Models die Sortierung der View zu setzen.
 */

class ResultModel : public QAbstractTableModel{
Q_OBJECT
public:
	ResultModel(Results* resultSpace, QObject* parent = NULL);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

signals:
	void modelInitialized(); //! Gesendet nach initialize(QString pjobFile)

public slots:
	void initialize(QString pjobFile); //!< Initialisert Model mit den Ergebnissen von pjobFile
	void update(QString pjobFile); //!< Aktualisiert Model wenn pjobFile mit m_pjobFile übereinstimmt
	void purge(); //!< Löscht die Model Daten und setzt es in den Ausgangszustand zurück

private:
	Results* m_resultSpace;
	QString m_pjobFile;
	QStringList m_paramNames;
	QStringList m_resultNames;
	QList< QHash<QString,double> > m_paramCombinations;
};
