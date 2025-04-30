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
public:
  void setInitialColor(const FFuColor& aColor);
  void setColor(const FFuColor& aColor, bool notify = true);
  const FFuColor& getColor() const;

  void setColorChangedCB(const FFaDynCB1<FFuColor>& aDynCB);
  void setOkButtonClickedCB(const FFaDynCB1<FFuColor>& aDynCB);
  void setCancelButtonClickedCB(const FFaDynCB1<FFuColor>& aDynCB);

  void setColorChangedCB(const FFaDynCB2<FFuColor,FFuComponentBase*>& aDynCB);
  void setOkButtonClickedCB(const FFaDynCB2<FFuColor,FFuComponentBase*>& aDynCB);
  void setCancelButtonClickedCB(const FFaDynCB2<FFuColor,FFuComponentBase*>& aDynCB);

protected:
  void onDlgButtonClicked(int);
  void onColorChanged(FFuColor aColor);

  // Initialisation to be called by UILib dep. subclass
  void init();

protected:
  FFaDynCB1<FFuColor> myOkButtonClickedCB;
  FFaDynCB1<FFuColor> myCancelButtonClickedCB;
  FFaDynCB1<FFuColor> myColorChangedCB;

  FFaDynCB2<FFuColor,FFuComponentBase*> myOkButtonClickedWPtrCB;
  FFaDynCB2<FFuColor,FFuComponentBase*> myCancelButtonClickedWPtrCB;
  FFaDynCB2<FFuColor,FFuComponentBase*> myColorChangedWPtrCB;

  FFuDialogButtons* myDialogButtons;
  FFuColorSelector* myColorSelector;
};

#endif
