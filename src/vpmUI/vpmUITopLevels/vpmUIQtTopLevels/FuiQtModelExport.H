// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_MODEL_EXPORT_H
#define FUI_QT_MODEL_EXPORT_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiModelExport.H"

class QGroupBox;
class QCheckBox;
class QLineEdit;
class QTableWidget;
class QPushButton;


class FuiQtFileSelector : public QWidget
{
  Q_OBJECT

public:
  FuiQtFileSelector(const char* filter);

  std::string getLabel() const;
  void setLabel(const std::string& label);

public slots:
  void browse();

private:
  QLineEdit*   myLabel;
  QPushButton* myButton;
  QString      myFilter;
};


class FuiQtModelExport : public FFuQtTopLevelShell, public FuiModelExport
{
  Q_OBJECT

public:
  FuiQtModelExport(int xpos, int ypos, int width, int height,
                   const char* title, const char* name);

protected:
  void setUIValues(const FFuaUIValues* values) override;
  void getUIValues(FFuaUIValues* values) override;

public slots:
  void xport();
  void apply();
  void help();

private:
  // Stream
  QGroupBox* apStreamBox;
  FuiQtFileSelector* apStreamFileDialog;
  QLineEdit* apStreamInpIndEdit;
  QLineEdit* apStreamOutIndEdit;
  QLineEdit* apStreamWindowEdit;
  QCheckBox* apStreamStateToggle;

  // Batch
  QGroupBox* apBatchBox;
  FuiQtFileSelector* apBatchFileDialog;
  QLineEdit* apBatchInpIndEdit;
  QCheckBox* apBatchSurfToggle;
  QCheckBox* apBatchStressToggle;
  QCheckBox* apBatchFEToggle;

  // FMU
  QGroupBox* apFMUBox;
  FuiQtFileSelector* apFMUFileDialog;
  QCheckBox* apFMUFileToggle;

  // Functions
  QTableWidget* inputTable;
  QTableWidget* outputTable;

  // Dialog buttons
  QPushButton* apExportButton;
  QPushButton* apApplyButton;
  QPushButton* apCancelButton;
  QPushButton* apHelpButton;
};

#endif
