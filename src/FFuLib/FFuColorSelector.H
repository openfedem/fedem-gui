// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_COLOR_SELECTOR_H
#define FFU_COLOR_SELECTOR_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuColor.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuScale;
class FFuIOField;


class FFuColorSelector : virtual public FFuComponentBase
{
public:
  FFuColorSelector() { myColor = myInitialColor = { 0,0,0 }; }

  const FFuColor& getInitialColor() const { return myInitialColor; }
  const FFuColor& getColor() const { return myColor; }
  void setInitialColor(const FFuColor& aColor);
  void setColor(const FFuColor& aColor, bool notify = true);
  void setColorChangedCB(const FFaDynCB1<const FFuColor&>& cb) { myCB = cb; }

protected:
  // Initialisation to be called by UILib dep. subclass
  void init();

  virtual void updateSampleFrame() = 0;

protected:
  FFuScale*   myRedScale;
  FFuScale*   myGreenScale;
  FFuScale*   myBlueScale;
  FFuIOField* myRedValue;
  FFuIOField* myGreenValue;
  FFuIOField* myBlueValue;

  FFaDynCB1<const FFuColor&> myCB;

  FFuColor myColor;
  FFuColor myInitialColor;

private:
  void updateSlidersAndFields();

  void fieldRedValue(int red);
  void fieldGreenValue(int green);
  void fieldBlueValue(int blue);

  void sliderRedValue(int red);
  void sliderGreenValue(int green);
  void sliderBlueValue(int blue);
};

#endif
