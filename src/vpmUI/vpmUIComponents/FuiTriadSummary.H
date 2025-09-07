// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_TRIAD_SUMMARY_H
#define FUI_TRIAD_SUMMARY_H

#include "vpmUI/vpmUIComponents/FuiSummaryTable.H"

class FFuLabelField;
class FFuPushButton;
class FFuOptionMenu;
struct FuiTriadDOFValues;


class FuiTriadSummary : public FuiSummaryTable
{
public:
  void setSummary(int dof, const FuiTriadDOFValues& tval);

  virtual void showMass(int onOff) = 0;
  virtual void showSysDir(bool onOff) = 0;
  virtual void showConnector(bool onOff) = 0;

  FFuLabelField* myFENodeField;
  FFuPushButton* mySetAllFreeButton;
  FFuPushButton* mySetAllFixedButton;

  FFuLabelField* myMassField;
  FFuLabelField* myIxField;
  FFuLabelField* myIyField;
  FFuLabelField* myIzField;

  FFuLabel*      myTriadLabel;
  FFuOptionMenu* mySysDirMenu;
  FFuOptionMenu* myConnectorMenu;

protected:
  virtual void initWidgets();
};

#endif
