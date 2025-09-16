/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * AirfoilSelector.C
 *
 *  Created on: Jul 4, 2012
 *      Author: runarhr
 */

#include "FFuLib/FFuCustom/components/guiComponents/AirfoilSelector.H"
#include "FFuLib/FFuCustom/components/DataNode.H"
#include "FFuLib/FFuCustom/mvcModels/AirfoilSelectionModel.H"
#include "FFuLib/Icons/turbineBlade.xpm"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractItemView>
#include <QFileDialog>
#include <QDir>

#include <iostream>


AirfoilSelector::AirfoilSelector(const std::string& instPath, QWidget* parent) : QWidget(parent)
{
  customFolder = standardFolder = QString(instPath.c_str());

  // Initialize widgets
  QLabel* apBanner = new QLabel(this);
  apBanner->setPixmap(QPixmap(turbineBlade_xpm));

  apCustomFolderRadioButton = new QRadioButton("Custom folder", this);
  apDefaultFolderRadioButton = new QRadioButton("Fedem library",this);
  apCustomFolderRadioButton->setEnabled(false);
  apDefaultFolderRadioButton->setChecked(true);

  apBrowseButton = new QPushButton("&Browse...");
  apBrowseButton->setMinimumWidth(2);

  apAirfoilSelectionModel = AirfoilSelectionModel::instance();

  apAvailableAirfoilsPullDown = new QComboBox(this);
  apAvailableAirfoilsPullDown->setModel(apAirfoilSelectionModel);
  apAvailableAirfoilsPullDown->setRootModelIndex(apAirfoilSelectionModel->index(0,0,QModelIndex()));

  apPathField = new QLineEdit(customFolder,this);
  apPathField->setEnabled(false);

  // Layouts
  QBoxLayout* apMainLayout = new QVBoxLayout(this);
  QBoxLayout* apButtonLayout = new QVBoxLayout();
  QBoxLayout* apFolderLayout = new QHBoxLayout();

  apMainLayout->setContentsMargins(2, 0, 2, 0);
  apButtonLayout->setContentsMargins(2, 0, 2, 0);

  apFolderLayout->addWidget(apCustomFolderRadioButton);
  apFolderLayout->addWidget(apPathField);
  apFolderLayout->addWidget(apBrowseButton);

  apButtonLayout->addLayout(apFolderLayout);
  apButtonLayout->addWidget(apDefaultFolderRadioButton);

  apMainLayout->addWidget(apBanner);
  apMainLayout->addLayout(apButtonLayout);
  apMainLayout->addWidget(apAvailableAirfoilsPullDown);

  this->setMinimumHeight(25);

  // Connections
  QObject::connect(apBrowseButton, SIGNAL(clicked()), this, SLOT(customFolderBrowse()));
  QObject::connect(apCustomFolderRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonsChanged()));
  QObject::connect(apDefaultFolderRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonsChanged()));
}


QAbstractItemView* AirfoilSelector::getView()
{
  return apAvailableAirfoilsPullDown->view();
}


void AirfoilSelector::selectItem(const QString& value)
{
  if (value == "")
  {
    apAvailableAirfoilsPullDown->setRootModelIndex(apAirfoilSelectionModel->index(0,0,QModelIndex()));
    apAvailableAirfoilsPullDown->setCurrentIndex(0);
    return;
  }

  DataNode* dNode = this->getNodeAt(0,0);
  int nNode = dNode ? dNode->getParentNode()->subNodeCount() : 0;
  for (int i = 0; i < nNode; i++)
    if ((dNode = this->getNodeAt(i,0)))
      for (int j = 0; j < dNode->subNodeCount(); j++)
        if (value == dNode->getData(0).toString() + dNode->subNode(j)->getData(0).toString())
          apAvailableAirfoilsPullDown->setCurrentIndex(j);
}


void AirfoilSelector::addEntry(const QString& fullPath)
{
  QString name = fullPath.split(QDir::separator()).last();
  int index = fullPath.lastIndexOf(QDir::separator());
  QString path = fullPath.left(index+1);

  apAirfoilSelectionModel->addItem(path, name);
}


void AirfoilSelector::removeEntry(const QString& fullPath)
{
  QString name = fullPath.split(QDir::separator()).last();
  int index = fullPath.lastIndexOf(QDir::separator());
  QString path = fullPath.left(index+1);

  apAirfoilSelectionModel->removeItem(path, name);
}


void AirfoilSelector::customFolderBrowse()
{
  customFolder = QFileDialog::getExistingDirectory(this, "Select Airfoil Directory", customFolder, QFileDialog::DontResolveSymlinks);
  customFolder = QDir::toNativeSeparators(customFolder);
  if (customFolder == "")
    customFolder = standardFolder;
  else
  {
    apPathField->setText(customFolder);
    apCustomFolderRadioButton->setEnabled(true);
    if (apCustomFolderRadioButton->isChecked())
      this->setLibrary(customFolder, customFolder == standardFolder);
  }
}


void AirfoilSelector::setLibrary(QString path, bool locked)
{
  if (path == "") return;

  apAirfoilSelectionModel->removeDirectory( 0 );
  apAirfoilSelectionModel->addDirectory(0, QDir::toNativeSeparators(path),locked);

  apAvailableAirfoilsPullDown->setRootModelIndex(apAirfoilSelectionModel->index(0,0,QModelIndex()));
  emit(this->selectionModelChanged());
  this->selectItem("");
  apAvailableAirfoilsPullDown->view()->setCurrentIndex(apAirfoilSelectionModel->index(0,0,apAvailableAirfoilsPullDown->rootModelIndex()));
}


void AirfoilSelector::radioButtonsChanged()
{
  if (apDefaultFolderRadioButton->isChecked())
    this->setLibrary(standardFolder, true);
  else if (apCustomFolderRadioButton->isChecked())
    this->setLibrary(customFolder, customFolder == standardFolder);
}


QString AirfoilSelector::getCurrentItem() const
{
  if (apAvailableAirfoilsPullDown->currentIndex() == -1)
    return QString("No Airfoil Selected");

  QModelIndex parentIndex = apAirfoilSelectionModel->index(0, 0, QModelIndex());
  QModelIndex childIndex = apAirfoilSelectionModel->index(apAvailableAirfoilsPullDown->currentIndex(), 0, parentIndex);
  QString folder = apAirfoilSelectionModel->data(parentIndex, Qt::DisplayRole).toString();
  QString name = apAirfoilSelectionModel->data(childIndex, Qt::DisplayRole).toString();
  return folder + name;
}


QString AirfoilSelector::getCurrentFolder() const
{
  if (apDefaultFolderRadioButton->isChecked())
    return standardFolder;
  else if (apCustomFolderRadioButton->isChecked())
    return customFolder;

  return QString();
}


void AirfoilSelector::refresh()
{
  QVector<QString> directories;
  QVector<bool> readOnlyStates;

  // Get all directories
  int i, numOfChildren = apAirfoilSelectionModel->rowCount();
  for (i = 0; i < numOfChildren; i++)
  {
    DataNode* dNode = this->getNodeAt(i,0);
    QString path = dNode->getData(0).toString();
    path.resize(path.size()-1);//remove last slash
    directories.push_back(path);
    readOnlyStates.push_back(dNode->getData(1).toBool());
  }

  // remove all directories
  apAirfoilSelectionModel->clearModel();

  // add all directories
  for (i = numOfChildren-1; i >= 0; i--)
    apAirfoilSelectionModel->addDirectory(0, directories.at(i), readOnlyStates.at(i));

  this->radioButtonsChanged();
}


DataNode* AirfoilSelector::getNodeAt(int r, int c) const
{
  void* ptr = apAirfoilSelectionModel->index(r,c).internalPointer();
  if (ptr) return static_cast<DataNode*>(ptr);

  std::cerr <<"  ** BladeSelector: Internal pointer at ("
            << r <<","<< c <<") is NULL"<< std::endl;
  return NULL;
}
