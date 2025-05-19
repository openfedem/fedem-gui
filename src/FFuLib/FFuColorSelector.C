// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuScale.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuColorSelector.H"

enum colorComponent { RED = 0, GREEN = 1, BLUE = 2 };


/*!
  \class FFuColorSelector FFuColorSelector.H
  \brief Simple color selector

  Color is represented by RGB values, range 0-255.
  In the implementation color is represented as 8 bit integer for each channel.
  Total resolution is 24 bits (millions of colors).

  \warning There is no management of the allocated colors yet.
  The color table will run full!

  \author Jens Lien
*/

void FFuColorSelector::init()
{
  myRedScale->setMinMax(0,255);
  myRedScale->setDragCB(FFaDynCB1M(FFuColorSelector, this, sliderRedValue, int));
  myRedValue->setAcceptedCB(FFaDynCB1M(FFuColorSelector, this, fieldRedValue, int));
  myRedValue->setInputCheckMode(FFuIOField::INTEGERCHECK);

  myGreenScale->setMinMax(0,255);
  myGreenScale->setDragCB(FFaDynCB1M(FFuColorSelector, this, sliderGreenValue, int));
  myGreenValue->setAcceptedCB(FFaDynCB1M(FFuColorSelector, this, fieldGreenValue, int));
  myGreenValue->setInputCheckMode(FFuIOField::INTEGERCHECK);

  myBlueScale->setMinMax(0,255);
  myBlueScale->setDragCB(FFaDynCB1M(FFuColorSelector, this, sliderBlueValue, int));
  myBlueValue->setAcceptedCB(FFaDynCB1M(FFuColorSelector, this, fieldBlueValue, int));
  myBlueValue->setInputCheckMode(FFuIOField::INTEGERCHECK);

  this->updateSlidersAndFields();
}


/*!
  Set the initial color for the dialog. The color is reset to
  this if the user presses the cancel button.
  \sa setColor
*/

void FFuColorSelector::setInitialColor(const FFuColor& aColor)
{
  myInitialColor = myColor = aColor;

  this->updateSlidersAndFields();
  this->updateSampleFrame();
}

/*!
  Set the selected color for the dialog.
  \param aColor the color to be set
  \param notify if true, the color change callback is called.
  \sa setInitialColor getColor setColorChangeCB
*/

void FFuColorSelector::setColor(const FFuColor& aColor, bool notify)
{
  myColor = aColor;

  this->updateSlidersAndFields();
  this->updateSampleFrame();

  if (notify)
    myColorChangedCB.invoke(myColor);
}


/*!
  Set the callback to be called when the color changes
*/
void FFuColorSelector::setColorChangedCB(const FFaDynCB1<FFuColor>& aDynCB)
{
  myColorChangedCB = aDynCB;
}


void FFuColorSelector::updateSlidersAndFields()
{
  myRedScale->  setValue(myColor[RED]);
  myRedValue->  setValue(myColor[RED]);
  myGreenScale->setValue(myColor[GREEN]);
  myGreenValue->setValue(myColor[GREEN]);
  myBlueScale-> setValue(myColor[BLUE]);
  myBlueValue-> setValue(myColor[BLUE]);
}


void FFuColorSelector::fieldRedValue(int red)
{
  if (red < 0)
    red = 0;
  else if (red > 255)
    red = 255;

  myRedScale->setValue(red);
  this->sliderRedValue(red);
}

void FFuColorSelector::fieldGreenValue(int green)
{
  if (green < 0)
    green = 0;
  else if (green > 255)
    green = 255;

  myGreenScale->setValue(green);
  this->sliderGreenValue(green);
}

void FFuColorSelector::fieldBlueValue(int blue)
{
  if (blue < 0)
    blue = 0;
  else if (blue > 255)
    blue = 255;

  myBlueScale->setValue(blue);
  this->sliderBlueValue(blue);
}


void FFuColorSelector::sliderRedValue(int val)
{
  myRedValue->setValue(val);
  myColor[RED] = val;
  myColorChangedCB.invoke(myColor);
  this->updateSampleFrame();
}

void FFuColorSelector::sliderGreenValue(int val)
{
  myGreenValue->setValue(val);
  myColor[GREEN] = val;
  myColorChangedCB.invoke(myColor);
  this->updateSampleFrame();
}

void FFuColorSelector::sliderBlueValue(int val)
{
  myBlueValue->setValue(val);
  myColor[BLUE] = val;
  myColorChangedCB.invoke(myColor);
  this->updateSampleFrame();
}
