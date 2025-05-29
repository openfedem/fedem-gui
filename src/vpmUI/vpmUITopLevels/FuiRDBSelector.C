// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiRDBSelector.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuDialogButtons.H"


enum {
  DONE = FFuDialogButtons::LEFTBUTTON,
  APPLY = FFuDialogButtons::MIDBUTTON,
  CANCEL = FFuDialogButtons::RIGHTBUTTON
};


Fmd_SOURCE_INIT(FcFUIRDBSELECTOR, FuiRDBSelector, FFuTopLevelShell);


//----------------------------------------------------------------------------

FuiRDBSelector::FuiRDBSelector()
{
  Fmd_CONSTRUCTOR_INIT(FuiRDBSelector);

  this->resLabel = 0;
  this->posLabel = 0;
  this->notesImage = 0;
  this->notesLabel = 0;
  this->notesText = 0;
  this->dialogButtons = 0;
}
//----------------------------------------------------------------------------

void FuiRDBSelector::initWidgets()
{
  dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiRDBSelector,this,
                                               onDialogButtonClicked,int));

  this->resLabel->setLabel("Existing Results");
  this->posLabel->setLabel("Possible Results");
  this->notesImage->setPixMap(info_xpm);
  this->notesLabel->setLabel("<b>Notes</b>");
  this->notesText->setLabel(
    "You can \"drag and drop\" these fields to the results viewer\n"
    "(i.e., the Results tree on the left side of the main window).");

  this->setOkCancelDialog(true);

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiRDBSelector::setOkCancelDialog(bool yesOrNo)
{
  dialogButtons->setButtonLabel(DONE, yesOrNo ? "OK" : "");
  dialogButtons->setButtonLabel(APPLY, yesOrNo ? "Apply" : "");
  dialogButtons->setButtonLabel(CANCEL, yesOrNo ? "Cancel" : "Close");
}
//----------------------------------------------------------------------------

void FuiRDBSelector::setApplyable(bool able)
{
  dialogButtons->setButtonSensitivity(DONE,able);
  dialogButtons->setButtonSensitivity(APPLY,able);
}
//----------------------------------------------------------------------------

void FuiRDBSelector::placeWidgets(int width,int height)
{
  int border = this->getBorder();
  int labh = this->resLabel->getHeightHint();
  int texth = labh+border;

  //borders
  int glbl = border;
  int glbr = width > border ? width-border : 0;
  int glbt = border;

  //hor
  int glh4 = height-this->dialogButtons->getHeightHint();
  int glh1 = glbt+texth/2;
  int glh2 = glh1+(glh4-glh1-texth)*67/100;
  int glh3 = glh2+texth;

  this->resLabel->setCenterYGeometrySizeHint(glbl,glh1-labh/2-border/4);
  this->posLabel->setCenterYGeometrySizeHint(glbl,glh3-labh/2-border/4);

  this->lvRes->setEdgeGeometry(glbl,glbr,glh1,glh2);
  this->lvPos->setEdgeGeometry(glbl,glbr,glh3,glh4-55);

  this->notesImage->setEdgeGeometry(glbl,    glbl+16, glh4-50, glh4-34);
  this->notesLabel->setEdgeGeometry(glbl+20, glbr, glh4-50, glh4-34);
  this->notesText->setEdgeGeometry( glbl,    glbr, glh4-31, glh4+8);

  this->dialogButtons->setEdgeGeometry(0,width,glh4,height);

  this->notesText->toFront();
}
//----------------------------------------------------------------------------

bool FuiRDBSelector::onClose()
{
  this->invokeFinishedCB();
  return false;
}
//----------------------------------------------------------------------------

void FuiRDBSelector::onDialogButtonClicked(int button)
{
  if (button == DONE || button == APPLY)
    this->resultAppliedCB.invoke();

  if (button == DONE || button == CANCEL)
    this->invokeFinishedCB();
}
