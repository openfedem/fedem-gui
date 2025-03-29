// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_SCROLLED_LIST_H
#define FFU_SCROLLED_LIST_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>


class FFuScrolledList : virtual public FFuComponentBase
{
public:
  FFuScrolledList() {}
  virtual ~FFuScrolledList() {}

  // List items interface

  virtual void addItem(const std::string& item) = 0; // Appending item to end of list
  virtual void setItems(const std::vector<std::string>& items) = 0; // Replace whole list

  virtual void deleteItem(int index) = 0; // index = 0,1,...,n

  virtual int  getSelectedItemIndex() const = 0;
  virtual void selectItem(int index, bool notify = false) = 0; // Selection callback invoked if notify==True
  virtual bool isItemSelected(int index) const = 0;

  virtual std::string getItemText(int index) const = 0;

  virtual int getNumberOfItems() const = 0;

  virtual void setSensitivity(bool isSensitive) = 0;

  // Callback interface

  void setBrowseSelectCB(const FFaDynCB1<int>& cb) { myBrowseSelectCB = cb; }
  void setActivateCB    (const FFaDynCB1<int>& cb) { myActivateCB = cb; }
  void setPasteCB       (const FFaDynCB1<const std::string&>& cb) { myPasteCB = cb; }
  void setClearCB       (const FFaDynCB0& cb) { myClearCB = cb; }
  void setDeleteButtonCB(const FFaDynCB0& cb) { myDeleteButtonCB = cb; }

protected:
  FFaDynCB1<int>         myBrowseSelectCB;
  FFaDynCB1<int>             myActivateCB;
  FFaDynCB1<const std::string&> myPasteCB;
  FFaDynCB0                     myClearCB;
  FFaDynCB0              myDeleteButtonCB;
};

#endif
