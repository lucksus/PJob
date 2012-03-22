#pragma once
#include <QtCore/QAbstractItemModel>
#include "PJobFileFormat.h"
#include "PJobDirTree.h"

//! Model Klasse, die eine PJob Dateistruktur in einem QTreeView darstellt.
/*!
 *  Implementiert Funktionen aus der Abstrakten Oberklasse QAbstractItemModel.
 *  Für weitere Informationen bitte den Qt Model/View Programming Guide lesen.
 *
 *  Als unterliegende Datenstruktur dient die Klasse PJobDirTree.
 */

class PJobDirModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	PJobDirModel(PJobFileFormat* pjobFile, QString rootDir = "", QObject* parent = NULL);
	~PJobDirModel();

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	bool dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	Qt::DropActions supportedDragActions() const;

	void removeEntries(const QModelIndexList& indexes);
	QModelIndex makeDir(const QModelIndex& parent);

public slots:
	void fileAdded(QList<QVariant> fileProperties);

signals:
	void changed();

private:
	PJobFileFormat* m_pjobFile;
	PJobDirTree* m_rootItem;
	const QString m_tempDir; // Pfad zu einem einzigartigen temporären Verzeichnis für jede Model Instanz
};
