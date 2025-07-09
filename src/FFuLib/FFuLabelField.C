// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"


void FFuLabelField::setLabel(const char* label)
{
  myLabel->setLabel(label);
}

void FFuLabelField::setValue(double value)
{
  myField->setValue(value);
}

void FFuLabelField::setValue(const std::string& value)
{
  myField->setValue(value.c_str());
}

double FFuLabelField::getValue() const
{
  return myField->getDouble();
}

std::string FFuLabelField::getText() const
{
  return myField->getValue();
}


// Field Accept :

void FFuLabelField::setAcceptedCB(const FFaDynCB1<double>& aDynCB)
{
  myField->setAcceptedCB(aDynCB);
}


// Label geometry :

int FFuLabelField::getLabelWidth(bool useHint)
{
  return useHint ? myLabel->getWidthHint() : myLabel->getWidth();
}

void FFuLabelField::setLabelWidth(int width)
{
  myLabel->setMinWidth(width);
}


// To prevent editing :

void FFuLabelField::setSensitivity(bool makeSensitive)
{
  myField->setSensitivity(makeSensitive);
}

bool FFuLabelField::getSensitivity()
{
  return myField->getSensitivity();
}


/*!
  Sets same tool tip to both label and field.
*/

void FFuLabelField::setToolTip(const char* tip)
{
  myLabel->setToolTip(tip);
  myField->setToolTip(tip);
}
