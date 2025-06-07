// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_COLOR_COMBOBOX_H
#define FFU_COLOR_COMBOBOX_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuColor.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


/*!
  Base class for two different types of color selection widgets.
  Provides an easy way of setting colors based on a few predefined colors.

  It is an interface to QtColorComboBox from Qt Solutions

  \note When entering rgb colors via FFuColor, keep values in the range 0..255
  \sa FFuQtColorComboBox
  \sa FFuQtColorPicker
  \author Sven-Kaare Evenseth
*/

class FFuColorChooser : virtual public FFuComponentBase
{
public:
  //! Sets current color to \a rgb. If \a rgb is not in list inserts it.
  virtual void setCurrentCol(const FFuColor& rgb) = 0;
  //! Inserts color \a rbg into list, and displays \a name next to it, at index \a index.
  virtual void insertCol(const FFuColor& rgb, const std::string& name, int index = -1) = 0;
  //! Returns current selected color.
  virtual FFuColor getCurrentCol() = 0;
  //! Turning this on will insert at new item at the bottom of the list,
  //! labeled "more...". Pressing this will pop up a more elaborate color select menu.
  virtual void enableColorDialog(bool enable) = 0;

  void setSelectionChangedCB(const FFaDynCB1<const FFuColor&>& cb) { mySelectionChangedCB = cb; }

protected:
  void callSelectionChangedCB(const FFuColor& newCol) { mySelectionChangedCB.invoke(newCol); }

private:
  FFaDynCB1<const FFuColor&> mySelectionChangedCB;
};

#endif
