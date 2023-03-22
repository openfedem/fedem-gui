// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QCloseEvent>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QFileDialog>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtModelExport.H"
#include "vpmUI/Fui.H"

#include "FFaLib/FFaDefinitions/FFaMsg.H"


FileSelectorWidget::FileSelectorWidget(QWidget* parent, const char* f) : QWidget(parent), filter(f)
{
	label = new QLineEdit();
	button = new QPushButton("Browse ...");
	connect(button, SIGNAL(clicked()), SLOT(browse()));
	layout = new QHBoxLayout();
	layout->addWidget(label);
	layout->addWidget(button);
	setLayout(layout);
}

void FileSelectorWidget::browse()
{
  QString dir = QFileDialog::getSaveFileName(this, "Choose location", label->text(), filter);
  if (!dir.isNull()) this->setLabel(dir.toStdString());
}

std::string FileSelectorWidget::getLabel() const
{
  return label->text().toStdString();
}

void FileSelectorWidget::setLabel(const std::string& lab)
{
  label->setText(lab.c_str());
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
	// ** Create Widgets **
	apExportButton = new QPushButton("Export");
	apApplyButton = new QPushButton("Apply");
	apCancelButton = new QPushButton("Cancel");
	apHelpButton = new QPushButton("Help");

	// Stream
	apStreamFileDialog = new FileSelectorWidget(this, "App(*.zip)");
	apStreamFileLabel = new QLabel("File Name:");
	apStreamInpIndLabel = new QLabel("Input Indicator Group:");
	apStreamInpIndEdit = new QLineEdit();
	apStreamOutIndLabel = new QLabel("Output Indicator Group:");
	apStreamOutIndEdit = new QLineEdit();
	apStreamWindowLabel = new QLabel("Window Size [sec]:");
	apStreamWindowEdit = new QLineEdit();
	apStreamStateCheckbox = new QCheckBox("Transfer solver state between windows");

	apStreamGrid = new QGridLayout();
	apStreamGrid->addWidget(apStreamFileLabel, 0, 0);
	apStreamGrid->addWidget(apStreamFileDialog, 0, 1);
	apStreamGrid->addWidget(apStreamInpIndLabel, 1, 0);
	apStreamGrid->addWidget(apStreamInpIndEdit, 1, 1);
	apStreamGrid->addWidget(apStreamOutIndLabel, 2, 0);
	apStreamGrid->addWidget(apStreamOutIndEdit, 2, 1);
	apStreamGrid->addWidget(apStreamWindowLabel, 3, 0);
	apStreamGrid->addWidget(apStreamWindowEdit, 3, 1);
	apStreamGrid->addWidget(apStreamStateCheckbox, 4, 1);

	apStreamBox = new QGroupBox("Stream app");
	apStreamBox->setCheckable(true);
	apStreamBox->setChecked(false);
	apStreamBox->setLayout(apStreamGrid);

	// Batch
	apBatchFileDialog = new FileSelectorWidget(this, "App(*.zip)");
	apBatchFileLabel = new QLabel("File Name:");
	apBatchInpIndLabel = new QLabel("Input Indicator Group:");
	apBatchInpIndEdit = new QLineEdit();
	apBatchSurfCheckbox = new QCheckBox("Surface Only");
	apBatchStressCheckbox = new QCheckBox("Stress Recovery");
	apBatchFECheckbox = new QCheckBox("All FE-Parts");

	apBatchGrid = new QGridLayout();
	apBatchGrid->addWidget(apBatchFileLabel, 0, 0);
	apBatchGrid->addWidget(apBatchFileDialog, 0, 1);
	apBatchGrid->addWidget(apBatchInpIndLabel, 1, 0);
	apBatchGrid->addWidget(apBatchInpIndEdit, 1, 1);
	apBatchGrid->addWidget(apBatchSurfCheckbox, 2, 1);
	apBatchGrid->addWidget(apBatchStressCheckbox, 3, 1);
	apBatchGrid->addWidget(apBatchFECheckbox, 4, 1);

	apBatchBox = new QGroupBox("Batch app");
	apBatchBox->setCheckable(true);
	apBatchBox->setChecked(false);
	apBatchBox->setLayout(apBatchGrid);

	// FMU
	apFMUFileDialog = new FileSelectorWidget(this, "FMU(*.fmu)");
	apFMUFileLabel = new QLabel("File Name:");

	apFMUGrid = new QGridLayout();
	apFMUGrid->addWidget(apFMUFileLabel, 0, 0);
	apFMUGrid->addWidget(apFMUFileDialog, 0, 1);

	apFMUBox = new QGroupBox("FMU");
	apFMUBox->setCheckable(true);
	apFMUBox->setChecked(false);
	apFMUBox->setLayout(apFMUGrid);

	// ** Create layouts **
	apMainLayout = new QVBoxLayout();
	apDialogButtonLayout = new QHBoxLayout();

	// ** Initialize layouts **
	apDialogButtonLayout->addWidget(apExportButton);
	apDialogButtonLayout->addWidget(apApplyButton);
	apDialogButtonLayout->addWidget(apCancelButton);
	apDialogButtonLayout->addWidget(apHelpButton);
	apDialogButtonLayout->insertStretch(-1);

	inputTable = new QTableWidget(0, 2, this);
	outputTable = new QTableWidget(0, 3, this);

	apWidgetLayout = new QHBoxLayout();

	apAppLayout = new QVBoxLayout();
	apAppLayout->addWidget(apStreamBox);
	apAppLayout->addWidget(apBatchBox);
	apAppLayout->addWidget(apFMUBox);

	apFunctionLayout = new QVBoxLayout();
	apInputsLabel = new QLabel("Input Indicators:");
	apOutputsLabel = new QLabel("Output Indicators:");
	apFunctionLayout->addWidget(apInputsLabel);
	apFunctionLayout->addWidget(inputTable);
	apFunctionLayout->addWidget(apOutputsLabel);
	apFunctionLayout->addWidget(outputTable);

	apWidgetLayout->addLayout(apAppLayout);
	apWidgetLayout->addLayout(apFunctionLayout);
	
	apMainLayout->addLayout(apWidgetLayout);
	
	apMainLayout->addStretch();
	apMainLayout->addLayout(apDialogButtonLayout);
	apMainLayout->setContentsMargins(5, 5, 5, 5);

	this->setLayout(apMainLayout);

	// Buttons
	QObject::connect(apExportButton, SIGNAL(clicked()), this, SLOT(xport()));
	QObject::connect(apApplyButton, SIGNAL(clicked()), this, SLOT(apply()));
	QObject::connect(apCancelButton, SIGNAL(clicked()), this, SLOT(close()));
	QObject::connect(apHelpButton, SIGNAL(clicked()), this, SLOT(help()));

	FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiQtModelExport::setUIValues(const FFuaUIValues* values)
{
	FuaModelExportValues* expValues = (FuaModelExportValues*)values;
	//UPDATE VALUES

	//STREAM
	apStreamFileDialog->setLabel(expValues->streamFilename);
	apStreamInpIndEdit->setText(expValues->streamInputIndGroup.c_str());
	apStreamOutIndEdit->setText(expValues->streamOutputIndGroup.c_str());
	apStreamWindowEdit->setText(QString::number(expValues->streamWindowSize));
	apStreamStateCheckbox->setChecked(expValues->streamTransferState);
	apStreamBox->setChecked(expValues->streamAppExport);

	//BATCH
	apBatchFileDialog->setLabel(expValues->batchFilename);
	apBatchInpIndEdit->setText(expValues->batchInputIndGroup.c_str());
	apBatchStressCheckbox->setChecked(expValues->batchStressRecovery);
	apBatchFECheckbox->setChecked(expValues->batchAllFEParts);
	apBatchSurfCheckbox->setChecked(expValues->batchSurfaceOnly);
	apBatchBox->setChecked(expValues->batchAppExport);

	//FMU
	apFMUFileDialog->setLabel(expValues->fmuFilename);
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
//-----------------------------------------------------------------------------

void FuiQtModelExport::getUIValues(FFuaUIValues* values)
{
	FuaModelExportValues* expValues = (FuaModelExportValues*)values;
	
	//STREAM
	expValues->streamFilename = apStreamFileDialog->getLabel();
	expValues->streamInputIndGroup = apStreamInpIndEdit->text().toStdString();
	expValues->streamOutputIndGroup = apStreamOutIndEdit->text().toStdString();
	expValues->streamWindowSize = apStreamWindowEdit->text().toDouble();
	expValues->streamTransferState = apStreamStateCheckbox->isChecked();
	expValues->streamAppExport = apStreamBox->isChecked();

	//BATCH
	expValues->batchFilename = apBatchFileDialog->getLabel();
	expValues->batchInputIndGroup = apBatchInpIndEdit->text().toStdString();
	expValues->batchStressRecovery = apBatchStressCheckbox->isChecked();
	expValues->batchAllFEParts = apBatchFECheckbox->isChecked();
	expValues->batchSurfaceOnly = apBatchSurfCheckbox->isChecked();
	expValues->batchAppExport = apBatchBox->isChecked();

	//FMU
	expValues->fmuFilename = apFMUFileDialog->getLabel();
	expValues->fmuAppExport = apFMUBox->isChecked();
}

// ************SLOTS**********

void FuiQtModelExport::showEvent(QShowEvent*)
{
  inputTable->clear();
  inputTable->setHorizontalHeaderLabels({ "Name", "Description" });
  inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

  outputTable->clear();
  outputTable->setHorizontalHeaderLabels({ "Name", "Description", "Threshold" });
  outputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}


void FuiQtModelExport::closeEvent(QCloseEvent*)
{
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
