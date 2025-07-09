// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiLinkTabs.H"
#include "vpmUI/vpmUIComponents/FuiPositionData.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmPart.H"

#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuFrame.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuScale.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFaLib/FFaString/FFaStringExt.H"


////////////////////////////////////////////////////////////////////////////////

void FuiLinkModelSheet::initWidgets()
{
  this->feModelBtn->setLabel("FE Part");
  this->genPartBtn->setLabel("Generic Part");

  this->suppressInSolverToggle->setLabel("Visualization only");
  this->suppressInSolverLabel->setLabel("Part is ignored in solvers. Triads will be attached to ground");
  this->suppressInSolverToggle->setToggleCB(FFaDynCB1M(FuiLinkModelSheet,this,onSuppressInSolverToggeled,bool));

  this->linkTypeBtnGroup.insert(feModelBtn);
  this->linkTypeBtnGroup.insert(genPartBtn);
  this->linkTypeBtnGroup.setExclusive(true);
  this->linkTypeBtnGroup.setValue(feModelBtn,true);
  this->linkTypeBtnGroup.setGroupToggleCB(FFaDynCB2M(FuiLinkModelSheet,this,onLinkTypeToggeled,int,bool));

  this->feModelFrame->setLabel("Finite Element Model");
  this->repositoryFileField->setLabel("Repository entry");
  this->repositoryFileField->setSensitivity(false);
  this->importedFileField->setLabel("Imported file");
  this->importedFileField->setSensitivity(false);

  this->vizFrame->setLabel("Visualization");
  this->vizField->setLabel("File:");
  this->vizField->myField->setAcceptedCB(FFaDynCB1M(FuiLinkModelSheet,this,onVizFileTyped,const std::string&));
  this->vizChangeBtn->setLabel("Change...");
  this->vizChangeBtn->setActivateCB(FFaDynCB0M(FuiLinkModelSheet,this,onChangeViz));

  this->changeLinkBtn->setLabel("Change...");
  this->changeLinkBtn->setActivateCB(FFaDynCB0M(FuiLinkModelSheet,this,onChangeLink));
  this->unitConversionLabel->setLabel("No unit conversion");

  this->needsReductionFrame->setLook(FFuFrame::PANEL_SUNKEN);
  this->needsReductionLabel->setLabel("Needs\nReduction");

  this->structDampFrame->setLabel("Structural Damping");
  this->massProportionalField->setLabel("Mass proportional");
  this->massProportionalField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->massProportionalField->setAcceptedCB(FFaDynCB1M(FuiLinkModelSheet,this,onDoubleChanged,double));
  this->stiffProportionalField->setLabel("Stiffness proportional");
  this->stiffProportionalField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->stiffProportionalField->setAcceptedCB(FFaDynCB1M(FuiLinkModelSheet,this,onDoubleChanged,double));

  this->dynPropFrame->setLabel("Scaling of Dynamic Properties");
  this->stiffScaleField->setLabel("Stiffness");
  this->stiffScaleField->setToolTip("Scaling of stiffness");
  this->stiffScaleField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->stiffScaleField->setAcceptedCB(FFaDynCB1M(FuiLinkModelSheet,this,onDoubleChanged,double));
  this->massScaleField->setLabel("Mass");
  this->massScaleField->setToolTip("Scaling of mass");
  this->massScaleField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->massScaleField->setAcceptedCB(FFaDynCB1M(FuiLinkModelSheet,this,onDoubleChanged,double));
}


void FuiLinkModelSheet::placeWidgets(int width, int height)
{
  int border = 6;
  int fieldHeight = 19;
  int fieldSpace = 6;
  int left = border;
  int right = width - border;

  // Vertical

  while (5*fieldHeight + 5*fieldSpace > height - 7*border)
    if (5*fieldHeight >= height - 7*border) {
      fieldHeight = (height - 7*border)/5;
      fieldSpace = 0;
      break;
    }
    else
      fieldSpace--;

  int topFrameTop = border;
  int line1 = topFrameTop + border + fieldSpace + fieldHeight;
  int line2 = line1 + fieldSpace + fieldHeight;
  int topFrameBtm = line2 + border + (fieldSpace + fieldHeight)/2;

  int btmFrameTop = topFrameBtm + border;
  int line3 = btmFrameTop + border + fieldSpace + fieldHeight;
  int line4 = line3 + fieldSpace + fieldHeight;
  int btmFrameBtm = line4 + border + (fieldSpace + fieldHeight)/2;

  int btnLeft = left;
  int btnRight = btnLeft + this->suppressInSolverToggle->getWidthHint();

  int needsReductionW = 60;
  int needsReductionRight = right;
  int needsReductionLeft = needsReductionRight - needsReductionW;

  int fileFrameLeft = btnRight + border;
  int fileFrameRight = needsReductionLeft - border;

  int leftFrameLeft = left;
  int leftFrameRight = width/2 - border/2;
  int rightFrameLeft = leftFrameRight + border;
  int rightFrameRight = right;

  int changeBtnRight = fileFrameRight - border;
  int changeBtnLeft = changeBtnRight - this->changeLinkBtn->getWidthHint();
  int repFieldWidth = changeBtnLeft - fileFrameLeft - 2*border;

  int unitConvLeft = changeBtnRight - this->unitConversionLabel->getWidthHint();
  int impFieldWidth = unitConvLeft - fileFrameLeft - 2*border;

  int buttonCenterY1 = topFrameTop + fieldHeight/2;
  int buttonCenterY2 = buttonCenterY1 + fieldHeight;
  int buttonCenterY3 = buttonCenterY2 + fieldSpace+fieldHeight;

  // radios
  this->feModelBtn->setCenterYGeometryWidthHint(left, buttonCenterY1, fieldHeight);
  this->genPartBtn->setCenterYGeometryWidthHint(left, buttonCenterY2, fieldHeight);

  // Suppress related things
  this->suppressInSolverToggle->setCenterYGeometryWidthHint(left, buttonCenterY3, fieldHeight);
  this->suppressInSolverLabel->setCenterYGeometrySizeHint(leftFrameLeft + border, line3);
  this->suppressInSolverToggle->toFront();

  // frames
  this->feModelFrame->setEdgeGeometry(fileFrameLeft, fileFrameRight, topFrameTop, topFrameBtm);
  this->vizFrame->setEdgeGeometry(fileFrameLeft, fileFrameRight, topFrameTop, topFrameBtm);
  this->structDampFrame->setEdgeGeometry(leftFrameLeft, leftFrameRight, btmFrameTop, btmFrameBtm);
  this->dynPropFrame->setEdgeGeometry(rightFrameLeft, rightFrameRight, btmFrameTop, btmFrameBtm);

  // Inside visualization frame
  this->vizField->setCenterYGeometry(fileFrameLeft + border, line1,
				     repFieldWidth, fieldHeight);
  this->vizChangeBtn->setCenterYGeometryWidthHint(changeBtnLeft, line1, fieldHeight);
  this->vizLabel->setCenterYGeometry(fileFrameLeft + border, line2,
				     repFieldWidth, fieldHeight);

  // Inside finite element model frame
  int labelWidth = this->repositoryFileField->myLabel->getWidthHint();
  this->importedFileField->setLabelWidth(labelWidth);

  this->repositoryFileField->setCenterYGeometry(fileFrameLeft + border, line1,
						repFieldWidth, fieldHeight);
  this->importedFileField->setCenterYGeometry(fileFrameLeft + border, line2,
					      impFieldWidth, fieldHeight);
  this->changeLinkBtn->setCenterYGeometryWidthHint(changeBtnLeft, line1, fieldHeight);
  this->unitConversionLabel->setCenterYGeometrySizeHint(unitConvLeft, line2);

  this->needsReductionFrame->setEdgeGeometry(needsReductionLeft, needsReductionRight,
					     topFrameTop + border, topFrameBtm);
  this->needsReductionLabel->setEdgeGeometry(needsReductionLeft + 2, needsReductionRight - 2,
					     topFrameTop + border + 2, topFrameBtm - 2);

  // Inside structural damping frame
  labelWidth = this->stiffProportionalField->myLabel->getWidthHint();
  this->massProportionalField->setLabelWidth(labelWidth);

  this->massProportionalField->setCenterYGeometry(leftFrameLeft + border, line3,
						  leftFrameRight - leftFrameLeft - 2*border, fieldHeight);
  this->stiffProportionalField->setCenterYGeometry(leftFrameLeft + border, line4,
						   leftFrameRight - leftFrameLeft - 2*border, fieldHeight);

  // Inside dynamic properties frame
  labelWidth = this->stiffScaleField->myLabel->getWidthHint();
  this->massScaleField->setLabelWidth(labelWidth);

  this->stiffScaleField->setCenterYGeometry(rightFrameLeft + border, line3,
					    rightFrameRight - rightFrameLeft - 2*border, fieldHeight);
  this->massScaleField->setCenterYGeometry(rightFrameLeft + border, line4,
					   rightFrameRight - rightFrameLeft - 2*border, fieldHeight);
}


void FuiLinkModelSheet::setValues(const FuiLinkValues& values)
{
  this->ICanChange = values.allowChange;
  this->changeLinkBtn->setSensitivity(this->IAmSensitive && this->ICanChange);
  if (!this->ICanChange)
    this->changeLinkBtn->setToolTip("The existing FE model cannot be change in this edition.\n"
				    "This feature is available in the commercial edition only.\n");
  else if (values.importedFile.empty())
    this->changeLinkBtn->setToolTip("Import an FE model for this part");
  else
    this->changeLinkBtn->setToolTip("Re-import the FE model");

  this->linkTypeBtnGroup.setValue(genPartBtn,values.useGenericPart);
  this->suppressInSolverToggle->setValue(values.suppressInSolver);

  this->repositoryFileField->setValue(values.repositoryFile);
  this->importedFileField->setValue(values.importedFile);
  this->unitConversionLabel->setLabel(values.unitConversion);

  if (values.reducedVersionNumber > 0)
    this->needsReductionLabel->setLabel(FFaNumStr("Reduced\n[%d]",values.reducedVersionNumber));
  else
    this->needsReductionLabel->setLabel("Needs\nreduction");

  this->ICanChangeViz = values.allowChangeViz;
  this->vizField->setValue(values.vizFile);
  this->vizField->setSensitivity(this->IAmSensitive && this->ICanChangeViz);
  this->vizChangeBtn->setSensitivity(this->IAmSensitive && this->ICanChangeViz);

  if (values.usingFEModelViz)
    this->vizLabel->setLabel("Using FE model as visualization");
  else
    this->vizLabel->setLabel("");

  this->massProportionalField->setValue(values.massDamping);
  this->stiffProportionalField->setValue(values.stiffDamping);
  this->stiffScaleField->setValue(values.stiffScale);
  this->massScaleField->setValue(values.massScale);

  this->update();
}


void FuiLinkModelSheet::getValues(FuiLinkValues& values)
{
  values.suppressInSolver = this->suppressInSolverToggle->getToggle();
  values.useGenericPart = this->genPartBtn->getToggle();
  values.massDamping = this->massProportionalField->getValue();
  values.stiffDamping = this->stiffProportionalField->getValue();
  values.stiffScale = this->stiffScaleField->getValue();
  values.massScale = this->massScaleField->getValue();
}


void FuiLinkModelSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiLinkModelSheet::onSuppressInSolverToggeled(bool)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkModelSheet::onLinkTypeToggeled(int, bool)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkModelSheet::onDoubleChanged(double)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkModelSheet::onIntChanged(int)
{
  this->valuesChangedCB.invoke();
}


void FuiLinkModelSheet::setChangeLinkCB(const FFaDynCB0& aDynCB)
{
  this->changeLinkCB = aDynCB;
}


void FuiLinkModelSheet::onChangeLink()
{
  this->changeLinkCB.invoke();
}


void FuiLinkModelSheet::setUpdateLinkCB(const FFaDynCB0& aDynCB)
{
  this->updateLinkCB = aDynCB;
}


void FuiLinkModelSheet::setChangeGPVizCB(const FFaDynCB2<const std::string&,bool>& aDynCB)
{
  this->changeGPViz = aDynCB;
}


void FuiLinkModelSheet::onChangeViz()
{
  this->changeGPViz.invoke("", true);
}

void FuiLinkModelSheet::onVizFileTyped(const std::string& file)
{
  this->changeGPViz.invoke(file, false);
}


void FuiLinkModelSheet::setSensitivity(bool s)
{
  this->IAmSensitive = s;

  this->linkTypeBtnGroup.setSensitivity(s);
  this->suppressInSolverToggle->setSensitivity(s);
  this->changeLinkBtn->setSensitivity(s && this->ICanChange);
  this->vizField->setSensitivity(s && this->ICanChangeViz);
  this->vizChangeBtn->setSensitivity(s && this->ICanChangeViz);
  this->massProportionalField->setSensitivity(s);
  this->stiffProportionalField->setSensitivity(s);
  this->stiffScaleField->setSensitivity(s);
  this->massScaleField->setSensitivity(s);
}


void FuiLinkModelSheet::update()
{
  if (this->genPartBtn->getToggle()) {
    feModelFrame->popDown();
    repositoryFileField->popDown();
    importedFileField->popDown();
    unitConversionLabel->popDown();
    needsReductionLabel->popDown();
    needsReductionFrame->popDown();
    changeLinkBtn->popDown();

    vizFrame->popUp();
    vizField->popUp();
    vizChangeBtn->popUp();
    vizLabel->popUp();
  }
  else {
    feModelFrame->popUp();
    repositoryFileField->popUp();
    importedFileField->popUp();
    unitConversionLabel->popUp();
    needsReductionLabel->popUp();
    needsReductionFrame->popUp();
    changeLinkBtn->popUp();

    vizFrame->popDown();
    vizField->popDown();
    vizChangeBtn->popDown();
    vizLabel->popDown();
  }

  if (this->suppressInSolverToggle->getToggle()) {
    needsReductionLabel->popDown();
    needsReductionFrame->popDown();
    structDampFrame->popDown();
    massProportionalField->popDown();
    stiffProportionalField->popDown();
    dynPropFrame->popDown();
    stiffScaleField->popDown();
    massScaleField->popDown();
    suppressInSolverLabel->popUp();
  }
  else {
    structDampFrame->popUp();
    dynPropFrame->popUp();
    massProportionalField->popUp();
    stiffProportionalField->popUp();
    stiffScaleField->popUp();
    massScaleField->popUp();
    suppressInSolverLabel->popDown();
  }
}


////////////////////////////////////////////////////////////////////////////////

void FuiLinkOriginSheet::placeWidgets(int width, int height)
{
  posData->setEdgeGeometry(0, width, 0, height);
}


void FuiLinkOriginSheet::setEditedObjs(const std::vector<FmModelMemberBase*>& objs)
{
  posData->setEditedObjs(objs);
}


void FuiLinkOriginSheet::setSensitivity(bool s)
{
  posData->setSensitivity(s);
}


////////////////////////////////////////////////////////////////////////////////

void FuiLinkNodeSheet::initWidgets ()
{
  myFENodeField->setLabel("FE Node");
  myFENodeField->setSensitivity(false);
  myNodePosition->setRefChangedCB(FFaDynCB1M(FuiLinkNodeSheet,this,onPosRefChanged,bool));
  myNodePosition->setSensitivity(false);
  myViewedObj = NULL;
}


void FuiLinkNodeSheet::placeWidgets (int width, int height)
{
  int border = 6;
  int fontHeight = this->getFontHeigth();
  int fieldHeight = fontHeight + 5;
  myFENodeField->setEdgeGeometry(border,width/4-border,border,border+fieldHeight);
  myNodePosition->setEdgeGeometry(border,width/4-border,2*border+fieldHeight,height-border);
}


void FuiLinkNodeSheet::setValues (const FuiLinkValues& values)
{
  myFENodeField->myField->setValue(values.feNode);
  myPos = values.feNodePos;
  if (values.feNode > 0) {
    this->onPosRefChanged(myNodePosition->isGlobal());
    myNodePosition->popUp();
  }
  else
    myNodePosition->popDown();
}


void FuiLinkNodeSheet::onPosRefChanged (bool toGlobal)
{
  if (toGlobal && myViewedObj)
    myNodePosition->setValue(myViewedObj->getGlobalCS()*myPos);
  else
    myNodePosition->setValue(myPos);
}


////////////////////////////////////////////////////////////////////////////////

void FuiLinkRedOptSheet::initWidgets()
{
  this->singCriterionField->setLabel("Singularity criterion");
  this->singCriterionField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->singCriterionField->setAcceptedCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onDoubleChanged,double));

  this->componentModesField->setLabel("Component modes");
  this->componentModesField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->componentModesField->myField->setAcceptedCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onIntChanged,int));

  this->eigValToleranceField->setLabel("Eigenvalue tolerance");
  this->eigValToleranceField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->eigValToleranceField->setAcceptedCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onDoubleChanged,double));

  this->consistentMassBtn->setLabel("Consistent mass matrix");
  this->consistentMassBtn->setToggleCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onBtnToggeled,bool));

  this->ignoreCSBtn->setLabel("Ignore check-sum test");
  this->ignoreCSBtn->setToggleCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onIgnoreCSBtnToggeled,bool));

  this->expandMSBtn->setLabel("Expand mode shapes");
  this->expandMSBtn->setToggleCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onBtnToggeled,bool));

  this->needsReductionFrame->setLook(FFuFrame::PANEL_SUNKEN);
  this->needsReductionLabel->setLabel("Needs\nReduction");

  this->eigValFactFrame->setLabel("Eigenvalue Factorization");
  this->massFactBtn->setLabel("Mass");
  this->stiffFactBtn->setLabel("Stiffness");
  this->eigValFactorizationGroup.insert(massFactBtn);
  this->eigValFactorizationGroup.insert(stiffFactBtn);
  this->eigValFactorizationGroup.setExclusive(true);
  this->eigValFactorizationGroup.setValue(massFactBtn,true);
  this->eigValFactorizationGroup.setGroupToggleCB(FFaDynCB2M(FuiLinkRedOptSheet,this,
							     onBtnToggeled,int,bool));

  this->recoveryMatrixPrecisionFrame->setLabel("Recovery matrix storage precision");
  this->singlePrecisionBtn->setLabel("Single precision");
  this->doublePrecisionBtn->setLabel("Double precision");
  this->recoveryMatrixPrecisionGroup.insert(singlePrecisionBtn);
  this->recoveryMatrixPrecisionGroup.insert(doublePrecisionBtn);
  this->recoveryMatrixPrecisionGroup.setExclusive(true);
  this->recoveryMatrixPrecisionGroup.setValue(doublePrecisionBtn,true);
  this->recoveryMatrixPrecisionGroup.setGroupToggleCB(FFaDynCB2M(FuiLinkRedOptSheet,this,
								 onBtnToggeled,int,bool));
}


void FuiLinkRedOptSheet::placeWidgets(int width, int height)
{
  int border = 6;
  int top = 0;
  int bottom = height - border;
  int left = border;
  int right = width - border;
  int nLines = 6;
  int fontHeigth = this->getFontHeigth();
  int fieldHeight = fontHeigth+5 < bottom/nLines ? fontHeigth+5 : bottom/nLines;

  int innerHeight = bottom - top;
  int lineSpace = innerHeight/nLines;
  int line1 = top + lineSpace/2;
  int line2 = line1 + lineSpace;
  int line3 = line2 + lineSpace;
  int line4 = line3 + lineSpace;
  int line5 = line4 + lineSpace;
  int line6 = line5 + lineSpace;

  int needsReductionW = 60;
  int fieldWidth = (right - left)/2;
  int frameLeft = left + fieldWidth + 3*border;
  int frameRight = width - border;
  int frameTop = top + border; // Eigenvalue factorization
  int massBtnCenterY = frameTop + fontHeigth + border + fieldHeight/2;
  int stiffBtnCenterY = massBtnCenterY + fieldHeight;
  int frameBottom = stiffBtnCenterY + fieldHeight/2 + border;
  int frameTopRM = frameBottom + border; // Recovery matrix data
  int singlePrecisionBtnCenterY = frameTopRM + fontHeigth+border + fieldHeight/2;
  int doublePrecisionBtnCenterY = singlePrecisionBtnCenterY + fieldHeight;
  int frameBottomRM = doublePrecisionBtnCenterY + fieldHeight/2 + border;

  int labelWidth = this->eigValToleranceField->myLabel->getWidthHint();
  this->componentModesField->setLabelWidth(labelWidth);
  this->singCriterionField->setLabelWidth(labelWidth);

  this->singCriterionField->setCenterYGeometry(left, line1, fieldWidth, fieldHeight);
  this->componentModesField->setCenterYGeometry(left, line2, fieldWidth, fieldHeight);
  this->eigValToleranceField->setCenterYGeometry(left, line3, fieldWidth, fieldHeight);

  this->consistentMassBtn->setCenterYGeometrySizeHint(left, line4);
  this->ignoreCSBtn->setCenterYGeometrySizeHint(left, line5);
  this->expandMSBtn->setCenterYGeometrySizeHint(left, line6);

  this->needsReductionFrame->setEdgeGeometry(frameRight-needsReductionW, frameRight,
					     frameTop+border, frameBottom);
  this->needsReductionLabel->setEdgeGeometry(frameRight-needsReductionW+2, frameRight-2,
					     frameTop+border+2, frameBottom-2);

  this->eigValFactFrame->setEdgeGeometry(frameLeft, frameRight-needsReductionW-border,
					 frameTop, frameBottom);
  this->massFactBtn->setCenterYGeometrySizeHint(frameLeft + border, massBtnCenterY);
  this->stiffFactBtn->setCenterYGeometrySizeHint(frameLeft + border, stiffBtnCenterY);

  this->recoveryMatrixPrecisionFrame->setEdgeGeometry(frameLeft, frameRight, frameTopRM, frameBottomRM);
  this->singlePrecisionBtn->setCenterYGeometrySizeHint(frameLeft + border, singlePrecisionBtnCenterY);
  this->doublePrecisionBtn->setCenterYGeometrySizeHint(frameLeft + border, doublePrecisionBtnCenterY);
}


void FuiLinkRedOptSheet::setValues(const FuiLinkValues& values)
{
  this->IAmLocked = values.locked;

  this->singCriterionField->setValue(values.singularityCrit);
  this->componentModesField->myField->setValue(values.compModes);
  this->singCriterionField->setValue(values.singularityCrit);
  this->eigValToleranceField->setValue(values.eigValTolerance);

  bool isReducable = this->componentModesField->getSensitivity();
  this->eigValFactorizationGroup.setSensitivity(isReducable && values.compModes > 0);
  this->recoveryMatrixPrecisionGroup.setSensitivity(isReducable);

  this->consistentMassBtn->setValue(values.consistentMassMx);
  this->ignoreCSBtn->setValue(values.ignoreCheckSum);
  this->expandMSBtn->setValue(values.expandModeShapes);

  this->eigValFactorizationGroup.setValue(stiffFactBtn,values.factorStiffMx);
  this->recoveryMatrixPrecisionGroup.setValue(doublePrecisionBtn,values.recoveryMxPrec == FmPart::DOUBLE_PRECISION);

  if (values.reducedVersionNumber > 0)
    this->needsReductionLabel->setLabel(FFaNumStr("Reduced\n[%d]",values.reducedVersionNumber));
  else
    this->needsReductionLabel->setLabel("Needs\nreduction");

  this->updateSensitivity();
}


void FuiLinkRedOptSheet::getValues(FuiLinkValues& values)
{
  values.singularityCrit = this->singCriterionField->getValue();
  values.compModes = this->componentModesField->myField->getInt();
  values.singularityCrit = this->singCriterionField->getValue();
  values.eigValTolerance = this->eigValToleranceField->getValue();
  values.consistentMassMx = this->consistentMassBtn->getValue();
  values.ignoreCheckSum = this->ignoreCSBtn->getValue();
  values.expandModeShapes = this->expandMSBtn->getValue();
  values.factorStiffMx = this->stiffFactBtn->getValue();
  values.recoveryMxPrec = this->doublePrecisionBtn->getValue() ? FmPart::DOUBLE_PRECISION : FmPart::SINGLE_PRECISION;
}


void FuiLinkRedOptSheet::setSensitivity(bool s)
{
  this->IAmSensitive = s;

  this->updateSensitivity();
}


void FuiLinkRedOptSheet::updateSensitivity()
{
  bool s = this->IAmSensitive && !this->IAmLocked;

  this->singCriterionField->setSensitivity(s);
  this->componentModesField->setSensitivity(s);
  this->eigValToleranceField->setSensitivity(s);
  this->consistentMassBtn->setSensitivity(s);
  this->ignoreCSBtn->setSensitivity(s);
  this->expandMSBtn->setSensitivity(s);
  this->eigValFactorizationGroup.setSensitivity(s && this->componentModesField->myField->getInt() > 0);
  this->recoveryMatrixPrecisionGroup.setSensitivity(s);
}


void FuiLinkRedOptSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiLinkRedOptSheet::onDoubleChanged(double)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkRedOptSheet::onIntChanged(int)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkRedOptSheet::onBtnToggeled(bool)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkRedOptSheet::onBtnToggeled(int, bool)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkRedOptSheet::onIgnoreCSBtnToggeled(bool doIgnore)
{
  if (doIgnore)
    Fui::okDialog("The 'Ignore check-sum test' option should be activated for a part\n"
		  "only if you have problems with Fedem accepting the reduced FE data.\n"
		  "It is then assumed that the reduced FE matrices found on disk are\n"
		  "consistent with the current model, without further checking.\n"
		  "Incorrect results or other problems may occur in the Dynamics Solver\n"
		  "if the reduced FE matrices are NOT consistent.",FFuDialog::WARNING);

  this->valuesChangedCB.invoke();
}


////////////////////////////////////////////////////////////////////////////////

void FuiLinkLoadSheet::initWidgets()
{
  this->label->setLabel("Load Case  ");
  this->label->toFront();

  this->table->showRowHeader(true);
  this->table->showColumnHeader(true);
  this->table->setNumberColumns(2);
  this->table->setColumnLabel(0,"Delay");
  this->table->setColumnLabel(1,"Load Amplitude");
  this->table->stretchColWidth(1,true);
  this->table->setSelectionPolicy(FFuTable::NO_SELECTION);
}


void FuiLinkLoadSheet::placeWidgets(int width, int height)
{
  int border = 5;
  int fieldH = 20;
  int labelT = border + 6;
  int labelW = this->label->getWidthHint();
  int tableH = 2*border + (1+this->table->getNumberRows())*fieldH;
  if (tableH > height) tableH = height;

  this->label->setEdgeGeometry(border, labelW, labelT, border+fieldH);
  this->table->setEdgeGeometry(0, width, border, tableH);
  for (int i = 0; i < this->table->getNumberRows(); i++)
    this->table->setRowHeight(i, fieldH);

  this->table->setColumnWidth(0,75);
  this->table->setColumnWidth(1,width-150-border);
}


void FuiLinkLoadSheet::buildDynamicWidgets(const FuiLinkValues& values)
{
  if (values.loadCases.empty()) return; // don't touch if no load cases at all

  int numCols = this->table->getNumberColumns();
  int numRows = this->table->getNumberRows();
  for (int row = values.loadCases.size(); row < numRows; row++)
    for (int col = 0; col < numCols; col++)
      this->table->clearCellContents(row, col);

  this->setNoLoadCases(values.loadCases.size());

  this->table->setNumberRows(values.loadCases.size());

  for (size_t i = 0; i < values.loadCases.size(); i++) {
    this->loadFact[i]->setQuery(values.loadEngineQuery);
    this->loadFact[i]->setChangedCB(FFaDynCB2M(FuiLinkLoadSheet,this,onIntDoubleChanged,int,double));
    this->loadFact[i]->setButtonCB(values.editLoadEngineCB);
    this->delay[i]->setAcceptedCB(FFaDynCB1M(FuiLinkLoadSheet,this,onDoubleChanged,double));
    this->delay[i]->setValue(values.loadDelays[i]);
    if ((int)i >= numRows) {
      this->table->insertWidget(i, 0, this->delay[i]);
      this->table->insertWidget(i, 1, this->loadFact[i]);
    }
    this->table->setRowLabel(i, FFaNumStr("%10d     ",values.loadCases[i]).c_str());
    this->table->stretchRowHeight(i, true);
  }
}


void FuiLinkLoadSheet::setValues(const FuiLinkValues& values)
{
  for (size_t i = 0; i < values.loadCases.size(); i++) {
    this->loadFact[i]->setSelectedRef(values.selectedLoadEngines[i]);
    this->loadFact[i]->setValue(values.loadFactors[i]);
    this->loadFact[i]->setSensitivity(IAmSensitive);
    this->delay[i]->setValue(values.loadDelays[i]);
    this->delay[i]->setSensitivity(IAmSensitive && !this->loadFact[i]->isAConstant());
  }
}


void FuiLinkLoadSheet::getValues(FuiLinkValues& v)
{
  v.selectedLoadEngines.clear();
  v.loadFactors.clear();
  v.loadDelays.clear();
  if (this->isPoppedUp())
    for (size_t i = 0; i < this->loadFact.size(); i++) {
      v.selectedLoadEngines.push_back(this->loadFact[i]->getSelectedRef());
      v.loadFactors.push_back(this->loadFact[i]->getValue());
      v.loadDelays.push_back(this->delay[i]->getDouble());
    }
}


void FuiLinkLoadSheet::setSensitivity(bool s)
{
  IAmSensitive = s;

  for (size_t i = 0; i < this->loadFact.size(); i++) {
    this->loadFact[i]->setSensitivity(s);
    this->delay[i]->setSensitivity(s && !this->loadFact[i]->isAConstant());
  }
}


void FuiLinkLoadSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiLinkLoadSheet::onDoubleChanged(double)
{
  this->valuesChangedCB.invoke();
}

void FuiLinkLoadSheet::onIntDoubleChanged(int, double)
{
  this->valuesChangedCB.invoke();
}


////////////////////////////////////////////////////////////////////////////////

void FuiGenericPartMassSheet::initWidgets()
{
  this->calculateMassPropExplicitBtn->setLabel("Specify mass, inertia and CoG");
  this->calculateMassPropFEBtn->setLabel("Calculate from FE model");
  this->calculateMassPropGeoBtn->setLabel("Calculate from CAD geometry");

  this->calculateMassPropGroup.insert(calculateMassPropExplicitBtn);
  this->calculateMassPropGroup.insert(calculateMassPropGeoBtn);
  this->calculateMassPropGroup.insert(calculateMassPropFEBtn);
  this->calculateMassPropGroup.setExclusive(true);
  this->calculateMassPropGroup.setValue(calculateMassPropExplicitBtn,true);
  this->calculateMassPropGroup.setGroupToggleCB(FFaDynCB2M(FuiGenericPartMassSheet,this,onCalculateMassPropToggeled,int,bool));

  this->materialLabel->setLabel("Material");
  this->materialField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->materialField->setTextForNoRefSelected("rho = 7850.0"); //TODO: How go get greek letter rho in here?
  this->materialField->setButtonMeaning(FuiQueryInputField::EDIT);

  this->inertiaRefLabel->setLabel("Inertia Reference");
  this->inertiaRefMenu->setOptionSelectedCB(FFaDynCB1M(FuiGenericPartMassSheet,this,onOptionSelected,int));
  this->inertiaRefMenu->addOption("Part Orientation");
  this->inertiaRefMenu->addOption("CG Orientation");

  this->inertiaLabels[IXX]->setLabel("Ixx");
  this->inertiaLabels[IYY]->setLabel("Iyy");
  this->inertiaLabels[IZZ]->setLabel("Izz");
  this->inertiaLabels[IXY]->setLabel("Ixy");
  this->inertiaLabels[IXZ]->setLabel("Ixz");
  this->inertiaLabels[IYZ]->setLabel("Iyz");

  for (FFuIOField* field : this->inertias)
  {
    field->setInputCheckMode(FFuIOField::DOUBLECHECK);
    field->setAcceptedCB(FFaDynCB1M(FuiGenericPartMassSheet,this,onDoubleChanged,double));
  }

  this->massInertiaFrame->setLabel("Mass and Inertias");
  this->massField->setLabel("Mass");
  this->massField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->massField->setAcceptedCB(FFaDynCB1M(FuiGenericPartMassSheet, this, onDoubleChanged, double));
}


void FuiGenericPartMassSheet::placeWidgets(int width, int height)
{
  int border = 6;
  int fontHeigth = this->getFontHeigth();
  int fieldHeight = 20;
  int fieldSpace  = 10;

  while (5*fieldHeight + 5*fieldSpace > height - 2*border)
    if (5*fieldHeight > height - 2*border) {
      fieldHeight = (height - border)/5;
      fieldSpace = 0;
      break;
    }
    else
      fieldSpace--;

  int left = border;
  int right = width - border;

  int frameTop = border;
  int line1 = frameTop + fontHeigth + fieldSpace + fieldHeight/2;
  int line2 = line1 + fieldHeight + fieldSpace;
  int line3 = line2 + fieldHeight + fieldSpace;
  int line4 = line3 + fieldHeight + fieldSpace;
  int frameBtm = line4 + fieldHeight/2 + border;

  int colWidth = (right - left - 2*border)/5;
  int col1 = left + border;
  int col2 = col1 + colWidth;
  int col3 = col2 + colWidth;
  int col4 = col3 + colWidth;
  int col5 = col4 + colWidth;

  int labelCol1 = col2 + colWidth/2;
  int labelCol2 = col2 + 3*colWidth/4 - this->inertiaLabels[IYY]->getWidthHint()/2;
  int labelCol3 = col3 - this->inertiaLabels[IYY]->getWidthHint();

  int massFrameRight = right;
  int massFrameLeft  = labelCol1 - border;
  int inertRefRight  = massFrameLeft - border;

  int y = frameTop + fieldHeight/2;
  this->calculateMassPropExplicitBtn->setCenterYGeometryWidthHint(left, y, fieldHeight);
  y += fieldHeight;
  this->calculateMassPropFEBtn->setCenterYGeometryWidthHint(left, y, fieldHeight);
  y += fieldHeight;
  this->calculateMassPropGeoBtn->setCenterYGeometryWidthHint(left, y, fieldHeight);

  y += fieldHeight/2;
  this->materialLabel->setEdgeGeometry(left, inertRefRight, y, y+fontHeigth);
  y += fontHeigth+2;
  this->materialField->setEdgeGeometry(left, inertRefRight, y, y+fieldHeight);

  y += fieldHeight+border;
  this->inertiaRefLabel->setEdgeGeometry(left, inertRefRight, y, y+fontHeigth);
  y += fontHeigth+2;
  this->inertiaRefMenu->setEdgeGeometry(left, inertRefRight, y, y+fieldHeight);

  this->massInertiaFrame->setEdgeGeometry(massFrameLeft, massFrameRight, frameTop, frameBtm);

  this->inertiaLabels[IXX]->setCenterYGeometrySizeHint(labelCol1, line2);
  this->inertiaLabels[IXY]->setCenterYGeometrySizeHint(labelCol1, line3);
  this->inertiaLabels[IXZ]->setCenterYGeometrySizeHint(labelCol1, line4);
  this->inertiaLabels[IYY]->setCenterYGeometrySizeHint(labelCol2, line3);
  this->inertiaLabels[IYZ]->setCenterYGeometrySizeHint(labelCol2, line4);
  this->inertiaLabels[IZZ]->setCenterYGeometrySizeHint(labelCol3, line4);

  this->massField->setCenterYGeometry(labelCol1, line1, colWidth, fieldHeight);
  this->massField->toFront();
  this->inertias[IXX]->setCenterYGeometry(col3 + border/4, line2, colWidth-border/2, fieldHeight);
  this->inertias[IXY]->setCenterYGeometry(col3 + border/4, line3, colWidth-border/2, fieldHeight);
  this->inertias[IXZ]->setCenterYGeometry(col3 + border/4, line4, colWidth-border/2, fieldHeight);
  this->inertias[IYY]->setCenterYGeometry(col4 + border/4, line3, colWidth-border/2, fieldHeight);
  this->inertias[IYZ]->setCenterYGeometry(col4 + border/4, line4, colWidth-border/2, fieldHeight);
  this->inertias[IZZ]->setCenterYGeometry(col5 + border/4, line4, colWidth-border/2, fieldHeight);
}


void FuiGenericPartMassSheet::setValues(const FuiLinkValues& v)
{
  this->calculateMassPropGroup.setValue(v.useCalculatedMass);
  this->materialField->setQuery(v.materialQuery);
  this->materialField->setSelectedRef(v.selectedMaterial);

  this->inertiaRefMenu->selectOption(v.inertiaRef);

  this->massField->setValue(v.genericPartMass);
  for (int i = 0; i < NINERTIAS; i++)
    this->inertias[i]->setValue(v.genericPartInertia[i]);

  ICanCalculateMass = v.canCalculateMass;

  this->updateSensitivity();
}


void FuiGenericPartMassSheet::getValues(FuiLinkValues& v)
{
  v.useCalculatedMass = this->calculateMassPropGroup.getValue();

  v.inertiaRef = this->inertiaRefMenu->getSelectedOption();

  v.genericPartMass = this->massField->getValue();
  for (int i = 0; i < NINERTIAS; i++)
    v.genericPartInertia[i] = this->inertias[i]->getDouble();

  this->updateSensitivity();
}


void FuiGenericPartMassSheet::setSensitivity(bool s)
{
  this->IAmSensitive = s;

  this->updateSensitivity();
}


void FuiGenericPartMassSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiGenericPartMassSheet::onDoubleChanged(double)
{
  this->valuesChangedCB.invoke();
}

void FuiGenericPartMassSheet::onOptionSelected(int)
{
  this->valuesChangedCB.invoke();
}

void FuiGenericPartMassSheet::onCalculateMassPropToggeled(int, bool)
{
  this->valuesChangedCB.invoke();
}


void FuiGenericPartMassSheet::updateSensitivity()
{
  this->calculateMassPropExplicitBtn->setSensitivity(IAmSensitive);
  this->calculateMassPropGeoBtn->setSensitivity(IAmSensitive && (ICanCalculateMass == 'G' || ICanCalculateMass == 'B'));
  this->calculateMassPropFEBtn->setSensitivity(IAmSensitive && (ICanCalculateMass == 'F' || ICanCalculateMass == 'B'));

  this->materialLabel->setSensitivity(IAmSensitive && this->calculateMassPropGeoBtn->getValue());
  this->materialField->setSensitivity(IAmSensitive && this->calculateMassPropGeoBtn->getValue());

  if (this->calculateMassPropExplicitBtn->getValue()) {

    this->inertiaRefMenu->setSensitivity(IAmSensitive);
    this->massField->setSensitivity(IAmSensitive);
    this->inertias[IXX]->setSensitivity(IAmSensitive);
    this->inertias[IYY]->setSensitivity(IAmSensitive);
    this->inertias[IZZ]->setSensitivity(IAmSensitive);

    if (this->inertiaRefMenu->getSelectedOption() == FmPart::POS_CG_ROT_CS) {
      this->inertias[IXY]->setSensitivity(IAmSensitive);
      this->inertias[IXZ]->setSensitivity(IAmSensitive);
      this->inertias[IYZ]->setSensitivity(IAmSensitive);
    }
    else {
      this->inertias[IXY]->setSensitivity(false);
      this->inertias[IXZ]->setSensitivity(false);
      this->inertias[IYZ]->setSensitivity(false);
    }
  }

  else {
    this->inertiaRefMenu->setSensitivity(false);
    this->massField->setSensitivity(false);
    for (FFuIOField* field : this->inertias)
      field->setSensitivity(false);
  }
}


//////////////////////////////////////////////////////////////////////////////

void FuiGenericPartStiffSheet::initWidgets()
{
  this->stiffTypeFrame->setLabel("Type");
  this->stiffPropFrame->setLabel("Properties");

  this->defaultStiffTypeBtn->setLabel("Automatic");
  this->defaultStiffTypeBtn->setToolTip("This option selects an automatic stiffness computation based on the mass of the part");
  this->nodeStiffTypeBtn->setLabel("Manual");
  this->nodeStiffTypeBtn->setToolTip("This option is used to set the stiffness parameters manually");

  this->stiffTypeBtnGroup.insert(defaultStiffTypeBtn);
  this->stiffTypeBtnGroup.insert(nodeStiffTypeBtn);
  this->stiffTypeBtnGroup.setExclusive(true);
  this->stiffTypeBtnGroup.setValue(defaultStiffTypeBtn,true);
  this->stiffTypeBtnGroup.setGroupToggleCB(FFaDynCB2M(FuiGenericPartStiffSheet,this,onStiffTypeBtnToggeled,int,bool));

  this->ktField->setLabel("Translational stiffness at each triad");
  this->ktField->setToolTip("Translational stiffness at each triad");
  this->ktField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->ktField->setAcceptedCB(FFaDynCB1M(FuiGenericPartStiffSheet,this,onDoubleChanged,double));
  this->krField->setLabel("Rotational stiffness at each triad");
  this->krField->setToolTip("Rotational stiffness at each triad");
  this->krField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->krField->setAcceptedCB(FFaDynCB1M(FuiGenericPartStiffSheet,this,onDoubleChanged,double));

  this->stiffDescrLabel->setLabel("Stiffness is calulated based on the mass\nand a high target eigenfrequency");
}


void FuiGenericPartStiffSheet::placeWidgets(int width, int height)
{
  int border = 6;
  int left   = border;
  int right  = width - border;
  int top    = border;

  int fieldHeight = 20;
  int fieldSpace  = 10;

  while (2*fieldHeight + fieldSpace > height - border*9/5)
    if (2*fieldHeight > height - border*9/5) {
      fieldHeight = (height - border*9/5)/2;
      fieldSpace = 0;
      break;
    }
    else
      fieldSpace--;

  int frameTop = top;
  int line1 = frameTop + border*5/2 + fieldHeight/2;
  int line2 = line1 + fieldSpace + fieldHeight;
  int frameBtm = line2 + border + fieldHeight/2;

  int typeFrameLeft  = left;
  int typeFrameRight = (width - 2*border)/3;
  int propFrameLeft  = typeFrameRight + border;
  int propFrameRight = right;

  this->stiffTypeFrame->setEdgeGeometry(typeFrameLeft, typeFrameRight, frameTop, frameBtm);
  this->stiffPropFrame->setEdgeGeometry(propFrameLeft, propFrameRight, frameTop, frameBtm);

  int labelPropWidth = this->ktField->myLabel->getWidthHint();

  this->defaultStiffTypeBtn->setCenterYGeometrySizeHint(typeFrameLeft + border, line1);
  this->nodeStiffTypeBtn->setCenterYGeometrySizeHint(typeFrameLeft + border, line2);

  this->ktField->setLabelWidth(labelPropWidth);
  this->krField->setLabelWidth(labelPropWidth);

  this->ktField->setCenterYGeometry(propFrameLeft + border, line1,
				    propFrameRight - propFrameLeft - 2*border, fieldHeight);
  this->krField->setCenterYGeometry(propFrameLeft + border, line2,
				    propFrameRight - propFrameLeft - 2*border, fieldHeight);

  this->stiffDescrLabel->setEdgeGeometry(propFrameLeft + border, propFrameRight - border,
					 line1-fieldHeight/2, line1+fieldHeight*3/2);
}


void FuiGenericPartStiffSheet::setValues(const FuiLinkValues& v)
{
  this->ktField->setValue(v.genericPartKT);
  this->krField->setValue(v.genericPartKR);

  this->stiffTypeBtnGroup.setValue(defaultStiffTypeBtn, v.genericPartStiffType == FmPart::DEFAULT_RIGID);
  this->stiffTypeBtnGroup.setValue(nodeStiffTypeBtn,    v.genericPartStiffType == FmPart::NODE_STIFFNESS);

  this->update();
}


void FuiGenericPartStiffSheet::getValues(FuiLinkValues& v)
{
  v.genericPartKT = this->ktField->getValue();
  v.genericPartKR = this->krField->getValue();

  v.genericPartStiffType = this->defaultStiffTypeBtn->getValue() ? FmPart::DEFAULT_RIGID : FmPart::NODE_STIFFNESS;

  this->update();
}


void FuiGenericPartStiffSheet::setSensitivity(bool s)
{
  this->ktField->setSensitivity(s);
  this->krField->setSensitivity(s);
  this->defaultStiffTypeBtn->setSensitivity(s);
  this->nodeStiffTypeBtn->setSensitivity(s);

  this->update();
}


void FuiGenericPartStiffSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiGenericPartStiffSheet::onDoubleChanged(double)
{
  this->valuesChangedCB.invoke();
}

void FuiGenericPartStiffSheet::onOptionSelected(int)
{
  this->valuesChangedCB.invoke();
}

void FuiGenericPartStiffSheet::onStiffTypeBtnToggeled(int, bool)
{
  this->valuesChangedCB.invoke();
}


void FuiGenericPartStiffSheet::update()
{
  if (this->defaultStiffTypeBtn->getValue()) {
    this->ktField->popDown();
    this->krField->popDown();
    this->stiffDescrLabel->popUp();
  }
  else {
    this->ktField->popUp();
    this->krField->popUp();
    this->stiffDescrLabel->popDown();
  }
}


////////////////////////////////////////////////////////////////////////////////

void FuiGenericPartCGSheet::initWidgets()
{
  this->posData->editLinkCG(true);
  this->posData->setPosLabel("Center of Gravity");
  this->posData->setRotLabel("Principal Axes of Inertia");
  this->condenseCGToggle->setLabel("Condense out Center of Gravity DOFs");
  this->condenseCGToggle->setToggleCB(FFaDynCB1M(FuiGenericPartCGSheet,this,onCondenseCGToggeled,bool));
}


void FuiGenericPartCGSheet::placeWidgets(int width, int heigth)
{
  int left = 6;
  int posHeigth = this->posData->getHeightHint();
  int cgtHeigth = this->condenseCGToggle->getHeightHint();
  this->posData->setEdgeGeometry(0, width, 0, heigth);
  this->condenseCGToggle->setCenterYGeometryWidthHint(left,posHeigth+cgtHeigth/2,cgtHeigth);
}


void FuiGenericPartCGSheet::setEditedObjs(const std::vector<FmModelMemberBase*>& objs)
{
  this->posData->setEditedObjs(objs);
}


void FuiGenericPartCGSheet::setValues(const FuiLinkValues& values)
{
  this->posData->popUpRotUI(values.inertiaRef != FmPart::POS_CG_ROT_CS);

  this->condenseCGToggle->setValue(values.genericPartNoCGTriad);

  IAmUsingCalculatedMass = values.useCalculatedMass;
  this->updateSensitivity();
}


void FuiGenericPartCGSheet::getValues(FuiLinkValues& values)
{
  values.genericPartNoCGTriad = this->condenseCGToggle->getToggle();
}


void FuiGenericPartCGSheet::setSensitivity(bool s)
{
  IAmSensitive = s;

  this->updateSensitivity();
}


void FuiGenericPartCGSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiGenericPartCGSheet::onCondenseCGToggeled(bool)
{
  this->valuesChangedCB.invoke();
}


void FuiGenericPartCGSheet::updateSensitivity()
{
  this->posData->setSensitivity(IAmUsingCalculatedMass ? false : IAmSensitive);
  this->condenseCGToggle->setSensitivity(IAmSensitive);
}


////////////////////////////////////////////////////////////////////////////////

void FuiHydrodynamicsSheet::initWidgets()
{
  this->buoyancyToggle->setLabel("Perform buoyancy calculations");
  this->buoyancyToggle->setToggleCB(FFaDynCB1M(FuiHydrodynamicsSheet,this,onOptionToggled,bool));
  this->buoyancyLabel->setLabel("Buoyancy forces and associated load correction stiffnesses\n"
				"will be calculated based on the geometry defined in the\n"
				"specified Visualization file (.wrl or .ftc), and the water density");
}


void FuiHydrodynamicsSheet::placeWidgets(int, int)
{
  int border = 6;
  int btHeigth = this->buoyancyToggle->getHeightHint();
  int bfHeigth = this->buoyancyLabel->getHeightHint();
  this->buoyancyToggle->setCenterYGeometryWidthHint(border,border+btHeigth/2,btHeigth);
  this->buoyancyLabel->setCenterYGeometryWidthHint(border,2*border+btHeigth+bfHeigth/2,bfHeigth);
}


void FuiHydrodynamicsSheet::setValues(const FuiLinkValues& values)
{
  this->buoyancyToggle->setValue(values.buoyancy);
}


void FuiHydrodynamicsSheet::getValues(FuiLinkValues& values)
{
  values.buoyancy = this->buoyancyToggle->getToggle();
}


void FuiHydrodynamicsSheet::setSensitivity(bool s)
{
  this->buoyancyToggle->setSensitivity(s);
}


void FuiHydrodynamicsSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiHydrodynamicsSheet::onOptionToggled(bool)
{
  this->valuesChangedCB.invoke();
}


////////////////////////////////////////////////////////////////////////////////

void FuiMeshingSheet::initWidgets()
{
  this->materialLabel->setLabel("Material");
  this->materialField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->materialField->setButtonMeaning(FuiQueryInputField::EDIT);

  this->minsizeField->setLabel("Minimum number of elements");
  this->minsizeField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);;
  this->minsizeField->myField->setAcceptedCB(FFaDynCB1M(FuiMeshingSheet,this,onIntChanged,int));

  this->qualityLabel[0]->setLabel("Angle control");
  this->qualityLabel[1]->setLabel("Loose");
  this->qualityLabel[2]->setLabel("Strong");

  this->qualityScale->setMinMax(0,100);
  this->qualityScale->setReleaseCB(FFaDynCB0M(FuiMeshingSheet,this,onChanged));

  this->linearBtn->setLabel("Linear");
  this->parabolicBtn->setLabel("Parabolic");

  this->orderBtnGroup.insert(linearBtn);
  this->orderBtnGroup.insert(parabolicBtn);
  this->orderBtnGroup.setExclusive(true);
  this->orderBtnGroup.setValue(linearBtn,true);

  this->meshBtn->setLabel("Generate mesh");
  this->meshBtn->setActivateCB(FFaDynCB0M(FuiMeshingSheet,this,onMeshLink));
}


void FuiMeshingSheet::placeWidgets(int width, int)
{
  int border = 6;
  int left = border;
  int right = width/2-border;
  int ypos = border;
  int dy = this->meshBtn->getHeightHint();
  int fontHeight = this->getFontHeigth();
  int fieldHeight = 20;

  this->materialLabel->setEdgeGeometry(left,right, ypos, ypos+fontHeight);
  ypos += fontHeight + border;
  this->materialField->setEdgeGeometry(left,right, ypos, ypos+fieldHeight);
  ypos += fieldHeight+2*border;
  this->noElmsLabel->setCenterYGeometry(left,ypos+dy/2,right-left,dy); ypos += dy;
  this->noNodesLabel->setCenterYGeometry(left,ypos+dy/2,right-left,dy);

  left = right + 2*border;
  right = width - border;
  ypos = border;
  int qwidth0 = this->qualityLabel[0]->getWidthHint();
  int qwidth1 = this->qualityLabel[1]->getWidthHint();
  int qwidth2 = this->qualityLabel[2]->getWidthHint();
  int sleft = left + qwidth0+border;
  this->qualityLabel[1]->setCenterYGeometry(left+qwidth0+border,ypos+dy/2,qwidth1,dy);
  this->qualityLabel[2]->setCenterYGeometry(right-qwidth2,ypos+dy/2,qwidth2,dy); ypos += dy;
  this->qualityLabel[0]->setCenterYGeometry(left,ypos+dy/2,qwidth0,dy);
  this->qualityScale->setCenterYGeometry(sleft,ypos+dy/2,right-sleft,dy); ypos += dy+border;
  this->minsizeField->setCenterYGeometry(left,ypos+dy/2,right-left,dy); ypos += dy+2*border;
  this->meshBtn->setCenterYGeometry(left,ypos+dy,width/4-border,2*dy); left += width/4+border;
  this->linearBtn->setCenterYGeometry(left,ypos+dy/2,width/4-border,dy); ypos += dy;
  this->parabolicBtn->setCenterYGeometry(left,ypos+dy/2,width/4-border,dy);
}


void FuiMeshingSheet::setValues(const FuiLinkValues& values)
{
  this->materialField->setQuery(values.materialQuery);
  this->materialField->setSelectedRef(values.selectedMaterial);
  this->qualityScale->setValue((int)((5.0-values.quality)/0.039));
  this->minsizeField->myField->setValue(values.minSize);
  this->orderBtnGroup.setValue(parabolicBtn,values.parabolic);
  this->noElmsLabel->setLabel(FFaNumStr("Number of elements in current grid :  %d",values.noElms));
  this->noNodesLabel->setLabel(FFaNumStr("Number of nodes in current grid :  %d",values.noNodes));
}


void FuiMeshingSheet::getValues(FuiLinkValues& values)
{
  values.selectedMaterial = this->materialField->getSelectedRef();
  values.quality = 5.0 - 0.039*this->qualityScale->getValue();
  values.minSize = this->minsizeField->myField->getInt();
  values.parabolic = this->parabolicBtn->getToggle();
}


void FuiMeshingSheet::setSensitivity(bool s)
{
  this->materialField->setSensitivity(s);
  this->minsizeField->setSensitivity(s);
  this->qualityScale->setSensitivity(s);
  this->orderBtnGroup.setSensitivity(s);
  this->meshBtn->setSensitivity(s);
}


void FuiMeshingSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiMeshingSheet::setMeshLinkCB(const FFaDynCB1<bool>& aDynCB)
{
  this->meshLinkCB = aDynCB;
}


void FuiMeshingSheet::onIntChanged(int)
{
  this->valuesChangedCB.invoke();
}


void FuiMeshingSheet::onChanged()
{
  this->valuesChangedCB.invoke();
}


void FuiMeshingSheet::onMeshLink()
{
  this->meshLinkCB.invoke(this->parabolicBtn->getToggle());
}


////////////////////////////////////////////////////////////////////////////////

void FuiAdvancedLinkOptsSheet::initWidgets()
{
  this->coordSysLabel->setLabel("Positioning algorithm for co-rotated reference coordinate system:");
  this->coordSysOptionMenu->setOptionSelectedCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionSelected,int));
  this->coordSysOptionMenu->addOption("Model default");
  this->coordSysOptionMenu->addOption("Max triangle, with unit offset when necessary");
  this->coordSysOptionMenu->addOption("Max triangle, with scaled offset when necessary");
  this->coordSysOptionMenu->addOption("Mass based nodal average");

  this->centripLabel->setLabel("Centripital force correction:");
  this->centripOptionMenu->setOptionSelectedCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionSelected,int));
  this->centripOptionMenu->addOption("Model default");
  this->centripOptionMenu->addOption("On");
  this->centripOptionMenu->addOption("Off");

  this->recoverStressToggle->setLabel("Perform stress recovery during dynamics simulation");
  this->recoverStressToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionToggled,bool));
  this->recoverGageToggle->setLabel("Perform strain rosette recovery during dynamics simulation");
  this->recoverGageToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionToggled,bool));

  this->extResToggle->setLabel("Import residual stresses from external file");
  this->extResToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onExtResToggeled,bool));
  this->extResField->setLabel("External result file");
  this->extResField->setAbsToRelPath("yes");
  this->extResField->setFileOpenedCB(FFaDynCB2M(FuiAdvancedLinkOptsSheet,this,onExtResFileChanged,const std::string&,int));
  this->extResField->setDialogType(FFuFileDialog::FFU_OPEN_FILE);
  this->extResField->setDialogRememberKeyword("ExternalResultsField");

  std::vector<std::string> ansys = { "rst", "rth" };
  this->extResField->addDialogFilter("SDRC universal file","unv",true);
  this->extResField->addDialogFilter("ABAQUS result file","fil",false);
  this->extResField->addDialogFilter("ANSYS result file",ansys,false);
  this->extResField->addDialogFilter("NASTRAN Output2 file","op2",false);
}


void FuiAdvancedLinkOptsSheet::placeWidgets(int width, int)
{
  int border = 5;
  int left   = border;
  int top    = border;

  this->centripOptionMenu->setMinWidth(this->coordSysOptionMenu->getWidthHint());

  this->coordSysLabel->setCenterYGeometrySizeHint(left, top + coordSysLabel->getHeightHint()/2);
  this->coordSysOptionMenu->setCenterYGeometrySizeHint(left, coordSysLabel->getYPos() + coordSysLabel->getHeight() + border +
                                                       coordSysOptionMenu->getHeightHint()/2);

  this->centripLabel->setCenterYGeometrySizeHint(left, coordSysOptionMenu->getYPos() + coordSysOptionMenu->getHeight() +
                                                 centripLabel->getHeightHint());
  this->centripOptionMenu->setCenterYGeometrySizeHint(left, centripLabel->getYPos() + centripLabel->getHeight() + border +
                                                      centripOptionMenu->getHeightHint()/2);

  this->recoverStressToggle->setCenterYGeometrySizeHint(left, centripOptionMenu->getYPos() + centripOptionMenu->getHeight() +
                                                        recoverStressToggle->getHeightHint());
  this->recoverGageToggle->setCenterYGeometrySizeHint(left, recoverStressToggle->getYPos() + recoverStressToggle->getHeight() +
                                                      recoverGageToggle->getHeightHint());
  this->extResToggle->setCenterYGeometrySizeHint(left, recoverGageToggle->getYPos() + recoverGageToggle->getHeight() +
						 extResToggle->getHeightHint());

  int glbl = this->getGridLinePos(width,border,FFuMultUIComponent::FROM_START);
  int glbr = this->getGridLinePos(width,border,FFuMultUIComponent::FROM_END);
  int ypos = extResToggle->getYPos() + extResToggle->getHeight() + border;
  this->extResField->setEdgeGeometry(glbl,glbr,ypos,ypos+centripOptionMenu->getHeight());

  if (this->showExtRes) {
    this->extResToggle->popUp();
    this->extResField->popUp();
  }
  else {
    this->extResToggle->popDown();
    this->extResField->popDown();
  }
}


void FuiAdvancedLinkOptsSheet::setValues(const FuiLinkValues& values)
{
  ICanRecover = values.useGenericPart ? 0 : (values.recoveryOption >= 10 ? 2 : 1);

  this->coordSysOptionMenu->selectOption(values.coordSysOption);
  this->centripOptionMenu->selectOption(values.centripOption);
  this->recoverStressToggle->setValue(values.recoveryOption%2 > 0);
  this->recoverGageToggle->setValue(values.recoveryOption%10 >= 2);
  this->recoverStressToggle->setSensitivity(IAmSensitive && ICanRecover > 0);
  this->recoverGageToggle->setSensitivity(IAmSensitive && ICanRecover > 1);

  this->showExtRes = values.extResSwitch >= 0;
  this->extResToggle->setValue(values.extResSwitch > 0);
  this->extResField->setAbsToRelPath(values.modelFilePath);
  this->extResField->setFileName(values.extResFileName);
  this->extResField->setSensitivity(values.extResSwitch > 0);
}


void FuiAdvancedLinkOptsSheet::getValues(FuiLinkValues& v)
{
  v.coordSysOption = this->coordSysOptionMenu->getSelectedOption();
  v.centripOption = this->centripOptionMenu->getSelectedOption();
  v.recoveryOption = this->recoverStressToggle->getToggle() ? 1 : 0;
  if (this->recoverGageToggle->getToggle()) v.recoveryOption += 2;

  v.extResFileName = this->extResField->getFileName();
  if (this->showExtRes)
    v.extResSwitch = this->extResToggle->getToggle() && !v.extResFileName.empty();
  else
    v.extResSwitch = -1;
}


void FuiAdvancedLinkOptsSheet::setSensitivity(bool s)
{
  IAmSensitive = s;

  this->coordSysOptionMenu->setSensitivity(s);
  this->centripOptionMenu->setSensitivity(s);
  this->recoverStressToggle->setSensitivity(s && ICanRecover > 0);
  this->recoverGageToggle->setSensitivity(s && ICanRecover > 1);
}


void FuiAdvancedLinkOptsSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiAdvancedLinkOptsSheet::onOptionSelected(int)
{
  this->valuesChangedCB.invoke();
}


void FuiAdvancedLinkOptsSheet::onOptionToggled(bool)
{
  this->valuesChangedCB.invoke();
}


void FuiAdvancedLinkOptsSheet::onExtResToggeled(bool toggle)
{
  if (toggle && this->extResField->getFileName().empty())
    this->extResField->onBrowseButtonClicked();
  this->valuesChangedCB.invoke();
}

void FuiAdvancedLinkOptsSheet::onExtResFileChanged(const std::string&, int)
{
  this->valuesChangedCB.invoke();
}


////////////////////////////////////////////////////////////////////////////////

void FuiNonlinearLinkOptsSheet::initWidgets()
{
  this->nonlinearDescrLabel->setLabel("Capturing nonlinear model behaviour using CFEM");

  this->useNonlinearToggle->setLabel("Perform nonlinear solve for FE part");
  this->useNonlinearToggle->setToggleCB(FFaDynCB1M(FuiNonlinearLinkOptsSheet,this,
                                        onNonlinearToggeled,bool));

  this->numberOfSolutionsField->setLabel("Number of nonlinear solutions to solve for");
  this->numberOfSolutionsField->setToolTip("Number of stored solutions with force, stiffness, and displacement");
  this->numberOfSolutionsField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);
  this->numberOfSolutionsField->myField->setAcceptedCB(FFaDynCB1M(FuiNonlinearLinkOptsSheet,this,onIntChanged,int));

  this->nonlinearInputFileField->setLabel("Additional nonlinear solve input file");
  this->nonlinearInputFileField->setAbsToRelPath("yes");
  this->nonlinearInputFileField->setFileOpenedCB(FFaDynCB2M(FuiNonlinearLinkOptsSheet,this,
                                                 onNonlinearInputFileChanged,const std::string&,int));
  this->nonlinearInputFileField->setDialogType(FFuFileDialog::FFU_OPEN_FILE);
  this->nonlinearInputFileField->setDialogRememberKeyword("NonlinearInputFileField");

  this->nonlinearInputFileField->addDialogFilter("CFEM input data file","dat",true);
}


void FuiNonlinearLinkOptsSheet::placeWidgets(int width, int)
{
  int border = 5;
  int left   = border;
  int top    = border;
  int right  = width - border;

  this->nonlinearDescrLabel->setCenterYGeometrySizeHint(left, top + nonlinearDescrLabel->getHeightHint()/2);
  this->useNonlinearToggle->setCenterYGeometrySizeHint(left, nonlinearDescrLabel->getYPos()
                      + nonlinearDescrLabel->getHeight() + border + useNonlinearToggle->getHeightHint()/2);

  int labelWidth = this->numberOfSolutionsField->myLabel->getWidthHint();
  this->numberOfSolutionsField->setLabelWidth(labelWidth);
  int ypos = useNonlinearToggle->getYPos() + useNonlinearToggle->getHeight() + border + numberOfSolutionsField->getHeightHint()/2;
  this->numberOfSolutionsField->setCenterYGeometry(left, ypos, right-left, useNonlinearToggle->getHeight());

  int glbl = this->getGridLinePos(width,border,FFuMultUIComponent::FROM_START);
  int glbr = this->getGridLinePos(width,border,FFuMultUIComponent::FROM_END);
  ypos     = numberOfSolutionsField->getYPos() + useNonlinearToggle->getHeight() + border;
  this->nonlinearInputFileField->setEdgeGeometry(glbl,glbr,ypos,ypos+useNonlinearToggle->getHeight());
}


void FuiNonlinearLinkOptsSheet::setValues(const FuiLinkValues& values)
{
  this->useNonlinearToggle->setValue(values.useNonlinearSwitch);
  this->numberOfSolutionsField->myField->setValue(values.numNonlinear);
  this->numberOfSolutionsField->myField->setSensitivity(IAmSensitive && values.useNonlinearSwitch);
  this->nonlinearInputFileField->setAbsToRelPath(values.modelFilePath);
  this->nonlinearInputFileField->setFileName(values.nonlinearInputFile);
  this->nonlinearInputFileField->setSensitivity(IAmSensitive && values.useNonlinearSwitch);
}


void FuiNonlinearLinkOptsSheet::getValues(FuiLinkValues& v)
{
  v.nonlinearInputFile = this->nonlinearInputFileField->getFileName();
  v.useNonlinearSwitch = this->useNonlinearToggle->getToggle() && !v.nonlinearInputFile.empty();
  v.numNonlinear = this->numberOfSolutionsField->myField->getInt();
}


void FuiNonlinearLinkOptsSheet::setSensitivity(bool s)
{
  IAmSensitive = s;

  this->useNonlinearToggle->setSensitivity(s);
  this->numberOfSolutionsField->setSensitivity(s && this->useNonlinearToggle->getValue());
  this->nonlinearInputFileField->setSensitivity(s && this->useNonlinearToggle->getValue());
}


void FuiNonlinearLinkOptsSheet::setValuesChangedCB(const FFaDynCB0& aDynCB)
{
  this->valuesChangedCB = aDynCB;
}


void FuiNonlinearLinkOptsSheet::onOptionSelected(int)
{
  this->valuesChangedCB.invoke();
}

void FuiNonlinearLinkOptsSheet::onNonlinearToggeled(bool toggle)
{
  if (toggle && this->nonlinearInputFileField->getFileName().empty())
    this->nonlinearInputFileField->onBrowseButtonClicked();
  this->valuesChangedCB.invoke();
}

void FuiNonlinearLinkOptsSheet::onNonlinearInputFileChanged(const std::string&, int)
{
  this->valuesChangedCB.invoke();
}

void FuiNonlinearLinkOptsSheet::onIntChanged(int)
{
  this->valuesChangedCB.invoke();
}
