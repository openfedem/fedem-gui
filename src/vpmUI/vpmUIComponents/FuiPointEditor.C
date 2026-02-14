// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiPointEditor.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"


void FuiPointEditor::initWidgets()
{
  myPointUI->setValue(0.0,0.0,0.0);
  myPointUI->setGlobal();

  myPickButton->setPixMap(mouseMedium_xpm);
  myViewPointButton->setPixMap(look_xpm);

  myViewWhatButton->setPixMap(look_xpm);
  myIdField->setLabel("On");
  myIdField->setSensitivity(false);

  myViewPointButton->setArmCB(FFaDynCB0M(FuiPointEditor,this,onMyViewPointTouched));
  myViewPointButton->setUnarmCB(FFaDynCB0M(FuiPointEditor,this,onMyViewPointTouched));

  myViewWhatButton->setArmCB(FFaDynCB0M(FuiPointEditor,this,onMyViewWhatTouched));
  myViewWhatButton->setUnarmCB(FFaDynCB0M(FuiPointEditor,this,onMyViewWhatTouched));
}


void FuiPointEditor::hideOnWhatDisplay(bool doHide)
{
  if (doHide)
  {
    myViewWhatButton->popDown();
    myIdField->popDown();
  }
  else
  {
    myViewWhatButton->popUp();
    myIdField->popUp();
  }
}


void FuiPointEditor::setSensitivity(bool isSensitive)
{
  myPointUI->setSensitivity(IAmEditable && isSensitive);
  myPickButton->setSensitivity(isSensitive);
}


// Callbacks :

void FuiPointEditor::setPickCB(const FFaDynCB0& aDynCB)
{
  myPickButton->setActivateCB(aDynCB);
}


void FuiPointEditor::setPointChangedCB(const FFaDynCB2<const FaVec3&,bool>& aDynCB)
{
  myPointUI->setPointChangedCB(aDynCB);
}


void FuiPointEditor::setRefChangedCB(const FFaDynCB1<bool>& aDynCB)
{
  myPointUI->setRefChangedCB(aDynCB);
}


void FuiPointEditor::setOnWhatText(const std::string& text)
{
  myIdField->setValue(text);
}


void FuiPointEditor::setValue(const FaVec3& point)
{
  myPointUI->setValue(point);
}


FaVec3 FuiPointEditor::getValue() const
{
  return myPointUI->getValue();
}


bool FuiPointEditor::isGlobal() const
{
  return myPointUI->isGlobal();
}


void FuiPointEditor::setGlobal(bool isGlobal)
{
  if (isGlobal)
    myPointUI->setGlobal();
  else
    myPointUI->setLocal();
}


void FuiPointEditor::onMyViewWhatTouched()
{
  static bool buttonIn = false;
  buttonIn = !buttonIn;
  myViewWhatCB.invoke(buttonIn);
}


void FuiPointEditor::onMyViewPointTouched()
{
  static bool buttonIn = false;
  buttonIn = !buttonIn;
  myViewPointCB.invoke(buttonIn);
}
