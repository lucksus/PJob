#include "ResultModel.h"
#include <QtGui/QColor>
#include "Results.h"

ResultModel::ResultModel()
{
    connect(&Results::getInstance(), SIGNAL(updated()), this, SLOT(update()));
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
    if(section < Results::getInstance().parameter_names().size()) return QVariant(Results::getInstance().parameter_names().at(section));
    else return QVariant(Results::getInstance().result_names().at(section - Results::getInstance().parameter_names().size()));
}

int ResultModel::rowCount(const QModelIndex &) const{
    return Results::getInstance().size();
}

int ResultModel::columnCount(const QModelIndex &) const{
    return Results::getInstance().parameter_names().size() + Results::getInstance().result_names().size();
}

QVariant ResultModel::data(const QModelIndex& index, int role) const{
    if(role == Qt::BackgroundRole && index.column() < Results::getInstance().parameter_names().size()) return QVariant(QColor(225,225,225));
	if(!index.isValid() || role != Qt::DisplayRole) return QVariant();

    QHash<QString,double> parameter_combination = Results::getInstance().parameter_combination_for_index(index.row());
    if(index.column() < Results::getInstance().parameter_names().size()){
        QString parameter_name = Results::getInstance().parameter_names().at(index.column());
        return QVariant(parameter_combination[parameter_name]);
	}

    QString result_name = Results::getInstance().result_names().at(index.column() - Results::getInstance().parameter_names().size());
    return QVariant(Results::getInstance().value(parameter_combination, result_name));
}

void ResultModel::initialize(QString pjobFile){
	emit modelInitialized();
}

void ResultModel::update(){
    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

void ResultModel::purge(){
}
