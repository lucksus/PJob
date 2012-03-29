#include "PJobDirModel.h"
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QStack>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtGui/QMessageBox>
#include <limits>
#include "PJobFileError.h"

PJobDirModel::PJobDirModel(PJobFileFormat* pjobFile, QString rootDir, QObject* parent) :
	QAbstractItemModel(parent), m_pjobFile(pjobFile), m_tempDir(QDir::tempPath() + QString("/PJobEditor/%1").arg(QDateTime::currentDateTime().toTime_t()))
{
	QList<QVariant> data;
	data << QVariant(rootDir);
	m_rootItem = new PJobDirTree(NULL, data);
	m_rootItem->populateTree(m_pjobFile->detailedContents(rootDir));

	connect(m_pjobFile, SIGNAL(fileAdded(QList<QVariant>)), this, SLOT(fileAdded(QList<QVariant>)));

	// Verhindert das drag/drop vom Model/ins Model indem drags auf move, und
	// drops auf copy Operationen beschränkt werden. Siehe auch supportedDropActions().
	this->setSupportedDragActions(Qt::MoveAction);
}

PJobDirModel::~PJobDirModel(){
	// Temporäres Verzeichnis löschen. Prüft nicht ob erfolgreich gelöscht wurde.
	QDir tempDir(m_tempDir);
	if(tempDir.exists()){		
		// Alle Dateien löschen
		QDirIterator files(m_tempDir, (QDir::Files | QDir::NoDotAndDotDot), QDirIterator::Subdirectories);
		while(files.hasNext()) QFile::remove(files.next());

		// Ordner löschen. Mit 2 Schleifen und einem Stack, da Qt keine Verzeichnis
		// Auflistung mit Unterverzeichnissen in invertierter Reihenfolge erlaubt.
		QDirIterator eachDir(m_tempDir, (QDir::Dirs | QDir::NoDotAndDotDot), QDirIterator::Subdirectories);
		QStack<QString> folders;
		folders.push(m_tempDir);
		while(eachDir.hasNext()) folders.push(eachDir.next());
		while(!folders.isEmpty()) tempDir.rmdir(folders.pop());
	}
	delete m_rootItem;
}


QVariant PJobDirModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if( orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
	
	if(section == 0) return QVariant("Filename");
	if(section == 1) return QVariant("Last modified");
	if(section == 2) return QVariant("Size");
	if(section == 3) return QVariant("Size(compressed)");

	return QVariant();

}

QModelIndex PJobDirModel::index(int row, int column, const QModelIndex& parent) const{
	PJobDirTree* parentItem;
	if(parent.isValid())
		parentItem = static_cast<PJobDirTree*>(parent.internalPointer());
	else
		parentItem = m_rootItem;
	
	if(row >= parentItem->childCount()) return QModelIndex();
	return createIndex(row, column, parentItem->childAt(row));
}

QModelIndex PJobDirModel::parent (const QModelIndex& index) const{
	if(!index.isValid()) return QModelIndex();
	PJobDirTree* parentItem = (static_cast<PJobDirTree*>(index.internalPointer()))->parent();

	if(parentItem == m_rootItem) return QModelIndex();
	return createIndex(parentItem->row(), 0, parentItem);
}

int PJobDirModel::rowCount(const QModelIndex& parent) const{
	if(!parent.isValid()) return m_rootItem->childCount();
	return (static_cast<PJobDirTree*>(parent.internalPointer()))->childCount();
}

int PJobDirModel::columnCount(const QModelIndex& parent) const{
	return 4;
}

QVariant PJobDirModel::data(const QModelIndex& index, int role) const{
	if(!index.isValid()) return QVariant();
	if(role == Qt::TextAlignmentRole && index.column() >= 2) return QVariant(Qt::AlignRight);
	if(role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

	PJobDirTree* item = static_cast<PJobDirTree*>(index.internalPointer());

	if(index.column() == 0) return QVariant(item->data(0).toString().split("/").last());
	return item->data(index.column());
}

bool PJobDirModel::setData(const QModelIndex& index, const QVariant& value, int role){
	if(!index.isValid() || role != Qt::EditRole) return false;

	QString valueString = value.toString().trimmed();
	if(valueString == "") return false;

	PJobDirTree* item = static_cast<PJobDirTree*>(index.internalPointer());
	QString oldPath = item->data(0).toString();
	QString newPath = item->parent()->data(0).toString() + "/" + valueString;
	// Prüfe ob der neue Pfad bereits existiert.
	if(item->parent()->findItem(newPath)) return false;

	if(item->isDirectory()){
		emit layoutAboutToBeChanged();
		m_pjobFile->rename(oldPath + "/", newPath + "/");
		item->updatePath(0, oldPath.length(), newPath);
		emit layoutChanged();
	}
	else{
		m_pjobFile->rename(oldPath, newPath);
		item->setData(0, newPath);
		emit dataChanged(index, index);
	}
	emit changed();
	return true;
}

QStringList PJobDirModel::mimeTypes() const{
	return QStringList("text/uri-list");
}

QMimeData* PJobDirModel::mimeData(const QModelIndexList& indexes) const{
	QString tempDir = m_tempDir + QString("/%1").arg(QDateTime::currentDateTime().toTime_t()) + "/";
	QDir().mkpath(tempDir);

	QList<QUrl> urls;
	QModelIndex index;
	foreach(index, indexes){
		if(!index.isValid() || index.column() > 0)continue;
		PJobDirTree* item = static_cast<PJobDirTree*>(index.internalPointer());
		m_pjobFile->extract(tempDir, item->data(0).toString());
		urls << QUrl("file:///" + tempDir + item->data(0).toString().split("/").last());
	}
	QMimeData* mimeData = new QMimeData();
	mimeData->setUrls(urls);
	return mimeData;
}

bool PJobDirModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent){
	if(action == Qt::IgnoreAction) return true;
	if(!mimeData->hasFormat("text/uri-list")) return false;

	PJobDirTree* parentItem = parent.isValid() ? static_cast<PJobDirTree*>(parent.internalPointer()) : m_rootItem;
	if(!parentItem->isDirectory()) parentItem = parentItem->parent();

	QUrl url;
	foreach(url, mimeData->urls()){
		QString sourcePath(url.toString());
                if(sourcePath.startsWith("file:///")){
#ifdef Q_WS_WIN
                    sourcePath.remove(0,8);     //Win: "file:///C:\foo.bar" -> "C:\foo.bar"
#endif
#ifndef Q_WS_WIN
                    sourcePath.remove(0,7);     //Unices: "file:///home/foo/bar" -> "/home/foo/bar"
#endif
                }
		if(sourcePath.endsWith("/")) sourcePath.remove(-1,1);
		QFileInfo file(sourcePath);
                try{
                    if(file.isDir())
                            m_pjobFile->appendFolder(sourcePath, parentItem->data(0).toString() + "/" + file.fileName(), true);
                    else
                            m_pjobFile->appendFile(sourcePath, parentItem->data(0).toString() + "/" + file.fileName(), true);
                }catch(PJobFileError& e){
                    QMessageBox::critical(0, "Problem reading file(s)", e.msg(), QMessageBox::Ok);
                }

	}
	emit changed();
	return true;
}

Qt::ItemFlags PJobDirModel::flags(const QModelIndex& index) const{
	if(!index.isValid()) return Qt::ItemIsDropEnabled;
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	if(index.column() == 0 && (static_cast<PJobDirTree*>(index.internalPointer()))->data(0).toString() != "Resources/main.pscript")
		flags = flags | Qt::ItemIsEditable;
	return flags;
}

Qt::DropActions PJobDirModel::supportedDragActions() const{
	return Qt::CopyAction;
}

void PJobDirModel::removeEntries(const QModelIndexList& indexes){
	QModelIndex index;
	QMap<int,QModelIndex> removedIndexes;
	PJobDirTree* item;
	foreach(index, indexes){
		if(index.column() != 0) continue;
		item = static_cast<PJobDirTree*>(index.internalPointer());
		// Speichert die Indexe gelöschter Items in einer Map(sortiert) mit der Tiefe des Pfads
		// als Schlüssel, damit die Items in der richtigen Reihenfolge gelöscht werden.
		removedIndexes.insert(item->data(0).toString().split("/").size(), index);
	}
	QMapIterator<int, QModelIndex> iterator(removedIndexes);
	iterator.toBack();
	while(iterator.hasPrevious()){
		index = iterator.previous().value();
		item = static_cast<PJobDirTree*>(index.internalPointer());
		m_pjobFile->remove( item->data(0).toString() );
		beginRemoveRows(index.parent(), item->row(), item->row());
		item->parent()->deleteChild(item);
		endRemoveRows();
	}
	emit changed();
}

QModelIndex PJobDirModel::makeDir(const QModelIndex& parent){
	PJobDirTree* parentItem = parent.isValid() ? static_cast<PJobDirTree*>(parent.internalPointer()) : m_rootItem;
	if(!parentItem->isDirectory()) parentItem = parentItem->parent();

	QString dirPath = parentItem->data(0).toString() + "/New folder";
	if(parentItem->findItem(dirPath) != NULL){
		int x = 1;
		while(parentItem->findItem(QString("%1 (%2)").arg(dirPath).arg(x)) && x < std::numeric_limits<int>::max()) x++;
		if(x == std::numeric_limits<int>::max()) return QModelIndex();
		dirPath = QString("%1 (%2)").arg(dirPath).arg(x);
	}

	QList<QVariant> data;
	data << QVariant(dirPath);
	emit layoutAboutToBeChanged();
	PJobDirTree* newFolder = parentItem->append(data);
	emit layoutChanged();
	return createIndex(newFolder->row(), 0, newFolder);
}

void PJobDirModel::fileAdded(QList<QVariant> fileProperties){
	emit layoutAboutToBeChanged();
        QList<QList<QVariant> > files;
	files << fileProperties;
	m_rootItem->populateTree(files);
	emit layoutChanged();
}
