// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_BEAM_PROP_SUMMARY_H
#define FUI_QT_BEAM_PROP_SUMMARY_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiBeamPropSummary.H"

class QGroupBox;


class FuiQtBeamPropSummary : public FFuQtWidget, public FuiBeamPropSummary
{
public:
  FuiQtBeamPropSummary(QWidget* parent,
                       const char* name = "FuiQtBeamPropSummary");

  virtual void onCrossSectionTypeChanged(int value);

private:
  QGroupBox* myMaterialDefFrame;
  QGroupBox* myPipeDefFrame;
  QGroupBox* myGenericDefFrame;
  QGroupBox* myShearRedFrame;
  QGroupBox* myShearStiffFrame;
  QGroupBox* myDependentFrame;
};

#endif
