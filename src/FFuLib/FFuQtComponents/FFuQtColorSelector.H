// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_COLOR_SELECTOR_H
#define FFU_QT_COLOR_SELECTOR_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuColorSelector.H"

class QFrame;


class FFuQtColorSelector : public FFuQtWidget, public FFuColorSelector
{
public:
  FFuQtColorSelector(QWidget* parent=NULL, const char* name="Color selector");

protected:
  virtual void updateSampleFrame();
  virtual void resizeEvent(QResizeEvent* e);

private:
  QWidget* mySelector;
  QFrame*  mySampleFrame;
};

#endif
