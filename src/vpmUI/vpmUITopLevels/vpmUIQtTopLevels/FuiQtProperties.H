// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_PROPERTIES_H
#define FUI_QT_PROPERTIES_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUITopLevels/FuiProperties.H"


class FuiQtStartGuide : public FFuQtWidget, public FuiStartGuide
{
public:
  FuiQtStartGuide(QWidget* parent, const char* name = "FuiQtStartGuide");

  virtual bool initWidgets();
};


class FuiQtProperties : public FFuQtWidget, public FuiProperties
{
public:
  FuiQtProperties(QWidget* parent, const char* name = "FuiQtProperties");

  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiProperties::setSensitivity(isSensitive);
  }

protected:
  virtual void showEvent(QShowEvent* e);
  virtual void resizeEvent(QResizeEvent* e);
};

#endif
