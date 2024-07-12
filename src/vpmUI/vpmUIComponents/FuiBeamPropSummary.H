// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_BEAM_PROP_SUMMARY_H
#define FUI_BEAM_PROP_SUMMARY_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include <array>

class FFuLabel;
class FFuLabelFrame;
class FFuLabelField;
class FFuOptionMenu;
class FFuToggleButton;
class FuiQueryInputField;


class FuiBeamPropSummary : public virtual FFuMultUIComponent
{
public:
  FuiBeamPropSummary() {}
  virtual ~FuiBeamPropSummary() {}

  // Combos
  FFuLabel*           myCrossSectionTypeLabel;
  FFuOptionMenu*      myCrossSectionTypeMenu;
  FFuLabel*           myMaterialDefLabel;
  FuiQueryInputField* myMaterialDefField;

  // Definition
  FFuLabelFrame*      myDefinitionFrame;
  FFuLabelField*      myDoField;
  FFuLabelField*      myDiField;
  FFuLabel*           myImage;
  FFuLabelField*      myGEAField;
  FFuLabelField*      myGEIyyField;
  FFuLabelField*      myGEIzzField;
  FFuLabelField*      myGItField;
  FFuLabelField*      myGmlField;
  FFuLabelField*      myGIpField;

  // Dependent properties
  FFuLabelFrame*      myDependencyFrame;
  FFuToggleButton*    myDependencyButton;
  FFuLabelField*      myAField;
  FFuLabelField*      myIyField;
  FFuLabelField*      myIzField;
  FFuLabelField*      myIpField;

  // Shear
  FFuLabelFrame*      myShearKFrame;
  FFuLabelField*      myShearKYField;
  FFuLabelField*      myShearKZField;
  FFuLabelField*      myShearGAsyField;
  FFuLabelField*      myShearGAszField;
  FFuLabelFrame*      myShearSFrame;
  FFuLabelField*      myShearSYField;
  FFuLabelField*      myShearSZField;

protected:
  virtual void placeWidgets(int width, int height);
  virtual void initWidgets();

public:
  void setAcceptedCBs(const FFaDynCB1<double>& aDynCB);
  void onCrossSectionTypeChanged(int value);
  void onBreakDependencyToggled(bool value);
  void setPropSensitivity(bool makeSensitive);
  void setValues(const std::array<double,18>& values);
  void getValues(std::array<double,18>& values) const;
};

#endif
