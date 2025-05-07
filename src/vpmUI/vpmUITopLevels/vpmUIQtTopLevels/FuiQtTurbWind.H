// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_TURB_WIND_H
#define FUI_QT_TURB_WIND_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiTurbWind.H"

#include <QProcess>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QTextEdit;


class FuiQtTurbWind : public FFuQtTopLevelShell, public FuiTurbWind
{
  Q_OBJECT

public:
  FuiQtTurbWind(int xpos = 100, int ypos = 100,
                int width = 100, int height = 100,
                const char* title= "Fedem",
                const char* name = "FuiQtTurbWind");

  virtual void setValues(double hubHeight, double gridSize,
                         double timeInc, double duration,
                         const char* outputPath);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiTurbWind::setSensitivity(isSensitive);
  }

private slots:
  void onBtnTemplateFileClick();
  void onBtnOutputFolderClick();
  void onBtnGenerateClick();
  void onBtnCloseClick();
  void onBtnHelpClick();
  void onReadyReadStandardOutput();
  void onReadyReadStandardError();
  void onFinished(int status, QProcess::ExitStatus);

private:
  QProcess*    pProcess;
  QLineEdit*   editHubHeight;
  QLineEdit*   editGridHeight;
  QLineEdit*   editGridWidth;
  QLineEdit*   editNumGridY;
  QLineEdit*   editNumGridZ;
  QLineEdit*   editTimeStep;
  QLineEdit*   editDuration;
  QComboBox*   cmbIECturbc;
  QLineEdit*   editRefHt;
  QComboBox*   cmbWindType;
  QLineEdit*   editWindSpeed;
  QLineEdit*   editPLExp;
  QCheckBox*   chkTowerPoints;
  QLineEdit*   editRandSeed1;
  QLineEdit*   editOutputFolder;
  QLineEdit*   editTemplateFile;
  QPushButton* btnTemplateFile;
  QPushButton* btnOutputFolder;
  QPushButton* btnGenerate;
  QPushButton* btnClose;
  QPushButton* btnHelp;
  QTextEdit*   textEdit;
};

#endif
