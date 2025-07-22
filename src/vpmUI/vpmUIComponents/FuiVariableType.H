// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_VARIABLE_TYPE_H
#define FUI_VARIABLE_TYPE_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuOptionMenu;


class FuiVariableType : public virtual FFuComponentBase
{
public:
  virtual void setSensitivity(bool isSensitive);

  void setChangedCB(const FFaDynCB0& aDynCB);
  void onMenuSelected(int);

  void setValue(int selection);
  int getValue() const;

protected:
  void initWidgets();

  FFuOptionMenu* myTypeMenu;

private:
  FFaDynCB0 myChangedCB;
};

#endif
