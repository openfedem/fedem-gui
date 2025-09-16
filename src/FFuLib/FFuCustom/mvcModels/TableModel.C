/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * TableModel.cpp
 *
 *  Created on: Jan 17, 2012
 *      Author: runarhr
 */

#include <QMimeData>
#include <QRegularExpressionValidator>

#include "FFuLib/FFuCustom/mvcModels/TableModel.H"


TableModel::TableModel(int rows, int columns, TableOrdering order, QObject *parent)
    : QAbstractTableModel(parent)
{
	aOrder = order;
	initialRows = rows;
	initialColumns = columns;

	apModelVector = new QVector<QVector<QVariant>*>;
	switch(aOrder){
	case ROW_DOMINANT:
		for(int i = 0; i < rows; i++)
			apModelVector->push_back(new QVector<QVariant>(columns));
	case COLUMN_DOMINANT:
		for(int i = 0; i < columns; i++)
			apModelVector->push_back(new QVector<QVariant>(rows));
	}

	SetCellSizeHint(QSize(100,30));

	apHeaders = new QVector<QVariant>;
	apHeadersTooltips = new QVector<QVariant>;

	aEditable = true;
}

TableModel::~TableModel() {
	//TODO clean validators
	apHeaders->clear();
	delete apHeaders;
	apHeadersTooltips->clear();
	delete apHeadersTooltips;
}


int TableModel::rowCount(const QModelIndex&) const
{
  switch (aOrder) {
  case ROW_DOMINANT:
    return apModelVector->size();
  case COLUMN_DOMINANT:
    if (apModelVector->size())
      return apModelVector->at(0)->size();
  }
  return 0;
}

int TableModel::columnCount(const QModelIndex&) const
{
  switch (aOrder) {
  case COLUMN_DOMINANT:
    return apModelVector->size();
  case ROW_DOMINANT:
    if (apModelVector->size())
      return apModelVector->at(0)->size();
  }
  return 0;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::SizeHintRole)
    return aCellSizeHint;

  int irow = index.row();
  int icol = index.column();
  if (irow >= 0 && icol >= 0)
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
      switch (aOrder) {
      case ROW_DOMINANT:
        if (irow < apModelVector->size())
          if (icol < apModelVector->at(irow)->size())
            return apModelVector->at(irow)->at(icol);
        break;
      case COLUMN_DOMINANT:
        if (icol < apModelVector->size())
          if (irow < apModelVector->at(icol)->size())
            return apModelVector->at(icol)->at(irow);
        break;
      }
      break;

    case Qt::TextAlignmentRole:
      return QVariant(Qt::AlignRight);
    }

  return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role == Qt::DisplayRole){
		switch(aOrder){
			case COLUMN_DOMINANT:
				if (orientation == Qt::Vertical) {
					if(section < apHeaders->size()){
						return apHeaders->at(section);
					}else{
						return QVariant();
					}
				}
				if (orientation == Qt::Horizontal) {
					return section + 1;
				}
				break;
			case ROW_DOMINANT:
				if (orientation == Qt::Horizontal) {
					if(section < apHeaders->size()){
						return QString(apHeaders->at(section).toString());
					}
					else{
						return QVariant();
					}
				}
				if (orientation == Qt::Vertical) {
					return section + 1;
				}
				break;
		}
    }

    if(role == Qt::ToolTipRole){
		switch(aOrder){
			case COLUMN_DOMINANT:
				if (orientation == Qt::Vertical) {
					if(section < apHeadersTooltips->size()){
						return apHeadersTooltips->at(section);
					}else{
						return QVariant();
					}
				}

				if (orientation == Qt::Horizontal) {
					return section + 1;
				}
				break;
			case ROW_DOMINANT:
				if (orientation == Qt::Horizontal) {
					if(section < apHeadersTooltips->size()){
						return apHeadersTooltips->at(section);
					}else{
						return QVariant();
					}
				}

				if (orientation == Qt::Vertical) {
					return section + 1;
				}
				break;
		}
    }

    return QVariant();
}

bool TableModel::SetModelData( QVector<QVector<QVariant>*> *data ){
		apModelVector = data;
		emit(layoutAboutToBeChanged());
		emit(dataChanged( index(0,0), index( rowCount()-1,columnCount()-1) ) );
		emit(layoutChanged());

		return true;
}

const  QVector<QVector<QVariant>*> *TableModel::GetModelData(){
	return apModelVector;
}


bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole && aEditable) {
		int row = index.row();
		int column = index.column();

		QVariant newValue;
		bool okToDouble;
		value.toDouble(&okToDouble);
		(okToDouble) ? newValue = value.toDouble() : newValue = value;

		if(newValue.toString() == "")
			newValue = 0;

		switch (aOrder) {
		case ROW_DOMINANT:
			if(validators.count(index.column())){
				QString aString(value.toString());
				int aPos = 0;
				if(!(*validators.find(column)).second->validate(aString,aPos))
					return false;
			}

			apModelVector->at(row)->replace(column, newValue);
			break;
		case COLUMN_DOMINANT:
			if(validators.count(row)){
				QString aString(value.toString());
				int aPos = 0;
				if(!(*validators.find(row)).second->validate(aString,aPos))
					return false;
			}

			apModelVector->at(column)->replace(row, newValue);
			break;
		}
	} else {
		return false;
	}

	emit
	(dataChanged(index, index));
	return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const{
	if(aEditable){
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled ;
	}
	else{
		return QAbstractTableModel::flags(index) ;
	}
}

bool TableModel::insertRow(int row, const QModelIndex & parent){
	if (row >= 0 && aEditable && (aOrder == ROW_DOMINANT)) {
		beginInsertRows(parent, row, row);
			apModelVector->insert(row, new QVector<QVariant>(initialColumns,0.0f));
		endInsertRows();

		for(int i=0; i<apModelVector->at(row)->size(); i++){
			QVariant value(0.0f);
			if(defaultValues.count(i)){
				value = defaultValues.find(i)->second;
			}

			setData(index(row, i, parent), value, Qt::EditRole);
		}

		emit(dataChanged(index(row, 0, parent), index(rowCount() - 1, columnCount() - 1, parent)));
		return true;
	}
	else{
		return false;
	}
}

bool TableModel::removeRow(int row, const QModelIndex & parent){
	if(rowCount() > row && row >= 0 && aEditable && (aOrder == ROW_DOMINANT)){
		beginRemoveRows(parent, rowCount()-1, rowCount()-1);
			delete apModelVector->at(row);
			apModelVector->remove(row);
		endRemoveRows();
		emit(dataChanged(index(row,0,parent), index(rowCount()-1, columnCount() -1, parent)));
		return true;
	}
	else{
		return false;
	}
}

bool TableModel::insertRows(int, int, const QModelIndex&){
	//TODO
	return false;
}

bool TableModel::removeRows(int, int, const QModelIndex&){
	//TODO
	return false;
}

bool TableModel::insertColumn(int column, const QModelIndex & parent){
	if (column >= 0 && aEditable && (aOrder == COLUMN_DOMINANT)) {
		beginInsertColumns(parent, column, column);
			apModelVector->insert(column, new QVector<QVariant>(initialRows,0.0f));
		endInsertColumns();

		for(int i=0; i<apModelVector->at(column)->size(); i++){
			QVariant value(0.0f);
			if(defaultValues.count(i)){
				value = defaultValues.find(i)->second;
			}

			setData(index(i,column, parent), value, Qt::EditRole);
		}

		emit(dataChanged(index(0, column, parent), index(rowCount() - 1, columnCount() - 1, parent)));
		return true;
	}
	else{
		return false;
	}
}

bool TableModel::removeColumn(int column, const QModelIndex & parent){
	if(columnCount() > column && column >= 0 && aEditable && (aOrder == COLUMN_DOMINANT)){
		beginRemoveColumns(parent, columnCount()-1, columnCount()-1);
			delete apModelVector->at(column);
			apModelVector->remove(column);
		emit(dataChanged(index(0,column,parent), index(rowCount() -1, columnCount() -1, parent)));
		endRemoveColumns();
		return true;
	}
	else{
		return false;
	}
}

bool TableModel::insertColumns(int, int, const QModelIndex&){
	//TODO
	return false;
}

bool TableModel::removeColumns(int, int, const QModelIndex&){
	//TODO
	return false;
}

void TableModel::SetCellSizeHint(QSize sizeHint){
	aCellSizeHint = sizeHint;
}

bool TableModel::dropMimeData(const QMimeData* data, Qt::DropAction, int row, int column, const QModelIndex& parent ){
	if (parent.isValid() && data->hasFormat("text/plain") && aEditable) {
		QString plainText = data->text();

		QStringList rowList = plainText.split("\n",Qt::SkipEmptyParts);
		for(int i = 0; i <  rowList.size(); i++)  {
			QStringList columnList = (rowList.at(i)).split(QRegularExpression("\\s+"));
				for(int j= 0; j < columnList.size(); j++)
				{
					if( (j+column) < columnCount(parent) && (i+row) < rowCount(parent) ){
						QVariant value;
						bool okToDouble;
						columnList.at(j).toDouble(&okToDouble);
						(okToDouble) ? value = QVariant(columnList.at(j).toDouble()) : value = QVariant(columnList.at(j));

						switch(aOrder){
						case ROW_DOMINANT:
							apModelVector->at(i + parent.row())->replace(j+parent.column(), value); break;
						case COLUMN_DOMINANT:
							apModelVector->at(j + parent.column())->replace(i+parent.row(), value); break;
						}
					}
				}
		}

		emit(dataChanged(index(0,0,parent), index(rowCount()-1, columnCount()-1, parent)));
		return true;
	}
	return false;
}

Qt::DropActions TableModel::supportedDropActions () const{
	return Qt::CopyAction | Qt::MoveAction ;
}

QStringList TableModel::mimeTypes () const{
	QStringList types;
	types << "text/plain";
	return	types;
}

void TableModel::SetEditable(bool editable)
{
	aEditable = editable;
}

bool TableModel::addValidator(int index, QString regExp){
	switch(aOrder){
	case ROW_DOMINANT:
		if(index >= initialColumns)
			return false;
	case COLUMN_DOMINANT:
		if(index >= initialRows)
			return false;
	}

	validators.insert(std::make_pair(index,new QRegularExpressionValidator(QRegularExpression(regExp),this)));
	return true;
}


bool TableModel::addDefaultValue(const int &index, const QVariant & value){
	switch(aOrder){
	case ROW_DOMINANT:
		if(index >= initialColumns)
			return false;
	case COLUMN_DOMINANT:
		if(index >= initialRows)
			return false;
	}

	defaultValues.insert(std::make_pair(index,QVariant(value)));
	return true;
}


bool TableModel::setHeaders(QStringList &pHeaders){
	if(pHeaders.size()){
		apHeaders->clear();
		for( QStringList::iterator headerIt = pHeaders.begin(); headerIt != pHeaders.end(); ++headerIt)
			apHeaders->push_back(*headerIt);
		return true;
	}
	return false;
}

bool TableModel::setHeadersToolTips(QStringList &pHeaders){
	if(pHeaders.size()){
		apHeadersTooltips->clear();
		for( QStringList::iterator headerIt = pHeaders.begin(); headerIt != pHeaders.end(); ++headerIt)
			apHeadersTooltips->push_back(*headerIt);
		return true;
	}
	return false;
}
