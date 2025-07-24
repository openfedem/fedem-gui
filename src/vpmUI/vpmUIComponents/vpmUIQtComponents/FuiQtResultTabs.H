// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_RESULT_TABS_H
#define FUI_QT_RESULT_TABS_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiResultTabs.H"

class QGroupBox;


class FuiQtTriadResults : public FFuQtWidget, public FuiTriadResults
{
public:
  FuiQtTriadResults(QWidget* parent, const char* name = "FuiQtTriadResults");
};


class FuiQtJointResults : public FFuQtWidget, public FuiJointResults
{
public:
  FuiQtJointResults(QWidget* parent, const char* name = "FuiQtJointResults");

protected:
  virtual void setSpringDamper(bool onOff);

private:
  QGroupBox* mySpringFrame;
  QGroupBox* myDamperFrame;
};

#endif
