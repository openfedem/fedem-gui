// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CURVE_AXIS_DEFINITION_H
#define FUI_CURVE_AXIS_DEFINITION_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>

class FFuPushButton;
class FFuOptionMenu;
class FFuIOField;
class FFuLabel;


class FuiCurveAxisDefinition : virtual public FFuComponentBase,
                               public FFuUAExistenceHandler
{
  using Strings     = std::vector<std::string>;
  using FFaDynCBstr = FFaDynCB1<std::string>;

public:
  FuiCurveAxisDefinition();

  void setUIValues(const std::string& result,
		   const Strings& opers, const std::string& selOper);

  void setEditResultCB(const FFaDynCB0& dynCB) { editResultCB = dynCB; }
  void setOperSelectedCB(const FFaDynCBstr& dynCB) { operSelectedCB = dynCB; }

protected:
  void initWidgets();

  FFuLabel*      resultLabel;
  FFuPushButton* editButton;
  FFuOptionMenu* operMenu;

private:
  FFaDynCB0   editResultCB;
  FFaDynCBstr operSelectedCB;
};


class FuiCurveTimeRange : virtual public FFuComponentBase
{
public:
  FuiCurveTimeRange();

  void setObjLabel(const std::string& label);
  void setUIValues(double tmin, double tmax, const std::string& selOper);
  void getUIValues(double& tmin, double& tmax, std::string& selOper);

  void setChangeCB(const FFaDynCB0& dynCB) { changeCB = dynCB; }

protected:
  void initWidgets();

private:
  void onDoubleChanged(double)     { changeCB.invoke(); }
  void onOperSelected(std::string) { changeCB.invoke(); }

protected:
  FFuLabel*      spaceObj;
  FFuIOField*    minField;
  FFuIOField*    maxField;
  FFuOptionMenu* operMenu;

private:
  FFaDynCB0 changeCB;
};

#endif
