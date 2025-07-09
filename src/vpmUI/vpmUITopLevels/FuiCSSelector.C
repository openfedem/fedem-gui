// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiCSSelector.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuPushButton.H"


Fmd_SOURCE_INIT(FcFUICSSELECTOR, FuiCSSelector, FFuTopLevelShell);


//----------------------------------------------------------------------------

FuiCSSelector::FuiCSSelector()
{
  Fmd_CONSTRUCTOR_INIT(FuiCSSelector);

  notesText = NULL;
  importButton = closeButton = NULL;
}
//----------------------------------------------------------------------------

void FuiCSSelector::initWidgets()
{
  importButton->setActivateCB(FFaDynCB0M(FFaDynCB0,&importCB,invoke));
  closeButton->setActivateCB(FFaDynCB0M(FFuComponentBase,this,popDown));

  notesText->setLabel(
    "Select one or more cross sections and one material from the above,\n"
    "then press the \"Import\" button to create beam cross section objects.");

  importButton->setLabel("Import");
  closeButton->setLabel("Close");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
