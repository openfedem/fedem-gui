/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * BladeSelector.H
 *
 *  Created on: Jul 4, 2012
 *      Author: runarhr
 */

#ifndef _BLADE_SELECTOR_H
#define _BLADE_SELECTOR_H

#include <QWidget>

class QTreeView;
class QToolButton;
class QLineEdit;
class BladeSelectionModel;
class DataNode;


class BladeSelector : public QWidget
{
  Q_OBJECT

public:
  BladeSelector(const std::string& dPath, QWidget* parent);
  virtual ~BladeSelector() {}

  QTreeView* getView() { return apAvailableBladesView; }
  BladeSelectionModel* getModel() { return apBladeSelectionModel; }

  void selectItem(const QString& value);
  bool itemExist(const QString& value);
  void addUnsavedItem(const QString& modelFileName, const QString& bladeName);
  void addEntry(const QString& fullPath);
  bool removeEntry(const QString& fullPath);
  void refresh();

  QString getCurrentFolder() const;

private:
  QToolButton* apHideShowButton;
  QToolButton* apAddLibButton;
  QToolButton* apRemoveLibButton;

  QTreeView* apAvailableBladesView;
  BladeSelectionModel* apBladeSelectionModel;
  QLineEdit* apPathField;

  QString propPath;
  QString currentPath;
  bool    expanded;

public slots:
  void hideShow();
  void bladeSelected();
  void addLibrary();
  void removeLibrary();

signals:
  void selectionModelChanged();

private:
  DataNode* getCurrentNode() const;
  DataNode* getNodeAt(int r, int c) const;
};

#endif
