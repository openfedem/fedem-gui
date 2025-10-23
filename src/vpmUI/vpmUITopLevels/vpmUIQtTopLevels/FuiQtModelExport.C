// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtModelExport.H"
#include "vpmUI/Fui.H"

#include "FFaLib/FFaDefinitions/FFaMsg.H"


FuiQtFileSelector::FuiQtFileSelector(const char* filter) : myFilter(filter)
{
  myLabel  = new QLineEdit();
  myButton = new QPushButton("Browse ...");

  this->connect(myButton, SIGNAL(clicked()), SLOT(browse()));

  QLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myLabel);
  layout->addWidget(myButton);
}


std::string FuiQtFileSelector::getLabel() const
{
  return myLabel->text().toStdString();
}


void FuiQtFileSelector::setLabel(const std::string& label)
{
  myLabel->setText(label.c_str());
}


void FuiQtFileSelector::browse()
{
  QString dir = QFileDialog::getSaveFileName(this, "Choose location",
                                             myLabel->text(), myFilter);
  if (!dir.isNull()) this->setLabel(dir.toStdString());
}


// Qt implementation of static create method in FuiModelExport
FuiModelExport* FuiModelExport::create(int xpos, int ypos,
                                       int width, int height,
                                       const char* title, const char* name)
{
  return new FuiQtModelExport(xpos, ypos, width, height, title, name);
}


FuiQtModelExport::FuiQtModelExport(int xpos, int ypos,
                                   int width, int height,
                                   const char* title, const char* name)
  : FFuQtTopLevelShell(NULL, xpos, ypos, width, height, title, name)
{
  apStreamBox = new QGroupBox("Stream app");
  apStreamBox->setCheckable(true);
  apStreamBox->setChecked(false);

  apStreamFileDialog  = new FuiQtFileSelector("App(*.zip)");
  apStreamInpIndEdit  = new QLineEdit();
  apStreamOutIndEdit  = new QLineEdit();
  apStreamWindowEdit  = new QLineEdit();
  apStreamStateToggle = new QCheckBox("Transfer solver state between windows");

  QGridLayout* apGrid = new QGridLayout(apStreamBox);
  apGrid->addWidget(new QLabel("File Name:"),              0, 0);
  apGrid->addWidget(new QLabel("Input Indicator Group:"),  1, 0);
  apGrid->addWidget(new QLabel("Output Indicator Group:"), 2, 0);
  apGrid->addWidget(new QLabel("Window Size [sec]:"),      3, 0);
  apGrid->addWidget(apStreamFileDialog,  0, 1);
  apGrid->addWidget(apStreamInpIndEdit,  1, 1);
  apGrid->addWidget(apStreamOutIndEdit,  2, 1);
  apGrid->addWidget(apStreamWindowEdit,  3, 1);
  apGrid->addWidget(apStreamStateToggle, 4, 1);

  apBatchBox = new QGroupBox("Batch app");
  apBatchBox->setCheckable(true);
  apBatchBox->setChecked(false);

  apBatchFileDialog   = new FuiQtFileSelector("App(*.zip)");
  apBatchInpIndEdit   = new QLineEdit();
  apBatchSurfToggle   = new QCheckBox("Surface Only");
  apBatchStressToggle = new QCheckBox("Stress Recovery");
  apBatchFEToggle     = new QCheckBox("All FE-Parts");

  apGrid = new QGridLayout(apBatchBox);
  apGrid->addWidget(new QLabel("File Name:"),             0, 0);
  apGrid->addWidget(new QLabel("Input Indicator Group:"), 1, 0);
  apGrid->addWidget(apBatchFileDialog,   0, 1);
  apGrid->addWidget(apBatchInpIndEdit,   1, 1);
  apGrid->addWidget(apBatchSurfToggle,   2, 1);
  apGrid->addWidget(apBatchStressToggle, 3, 1);
  apGrid->addWidget(apBatchFEToggle,     4, 1);

  apFMUBox = new QGroupBox("FMU");
  apFMUBox->setCheckable(true);
  apFMUBox->setChecked(false);

  apFMUFileDialog = new FuiQtFileSelector("FMU(*.fmu)");
  apFMUFileToggle = new QCheckBox("Include the external functions file");

  apGrid = new QGridLayout(apFMUBox);
  apGrid->addWidget(new QLabel("File Name:"), 0, 0);
  apGrid->addWidget(apFMUFileDialog, 0, 1);
  apGrid->addWidget(apFMUFileToggle, 1, 1);

  inputTable = new QTableWidget(0, 2);
  inputTable->setHorizontalHeaderLabels({ "Name", "Description" });
  inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

  outputTable = new QTableWidget(0, 3);
  outputTable->setHorizontalHeaderLabels({ "Name", "Description", "Threshold" });
  outputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

  QLayout* apFunctionLayout = new QVBoxLayout();
  apFunctionLayout->addWidget(new QLabel("Input Indicators:"));
  apFunctionLayout->addWidget(inputTable);
  apFunctionLayout->addWidget(new QLabel("Output Indicators:"));
  apFunctionLayout->addWidget(outputTable);

  apExportButton = new QPushButton("Export");
  apApplyButton  = new QPushButton("Apply");
  apCancelButton = new QPushButton("Cancel");
  apHelpButton   = new QPushButton("Help");

  QBoxLayout* apButtonLayout = new QHBoxLayout();
  apButtonLayout->addWidget(apExportButton);
  apButtonLayout->addWidget(apApplyButton);
  apButtonLayout->addWidget(apCancelButton);
  apButtonLayout->addWidget(apHelpButton);
  apButtonLayout->addStretch(1);

  QBoxLayout* apLayout = new QVBoxLayout();
  apLayout->addWidget(apStreamBox);
  apLayout->addWidget(apBatchBox);
  apLayout->addWidget(apFMUBox);

  QBoxLayout* apMainLayout = new QHBoxLayout();
  apMainLayout->addLayout(apLayout);
  apMainLayout->addLayout(apFunctionLayout);

  apLayout = new QVBoxLayout(this);
  apLayout->addLayout(apMainLayout);
  apLayout->addStretch(1);
  apLayout->addLayout(apButtonLayout);

  this->connect(apExportButton, SIGNAL(clicked()), this, SLOT(xport()));
  this->connect(apApplyButton,  SIGNAL(clicked()), this, SLOT(apply()));
  this->connect(apCancelButton, SIGNAL(clicked()), this, SLOT(close()));
  this->connect(apHelpButton,   SIGNAL(clicked()), this, SLOT(help()));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiQtModelExport::setUIValues(const FFuaUIValues* values)
{
  FuaModelExportValues* expValues = (FuaModelExportValues*)values;

  apStreamFileDialog->setLabel(expValues->streamFilename);
  apStreamInpIndEdit->setText(expValues->streamInputIndGroup.c_str());
  apStreamOutIndEdit->setText(expValues->streamOutputIndGroup.c_str());
  apStreamWindowEdit->setText(QString::number(expValues->streamWindowSize));
  apStreamStateToggle->setChecked(expValues->streamTransferState);
  apStreamBox->setChecked(expValues->streamAppExport);

  apBatchFileDialog->setLabel(expValues->batchFilename);
  apBatchInpIndEdit->setText(expValues->batchInputIndGroup.c_str());
  apBatchStressToggle->setChecked(expValues->batchStressRecovery);
  apBatchFEToggle->setChecked(expValues->batchAllFEParts);
  apBatchSurfToggle->setChecked(expValues->batchSurfaceOnly);
  apBatchBox->setChecked(expValues->batchAppExport);

  apFMUFileDialog->setLabel(expValues->fmuFilename);
  apFMUFileToggle->setChecked(expValues->includeExtFuncFile);
  apFMUBox->setChecked(expValues->fmuAppExport);

  int iRow = 0;
  inputTable->setRowCount(expValues->inputs.size());
  for (const DTInput& inp : expValues->inputs)
  {
    inputTable->setItem(iRow, 0, new QTableWidgetItem(std::get<0>(inp).c_str()));
    inputTable->setItem(iRow, 1, new QTableWidgetItem(std::get<1>(inp).c_str()));
    for (int iCol = 0; iCol < 2; iCol++)
      inputTable->item(iRow, iCol)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ++iRow;
  }

  iRow = 0;
  outputTable->setRowCount(expValues->outputs.size());
  for (const DTOutput& out : expValues->outputs)
  {
    outputTable->setItem(iRow, 0, new QTableWidgetItem(std::get<0>(out).c_str()));
    outputTable->setItem(iRow, 1, new QTableWidgetItem(std::get<1>(out).c_str()));
    outputTable->setItem(iRow, 2, new QTableWidgetItem(std::get<2>(out).c_str()));
    for (int iCol = 0; iCol < 2; iCol++)
      outputTable->item(iRow, iCol)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ++iRow;
  }
}


void FuiQtModelExport::getUIValues(FFuaUIValues* values)
{
  FuaModelExportValues* expValues = (FuaModelExportValues*)values;

  expValues->streamFilename = apStreamFileDialog->getLabel();
  expValues->streamInputIndGroup = apStreamInpIndEdit->text().toStdString();
  expValues->streamOutputIndGroup = apStreamOutIndEdit->text().toStdString();
  expValues->streamWindowSize = apStreamWindowEdit->text().toDouble();
  expValues->streamTransferState = apStreamStateToggle->isChecked();
  expValues->streamAppExport = apStreamBox->isChecked();

  expValues->batchFilename = apBatchFileDialog->getLabel();
  expValues->batchInputIndGroup = apBatchInpIndEdit->text().toStdString();
  expValues->batchStressRecovery = apBatchStressToggle->isChecked();
  expValues->batchAllFEParts = apBatchFEToggle->isChecked();
  expValues->batchSurfaceOnly = apBatchSurfToggle->isChecked();
  expValues->batchAppExport = apBatchBox->isChecked();

  expValues->fmuFilename = apFMUFileDialog->getLabel();
  expValues->includeExtFuncFile = apFMUFileToggle->isChecked();
  expValues->fmuAppExport = apFMUBox->isChecked();
}


void FuiQtModelExport::xport()
{
  // Verify that the export paths are valid
  bool invalid = false;
  if (apStreamBox->isChecked() && apStreamFileDialog->getLabel().empty())
  {
    FFaMsg::dialog("No path for stream app specified", FFaMsg::ERROR);
    invalid = true;
  }
  if (apBatchBox->isChecked() && apBatchFileDialog->getLabel().empty())
  {
    FFaMsg::dialog("No path for batch app specified", FFaMsg::ERROR);
    invalid = true;
  }
  if (apFMUBox->isChecked() && apFMUFileDialog->getLabel().empty())
  {
    FFaMsg::dialog("No path for fmu file specified", FFaMsg::ERROR);
    invalid = true;
  }
  if (invalid) return;

  this->updateDBValues();
  exportCB.invoke();
}


void FuiQtModelExport::apply()
{
  this->updateDBValues();
}


void FuiQtModelExport::help()
{
  Fui::showCHM("dialogbox/dt-export.htm");
}
