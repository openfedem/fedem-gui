// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CTRL_ELEM_PROPERTIES_H
#define FUI_CTRL_ELEM_PROPERTIES_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include <string>
#include <vector>

class FFuLabel;
class FFuLabelFrame;
class FuiParameterView;


class FuiCtrlElemProperties : virtual public FFuMultUIComponent,
			      public FFuUAExistenceHandler,
			      public FFuUADataHandler
{
public:
  FuiCtrlElemProperties() {}
  virtual ~FuiCtrlElemProperties() {}

  // Virtual override of the Data dialog methods :
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void buildDynamicWidgets(const FFuaUIValues* values);
  virtual void eraseDynamicWidgets();

  // Lock from user input :
  virtual void setSensitivity(bool isSensitive);

protected:
  // Init method to be called by UILibDep sub class after it
  // has initialized all the shado widget pointers.
  void initWidgets();

  // Overridden method that does the geometry management :
  virtual void placeWidgets(int width, int height);

private:
  // Callback called by all fields. Collects all the data from
  // the UI and calls setAndGetDBValuesCB with the new data.
  void onValueChanged(double);

protected:
  FFuLabel*         myElemPixmap;
  FFuLabelFrame*    myParameterFrame;
  FuiParameterView* myParameterView;
};


class FuaCtrlElemPropertiesValues : public FFuaUIValues
{
public:
  FuaCtrlElemPropertiesValues() { pixmap = NULL; }
  virtual ~FuaCtrlElemPropertiesValues() {}

  struct CtrlParameter
  {
    double value;
    std::string description;
    CtrlParameter(double v = 0.0) { value = v; }
  };

  const char**               pixmap;
  std::vector<CtrlParameter> parameters;
};

#endif
