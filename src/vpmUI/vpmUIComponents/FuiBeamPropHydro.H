// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_BEAM_PROP_HYDRO_H
#define FUI_BEAM_PROP_HYDRO_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include <vector>

class FFuToggleButton;
class FFuLabelField;


class FuiBeamPropHydro : public virtual FFuMultUIComponent
{
public:
  FuiBeamPropHydro() {}
  virtual ~FuiBeamPropHydro() {}

  FFuToggleButton*    myBeamHydroToggleButton;
  FFuLabelField*      myBeamHydroDbField;
  FFuLabelField*      myBeamHydroDdField;
  FFuLabelField*      myBeamHydroCdField;
  FFuLabelField*      myBeamHydroCaField;
  FFuLabelField*      myBeamHydroCmField;
  FFuLabelField*      myBeamHydroCdAxialField;
  FFuLabelField*      myBeamHydroCaAxialField;
  FFuLabelField*      myBeamHydroCmAxialField;
  FFuLabelField*      myBeamHydroCdSpinField;
  FFuLabelField*      myBeamHydroDiField;

protected:
  virtual void placeWidgets(int width, int height);
  virtual void initWidgets();

public:
  void setAcceptedCBs(const FFaDynCB1<double>& aDynCB);
  void onBeamHydroToggled(bool value);
  void setPropSensitivity(bool makeSensitive);
  void setValues(const std::array<double,10>& values);
  void getValues(std::array<double,10>& values) const;
};

#endif
