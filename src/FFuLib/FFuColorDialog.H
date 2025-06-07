// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_COLOR_DIALOG_H
#define FFU_COLOR_DIALOG_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuColor;
class FFuColorSelector;
class FFuDialogButtons;


class FFuColorDialog : virtual public FFuTopLevelShell
{
  using DynCB1 = FFaDynCB1<const FFuColor&>;
  using DynCB2 = FFaDynCB2<const FFuColor&,FFuColorDialog*>;

public:
  void setInitialColor(const FFuColor& aColor);
  void setColor(const FFuColor& aColor);
  const FFuColor& getColor() const;

  void setColorChangedCB(const DynCB1& cb);
  void setOkButtonClickedCB(const DynCB1& cb);
  void setCancelButtonClickedCB(const DynCB1& cb);

  void setColorChangedCB(const DynCB2& cb);
  void setOkButtonClickedCB(const DynCB2& cb);
  void setCancelButtonClickedCB(const DynCB2& cb);

protected:
  void onDlgButtonClicked(int);
  void onColorChanged(const FFuColor& aColor);

  // Initialisation to be called by UILib dep. subclass
  void init();

protected:
  DynCB1 myOkButtonClickedCB;
  DynCB1 myCancelButtonClickedCB;
  DynCB1 myColorChangedCB;

  DynCB2 myOkButtonClickedCB2;
  DynCB2 myCancelButtonClickedCB2;
  DynCB2 myColorChangedCB2;

  FFuDialogButtons* myDialogButtons;
  FFuColorSelector* myColorSelector;
};

#endif
