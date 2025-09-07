// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_BEAM_PROP_HYDRO_H
#define FUI_BEAM_PROP_HYDRO_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <array>

class FFuToggleButton;
class FFuLabelField;


class FuiBeamPropHydro : public virtual FFuComponentBase
{
protected:
  enum { DB, DD, CD, CA, CM,
         CD_AXIAL, CA_AXIAL, CM_AXIAL, CD_SPIN, DI,
         NFIELD };

  FFuToggleButton*                  myToggle;
  std::array<FFuLabelField*,NFIELD> myFields;

  void initWidgets();

public:
  void setToggledCB(const FFaDynCB1<bool>& aDynCB);
  void setAcceptedCBs(const FFaDynCB1<double>& aDynCB);
  void onBeamHydroToggled(bool value);
  void setPropSensitivity(bool makeSensitive);
  void setValues(bool onOrOff, const std::array<double,NFIELD>& values);
  bool getValues(std::array<double,NFIELD>& values) const;
};

#endif
