// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_SCROLLED_LIST_DIALOG_H
#define FFU_SCROLLED_LIST_DIALOG_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuScrolledList;
class FFuNotes;
class FFuDialogButtons;


class FFuScrolledListDialog : virtual public FFuTopLevelShell
{
  using DynCB1 = FFaDynCB1<int>;
  using DynCB2 = FFaDynCB2<int,FFuComponentBase*>;

public:
  FFuScrolledListDialog();
  void setNotesText(const char* text);

  int getItem() const;

  void setOkButtonClickedCB(const DynCB1& aDynCB);
  void setCancelButtonClickedCB(const DynCB1& aDynCB);
  void setApplyButtonClickedCB(const DynCB1& aDynCB);

  void setOkButtonClickedCB(const DynCB2& aDynCB);
  void setCancelButtonClickedCB(const DynCB2& aDynCB);
  void setApplyButtonClickedCB(const DynCB2& aDynCB);

  FFuScrolledList*  myItemSelector;

protected:
  FFuNotes*         myNotes;
  FFuDialogButtons* myDialogButtons;

  void onDlgButtonClicked(int);

  void initWidgets();

private:
  DynCB1 myOkButtonClickedCB;
  DynCB1 myCancelButtonClickedCB;
  DynCB1 myApplyButtonClickedCB;

  DynCB2 myOkButtonClickedWPtrCB;
  DynCB2 myCancelButtonClickedWPtrCB;
  DynCB2 myApplyButtonClickedWPtrCB;
};

#endif
