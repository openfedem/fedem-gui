// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_JOINT_SUMMARY_H
#define FUI_QT_JOINT_SUMMARY_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiJointSummary.H"

class QGroupBox;


class FuiQtJointSummary : public FFuQtWidget, public FuiJointSummary
{
public:
  FuiQtJointSummary(QWidget* parent, const char* name = "FuiQtJointSummary");

  virtual void showCamVars(bool onOff) { showFrame(myCamFrame,onOff); }
  virtual void showScrew(bool onOff) { showFrame(myScrewFrame,onOff); }
  virtual void showFriction(char onOff);

protected:
  void resizeEvent(QResizeEvent* e);

private:
  static void showFrame(QGroupBox* frame, bool onOff);

  QGroupBox* myCamFrame;
  QGroupBox* myScrewFrame;
  QGroupBox* myFrictionFrame;
  QWidget*   qFrictionDof;
};

#endif
