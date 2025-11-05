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
#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"

#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
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
  IAmEditable = true;
  IAmEditingLinkCG = false;
  IAmDoingRefCSSelection = false;
}


void FuiPositionData::initWidgets()
{
  myPosRefCSField->setBehaviour(FuiQueryInputField::REF_NONE);
  myPosRefCSField->setRefSelectedCB(FFaDynCB1M(FuiPositionData,this,
                                               onPosRefChanged,FmModelMemberBase*));
  myPosRefCSField->setButtonCB(FFaDynCB0M(FuiPositionData,this,
                                          onPosRefButtonPressed));
  myPosRefCSField->setButtonMeaning(FuiQueryInputField::SELECT);
  myPosRefCSField->setTextForNoRefSelected("Global");

  myRotRefCSField->setBehaviour(FuiQueryInputField::REF_NONE);
  myRotRefCSField->setRefSelectedCB(FFaDynCB1M(FuiPositionData,this,
                                               onRotRefChanged,FmModelMemberBase*));
  myRotRefCSField->setButtonCB(FFaDynCB0M(FuiPositionData,this,
                                          onRotRefButtonPressed));
  myRotRefCSField->setButtonMeaning(FuiQueryInputField::SELECT);
  myRotRefCSField->setTextForNoRefSelected("Global");

  for (size_t i = 0; i < PosTypeMapping::map().size(); i++)
    myPosViewTypeMenu->addOption(PosTypeMapping::map()[i].second, i);
  myPosViewTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiPositionData,this,
                                                    onPosTypeChanged,int));

  for (size_t i = 0; i < RotTypeMapping::map().size(); i++)
    myRotViewTypeMenu->addOption(RotTypeMapping::map()[i].second, i);
  myRotViewTypeMenu->setOptionSelectedCB(FFaDynCB1M(FuiPositionData,this,
                                                    onRotTypeChanged,int));

  for (int i = 0; i < 9; i++)
  {
    myFieldCB[i] = { this, i/3, i%3 };
    myFields[i]->setInputCheckMode(FFuIOField::DOUBLECHECK);
    myFields[i]->setAcceptedCB(FFaDynCB1M(FieldChangedCB,&myFieldCB[i],
                                          onFieldAccepted,double));
  }

  myMasterFollowToggle->setToggleCB(FFaDynCB1M(FuiPositionData,this,
                                               onTriadsFollowToggled,bool));
  mySlaveFollowToggle->setToggleCB(FFaDynCB1M(FuiPositionData,this,
                                              onTriadsFollowToggled,bool));
}


void FuiPositionData::setPosLabel(const char* text)
{
  myTranslationFrame->setLabel(text);
}

void FuiPositionData::setRotLabel(const char* text)
{
  myRotationFrame->setLabel(text);
}


void FuiPositionData::popUpPosUI(bool onOff)
{
  if (onOff)
    myTranslationFrame->popUp();
  else
    myTranslationFrame->popDown();
}

void FuiPositionData::popUpRotUI(bool onOff)
{
  if (onOff)
    myRotationFrame->popUp();
  else
    myRotationFrame->popDown();
}


void FuiPositionData::setEditedObjs(const ObjectVec& objs)
{
  if (IAmEditingLinkCG)
  {
    myEditedObjs.clear();
    myEditedObjs.reserve(objs.size());
    for (FmModelMemberBase* obj : objs)
      if (obj->isOfType(FmPart::getClassTypeID()))
        myEditedObjs.push_back(obj);
  }
  else
    myEditedObjs = objs;

  this->updateUI();
}


void FuiPositionData::updateUI()
{
  if (myEditedObjs.empty()) return;
  FmModelMemberBase* myEditedObj = myEditedObjs.front();

  FFa3DLocation loc;
  FmIsPositionedBase* posRef = NULL;
  FmIsPositionedBase* rotRef = NULL;
  FapUAQuery*         refQue = NULL;

  if (IAmEditingLinkCG)
  {
    FmPart* part = static_cast<FmPart*>(myEditedObj);
    loc    = part->getLocationCG();
    posRef = part->getCGPosRef();
    rotRef = part->getCGRotRef();
  }
  else if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
  {
    FmIsPositionedBase* obj = static_cast<FmIsPositionedBase*>(myEditedObj);
    loc    = obj->getLocation();
    posRef = obj->getPosRef();
    rotRef = obj->getRotRef();
  }
  else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
    loc = static_cast<FmAssemblyBase*>(myEditedObj)->getLocation();
  else
    return;

#ifdef FUI_DEBUG
  std::cout <<"FuiPositionData::updateUI: ";
  if (IAmEditingLinkCG) std::cout <<"CoG ";
  std::cout <<"Location for "<< myEditedObj->getIdString() << loc << std::endl;
#endif

  PosType posType = loc.getPosType();
  RotType rotType = loc.getRotType();

  size_t i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      myFields[3*i+j]->setValue(loc[i][j]);

  if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
  {
    static FapUAQuery query1;
    query1.typesToFind.emplace(FmIsPositionedBase::getClassTypeID(),true);
    refQue = &query1;
  }

  myPosRefCSField->setQuery(refQue);
  myPosRefCSField->setSelectedRef(posRef);
  myRotRefCSField->setQuery(refQue);
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

  this->setSensitivity(IAmEditable);

  if (loc.getNumFields() == 9)
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


void FuiPositionData::setSensitivity(bool onOff)
{
  IAmEditable = onOff;
  bool canTra = onOff;
  bool canRot = onOff;

  if (myEditedObjs.empty())
    canTra = canRot = false;
  else if (!IAmEditingLinkCG)
  {
    FmModelMemberBase* myEditedObj = myEditedObjs.front();
    if (myEditedObj->isOfType(FmIsPositionedBase::getClassTypeID()))
    {
      canTra = static_cast<FmIsPositionedBase*>(myEditedObj)->isTranslatable();
      canRot = static_cast<FmIsPositionedBase*>(myEditedObj)->isRotatable();
    }
    else if (myEditedObj->isOfType(FmAssemblyBase::getClassTypeID()))
      canTra = canRot = static_cast<FmAssemblyBase*>(myEditedObj)->isMovable();
  }

  for (int i = 0; i < 9; i++)
    myFields[i]->setSensitivity(i < 3 ? canTra : canRot);

  myMasterFollowToggle->setSensitivity(IAmEditable);
  mySlaveFollowToggle->setSensitivity(IAmEditable);
}


void FuiPositionData::onPosTypeChanged(int option)
{
  FFa3DLocation loc;
  bool changed = false;
  for (FmModelMemberBase* obj : myEditedObjs)
  {
    if (IAmEditingLinkCG)
      loc = static_cast<FmPart*>(obj)->getLocationCG();
    else if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
      loc = static_cast<FmIsPositionedBase*>(obj)->getLocation();
    else if (obj->isOfType(FmAssemblyBase::getClassTypeID()))
      loc = static_cast<FmAssemblyBase*>(obj)->getLocation();
    else
      continue;

#ifdef FUI_DEBUG
    std::cout <<"FuiPositionData::onPosTypeChanged: ";
    if (IAmEditingLinkCG) std::cout <<"CoG ";
    std::cout <<"Location for "<< obj->getIdString() << loc << std::endl;
#endif

    if (!loc.changePosType(PosTypeMapping::map()[option].first))
      continue; // not changed

#ifdef FUI_DEBUG
    std::cout <<"- updated to"<< loc << std::endl;
#endif

    if (IAmEditingLinkCG)
      static_cast<FmPart*>(obj)->setLocationCG(loc);
    else if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
      static_cast<FmIsPositionedBase*>(obj)->setLocation(loc);
    else
      static_cast<FmAssemblyBase*>(obj)->setLocation(loc);

    changed = true;
    if (obj->isOfType(FmIsRenderedBase::getClassTypeID()))
      static_cast<FmIsRenderedBase*>(obj)->updateDisplayCS();
  }

  if (changed)
    FpPM::touchModel();

  this->updateUI();
}


void FuiPositionData::onRotTypeChanged(int option)
{
  FFa3DLocation loc;
  bool changed = false;
  for (FmModelMemberBase* obj : myEditedObjs)
  {
    if (IAmEditingLinkCG)
      loc = static_cast<FmPart*>(obj)->getLocationCG();
    else if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
      loc = static_cast<FmIsPositionedBase*>(obj)->getLocation();
    else if (obj->isOfType(FmAssemblyBase::getClassTypeID()))
      loc = static_cast<FmAssemblyBase*>(obj)->getLocation();
    else
      continue;

#ifdef FUI_DEBUG
    std::cout <<"FuiPositionData::onRotTypeChanged: ";
    if (IAmEditingLinkCG) std::cout <<"CoG ";
    std::cout <<"Location for "<< obj->getIdString() << loc << std::endl;
#endif

    if (!loc.changeRotType(RotTypeMapping::map()[option].first))
      continue;

#ifdef FUI_DEBUG
    std::cout <<"- updated to"<< loc << std::endl;
#endif

    if (IAmEditingLinkCG)
      static_cast<FmPart*>(obj)->setLocationCG(loc);
    else if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
      static_cast<FmIsPositionedBase*>(obj)->setLocation(loc);
    else
      static_cast<FmAssemblyBase*>(obj)->setLocation(loc);

    changed = true;
    if (obj->isOfType(FmIsRenderedBase::getClassTypeID()))
      static_cast<FmIsRenderedBase*>(obj)->updateDisplayCS();
  }

  if (changed)
    FpPM::touchModel();

  this->updateUI();
}


void FuiPositionData::FieldChangedCB::onFieldAccepted(double value)
{
  FpPM::vpmSetUndoPoint("Position data");

  FFa3DLocation loc;
  bool changed = false;
  for (FmModelMemberBase* obj : owner->myEditedObjs)
  {
    if (owner->IAmEditingLinkCG)
      loc = static_cast<FmPart*>(obj)->getLocationCG();
    else if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
      loc = static_cast<FmIsPositionedBase*>(obj)->getLocation();
    else if (obj->isOfType(FmAssemblyBase::getClassTypeID()))
      loc = static_cast<FmAssemblyBase*>(obj)->getLocation();
    else
      continue;

#ifdef FUI_DEBUG
    std::cout <<"FuiPositionData::onFieldAccepted: ";
    if (owner->IAmEditingLinkCG) std::cout <<"CoG ";
    std::cout <<"Location for "<< obj->getIdString() << loc << std::endl;
#endif

    if (loc[i][j] == value)
      continue; // not changed

    loc[i][j] = value;
    if (!loc.isValid())
      continue; // ignore invalid input

#ifdef FUI_DEBUG
    std::cout <<"- updated to"<< loc << std::endl;
#endif

    if (owner->IAmEditingLinkCG)
      static_cast<FmPart*>(obj)->setLocationCG(loc);
    else if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
      static_cast<FmIsPositionedBase*>(obj)->setLocation(loc);
    else
      static_cast<FmAssemblyBase*>(obj)->setLocation(loc);

    if (obj->isOfType(FmPart::getClassTypeID()))
      static_cast<FmPart*>(obj)->updateDisplayTopology(); // Bugfix #493
    else if (obj->isOfType(FmIsRenderedBase::getClassTypeID()))
      static_cast<FmIsRenderedBase*>(obj)->draw();
    else if (obj->isOfType(FmAssemblyBase::getClassTypeID()))
    {
      FmDB::displayAll(*static_cast<FmAssemblyBase*>(obj)->getHeadMap());
      if (obj == owner->myEditedObjs.front())
        Fui::getProperties()->updateSubassCoG();
    }

#ifdef FT_HAS_WND
    FmNacelle* nac = NULL;
    FFuTopLevelShell* cta = NULL;
    if (static_cast<FmTurbine*>(obj) == FmDB::getTurbineObject())
      cta = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineAssembly::getClassTypeID());
    else if (owner->IAmEditingLinkCG &&
             (nac = dynamic_cast<FmNacelle*>(obj->getParentAssembly())))
    {
      // Update the nacelle CoG field in the wind turbine definition dialog
      nac->CoG.setValue(nac->toLocal(nac->getGlobalCoG(false)));
      cta = FFuTopLevelShell::getInstanceByType(FuiCreateTurbineAssembly::getClassTypeID());
    }
    if (cta) dynamic_cast<FuiCreateTurbineAssembly*>(cta)->updateUIValues();
#endif
  }

  if (changed)
    FpPM::touchModel();

  owner->updateUI();
}


void FuiPositionData::onPosRefChanged(FmModelMemberBase* obj)
{
  FmIsPositionedBase* posRefObj = dynamic_cast<FmIsPositionedBase*>(obj);

  bool changed = false;
  for (FmModelMemberBase* eobj : myEditedObjs)
    if (IAmEditingLinkCG)
      changed |= static_cast<FmPart*>(eobj)->setCGPosRef(posRefObj);
    else if (eobj->isOfType(FmIsPositionedBase::getClassTypeID()))
      changed |= static_cast<FmIsPositionedBase*>(eobj)->setPosRef(posRefObj);

  if (changed)
    FpPM::touchModel();

  this->updateUI();
}


void FuiPositionData::onRotRefChanged(FmModelMemberBase* obj)
{
  FmIsPositionedBase* rotRefObj = dynamic_cast<FmIsPositionedBase*>(obj);

  bool changed = false;
  for (FmModelMemberBase* eobj : myEditedObjs)
    if (IAmEditingLinkCG)
      changed |= static_cast<FmPart*>(eobj)->setCGRotRef(rotRefObj);
    else if (eobj->isOfType(FmIsPositionedBase::getClassTypeID()))
      changed |= static_cast<FmIsPositionedBase*>(eobj)->setRotRef(rotRefObj);

  if (changed)
    FpPM::touchModel();

  this->updateUI();
}


void FuiPositionData::onTriadsFollowToggled(bool)
{
  FmSMJointBase* joint = NULL;
  bool changed = false;
  for (FmModelMemberBase* obj : myEditedObjs)
    if ((joint = dynamic_cast<FmSMJointBase*>(obj)))
      if (joint->setMasterMovedAlong(myMasterFollowToggle->getValue()) ||
          joint->setSlaveMovedAlong(mySlaveFollowToggle->getValue()))
      changed = true;

  if (changed)
    FpPM::touchModel();

  this->updateUI();
}


void FuiPositionData::prepareRefCSSelection(char posOrRot)
{
  IAmDoingRefCSSelection = posOrRot;

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
  if (!IAmDoingRefCSSelection) return;

  FmIsPositionedBase* candidate = NULL;
  for (FFaViewItem* item : totalSelection)
    if ((candidate = dynamic_cast<FmIsPositionedBase*>(item)))
      break;

  if (!candidate) return;

  if (IAmDoingRefCSSelection == 'p')
    this->onPosRefChanged(candidate);
  else if (IAmDoingRefCSSelection == 'r')
    this->onRotRefChanged(candidate);
}


void FuiPositionData::finishRefCSSelection()
{
  IAmDoingRefCSSelection = false;

  FapEventManager::popPermSelection();

  FapUAProperties* uap = FapUAProperties::getPropertiesHandler();
  if (uap) uap->setIgnorePickNotify(false);
}


void FuiPositionData::onPoppedUp()
{
#ifdef USE_INVENTOR
  if (myEditedObjs.empty()) return;
  FmModelMemberBase* myEditedObj = myEditedObjs.front();

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
  std::cout <<"FuiPositionData::onPoppedUp: ";
  if (IAmEditingLinkCG) std::cout <<"CoG ";
  std::cout <<"Location for "<< myEditedObj->getIdString() << loc << std::endl;
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
