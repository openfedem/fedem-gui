// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CTRL_ELEM_PROPERTIES_H
#define FUI_CTRL_ELEM_PROPERTIES_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include <string>
#include <vector>

class FFuLabel;
class FFuLabelFrame;
class FuiParameterView;


class FuiCtrlElemProperties : virtual public FFuComponentBase,
                              public FFuUAExistenceHandler,
                              public FFuUADataHandler
{
public:
  // Virtual override of the Data dialog methods :
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void buildDynamicWidgets(const FFuaUIValues* values);

  // Lock from user input :
  virtual void setSensitivity(bool isSensitive);

protected:
  // Init method to be called by UILibDep sub class after it
  // has initialized all the shado widget pointers.
  void initWidgets();

private:
  // Callback called by all fields. Collects all the data from
  // the UI and calls setAndGetDBValuesCB with the new data.
  void onValueChanged(double);

protected:
  FFuLabelFrame*    myElemFrame;
  FFuLabel*         myElemPixmap;
  FFuLabelFrame*    myParameterFrame;
  FuiParameterView* myParameterView;
};


class FuaCtrlElemPropertiesValues : public FFuaUIValues
{
public:
  using CtrlParameter = std::pair<std::string,double>;

  int                        ctrlTypeIdx = 0;
  const char**               pixmap = NULL;
  std::vector<CtrlParameter> parameters;
};

#endif
