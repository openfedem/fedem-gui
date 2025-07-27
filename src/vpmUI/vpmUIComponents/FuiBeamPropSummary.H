// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_BEAM_PROP_SUMMARY_H
#define FUI_BEAM_PROP_SUMMARY_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <array>

class FFuLabel;
class FFuLabelField;
class FFuOptionMenu;
class FFuToggleButton;
class FuiQueryInputField;


class FuiBeamPropSummary : public virtual FFuComponentBase
{
public:
  FFuOptionMenu*      myCrossSectionTypeMenu;
  FuiQueryInputField* myMaterialDefField;
  FFuToggleButton*    myDependencyButton;

protected:
  enum { DO, DI, GEA, GEIYY, GEIZZ, GIT, GML, GIP,
         AREA, IY, IZ, IP,
	 KY, KZ, GASY, GASZ, SY, SZ,
         NFIELD };

  FFuLabel*                         myImage;
  std::array<FFuLabelField*,NFIELD> myFields;

  void initWidgets();

public:
  void setAcceptedCBs(const FFaDynCB1<double>& aDynCB);
  virtual void onCrossSectionTypeChanged(int value) = 0;
  void onBreakDependencyToggled(bool value);
  void setPropSensitivity(bool makeSensitive);
  void setValues(const std::array<double,NFIELD>& values);
  void getValues(std::array<double,NFIELD>& values) const;
};

#endif
