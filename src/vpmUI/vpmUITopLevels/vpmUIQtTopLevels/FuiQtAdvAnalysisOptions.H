// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_ADV_ANALYSIS_OPTIONS_H
#define FUI_QT_ADV_ANALYSIS_OPTIONS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiAdvAnalysisOptions.H"


class FuiQtAdvAnalysisOptions : public FFuQtTopLevelShell,
                                public FuiAdvAnalysisOptions
{
public:
  FuiQtAdvAnalysisOptions(int xpos, int ypos, int width, int height,
                          bool basicMode, const char* title, const char* name);

private:
  void initLayout(QWidget* parentSheet, int iOpt);

  virtual void setSensitivity(bool sens)
  {
    this->FuiAdvAnalysisOptions::setSensitivity(sens);
  }

protected:
  virtual void onAdvBtnClicked();

  virtual void resizeEvent(QResizeEvent* e);
};

#endif
