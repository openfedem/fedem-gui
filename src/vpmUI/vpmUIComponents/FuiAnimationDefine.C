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

  this->typeFrame->setLabel("Type");
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

void FuiAnimationDefine::placeWidgets(int width, int height)
{
  int border = 6;

  int sep = 3*height/100;
  int toggleh = height/10;
  int buttonh = 13*height/100;
  int frameFonth = this->typeFrame->getFontHeigth();

  int tabl = this->typeFrame->getWidthHint();

  if (this->timeRadio->getWidthHint() > tabl)
    tabl = this->timeRadio->getWidthHint();

  if (this->eigenRadio->getWidthHint() > tabl)
    tabl = this->eigenRadio->getWidthHint();

  if (this->loadFringeToggle->getWidthHint() > tabl)
    tabl = this->loadFringeToggle->getWidthHint();

  if (this->loadLineFringeToggle->getWidthHint() > tabl)
    tabl = this->loadLineFringeToggle->getWidthHint();

  if (this->loadDefToggle->getWidthHint() > tabl)
    tabl = this->loadDefToggle->getWidthHint();

  if (this->loadAnimButton->getWidthHint() > tabl)
    tabl = this->loadAnimButton->getWidthHint();

  tabl += 3*border/2;

  int timec      = border + frameFonth + toggleh/2;
  int eigenc     = timec  + sep + toggleh;
  int summaryc   = eigenc  + sep + toggleh;
  int typeframeb = summaryc + toggleh/2 + sep;

  int fringec     = typeframeb + sep + toggleh/2;
  int fringeLinec = fringec + sep + toggleh;
  int defc        = fringeLinec + sep + toggleh;
  int animc       = height - buttonh/2;

  this->typeFrame->setEdgeGeometry(0,tabl-border,0,typeframeb);
  this->timeRadio->setCenterYGeometryWidthHint(2*border,timec,toggleh);
  this->eigenRadio->setCenterYGeometryWidthHint(2*border,eigenc,toggleh);
  this->summaryRadio->setCenterYGeometryWidthHint(2*border,summaryc,toggleh);

  this->loadFringeToggle->setCenterYGeometryWidthHint(0,fringec,toggleh);
  this->loadLineFringeToggle->setCenterYGeometryWidthHint(0,fringeLinec,toggleh);
  this->loadDefToggle->setCenterYGeometryWidthHint(0,defc,toggleh);
  this->loadAnimButton->setCenterYGeometryWidthHint(0,animc-1,toggleh);

  this->tabStack->setEdgeGeometry(tabl,width, 0,height);
}
//-----------------------------------------------------------------------------

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

    // Need to touch the geometry
    this->tabStack->setWidth(this->tabStack->getWidth()+1);
    this->tabStack->setWidth(this->tabStack->getWidth()-1);

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

  this->averageOperLabel->setLabel("Operation");
  this->averageOnLabel->setLabel("on");
  this->shellAngleLabel->setLabel("Max shell average angle");
  this->elementToggle->setLabel("Average across element type");

  this->fringeFrame->setLabel("Contour Value");
  this->resultClassLabel->setLabel("Result class");
  this->resultLabel->setLabel("Result");
  this->resultOperLabel->setLabel("Operation");

  this->resultSetFrame->setLabel("Result Set");
  this->setByOperRadio->setLabel("By operation");
  this->setByNameRadio->setLabel("By name");

  this->averageFrame->setLabel("Averaging");
  this->multiFaceFrame->setLabel("Multiple Face Results");
  this->elemGrpOperLabel->setLabel("Operation/Element group");

  this->autoExportToggle->setLabel("Export animation automatically to VTF");
}
//----------------------------------------------------------------------------

void FuiFringeSheet::placeWidgets(int width, int height)
{
  int border = 6;

  int sep = height/100;
  int vsplit = width/2;

  int frameFonth = this->fringeFrame->getFontHeigth();
  int toggleh = (height - 2*frameFonth - 6*sep - 4 - border)/5;

  int fringeframet = 0;
  int resclassc = fringeframet+frameFonth+toggleh/2;
  int resc = resclassc+sep+toggleh;
  int resoperc = resc+sep+toggleh;
  int fringeframeb = resoperc+toggleh/2+sep+ 2;

  int setframet = fringeframeb+sep;
  int byoperc = setframet+frameFonth+toggleh/2;
  int bynamec = byoperc+sep+toggleh;
  int setframeb = height-border;//bynamec+toggleh/2+sep;

  int averframet = fringeframet;
  int averframeb = fringeframeb;

  int multiframet = setframet;//averframeb+sep;
  int elemoperc   = byoperc;
  int multiframeb = height-border-toggleh;

  // Compute menu widths

  int fringemenuw = 0;
  if (this->resultClassMenu->getWidthHint() > fringemenuw)
    fringemenuw = this->resultClassMenu->getWidthHint();
  if (this->resultMenu->getWidthHint() > fringemenuw)
    fringemenuw = this->resultMenu->getWidthHint();
  if (this->resultOperMenu->getWidthHint() > fringemenuw)
    fringemenuw = this->resultOperMenu->getWidthHint();

  int fringelabelw = 0;
  if (this->resultClassLabel->getWidthHint() > fringelabelw)
    fringelabelw = this->resultClassLabel->getWidthHint();
  if (this->resultLabel->getWidthHint() > fringelabelw)
    fringelabelw = this->resultLabel->getWidthHint();
  if (this->resultOperLabel->getWidthHint() > fringelabelw)
    fringelabelw = this->resultOperLabel->getWidthHint();

  int right = vsplit - border/2;
  int availfringemenuw = right - fringelabelw - 4*border;
  if (fringemenuw > availfringemenuw) fringemenuw = availfringemenuw;

  int setmenuw = 0;
  if (this->setOperMenu->getWidthHint() > setmenuw)
    setmenuw = this->setOperMenu->getWidthHint();
  if (this->setNameMenu->getWidthHint() > setmenuw)
    setmenuw = this->setNameMenu->getWidthHint();

  int setlabelw = 0;
  if (this->setByOperRadio->getWidthHint() > setlabelw)
    setlabelw = this->setByOperRadio->getWidthHint();
  if (this->setByNameRadio->getWidthHint() > setlabelw)
    setlabelw = this->setByNameRadio->getWidthHint();

  int availsetmenuw = right - setlabelw - 4*border;
  if (setmenuw > availsetmenuw) setmenuw = availsetmenuw;

  int multimenuw = 0;
  if (this->averageOperMenu->getWidthHint() > multimenuw)
    multimenuw = this->averageOperMenu->getWidthHint();
  if (this->elemGrpOperMenu->getWidthHint() > multimenuw)
    multimenuw = this->elemGrpOperMenu->getWidthHint();

  int multilabelw = this->elemGrpOperLabel->getWidthHint();
  int left = right + border;
  int availmultimenuw = width - 4*border - multilabelw - left;
  if (multimenuw > availmultimenuw) multimenuw = availmultimenuw;

  // Set widget geometries

  int menubegin = right - border - fringemenuw;
  this->fringeFrame->setEdgeGeometry(border,right,fringeframet,fringeframeb);

  this->resultClassLabel->setCenterYGeometryWidthHint(2*border,resclassc,toggleh);
  this->resultClassMenu->setCenterYGeometry(menubegin,resclassc,fringemenuw,toggleh);

  this->resultLabel->setCenterYGeometryWidthHint(2*border,resc,toggleh);
  this->resultMenu->setCenterYGeometry(menubegin,resc,fringemenuw,toggleh);

  this->resultOperLabel->setCenterYGeometryWidthHint(2*border,resoperc,toggleh);
  this->resultOperMenu->setCenterYGeometry(menubegin,resoperc,fringemenuw,toggleh);

  menubegin = right - border - setmenuw;
  this->resultSetFrame->setEdgeGeometry(border,right,setframet,setframeb);

  this->setByOperRadio->setCenterYGeometryWidthHint(2*border,byoperc,toggleh);
  this->setOperMenu->setCenterYGeometry(menubegin,byoperc,setmenuw,toggleh);

  this->setByNameRadio->setCenterYGeometryWidthHint(2*border,bynamec,toggleh);
  this->setNameMenu->setCenterYGeometry(menubegin,bynamec,setmenuw,toggleh);

  this->averageFrame->setEdgeGeometry(left,width-border,averframet,averframeb);
  this->multiFaceFrame->setEdgeGeometry(left,width-border,multiframet,multiframeb);
  left += border;

  int y = resclassc;
  this->averageOperLabel->setCenterYGeometry(left,y,this->averageOperLabel->getWidthHint(),toggleh);
  menubegin = left + this->averageOperLabel->getWidthHint() + border;
  int operMenuEnd = menubegin + multimenuw;
  this->averageOperMenu->setCenterYGeometry(menubegin,y,multimenuw,toggleh);
  menubegin = width - this->averageOnMenu->getWidthHint() - 2*border;
  this->averageOnMenu->setCenterYGeometry(menubegin,y,this->averageOnMenu->getWidthHint(),toggleh);
  menubegin -= this->averageOnLabel->getWidthHint() + border;
  if (menubegin > operMenuEnd) {
    this->averageOnLabel->popUp();
    this->averageOnLabel->setCenterYGeometry(menubegin,y,this->averageOnLabel->getWidthHint(),toggleh);
  }
  else
    this->averageOnLabel->popDown();
  y += toggleh + 2*sep;
  this->shellAngleLabel->setCenterYGeometry(left,y,this->shellAngleLabel->getWidthHint(),toggleh);
  this->shellAngleField->setCenterYGeometry(left+this->shellAngleLabel->getWidthHint()+border,y,
					    width-left-3*border-this->shellAngleLabel->getWidthHint(),toggleh);
  y += toggleh + 2*sep;
  this->elementToggle->setCenterYGeometry(left,y,this->elementToggle->getWidthHint(),toggleh);

  menubegin = width - 2*border - multimenuw;
  this->elemGrpOperLabel->setCenterYGeometryWidthHint(left,elemoperc,toggleh);
  this->elemGrpOperMenu->setCenterYGeometry(menubegin,elemoperc,multimenuw,toggleh);

  this->autoExportToggle->setCenterYGeometryWidthHint(left,height-toggleh/2,toggleh);
  if (this->showAutoExToggle)
    this->autoExportToggle->popUp();
  else
    this->autoExportToggle->popDown();
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

  this->autoExportToggle->setValue(animValues->autoExport > 0);
  this->showAutoExToggle = animValues->autoExport >= 0;

  this->placeWidgets(this->getWidth(),this->getHeight());
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

  if (this->showAutoExToggle)
    animValues->autoExport = this->autoExportToggle->getValue();
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

  this->timeFrame->setLabel("Time Window");
  this->completeSimRadio->setLabel("Complete simulation");
  this->timeIntRadio->setLabel("Time interval");
  this->startLabel->setLabel("Start");
  this->stopLabel->setLabel("Stop");
  this->framesFrame->setLabel("Animation Frames");
  this->mostFramesRadio->setLabel("For every time step");
  this->leastFramesRadio->setLabel("Only for requested results");
}
//----------------------------------------------------------------------------

void FuiTimeSheet::placeWidgets(int width,int height)
{
  int border = 6;

  int sep = 3*height/100;
  int vsplit = width/2;

  int toggleh = 13*height/100;
  int frameFonth = this->timeFrame->getFontHeigth();

  int timeframet = border;
  int completec = timeframet+frameFonth+toggleh/2;
  int timec = completec+sep+toggleh;
  int startc = timec+sep+toggleh;
  int stopc = startc+sep+toggleh;
  int timeframeb = height-border;//stopc+toggleh/2+sep;

  int framesframet = timeframet;
  int mostc = completec;
  int leastc = timec;
  int framesframeb = timeframeb;

  this->timeFrame->setEdgeGeometry(border,vsplit-border/2,timeframet,timeframeb);

  this->completeSimRadio->setCenterYGeometryWidthHint(2*border,completec,toggleh);
  this->timeIntRadio->setCenterYGeometryWidthHint(2*border,timec,toggleh);

  this->startLabel->setCenterYGeometryWidthHint(5*border,startc,toggleh);
  int fieldl = this->startLabel->getXPos()+this->startLabel->getWidth()+border;
  int fieldw = vsplit-3*border/2 - fieldl;
  this->startField->setCenterYGeometry(fieldl,startc,fieldw,toggleh);

  this->stopLabel->setCenterYGeometryWidthHint(5*border,stopc,toggleh);
  this->stopField->setCenterYGeometry(fieldl,stopc,fieldw,toggleh);

  this->framesFrame->setEdgeGeometry(vsplit+border/2,width-border,framesframet,framesframeb);

  this->mostFramesRadio->setCenterYGeometryWidthHint(vsplit+3*border/2,mostc,toggleh);
  this->leastFramesRadio->setCenterYGeometryWidthHint(vsplit+3*border/2,leastc,toggleh);
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
  this->eigenFrame->setLabel("Mode Selection");
  this->linkLabel->setLabel("Part");
  this->timeLabel->setLabel("Time");
  this->modeLabel->setLabel("Mode");
  this->scaleLabel->setLabel("Scale");

  this->animFrame->setLabel("Frame Generation");
  this->framesPrLabel->setLabel("Frames per cycle");

  this->lengthFrame->setLabel("Length");
  this->timeLengthRadio->setLabel("Time");
  this->nCyclRadio->setLabel("No. of cycles");
  this->dampedRadio->setLabel("Until % damped");
}
//----------------------------------------------------------------------------

void FuiModesSheet::placeWidgets(int width, int height)
{
  int border = 6;

  int sep = 3*height/100;
  int vsplit = 2*width/5;

  int toggleh = 13*height/100;
  int frameFonth = this->eigenFrame->getFontHeigth();

  int modeframet = border;
  int typec = modeframet + frameFonth + toggleh/2;
  int timec = typec + sep + toggleh;
  int modec = timec + sep + toggleh;
  int scalec = modec + sep + toggleh;
  int modeframeb = height - border;

  int animframet = modeframet;
  int prcycl = typec;
  int animframeb = modeframeb;

  // Set geometries

  // Mode Selection frame
  int fieldl = this->modeLabel->getWidth() + 5*border;
  int fieldw = vsplit - 3*border/2 - fieldl;

  this->eigenFrame->setEdgeGeometry(border,vsplit-border/2,modeframet,modeframeb);
  this->typeMenu->setCenterYGeometry(2*border,typec,vsplit-7*border/2,toggleh);

  // Dynamic UI depending on the selected modes type
  if (this->showLinkMenu) {
    // component modes or free-free reduced modes
    this->timeLabel->popDown();
    this->timeMenu->popDown();
    this->linkLabel->setCenterYGeometryWidthHint(2*border,timec,toggleh);
    this->linkMenu->setCenterYGeometry(fieldl,timec,fieldw,toggleh);
    this->linkLabel->popUp();
    this->linkMenu->popUp();
  }
  else {
    // system modes
    this->linkLabel->popDown();
    this->linkMenu->popDown();
    this->timeLabel->setCenterYGeometryWidthHint(2*border,timec,toggleh);
    this->timeMenu->setCenterYGeometry(fieldl,timec,fieldw,toggleh);
    this->timeLabel->popUp();
    this->timeMenu->popUp();
  }

  this->modeLabel->setCenterYGeometryWidthHint(2*border,modec,toggleh);
  this->modeMenu->setCenterYGeometry(fieldl,modec,fieldw,toggleh);

  this->scaleLabel->setCenterYGeometryWidthHint(2*border,scalec,toggleh);
  this->scaleField->setCenterYGeometry(fieldl,scalec,fieldw,toggleh);

  // Frame Generation frame
  this->animFrame->setEdgeGeometry(vsplit+border/2,width-border,animframet,animframeb);

  this->framesPrLabel->setCenterYGeometryWidthHint(vsplit+3*border/2,prcycl,toggleh);

  fieldl = this->framesPrLabel->getXPos() + this->framesPrLabel->getWidth() + border;
  fieldw = width - 3*border - fieldl;
  this->framesPrField->setCenterYGeometry(fieldl,prcycl,fieldw,toggleh);

  // Dynamic UI depending on the selected modes type
  if (this->showLinkMenu) {
    // component modes or free-free reduced modes
    this->lengthFrame->popDown();
    this->timeLengthRadio->popDown();
    this->timeLengthField->popDown();
    this->nCyclRadio->popDown();
    this->nCyclField->popDown();
    this->dampedRadio->popDown();
    this->dampedField->popDown();
  }
  else {
    // system modes
    int lengthframet = prcycl + sep + toggleh/2;
    int timelengthc = lengthframet + frameFonth + toggleh/2;
    int ncyclc = timelengthc + sep + toggleh;
    int dampedc = ncyclc + sep + toggleh;
    int lengthframeb = animframeb - border;

    this->lengthFrame->setEdgeGeometry(vsplit+3*border/2,width-2*border,lengthframet,lengthframeb);

    this->timeLengthRadio->setCenterYGeometryWidthHint(vsplit+5*border/2,timelengthc,toggleh);
    this->nCyclRadio->setCenterYGeometryWidthHint(vsplit+5*border/2,ncyclc,toggleh);
    this->dampedRadio->setCenterYGeometryWidthHint(vsplit+5*border/2,dampedc,toggleh);

    fieldl = this->dampedRadio->getXPos() + this->dampedRadio->getWidth() + border;
    fieldw = width - 3*border - fieldl;
    this->timeLengthField->setCenterYGeometry(fieldl,timelengthc,fieldw,toggleh);
    this->nCyclField->setCenterYGeometry(fieldl,ncyclc,fieldw,toggleh);
    this->dampedField->setCenterYGeometry(fieldl,dampedc,fieldw,toggleh);

    this->lengthFrame->popUp();
    this->timeLengthRadio->popUp();
    this->timeLengthField->popUp();
    this->nCyclRadio->popUp();
    this->nCyclField->popUp();
    this->dampedRadio->popUp();
    this->dampedField->popUp();
  }
}
//-----------------------------------------------------------------------------

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
  this->showLinkMenu = animValues->modeTyp > 0;

  this->timeLengthField->setValue(animValues->timeLengthVal);
  this->nCyclField->setValue(animValues->nCyclesVal);
  this->dampedField->setValue(animValues->untilDampedVal);

  this->placeWidgets(this->getWidth(),this->getHeight());
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
