// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_TRIAD_SUMMARY_H
#define FUI_QT_TRIAD_SUMMARY_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiTriadSummary.H"

class QGroupBox;


class FuiQtTriadSummary : public FFuQtWidget, public FuiTriadSummary
{
public:
  FuiQtTriadSummary(QWidget* parent, const char* name = "FuiQtTriadSummary");

  virtual void showMass(int onOff);
  virtual void showSysDir(bool onOff) { showFrame(mySysDirFrame,onOff); }
  virtual void showConnector(bool onOff) { showFrame(myConnectorFrame,onOff); }

protected:
  void resizeEvent(QResizeEvent* e);

private:
  static void showFrame(QGroupBox* frame, bool onOff);

  QGroupBox* myMassFrame;
  QGroupBox* mySysDirFrame;
  QGroupBox* myConnectorFrame;
};

#endif
