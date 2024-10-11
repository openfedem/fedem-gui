// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiPositionData.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#ifdef FT_HAS_WND
#include "vpmUI/vpmUITopLevels/FuiCreateTurbineAssembly.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuToggleButton.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmSMJointBase.H"
#ifdef FT_HAS_WND
#include "vpmDB/FmTurbine.H"
#else
#include "vpmDB/FmAssemblyBase.H"
#endif

#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdExtraGraphics.H"
#endif
#include "vpmPM/FpPM.H"

FuiPositionData* FuiPositionData::ourActiveUI = NULL;

typedef FFa3DLocation::PosType PosType;
typedef FFa3DLocation::RotType RotType;


FFaEnumMapping(PosType){
  FFaEnumEntry(FFa3DLocation::CART_X_Y_Z, "Cartesian XYZ");
  FFaEnumEntry(FFa3DLocation::CYL_R_YR_X, "Cylindrical R RotX X");
  FFaEnumEntry(FFa3DLocation::CYL_R_ZR_Y, "Cylindrical R RotY Y");
  FFaEnumEntry(FFa3DLocation::CYL_R_XR_Z, "Cylindrical R RotZ Z");
  FFaEnumEntryEnd;
};

FFaEnumMapping(RotType){
  FFaEnumEntry(FFa3DLocation::EUL_Z_Y_X , "EulerZYX as X, Y, Z");
  FFaEnumEntry(FFa3DLocation::PNT_PZ_PXZ, "Point on Z axis and in XZ plane");
  FFaEnumEntry(FFa3DLocation::PNT_PX_PXY, "Point on X axis and in XY plane");
  FFaEnumEntry(FFa3DLocation::DIR_EX_EXY, "X vector, and XY vector");
  FFaEnumEntryEnd;
};


FuiPositionData::FuiPositionData() : signalConnector(this)
{
  myEditedObj = NULL;
  IAmEditingLinkCG = false;
  ImAwareOfPosRefSelections = false;
  ImAwareOfRotRefSelections = false;
  IAmEditable = true;
}


void FuiPositionData::initWidgets()
{
  myPosRefCSField->popUp();
  myPosRefCSField->setBehaviour(FuiQueryInputField::REF_NONE);
  myPosRefCSField->setRefSelectedCB(FFaDynCB1M(FuiPositionData,this,
                                               onPosRefChanged, FmModelMemberBase*));
  myPosRefCSField->setButtonCB(FFaDynCB0M(FuiPositionData,this, onPosRefButtonPressed));
  myPosRefCSField->setButtonMeaning(FuiQueryInputField::SELECT);
  myPosRefCSField->setTextForNoRefSelected("Global");

  myRotRefCSField->popUp();
  myRotRefCSField->setBehaviour(FuiQueryInputField::REF_NONE);
  myRotRefCSField->setRefSelectedCB(FFaDynCB1M(FuiPositionData,this,
                                               onRotRefChanged, FmModelMemberBase*));
  myRotRefCSField->setButtonCB(FFaDynCB0M(FuiPositionData,this, onRotRefButtonPressed));
  myRotRefCSField->setButtonMeaning(FuiQueryInputField::SELECT);
  myRotRefCSField->setTextForNoRefSelected("Global");

  size_t i;
  for (i = 0; i < PosTypeMapping::map().size(); i++)
    myPosViewTypeMenu->addOption(PosTypeMapping::map()[i].second, i);

  myPosViewTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiPositionData,this,onPosTypeChanged, int));
  myPosViewTypeMenu->popUp();

  for (i = 0; i < RotTypeMapping::map().size(); i++)
    myRotViewTypeMenu->addOption(RotTypeMapping::map()[i].second, i);

  myRotViewTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiPositionData,this,onRotTypeChanged, int));
  myRotViewTypeMenu->popUp();

  for (FFuIOField* fld : myFields) {
    fld->popUp();
    fld->setInputCheckMode(FFuIOField::DOUBLECHECK);
    fld->setAcceptedCB(FFaDynCB1M(FuiPositionData,this,onFieldAccepted,double));
  }

  for (FFuLabel* lab : myLabels) {
    lab->popUp();
    lab->toFront();
  }

  myMasterFollowToggle->setToggleCB(FFaDynCB1M(FuiPositionData,this,onTriadsFollowToggled,bool));
  mySlaveFollowToggle->setToggleCB(FFaDynCB1M(FuiPositionData,this,onTriadsFollowToggled,bool));
}


bool FuiPositionData::isUsing6RotParams()
{
  if (!myEditedObj) return false;

  if (IAmEditingLinkCG)
    return (static_cast<FmPart*>(myEditedObj)->getLocationCG().getNumFields() == 9);
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    return (static_cast<FmIsPositionedBase*>(myEditedObj)->getLocation().getNumFields() == 9);
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    return (static_cast<FmAssemblyBase*>(myEditedObj)->getLocation().getNumFields() == 9);

  return false;
}


void FuiPositionData::editLinkCG(bool pleaseDo)
{
  IAmEditingLinkCG = pleaseDo;
}


void FuiPositionData::setEditedObj(FmModelMemberBase* obj)
{
  if (IAmEditingLinkCG && obj)
    if (obj->isOfType(FmPart::getClassTypeID()))
      myEditedObj = obj;
    else
      myEditedObj = NULL;
  else
    myEditedObj = obj;

  this->updateUI();
}


void FuiPositionData::updateUI()
{
  if (!myEditedObj) return;

  FFa3DLocation loc;
  if (IAmEditingLinkCG)
    loc = static_cast<FmPart*>(myEditedObj)->getLocationCG();
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    loc = static_cast<FmIsPositionedBase*>(myEditedObj)->getLocation();
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    loc = static_cast<FmAssemblyBase*>(myEditedObj)->getLocation();
  else
    return;

#ifdef FUI_DEBUG
  std::cout <<"FuiPositionData::updateUI: Location for "
	    << myEditedObj->getIdString() << loc << std::endl;
#endif

  PosType posType = loc.getPosType();
  RotType rotType = loc.getRotType();

  size_t i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      myFields[3*i+j]->setValue(loc[i][j]);

  FmIsPositionedBase* posRef = NULL;
  FmIsPositionedBase* rotRef = NULL;

  if (IAmEditingLinkCG) {
    posRef = static_cast<FmPart*>(myEditedObj)->getCGPosRef();
    rotRef = static_cast<FmPart*>(myEditedObj)->getCGRotRef();
  }
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID())) {
    posRef = static_cast<FmIsPositionedBase*>(myEditedObj)->getPosRef();
    rotRef = static_cast<FmIsPositionedBase*>(myEditedObj)->getRotRef();
  }

  static FapUAQuery query1;
  if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    query1.typesToFind.insert(std::make_pair(FmIsPositionedBase::getClassTypeID(),true));
  myPosRefCSField->setQuery(&query1);
  myRotRefCSField->setQuery(&query1);

  myPosRefCSField->setSelectedRef(posRef);
  myRotRefCSField->setSelectedRef(rotRef);

  for (i = 0; i < PosTypeMapping::map().size(); i++)
    if (PosTypeMapping::map()[i].first == posType) {
      myPosViewTypeMenu->selectOption(i);
      break;
    }

  for (i = 0; i < RotTypeMapping::map().size(); i++)
    if (RotTypeMapping::map()[i].first == rotType) {
      myRotViewTypeMenu->selectOption(i);
      break;
    }

  this->updateSensitivity();

  if (this->isUsing6RotParams())
    for (i = 6; i < 9; i++) {
      myFields[i]->popUp();
      myLabels[i]->popUp();
    }
  else
    for (i = 6; i < 9; i++) {
      myFields[i]->popDown();
      myLabels[i]->popDown();
    }

  switch (posType) {
  case FFa3DLocation::CART_X_Y_Z:
    myLabels[0]->setLabel("X");
    myLabels[1]->setLabel("Y");
    myLabels[2]->setLabel("Z");
    break;
  case FFa3DLocation::CYL_R_YR_X:
    myLabels[0]->setLabel("Radius");
    myLabels[1]->setLabel("RotX from Y [Deg]");
    myLabels[2]->setLabel("X");
    break;
  case FFa3DLocation::CYL_R_ZR_Y:
    myLabels[0]->setLabel("Radius");
    myLabels[1]->setLabel("RotY from Z [Deg]");
    myLabels[2]->setLabel("Y");
    break;
  case FFa3DLocation::CYL_R_XR_Z:
    myLabels[0]->setLabel("Radius");
    myLabels[1]->setLabel("RotZ from X [Deg]");
    myLabels[2]->setLabel("Z");
    break;
  default:
    break;
  }

  switch (rotType) {
  case FFa3DLocation::EUL_Z_Y_X:
    myLabels[3]->setLabel("RotX [Deg]");
    myLabels[4]->setLabel("RotY [Deg]");
    myLabels[5]->setLabel("RotZ [Deg]");
    break;
  case FFa3DLocation::PNT_PZ_PXZ:
    myLabels[3]->setLabel("Z-axis, X");
    myLabels[4]->setLabel("Z-axis, Y");
    myLabels[5]->setLabel("Z-axis, Z");
    myLabels[6]->setLabel("XZ-plane, X");
    myLabels[7]->setLabel("XZ-plane, Y");
    myLabels[8]->setLabel("XZ-plane, Z");
    break;
  case FFa3DLocation::PNT_PX_PXY:
    myLabels[3]->setLabel("X-axis, X");
    myLabels[4]->setLabel("X-axis, Y");
    myLabels[5]->setLabel("X-axis, Z");
    myLabels[6]->setLabel("XY-plane, X");
    myLabels[7]->setLabel("XY-plane, Y");
    myLabels[8]->setLabel("XY-plane, Z");
    break;
  case FFa3DLocation::DIR_EX_EXY:
    myLabels[3]->setLabel("X-vector, X");
    myLabels[4]->setLabel("X-vector, Y");
    myLabels[5]->setLabel("X-vector, Z");
    myLabels[6]->setLabel("XY-vector, X");
    myLabels[7]->setLabel("XY-vector, Y");
    myLabels[8]->setLabel("XY-vector, Z");
    break;
  default:
    break;
  }

  FmSMJointBase* joint = dynamic_cast<FmSMJointBase*>(myEditedObj);
  if (joint) {
    myTriadPosFollowFrame->popUp();
    myMasterFollowToggle->setValue(joint->isMasterMovedAlong());
    mySlaveFollowToggle->setValue(joint->isSlaveMovedAlong());
  }
  else
    myTriadPosFollowFrame->popDown();

#ifdef USE_INVENTOR
  if (this->isPoppedUp()) {
    FaMat34 posCS = posRef ? posRef->getGlobalCS() : FaMat34();
    FaMat34 rotCS = rotRef ? rotRef->getGlobalCS() : FaMat34();
    FdExtraGraphics::show3DLocation(posCS, rotCS, loc);
  }
#endif
}


void FuiPositionData::setPosSensitivity(bool isSensitive)
{
  myFields[0]->setSensitivity(isSensitive);
  myFields[1]->setSensitivity(isSensitive);
  myFields[2]->setSensitivity(isSensitive);
}


void FuiPositionData::setRotSensitivity(bool isSensitive)
{
  myFields[3]->setSensitivity(isSensitive);
  myFields[4]->setSensitivity(isSensitive);
  myFields[5]->setSensitivity(isSensitive);
  myFields[6]->setSensitivity(isSensitive);
  myFields[7]->setSensitivity(isSensitive);
  myFields[8]->setSensitivity(isSensitive);
}


void FuiPositionData::updateSensitivity()
{
  myMasterFollowToggle->setSensitivity(IAmEditable);
  mySlaveFollowToggle->setSensitivity(IAmEditable);

  if (!IAmEditable || !myEditedObj) {
    this->setRotSensitivity(false);
    this->setPosSensitivity(false);
  }
  else if (IAmEditingLinkCG) {
    this->setRotSensitivity(true);
    this->setPosSensitivity(true);
  }
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID())) {
    this->setRotSensitivity(static_cast<FmIsPositionedBase*>(myEditedObj)->isRotatable());
    this->setPosSensitivity(static_cast<FmIsPositionedBase*>(myEditedObj)->isTranslatable());
  }
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID())) {
    bool canMove = static_cast<FmAssemblyBase*>(myEditedObj)->isMovable();
    this->setRotSensitivity(canMove);
    this->setPosSensitivity(canMove);
  }
}


void FuiPositionData::onPosTypeChanged(int option)
{
  if (!myEditedObj) return;

  FFa3DLocation loc;
  if (IAmEditingLinkCG)
    loc = static_cast<FmPart*>(myEditedObj)->getLocationCG();
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    loc = static_cast<FmIsPositionedBase*>(myEditedObj)->getLocation();
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    loc = static_cast<FmAssemblyBase*>(myEditedObj)->getLocation();
  else
    return;

#ifdef FUI_DEBUG
  std::cout <<"FuiPositionData::onPosTypeChanged: Location for "
	    << myEditedObj->getIdString() << loc << std::endl;
#endif
  loc.changePosType(PosTypeMapping::map()[option].first);
#ifdef FUI_DEBUG
  std::cout <<"- updated to"<< loc << std::endl;
#endif

  if (IAmEditingLinkCG)
    static_cast<FmPart*>(myEditedObj)->setLocationCG(loc);
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    static_cast<FmIsPositionedBase*>(myEditedObj)->setLocation(loc);
  else
    static_cast<FmAssemblyBase*>(myEditedObj)->setLocation(loc);

  FpPM::touchModel();
  if (myEditedObj->isOfType(FmIsRenderedBase::getClassTypeID()))
    static_cast<FmIsRenderedBase*>(myEditedObj)->updateDisplayCS();
  this->updateUI();
}


void FuiPositionData::onRotTypeChanged(int option)
{
  if (!myEditedObj) return;

  FFa3DLocation loc;
  if (IAmEditingLinkCG)
    loc = static_cast<FmPart*>(myEditedObj)->getLocationCG();
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    loc = static_cast<FmIsPositionedBase*>(myEditedObj)->getLocation();
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    loc = static_cast<FmAssemblyBase*>(myEditedObj)->getLocation();
  else
    return;

#ifdef FUI_DEBUG
  std::cout <<"FuiPositionData::onRotTypeChanged: Location for "
	    << myEditedObj->getIdString() << loc << std::endl;
#endif
  loc.changeRotType(RotTypeMapping::map()[option].first);
#ifdef FUI_DEBUG
  std::cout <<"- updated to"<< loc << std::endl;
#endif

  if (IAmEditingLinkCG)
    static_cast<FmPart*>(myEditedObj)->setLocationCG(loc);
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    static_cast<FmIsPositionedBase*>(myEditedObj)->setLocation(loc);
  else
    static_cast<FmAssemblyBase*>(myEditedObj)->setLocation(loc);

  FpPM::touchModel();
  if (myEditedObj->isOfType(FmIsRenderedBase::getClassTypeID()))
    static_cast<FmIsRenderedBase*>(myEditedObj)->updateDisplayCS();
  this->updateUI();
}


void FuiPositionData::onFieldAccepted(double)
{
  if (!myEditedObj) return;

  FpPM::vpmSetUndoPoint("Position data");

  FFa3DLocation loc;
  if (IAmEditingLinkCG)
    loc = static_cast<FmPart*>(myEditedObj)->getLocationCG();
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    loc = static_cast<FmIsPositionedBase*>(myEditedObj)->getLocation();
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    loc = static_cast<FmAssemblyBase*>(myEditedObj)->getLocation();
  else
    return;

#ifdef FUI_DEBUG
  std::cout <<"FuiPositionData::onFieldAccepted: Location for "
	    << myEditedObj->getIdString() << loc << std::endl;
#endif

  size_t i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      loc[i][j] = myFields[3*i+j]->getDouble();

  if (loc.isValid()) // ignore invalid input
  {
#ifdef FUI_DEBUG
    std::cout <<"- updated to"<< loc << std::endl;
#endif
    if (IAmEditingLinkCG)
      static_cast<FmPart*>(myEditedObj)->setLocationCG(loc);
    else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
      static_cast<FmIsPositionedBase*>(myEditedObj)->setLocation(loc);
    else
      static_cast<FmAssemblyBase*>(myEditedObj)->setLocation(loc);

    FpPM::touchModel();
    if (myEditedObj->isOfType(FmPart::getClassTypeID()))
      static_cast<FmPart*>(myEditedObj)->updateDisplayTopology(); // Bugfix #493
    else if (myEditedObj->isOfType(FmIsRenderedBase::getClassTypeID()))
      static_cast<FmIsRenderedBase*>(myEditedObj)->draw();
    else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID())) {
      FmDB::displayAll(*static_cast<FmAssemblyBase*>(myEditedObj)->getHeadMap());
      Fui::getMainWindow()->getProperties()->updateSubassCoG();
    }

#ifdef FT_HAS_WND
    FFuTopLevelShell* cta = NULL;
    if (static_cast<FmTurbine*>(myEditedObj) == FmDB::getTurbineObject())
      cta = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineAssembly::getClassTypeID());
    else if (IAmEditingLinkCG && dynamic_cast<FmNacelle*>(myEditedObj->getParentAssembly())) {
      // Update the nacelle CoG field in the wind turbine definition dialog
      FmNacelle* nac = static_cast<FmNacelle*>(myEditedObj->getParentAssembly());
      nac->CoG.setValue(nac->toLocal(nac->getGlobalCoG(false)));
      cta = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineAssembly::getClassTypeID());
    }
    if (cta) dynamic_cast<FuiCreateTurbineAssembly*>(cta)->updateUIValues();
#endif
  }
  this->updateUI();
}


void FuiPositionData::onPosRefChanged(FmModelMemberBase* obj)
{
  if (!myEditedObj) return;

  if (IAmEditingLinkCG) {
    FmPart* part = static_cast<FmPart*>(myEditedObj);
    part->setCGPosRef(dynamic_cast<FmIsPositionedBase*>(obj));
  }
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID())) {
    FmIsPositionedBase* editedObj = static_cast<FmIsPositionedBase*>(myEditedObj);
    editedObj->setPosRef(dynamic_cast<FmIsPositionedBase*>(obj));
  }
  else
    return;

  FpPM::touchModel();
  this->updateUI();
}


void FuiPositionData::onRotRefChanged(FmModelMemberBase* obj)
{
  if (!myEditedObj) return;

  if (IAmEditingLinkCG) {
    FmPart* part = static_cast<FmPart*>(myEditedObj);
    part->setCGRotRef(dynamic_cast<FmIsPositionedBase*>(obj));
  }
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID())) {
    FmIsPositionedBase* editedObj = static_cast<FmIsPositionedBase*>(myEditedObj);
    editedObj->setRotRef(dynamic_cast<FmIsPositionedBase*>(obj));
  }
  else
    return;

  FpPM::touchModel();
  this->updateUI();
}


void FuiPositionData::onRotRefButtonPressed()
{
  ImAwareOfRotRefSelections = true;
  this->prepareRefCSSelection();
}


void FuiPositionData::onPosRefButtonPressed()
{
  ImAwareOfPosRefSelections = true;
  this->prepareRefCSSelection();
}

void FuiPositionData::onTriadsFollowToggled(bool)
{
  if (!myEditedObj) return;

  FmSMJointBase* joint = dynamic_cast<FmSMJointBase*>(myEditedObj);
  if (joint) {
    joint->setMasterMovedAlong(myMasterFollowToggle->getValue());
    joint->setSlaveMovedAlong(mySlaveFollowToggle->getValue());
    FpPM::touchModel();
  }
  this->updateUI();
}


void FuiPositionData::prepareRefCSSelection()
{
  FapUAProperties* uap = FapUAProperties::getPropertiesHandler();
  if (uap) uap->setIgnorePickNotify(true);

  FapEventManager::pushPermSelection();
  ourActiveUI = this;

  FuiModes::setMode(FuiModes::SELECTREFCS_MODE);
}


void FuiPositionData::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                                             const std::vector<FFaViewItem*>&,
                                             const std::vector<FFaViewItem*>&)
{
  if (!ImAwareOfPosRefSelections && !ImAwareOfRotRefSelections) return;

  FmIsPositionedBase* candidate = NULL;
  for (FFaViewItem* item : totalSelection)
    if ((candidate = dynamic_cast<FmIsPositionedBase*>(item)))
      break;

  if (!candidate) return;

  if (ImAwareOfRotRefSelections)
    this->onRotRefChanged(candidate);

  if (ImAwareOfPosRefSelections)
    this->onPosRefChanged(candidate);
}


void FuiPositionData::finishRefCSSelection()
{
  ImAwareOfPosRefSelections = false;
  ImAwareOfRotRefSelections = false;

  FapEventManager::popPermSelection();

  FapUAProperties* uap = FapUAProperties::getPropertiesHandler();
  if (uap) uap->setIgnorePickNotify(false);
}


void FuiPositionData::onPoppedUp()
{
#ifdef USE_INVENTOR
  if (!myEditedObj) return;

  FFa3DLocation       loc;
  FmIsPositionedBase* posRef = NULL;
  FmIsPositionedBase* rotRef = NULL;

  if (IAmEditingLinkCG)
  {
    loc    = static_cast<FmPart*>(myEditedObj)->getLocationCG();
    posRef = static_cast<FmPart*>(myEditedObj)->getCGPosRef();
    rotRef = static_cast<FmPart*>(myEditedObj)->getCGRotRef();
  }
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
  {
    loc    = static_cast<FmIsPositionedBase*>(myEditedObj)->getLocation();
    posRef = static_cast<FmIsPositionedBase*>(myEditedObj)->getPosRef();
    rotRef = static_cast<FmIsPositionedBase*>(myEditedObj)->getRotRef();
  }
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    loc    = static_cast<FmAssemblyBase*>(myEditedObj)->getLocation();
  else
    return;

#ifdef FUI_DEBUG
  std::cout <<"FuiPositionData::onPoppedUp: Location for "
	    << myEditedObj->getIdString() << loc << std::endl;
#endif
  FaMat34 posCS = posRef ? posRef->getGlobalCS() : FaMat34();
  FaMat34 rotCS = rotRef ? rotRef->getGlobalCS() : FaMat34();
  FdExtraGraphics::show3DLocation(posCS, rotCS, loc);
#endif
}


void FuiPositionData::onPoppedDown()
{
#ifdef USE_INVENTOR
  FdExtraGraphics::hide3DLocation();
#endif
}


void FuiPositionData::cancelActiveUI()
{
  if (ourActiveUI)
    ourActiveUI->finishRefCSSelection();
  ourActiveUI = NULL;
}
