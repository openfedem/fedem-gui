// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiLinkTabs.H"
#include "vpmUI/vpmUIComponents/FuiDynamicProperties.H"
#include "vpmUI/vpmUIComponents/FuiPositionData.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Fui.H"
#include "vpmDB/FmPart.H"

#include "FFuLib/FFuTable.H"
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
#include "FFuLib/FFuScrolledList.H"
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
  this->importedFileField->setLabelWidth(this->repositoryFileField->myLabel->getWidthHint());

  this->vizFrame->setLabel("Visualization");
  this->vizField->setLabel("File:");
  this->vizField->myField->setAcceptedCB(FFaDynCB1M(FuiLinkModelSheet,this,onVizFileTyped,const std::string&));
  this->vizChangeBtn->setLabel("Change...");
  this->vizChangeBtn->setActivateCB(FFaDynCB0M(FuiLinkModelSheet,this,onChangeViz));

  this->changeLinkBtn->setLabel("Change...");
  this->changeLinkBtn->setActivateCB(FFaDynCB0M(FFaDynCB0,&changeLinkCB,invoke));
  this->unitConversionLabel->setLabel("No unit conversion");

  dynamicProps->initWidgets(FFaDynCB1M(FuiLinkModelSheet,this,onDoubleChanged,double));
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

  this->ICanChangeViz = values.allowChangeViz;
  this->vizField->setValue(values.vizFile);
  this->vizField->setSensitivity(this->IAmSensitive && this->ICanChangeViz);
  this->vizChangeBtn->setSensitivity(this->IAmSensitive && this->ICanChangeViz);

  if (values.usingFEModelViz)
    this->vizLabel->setLabel("Using FE model as visualization");
  else
    this->vizLabel->setLabel("");

  dynamicProps->setValues(values.massDamping,values.stiffDamping,
                          values.stiffScale,values.massScale);

  reductionFrame->setReducedVersion(values.reducedVersionNumber);

  this->update();
}


void FuiLinkModelSheet::getValues(FuiLinkValues& values)
{
  values.suppressInSolver = this->suppressInSolverToggle->getToggle();
  values.useGenericPart = this->genPartBtn->getToggle();

  dynamicProps->getValues(values.massDamping,values.stiffDamping,
                          values.stiffScale,values.massScale);
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

  dynamicProps->setSensitivity(s);
}


void FuiLinkModelSheet::update()
{
  if (genPartBtn->getToggle()) {
    feModelFrame->popDown();
    reductionFrame->popDown();
    vizFrame->popUp();
  }
  else {
    feModelFrame->popUp();
    reductionFrame->popUp();
    vizFrame->popDown();
  }

  if (suppressInSolverToggle->getToggle()) {
    suppressInSolverLabel->popUp();
    reductionFrame->popDown();
    dynamicProps->popDown();
  }
  else {
    suppressInSolverLabel->popDown();
    dynamicProps->popUp();
  }
}


////////////////////////////////////////////////////////////////////////////////

void FuiLinkNodeSheet::initWidgets()
{
  myNodePosition->setRefChangedCB(FFaDynCB1M(FuiLinkNodeSheet,this,onPosRefChanged,bool));
  myNodePosition->setSensitivity(false);
  myNodePosition->setMaxWidth(150);
  myElementLabel->setLabel("Connected elements");
  myElements->setSensitivity(false);
}


void FuiLinkNodeSheet::setValues(const FuiLinkValues& values)
{
  if (values.feNode > 0) {
    myNodePosition->setValue(values.feNode);
    myElements->setItems(values.elmList);
    myPos = values.feNodePos;
    this->onPosRefChanged(myNodePosition->isGlobal());
    myNodePosition->popUp();
    myElementLabel->popUp();
    myElements->popUp();
  }
  else {
    myNodePosition->popDown();
    myElementLabel->popDown();
    myElements->popDown();
  }
}


void FuiLinkNodeSheet::onPosRefChanged(bool toGlobal)
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

  singCriterionField->setLabelWidth(eigValToleranceField->myLabel->getWidthHint());
  componentModesField->setLabelWidth(eigValToleranceField->myLabel->getWidthHint());

  this->consistentMassBtn->setLabel("Consistent mass matrix");
  this->consistentMassBtn->setToggleCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onBtnToggeled,bool));

  this->ignoreCSBtn->setLabel("Ignore check-sum test");
  this->ignoreCSBtn->setToggleCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onIgnoreCSBtnToggeled,bool));

  this->expandMSBtn->setLabel("Expand mode shapes");
  this->expandMSBtn->setToggleCB(FFaDynCB1M(FuiLinkRedOptSheet,this,onBtnToggeled,bool));

  this->massFactBtn->setLabel("Mass");
  this->stiffFactBtn->setLabel("Stiffness");
  this->eigValFactorizationGroup.insert(massFactBtn);
  this->eigValFactorizationGroup.insert(stiffFactBtn);
  this->eigValFactorizationGroup.setExclusive(true);
  this->eigValFactorizationGroup.setValue(massFactBtn,true);
  this->eigValFactorizationGroup.setGroupToggleCB(FFaDynCB2M(FuiLinkRedOptSheet,this,
							     onBtnToggeled,int,bool));

  this->singlePrecisionBtn->setLabel("Single precision");
  this->doublePrecisionBtn->setLabel("Double precision");
  this->recoveryMatrixPrecisionGroup.insert(singlePrecisionBtn);
  this->recoveryMatrixPrecisionGroup.insert(doublePrecisionBtn);
  this->recoveryMatrixPrecisionGroup.setExclusive(true);
  this->recoveryMatrixPrecisionGroup.setValue(doublePrecisionBtn,true);
  this->recoveryMatrixPrecisionGroup.setGroupToggleCB(FFaDynCB2M(FuiLinkRedOptSheet,this,
								 onBtnToggeled,int,bool));
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

  reductionFrame->setReducedVersion(values.reducedVersionNumber);

  this->setSensitivity(IAmSensitive);
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
  IAmSensitive = s;
  if (IAmLocked) s = false;

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

void FuiLinkLoadSheet::initWidgets(const FuiLinkValues& values)
{
  for (size_t i = 0; i < values.loadCases.size(); i++)
  {
    loadCase[i]->setLabel(FFaNumStr("%8d",values.loadCases[i]));

    loadFact[i]->setQuery(values.loadEngineQuery);
    loadFact[i]->setChangedCB(FFaDynCB2M(FuiLinkLoadSheet,this,
                                         onIntDoubleChanged,int,double));
    loadFact[i]->setButtonCB(values.editLoadEngineCB);

    delay[i]->setValue(values.loadDelays[i]);
    delay[i]->setAcceptedCB(FFaDynCB1M(FuiLinkLoadSheet,this,
                                       onDoubleChanged,double));
  }
}


void FuiLinkLoadSheet::setValues(const FuiLinkValues& values)
{
  for (size_t i = 0; i < values.loadCases.size(); i++)
  {
    loadFact[i]->setSelectedRef(values.selectedLoadEngines[i]);
    loadFact[i]->setValue(values.loadFactors[i]);
    loadFact[i]->setSensitivity(IAmSensitive);
    delay[i]->setValue(values.loadDelays[i]);
    delay[i]->setSensitivity(IAmSensitive && !loadFact[i]->isAConstant());
  }
}


void FuiLinkLoadSheet::getValues(FuiLinkValues& v)
{
  v.selectedLoadEngines.clear();
  v.loadFactors.clear();
  v.loadDelays.clear();
  if (this->isPoppedUp())
    for (size_t i = 0; i < loadFact.size(); i++)
    {
      v.selectedLoadEngines.push_back(loadFact[i]->getSelectedRef());
      v.loadFactors.push_back(loadFact[i]->getValue());
      v.loadDelays.push_back(delay[i]->getDouble());
    }
}


void FuiLinkLoadSheet::setSensitivity(bool s)
{
  IAmSensitive = s;

  for (size_t i = 0; i < loadFact.size(); i++)
  {
    loadFact[i]->setSensitivity(s);
    delay[i]->setSensitivity(s && !loadFact[i]->isAConstant());
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

  this->materialField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->materialField->setTextForNoRefSelected("rho = 7850.0"); //TODO: How go get greek letter rho in here?
  this->materialField->setButtonMeaning(FuiQueryInputField::EDIT);

  this->inertiaRefMenu->setOptionSelectedCB(FFaDynCB1M(FuiGenericPartMassSheet,this,onOptionSelected,int));
  this->inertiaRefMenu->addOption("Part Orientation");
  this->inertiaRefMenu->addOption("CG Orientation");

  for (FFuIOField* field : this->inertias)
  {
    field->setInputCheckMode(FFuIOField::DOUBLECHECK);
    field->setAcceptedCB(FFaDynCB1M(FuiGenericPartMassSheet,this,onDoubleChanged,double));
  }

  this->massField->setLabel("Mass");
  this->massField->myField->setInputCheckMode(FFuIOField::DOUBLECHECK);
  this->massField->setAcceptedCB(FFaDynCB1M(FuiGenericPartMassSheet, this, onDoubleChanged, double));
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
  this->krField->setLabelWidth(this->ktField->myLabel->getWidthHint());

  this->stiffDescrLabel->setLabel("Stiffness is calulated based on the mass\n"
                                  "and a high target eigenfrequency");
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
  this->materialField->setBehaviour(FuiQueryInputField::REF_NONE);
  this->materialField->setButtonMeaning(FuiQueryInputField::EDIT);

  this->minsizeField->setLabel("Minimum number of elements");
  this->minsizeField->myField->setInputCheckMode(FFuIOField::INTEGERCHECK);;
  this->minsizeField->myField->setAcceptedCB(FFaDynCB1M(FuiMeshingSheet,this,onIntChanged,int));

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
  this->coordSysOptionMenu->setOptionSelectedCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionSelected,int));
  this->coordSysOptionMenu->addOption("Model default");
  this->coordSysOptionMenu->addOption("Max triangle, with unit offset when necessary");
  this->coordSysOptionMenu->addOption("Max triangle, with scaled offset when necessary");
  this->coordSysOptionMenu->addOption("Mass based nodal average");

  this->centripOptionMenu->setOptionSelectedCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionSelected,int));
  this->centripOptionMenu->addOption("Model default");
  this->centripOptionMenu->addOption("On");
  this->centripOptionMenu->addOption("Off");

  this->recoverStressToggle->setLabel("Perform stress recovery during dynamics simulation");
  this->recoverStressToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionToggled,bool));
  this->recoverGageToggle->setLabel("Perform strain rosette recovery during dynamics simulation");
  this->recoverGageToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionToggled,bool));
  this->ignoreRecoveryToggle->setLabel("Skip stress recovery for this part");
  this->ignoreRecoveryToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onOptionToggled,bool));

  this->extResToggle->setLabel("Import residual stresses from external file:");
  this->extResToggle->setToggleCB(FFaDynCB1M(FuiAdvancedLinkOptsSheet,this,onExtResToggeled,bool));
  this->extResField->setLabel("");
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


void FuiAdvancedLinkOptsSheet::setValues(const FuiLinkValues& values)
{
  ICanRecover = values.useGenericPart ? 0 : (values.recoveryOption >= 10 ? 2 : 1);

  this->coordSysOptionMenu->selectOption(values.coordSysOption);
  this->centripOptionMenu->selectOption(values.centripOption);
  this->recoverStressToggle->setValue(values.recoveryOption%2 > 0);
  this->recoverGageToggle->setValue(values.recoveryOption%10 >= 2);
  this->ignoreRecoveryToggle->setValue(values.ignoreRecovery);
  this->recoverStressToggle->setSensitivity(IAmSensitive && ICanRecover > 0);
  this->recoverGageToggle->setSensitivity(IAmSensitive && ICanRecover > 1);
  this->ignoreRecoveryToggle->setSensitivity(IAmSensitive && ICanRecover > 0);

  this->extResToggle->setValue(values.extResSwitch > 0);
  this->extResField->setAbsToRelPath(values.modelFilePath);
  this->extResField->setFileName(values.extResFileName);
  this->extResField->setSensitivity(values.extResSwitch > 0);

  if (values.extResSwitch >= 0) {
    extResToggle->popUp();
    extResField->popUp();
  }
  else {
    extResToggle->popDown();
    extResField->popDown();
  }
}


void FuiAdvancedLinkOptsSheet::getValues(FuiLinkValues& v)
{
  v.coordSysOption = this->coordSysOptionMenu->getSelectedOption();
  v.centripOption = this->centripOptionMenu->getSelectedOption();
  v.recoveryOption = this->recoverStressToggle->getToggle() ? 1 : 0;
  if (this->recoverGageToggle->getToggle()) v.recoveryOption += 2;
  v.ignoreRecovery = this->ignoreRecoveryToggle->getToggle();

  v.extResFileName = this->extResField->getFileName();
  if (extResToggle->isPoppedUp())
    v.extResSwitch = extResToggle->getToggle() && !v.extResFileName.empty();
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
  this->ignoreRecoveryToggle->setSensitivity(s && ICanRecover > 0);
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
