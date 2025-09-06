/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * TableModel.hpp
 *
 *  Created on: Jan 17, 2012
 *      Author: runarhr
 */

#ifndef TABLE_MODEL_HPP
#define TABLE_MODEL_HPP

#include <QSize>
#include <QVector>
#include <QVariant>
#include <QAbstractTableModel>

class QRegularExpressionValidator;


enum TableOrdering { ROW_DOMINANT, COLUMN_DOMINANT};

class TableModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  TableModel(int rows, int columns, TableOrdering order, QObject* parent=NULL);
  virtual ~TableModel();

  //! Inherited from QAbstractTableModel
  int rowCount(const QModelIndex& = QModelIndex()) const override;
  int columnCount(const QModelIndex& = QModelIndex()) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
    bool removeRow(int row, const QModelIndex & parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    bool insertColumn(int column, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex & parent = QModelIndex());
    bool removeColumn(int column, const QModelIndex & parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex());

    bool dropMimeData( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
    Qt::DropActions supportedDropActions () const;
    QStringList mimeTypes () const;

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setHeaders(QStringList &pHeaders);
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    //! Sets the desired size-hint for cells in the table. Used in SizeHintRole of the data-function.
    virtual void SetCellSizeHint(QSize size);
    //! Determines if the model is editable. If not, dropMimeData and insert & remove rows will not work
    void SetEditable(bool editable);
    //! Set data for entire model
    bool SetModelData( QVector<QVector<QVariant>*> *data);
	const QVector<QVector<QVariant>*> *GetModelData();

    bool addValidator(int index, QString regExp);
    bool addDefaultValue(const int &index, const QVariant &value);

	bool setHeadersToolTips(QStringList &pHeaders);

    TableOrdering getTableOrdering(){ return aOrder; }


protected:
    TableOrdering aOrder;
    QVector<QVector<QVariant>*>* apModelVector;
    QSize aCellSizeHint;
    bool aEditable;
    int initialRows;
    int initialColumns;

    std::map<int,QRegularExpressionValidator*> validators;
    std::map<int,QVariant> defaultValues;
	QVector<QVariant>* apHeaders;
    QVector<QVariant>* apHeadersTooltips;
};

#endif /* TABLEMODEL_HPP */
