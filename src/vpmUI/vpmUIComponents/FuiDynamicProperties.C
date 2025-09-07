// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiDynamicProperties.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"


void FuiDynamicProperties::initWidgets(const FFaDynCB1<double>& acceptedCB)
{
  const char* labels[5] = {
    "Mass proportional",
    "Stiffness proportional",
    "Stiffness scale",
    "Mass scale",
    NULL };

  const char** label = labels;
  for (FFuLabelField* fld : myFields)
  {
    fld->setLabel(*(label++));
    fld->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
    fld->setAcceptedCB(acceptedCB);
  }

  myFields[0]->setLabelWidth(myFields[1]->myLabel->getWidthHint());
  myFields[3]->setLabelWidth(myFields[2]->myLabel->getWidthHint());

}


void FuiDynamicProperties::setValues(double alpha1, double alpha2,
                                     double sclK, double sclM,
                                     int blankFlags)
{
  if (blankFlags & 0x0002)
    myFields[0]->setValue("");
  else
    myFields[0]->setValue(alpha1);

  if (blankFlags & 0x0004)
    myFields[1]->setValue("");
  else
    myFields[1]->setValue(alpha2);

  if (blankFlags & 0x0010)
    myFields[2]->setValue("");
  else
    myFields[2]->setValue(sclK);

  if (blankFlags & 0x0008)
    myFields[3]->setValue("");
  else
    myFields[3]->setValue(sclM);
}


int FuiDynamicProperties::getValues(double& alpha1, double& alpha2,
                                    double& sclK, double& sclM)
{
  alpha1 = myFields[0]->getValue();
  alpha2 = myFields[1]->getValue();
  sclK   = myFields[2]->getValue();
  sclM   = myFields[3]->getValue();

  int blankFlags = 0x0000;

  if (myFields[0]->getText().length() == 0)
    blankFlags |= 0x0002;

  if (myFields[1]->getText().length() == 0)
    blankFlags |= 0x0004;

  if (myFields[2]->getText().length() == 0)
    blankFlags |= 0x0010;

  if (myFields[3]->getText().length() == 0)
    blankFlags |= 0x0008;

  return blankFlags;
}


void FuiDynamicProperties::setSensitivity(bool isSensitive)
{
  for (FFuLabelField* fld : myFields)
    fld->setSensitivity(isSensitive);
}
