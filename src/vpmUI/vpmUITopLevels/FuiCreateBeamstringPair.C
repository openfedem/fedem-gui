// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiCreateBeamstringPair.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Fui.H"
#include "vpmUI/Pixmaps/beamstringPair.xpm"

#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuToggleButton.H"


Fmd_SOURCE_INIT(FUI_CREATEBEAMSTRINGPAIR,FuiCreateBeamstringPair,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiCreateBeamstringPair::FuiCreateBeamstringPair()
{
  Fmd_CONSTRUCTOR_INIT(FuiCreateBeamstringPair);
}
//----------------------------------------------------------------------------

void FuiCreateBeamstringPair::setSensitivity(bool isSensitive)
{
  this->createButton->setSensitivity(isSensitive);
  this->eraseButton->setSensitivity(isSensitive);
}
//----------------------------------------------------------------------------

void FuiCreateBeamstringPair::initWidgets()
{
  this->headerImage->setPixMap(beamstringPair_xpm);

  this->beamstringMenu1->setBehaviour(FuiQueryInputField::REF_NONE);
  this->beamstringMenu1->setButtonMeaning(FuiQueryInputField::EDIT);
  this->beamstringMenu1->turnButtonOff(true);

  this->beamstringMenu2->setBehaviour(FuiQueryInputField::REF_NONE);
  this->beamstringMenu2->setButtonMeaning(FuiQueryInputField::EDIT);
  this->beamstringMenu2->turnButtonOff(true);

  this->stiffnessFunctionMenu->setBehaviour(FuiQueryInputField::REF_NONE);
  this->stiffnessFunctionMenu->setButtonMeaning(FuiQueryInputField::EDIT);
  this->stiffnessFunctionMenu->turnButtonOff(true);

  this->useRadialSpringsToggle->setLabel("Use radial springs");

  this->notesText->setLabel(
    "The Beamstring Pair Definition tool is used to generate Free Joints between the triads of a beamstring pair.\n"
    "The primary purpose of this approach is to simulate that the inner pipe hits the outer pipe, and then the\n"
    "inner pipe will bounce back. The Contact stiffness function provides the magnitude of the contact spring\n"
    "that is applied on all joints. The Use radial springs check box indicates whether radial springs are to be used.");

  this->createButton->setLabel("Generate Free Joints");
  this->eraseButton->setLabel("Erase Free Joints");
  this->closeButton->setLabel("Close");
  this->helpButton->setLabel("Help");
  this->createButton->setActivateCB(FFaDynCB0M(FuiCreateBeamstringPair,this,onCreateButtonClicked));
  this->eraseButton->setActivateCB(FFaDynCB0M(FuiCreateBeamstringPair,this,onEraseButtonClicked));
  this->closeButton->setActivateCB(FFaDynCB0M(FFuComponentBase,this,popDown));
  this->helpButton->setActivateCB(FFaDynCB0M(FuiCreateBeamstringPair,this,onHelpButtonClicked));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//-----------------------------------------------------------------------------

void FuiCreateBeamstringPair::onPoppedUp()
{
  this->updateUIValues();
}
//----------------------------------------------------------------------------

bool FuiCreateBeamstringPair::onClose()
{
  this->popDown();
  return false;
}
//----------------------------------------------------------------------------

void FuiCreateBeamstringPair::onCreateButtonClicked()
{
  this->myCBs.createCB.invoke(beamstringMenu1->getSelectedRef(),
                              beamstringMenu2->getSelectedRef(),
                              stiffnessFunctionMenu->getSelectedRef(),
                              useRadialSpringsToggle->getValue());
}
//----------------------------------------------------------------------------

void FuiCreateBeamstringPair::onEraseButtonClicked()
{
  this->myCBs.deleteCB.invoke(beamstringMenu1->getSelectedRef(),
                              beamstringMenu2->getSelectedRef());
}
//----------------------------------------------------------------------------

void FuiCreateBeamstringPair::onHelpButtonClicked()
{
  Fui::showCHM("dialogbox/marine/beamstring-pair-definition.htm");
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiCreateBeamstringPair::createValuesObject()
{
  return new FuaCreateBeamstringPairValues();
}
//----------------------------------------------------------------------------

void FuiCreateBeamstringPair::setUIValues(const FFuaUIValues* values)
{
  FuaCreateBeamstringPairValues* cbpValues = (FuaCreateBeamstringPairValues*) values;

  this->beamstringMenu1->setQuery(cbpValues->beamstringQuery1);
  this->beamstringMenu1->setSelectedRef(NULL);
  this->beamstringMenu2->setQuery(cbpValues->beamstringQuery2);
  this->beamstringMenu2->setSelectedRef(NULL);
  this->stiffnessFunctionMenu->setQuery(cbpValues->stiffnessFunctionQuery);
  this->stiffnessFunctionMenu->setSelectedRef(NULL);
  this->setSensitivity(cbpValues->isSensitive);
}
