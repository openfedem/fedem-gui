// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiAnimationDefine.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuTabbedWidgetStack.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"


//----------------------------------------------------------------------------

void FuiAnimationDefine::initWidgets()
{
  this->typeRadioGroup.insert(this->timeRadio,0);
  this->typeRadioGroup.insert(this->eigenRadio,1);
  this->typeRadioGroup.insert(this->summaryRadio,2);
  this->typeRadioGroup.setExclusive(true);
  this->typeRadioGroup.setGroupToggleCB(FFaDynCB2M(FuiAnimationDefine,this,onRadioGroupToggled,int,bool));

  this->loadFringeToggle->setToggleCB(FFaDynCB1M(FuiAnimationDefine,this,onButtonToggled,bool));
  this->loadLineFringeToggle->setToggleCB(FFaDynCB1M(FuiAnimationDefine,this,onButtonToggled,bool));
  this->loadDefToggle->setToggleCB(FFaDynCB1M(FuiAnimationDefine,this,onButtonToggled,bool));
  this->loadAnimButton->setActivateCB(FFaDynCB0M(FuiAnimationDefine,this,onLoadAnimButtonClicked));

  this->fringeSheet->setDataChangedCB(FFaDynCB0M(FuiAnimationDefine,this,onTabSheetChanged));
  this->timeSheet->setDataChangedCB(FFaDynCB0M(FuiAnimationDefine,this,onTabSheetChanged));
  this->modesSheet->setDataChangedCB(FFaDynCB0M(FuiAnimationDefine,this,onTabSheetChanged));

  this->tabStack->addTabPage(this->timeSheet,"Time");
  this->tabStack->addTabPage(this->fringeSheet,"Contours");
  this->tabStack->addTabPage(this->modesSheet,"Eigen Modes");

  this->timeRadio->setLabel("Time History");
  this->eigenRadio->setLabel("Eigen Mode");
  this->summaryRadio->setLabel("Time Summary");

  this->loadFringeToggle->setLabel("Load face contours");
  this->loadLineFringeToggle->setLabel("Load line contours");
  this->loadDefToggle->setLabel("Load deformations");
  this->loadAnimButton->setLabel("Load Animation");

  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

FFuaUIValues* FuiAnimationDefine::createValuesObject()
{
  return new FuaAnimationDefineValues();
}
//----------------------------------------------------------------------------

void FuiAnimationDefine::setUIValues(const FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  static bool isTimeHistAnimOld = false;
  static bool isContoursAnimOld = false;
  static bool isModesAnimOld    = false;

  bool isModesAnim    = animValues->isModesAnim;
  bool isSummaryAnim  = animValues->isSummaryAnim;
  bool isTimeHistAnim = !isModesAnim && !isSummaryAnim;
  bool isContoursAnim = animValues->loadFringe || animValues->loadLineFringe;

  this->timeRadio->setValue(isTimeHistAnim);
  this->eigenRadio->setValue(isModesAnim);
  this->summaryRadio->setValue(isSummaryAnim);

  if (isTimeHistAnim != isTimeHistAnimOld ||
      isContoursAnim != isContoursAnimOld ||
      isModesAnim    != isModesAnimOld) {

    // Rebuild the tabs since they are different from current
    std::string tmpSel = this->tabStack->getCurrentTabName();
    this->tabStack->popDown();
    this->tabStack->removeTabPage(this->timeSheet);
    this->tabStack->removeTabPage(this->fringeSheet);
    this->tabStack->removeTabPage(this->modesSheet);

    if (isTimeHistAnim)
      this->tabStack->addTabPage(this->timeSheet,"Time");

    if (isContoursAnim)
      this->tabStack->addTabPage(this->fringeSheet,"Contours");

    if (isModesAnim)
      this->tabStack->addTabPage(this->modesSheet,"Eigen Modes");

    this->tabStack->popUp();
    this->tabStack->setCurrentTab(tmpSel);

    isTimeHistAnimOld = isTimeHistAnim;
    isContoursAnimOld = isContoursAnim;
    isModesAnimOld    = isModesAnim;
  }

  this->loadFringeToggle->setValue(animValues->loadFringe);
  this->loadFringeToggle->setSensitivity(isTimeHistAnim);

  this->loadLineFringeToggle->setValue(animValues->loadLineFringe);
  this->loadLineFringeToggle->setSensitivity((isTimeHistAnim || isSummaryAnim) &&
					     animValues->selResultClass != "Element");

  this->loadDefToggle->setValue(animValues->loadDef);
  this->loadDefToggle->setSensitivity(isTimeHistAnim ||
				      (isModesAnim && animValues->modeTyp == 0));

  this->fringeSheet->setUIValues(values);
  this->timeSheet->setUIValues(values);
  this->modesSheet->setUIValues(values);
}
//-----------------------------------------------------------------------------

void FuiAnimationDefine::getUIValues(FFuaUIValues* values)
{
  this->fringeSheet->getUIValues(values);
  this->timeSheet->getUIValues(values);
  this->modesSheet->getUIValues(values);

  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  animValues->isModesAnim = this->eigenRadio->getValue();
  animValues->isSummaryAnim = this->summaryRadio->getValue();

  animValues->loadFringe = this->loadFringeToggle->getValue();
  animValues->loadLineFringe = this->loadLineFringeToggle->getValue();
  animValues->loadDef = this->loadDefToggle->getValue();
}

/////////////////////////////////////////////////////////////////////////////

void FuiFringeSheet::initWidgets()
{
  this->resultClassMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));
  this->resultMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));
  this->resultOperMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));
  this->setOperMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));
  this->setNameMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));

  this->resultSetRadioGroup.insert(this->setByOperRadio,0);
  this->resultSetRadioGroup.insert(this->setByNameRadio,1);
  this->resultSetRadioGroup.setExclusive(true);
  this->resultSetRadioGroup.setGroupToggleCB(FFaDynCB2M(FuiFringeSheet,this,onRadioGroupToggled,int,bool));

  this->averageOperMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));
  this->averageOnMenu->setOptionSelectedCB(FFaDynCB1M(FuiFringeSheet,this,onMenuSelected,int));
  this->shellAngleField->setAcceptedCB(FFaDynCB1M(FuiFringeSheet,this,onFieldValueChanged,char*));
  this->elementToggle->setToggleCB(FFaDynCB1M(FuiFringeSheet,this,onButtonToggled,bool));
  this->autoExportToggle->setToggleCB(FFaDynCB1M(FuiFringeSheet,this,onButtonToggled,bool));

  this->shellAngleField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->shellAngleField->setDoubleDisplayMode(FFuIOField::DECIMAL);
  this->shellAngleField->setDoubleDisplayPrecision(2);
  this->shellAngleField->setZeroDisplayPrecision(1);

  this->elementToggle->setLabel("Average across element type");

  this->setByOperRadio->setLabel("By operation");
  this->setByNameRadio->setLabel("By name");

  this->elemGrpOperLabel->setLabel("Operation/Element group");

  this->autoExportToggle->setLabel("Export animation automatically to VTF");
}
//-----------------------------------------------------------------------------

void FuiFringeSheet::setUIValues(const FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  this->resultClassMenu->setOptions(animValues->resultClasses);
  this->resultClassMenu->selectOption(animValues->selResultClass,false);
  this->resultMenu->setOptions(animValues->results);
  this->resultMenu->selectOption(animValues->selResult,false);
  this->resultOperMenu->setOptions(animValues->resultOpers);
  this->resultOperMenu->selectOption(animValues->selResultOper,false);

  this->setByNameRadio->setValue(animValues->resultSetByName);
  this->setOperMenu->setSensitivity(!animValues->resultSetByName);
  this->setNameMenu->setSensitivity(animValues->resultSetByName);

  this->setOperMenu->setOptions(animValues->resultSetOpers);
  this->setOperMenu->selectOption(animValues->selResultSetOper,false);
  this->setNameMenu->setOptions(animValues->resultSets);
  this->setNameMenu->selectOption(animValues->selResultSet,false);

  if (!animValues->resultClasses.empty() && !animValues->averOpers.empty()) {
    bool canAverage = animValues->selResultClass == "Element node";
    bool doAverage = canAverage && animValues->selAverOper != animValues->averOpers.front();
    this->averageOnMenu->setOptions(animValues->averOns);
    this->averageOnMenu->selectOption(animValues->selAverOn,false);
    this->averageOnMenu->setSensitivity(doAverage);
    this->averageOperMenu->setOptions(animValues->averOpers);
    this->averageOperMenu->selectOption(animValues->selAverOper,false);
    this->averageOperMenu->setSensitivity(canAverage);
    this->shellAngleField->setValue(animValues->shellAngle);
    this->shellAngleField->setSensitivity(doAverage);
    this->elementToggle->setValue(animValues->acrossElemType);
    this->elementToggle->setSensitivity(doAverage);
  }
  if (animValues->elemGrps.empty()) {
    this->elemGrpOperLabel->setLabel("Operation");
    this->elemGrpOperMenu->setOptions(animValues->elemGrpOpers);
  }
  else {
    this->elemGrpOperLabel->setLabel("Operation/Element group");
    std::vector<std::string> opers(animValues->elemGrpOpers);
    opers.insert(opers.end(),animValues->elemGrps.begin(),animValues->elemGrps.end());
    this->elemGrpOperMenu->setOptions(opers);
  }
  this->elemGrpOperMenu->selectOption(animValues->selElemGrpOper,false);

  autoExportToggle->setValue(animValues->autoExport > 0);
  if (animValues->autoExport >= 0)
    autoExportToggle->popUp();
  else
    autoExportToggle->popDown();
}
//----------------------------------------------------------------------------

void FuiFringeSheet::getUIValues(FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  animValues->selResultClass = this->resultClassMenu->getSelectedOptionStr();
  animValues->selResult = this->resultMenu->getSelectedOptionStr();
  animValues->selResultOper = this->resultOperMenu->getSelectedOptionStr();

  animValues->resultSetByName = this->setByNameRadio->getValue();
  animValues->selResultSetOper = this->setOperMenu->getSelectedOptionStr();
  animValues->selResultSet = this->setNameMenu->getSelectedOptionStr();

  animValues->selAverOn = this->averageOnMenu->getSelectedOptionStr();
  animValues->selAverOper = this->averageOperMenu->getSelectedOptionStr();
  animValues->shellAngle = this->shellAngleField->getDouble();
  animValues->acrossElemType = this->elementToggle->getValue();

  animValues->selElemGrpOper = this->elemGrpOperMenu->getSelectedOptionStr();

  if (autoExportToggle->isPoppedUp())
    animValues->autoExport = autoExportToggle->getValue();
  else
    animValues->autoExport = -1;
}
//////////////////////////////////////////////////////////////////////////////

void FuiTimeSheet::initWidgets()
{
  this->timeRadioGroup.insert(this->completeSimRadio,0);
  this->timeRadioGroup.insert(this->timeIntRadio,1);
  this->timeRadioGroup.setExclusive(true);
  this->timeRadioGroup.setGroupToggleCB(FFaDynCB2M(FuiTimeSheet,this,onRadioGroupToggled,int,bool));

  this->framesRadioGroup.insert(this->mostFramesRadio,0);
  this->framesRadioGroup.insert(this->leastFramesRadio,1);
  this->framesRadioGroup.setExclusive(true);
  this->framesRadioGroup.setGroupToggleCB(FFaDynCB2M(FuiTimeSheet,this,onRadioGroupToggled,int,bool));

  this->startField->setAcceptedCB(FFaDynCB1M(FuiTimeSheet,this,onFieldValueChanged,char*));
  this->startField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->startField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->startField->setDoubleDisplayPrecision(6);
  this->startField->setZeroDisplayPrecision(1);

  this->stopField->setAcceptedCB(FFaDynCB1M(FuiTimeSheet,this,onFieldValueChanged,char*));
  this->stopField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->stopField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->stopField->setDoubleDisplayPrecision(6);
  this->stopField->setZeroDisplayPrecision(1);

  this->completeSimRadio->setLabel("Complete simulation");
  this->timeIntRadio->setLabel("Time interval");
  this->mostFramesRadio->setLabel("For every time step");
  this->leastFramesRadio->setLabel("Only for requested results");
}
//-----------------------------------------------------------------------------

void FuiTimeSheet::setUIValues(const FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  this->completeSimRadio->setValue(animValues->complSimul);

  this->startField->setSensitivity(!animValues->complSimul);
  this->stopField->setSensitivity(!animValues->complSimul);

  this->startField->setValue(animValues->start);
  this->stopField->setValue(animValues->stop);

  this->mostFramesRadio->setValue(animValues->mostFrames);
}
//----------------------------------------------------------------------------

void FuiTimeSheet::getUIValues(FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  animValues->complSimul = this->completeSimRadio->getValue();

  animValues->start = this->startField->getDouble();
  animValues->stop = this->stopField->getDouble();

  animValues->mostFrames = this->mostFramesRadio->getValue();
}
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////

void FuiModesSheet::initWidgets()
{
  this->typeMenu->setOptionSelectedCB(FFaDynCB1M(FuiModesSheet,this,onMenuSelected,int));
  this->linkMenu->setRefSelectedCB   (FFaDynCB1M(FuiModesSheet,this,onMenuSelected,int));
  this->timeMenu->setOptionSelectedCB(FFaDynCB1M(FuiModesSheet,this,onMenuSelected,int));
  this->modeMenu->setOptionSelectedCB(FFaDynCB1M(FuiModesSheet,this,onMenuSelected,int));

  this->scaleField->setAcceptedCB     (FFaDynCB1M(FuiModesSheet,this,onFieldValueChanged,char*));
  this->framesPrField->setAcceptedCB  (FFaDynCB1M(FuiModesSheet,this,onFieldValueChanged,char*));
  this->timeLengthField->setAcceptedCB(FFaDynCB1M(FuiModesSheet,this,onFieldValueChanged,char*));
  this->nCyclField->setAcceptedCB     (FFaDynCB1M(FuiModesSheet,this,onFieldValueChanged,char*));
  this->dampedField->setAcceptedCB    (FFaDynCB1M(FuiModesSheet,this,onFieldValueChanged,char*));

  this->lengthRadioGroup.insert(this->timeLengthRadio,0);
  this->lengthRadioGroup.insert(this->nCyclRadio,1);
  this->lengthRadioGroup.insert(this->dampedRadio,2);
  this->lengthRadioGroup.setExclusive(true);
  this->lengthRadioGroup.setGroupToggleCB(FFaDynCB2M(FuiModesSheet,this,onRadioGroupToggled,int,bool));

  // Static menu contents
  this->typeMenu->addOption("System modes");
  this->typeMenu->addOption("Component modes of part");
  this->typeMenu->addOption("Free-free modes of reduced part");
  this->typeMenu->addOption("Eigenmodes of non-reduced part");

  this->linkMenu->setBehaviour(FuiQueryInputField::REF_NONE,true);
  this->linkMenu->setTextForNoRefSelected("(All parts)");
  this->linkMenu->setMaxHeight(25);

  this->scaleField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->scaleField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->scaleField->setDoubleDisplayPrecision(6);
  this->scaleField->setZeroDisplayPrecision(1);

  this->timeLengthField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->timeLengthField->setDoubleDisplayMode(FFuIOField::AUTO);
  this->timeLengthField->setDoubleDisplayPrecision(6);
  this->timeLengthField->setZeroDisplayPrecision(1);

  this->framesPrField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->nCyclField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->dampedField->setInputCheckMode(FFuIOField::INTEGERCHECK);

  // Labels
  this->linkLabel->setLabel("Part");
  this->timeLabel->setLabel("Time");

  this->lengthFrame->setLabel("Length");
  this->timeLengthRadio->setLabel("Time");
  this->nCyclRadio->setLabel("No. of cycles");
  this->dampedRadio->setLabel("Until % damped");
}
//----------------------------------------------------------------------------

void FuiModesSheet::setUIValues(const FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  this->typeMenu->selectOption(animValues->modeTyp);

  this->linkMenu->setQuery(animValues->linkQuery);
  this->linkMenu->setSelectedRef(animValues->selLink);

  this->timeMenu->setOptions(animValues->times);
  this->timeMenu->selectDoubleOption(animValues->selTime);

  this->modeMenu->setOptions(animValues->modes);
  this->modeMenu->selectIntOption(animValues->selMode);

  this->scaleField->setValue(animValues->modeScale);

  this->framesPrField->setValue(animValues->framesPrCycle);

  this->timeLengthRadio->setValue(animValues->timeLength);
  this->nCyclRadio->setValue(animValues->nCycles);
  this->dampedRadio->setValue(animValues->untilDamped);

  this->timeLengthField->setSensitivity(animValues->timeLength);
  this->nCyclField->setSensitivity(animValues->nCycles);
  this->dampedField->setSensitivity(animValues->untilDamped);

  this->timeLengthField->setValue(animValues->timeLengthVal);
  this->nCyclField->setValue(animValues->nCyclesVal);
  this->dampedField->setValue(animValues->untilDampedVal);

  // Dynamic UI depending on the selected modes type
  if (animValues->modeTyp > 0) {
    // component modes or free-free reduced modes
    timeLabel->popDown();
    timeMenu->popDown();
    linkLabel->popUp();
    linkMenu->popUp();
    lengthFrame->popDown();
  }
  else {
    // system modes
    linkLabel->popDown();
    linkMenu->popDown();
    timeLabel->popUp();
    timeMenu->popUp();
    lengthFrame->popUp();
  }
}
//----------------------------------------------------------------------------

void FuiModesSheet::getUIValues(FFuaUIValues* values)
{
  FuaAnimationDefineValues* animValues = (FuaAnimationDefineValues*) values;

  animValues->modeTyp = this->typeMenu->getSelectedOption();
  animValues->selLink = this->linkMenu->getSelectedRef();
  animValues->selTime = this->timeMenu->getSelectedDouble();
  animValues->selMode = this->modeMenu->getSelectedInt();

  animValues->modeScale = this->scaleField->getDouble();

  animValues->framesPrCycle = this->framesPrField->getInt();

  animValues->timeLength = this->timeLengthRadio->getValue();
  animValues->nCycles = this->nCyclRadio->getValue();
  animValues->untilDamped = this->dampedRadio->getValue();

  animValues->timeLengthVal = this->timeLengthField->getDouble();
  animValues->nCyclesVal = this->nCyclField->getInt();
  animValues->untilDampedVal = this->dampedField->getInt();
}
