// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAProperties.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmApp/vpmAppUAMap/FapUACurveDefine.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/vpmAppProcess/FapLinkReducer.H"
#include "vpmApp/vpmAppCmds/FapFileCmds.H"
#include "vpmApp/vpmAppCmds/FapStrainRosetteCmds.H"
#include "vpmApp/vpmAppProcess/FapSolutionProcessMgr.H"
#include "vpmApp/vpmAppProcess/FapSimEventHandler.H"

#include "vpmUI/Fui.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuFileDialog.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmFuncAdmin.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmAxialDamper.H"
#include "vpmDB/FmAxialSpring.H"
#include "vpmDB/FmSpringChar.H"
#include "vpmDB/FmJointSpring.H"
#include "vpmDB/FmJointDamper.H"
#include "vpmDB/FmJointMotion.H"
#include "vpmDB/FmDofMotion.H"
#include "vpmDB/FmDofLoad.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLoad.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmUserDefinedElement.H"
#include "vpmDB/FmHPBase.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmBallJoint.H"
#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmPrismJoint.H"
#include "vpmDB/FmRigidJoint.H"
#include "vpmDB/FmRevJoint.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmBearingFriction.H"
#include "vpmDB/FmPrismaticFriction.H"
#include "vpmDB/FmCamFriction.H"
#include "vpmDB/FmRefPlane.H"
#include "vpmDB/FmSticker.H"
#include "vpmDB/FmCtrlLine.H"
#include "vpmDB/FmEngine.H"
#include "vpmDB/FmAnimation.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmGenericDBObject.H"
#ifdef FT_HAS_EXTCTRL
#include "vpmDB/FmExternalCtrlSys.H"
#endif
#include "vpmDB/FmFileReference.H"
#include "vpmDB/FmTire.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmcInput.H"
#include "vpmDB/FmcOutput.H"
#include "vpmDB/FmPipeSurface.H"
#include "vpmDB/FmVesselMotion.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmBladeProperty.H"
#include "vpmDB/FmMaterialProperty.H"
#include "vpmDB/FmBeamProperty.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmRiser.H"
#include "vpmDB/FmJacket.H"
#include "vpmDB/FmSoilPile.H"
#include "vpmDB/FmSeaState.H"
#include "vpmDB/FmRingStart.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdPickedPoints.H"
#endif
#include "vpmPM/FpPM.H"
#include "vpmPM/FpFileSys.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaAlgebra/FFaMath.H"

#include <algorithm>
#include <cctype>


Fmd_SOURCE_INIT(FAPUAPROPERTIES, FapUAProperties, FapUAExistenceHandler);


FapUAProperties::FapUAProperties(FuiProperties* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic),
    permSignalConnector(this), modelMemberChangedConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUAProperties);

  IAmIgnoringPickNotify = IAmIgnoringPickNotifyNotCurves = false;

  mySelectedFmItem = NULL;
  myPropertiesUI   = uic;

  // Sending the non-changing call-backs into the UI

  FuaPropertiesValues pv;

  pv.myEditButtonCB           = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);

  pv.myTopologyHighlightCB    = FFaDynCB2M(FapUAProperties, this, topologyHighlightCB, int, bool);
  pv.myTopologyActivatedCB    = FFaDynCB1M(FapUAProperties, this, topologyActivatedCB, int);
  pv.myTopologyRightClickedCB = FFaDynCB2M(FapUAProperties, this, onTopViewRightClick, int, std::vector<FFuaCmdItem*>&);

  pv.myAxialDaForceValues.myFunctionQIFieldButtonCB  = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
  pv.myAxialDaForceValues.myEngineQIFieldButtonCB    = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);

  pv.myAxialSprForceValues.myFunctionQIFieldButtonCB = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
  pv.myAxialSprForceValues.myEngineQIFieldButtonCB   = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);

  pv.myAxialSpringDefCalcValues.myQIFieldButtonCB    = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);

  pv.mySpringCharValues.springFunctionFieldButtonCB  = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
  pv.mySpringCharValues.yieldForceFieldMaxButtonCB   = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
  pv.mySpringCharValues.yieldForceFieldMinButtonCB   = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);

  pv.myFlipZCB         = FFaDynCB0S(FapStrainRosetteCmds::flipStrainRosetteZDirection);
  pv.myEditNodesCB     = FFaDynCB0S([](){ FuiModes::setMode(FuiModes::EDITSTRAINROSETTENODES_MODE); });
  pv.myEditDirectionCB = FFaDynCB0S([](){ FuiModes::setMode(FuiModes::EDITSTRAINROSETTEDIR_MODE); });

  pv.myLinkValues.linkMeshCB      = FFaDynCB1M(FapUAProperties, this, linkMeshCB, bool);
  pv.myLinkValues.linkChangeCB    = FFaDynCB0M(FapUAProperties, this, linkChangeCB);
  pv.myLinkValues.linkChangeVizCB = FFaDynCB2M(FapUAProperties, this, linkChangeVizCB, const std::string&, bool);

  pv.myTriadVals.resize(6);
  for (FuiTriadDOFValues& tdof : pv.myTriadVals)
    tdof.myLoadVals.myEngineQIFieldButtonCB = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);

  pv.myJointVals.resize(6);
  for (FuiJointDOFValues& jdof : pv.myJointVals)
  {
    jdof.myLoadVals.myEngineQIFieldButtonCB       = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
    jdof.mySpringDCVals.myQIFieldButtonCB         = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
    jdof.mySpringFSVals.myFunctionQIFieldButtonCB = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
    jdof.mySpringFSVals.myEngineQIFieldButtonCB   = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
    jdof.myDamperFCVals.myFunctionQIFieldButtonCB = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
    jdof.myDamperFCVals.myEngineQIFieldButtonCB   = FFaDynCB1M(FapUAProperties, this, onQIFieldButtonCB, FuiQueryInputFieldValues&);
  }

  pv.myDofStatusCB = FFaDynCB2M(FapUAProperties,this,dofStatusToggledCB,int,int);
  pv.mySwapTriadCB = FFaDynCB0M(FapUAProperties,this,swapMasterSlaveCB);
  pv.myAddMasterCB = FFaDynCB0M(FapUAProperties,this,addMasterCB);
  pv.myRevMasterCB = FFaDynCB0M(FapUAProperties,this,reverseMasterCB);

  pv.myDofSetAllFreeCB = FFaDynCB0M(FapUAProperties,this,dofSetAllFree);
  pv.myDofSetAllFixedCB = FFaDynCB0M(FapUAProperties,this,dofSetAllFixed);
  pv.mySwapJointCB = FFaDynCB0M(FapUAProperties,this,swapJointCB);

  pv.myLoadViewAttackPointCB = FFaDynCB1M(FapUAProperties,this,loadViewAttackPointCB,bool);
  pv.myLoadViewAttackWhatCB  = FFaDynCB1M(FapUAProperties,this,loadViewAttackWhatCB,bool);
  pv.myLoadPickAttackPointCB = FFaDynCB0M(FapUAProperties,this,loadPickAttackPointCB);
  pv.myLoadApplyCB           = FFaDynCB2M(FapUAProperties,this,applyAttackPointCB,bool,FaVec3);

  pv.myLoadViewFromPointCB = FFaDynCB1M(FapUAProperties,this,loadViewFromPointCB,bool);
  pv.myLoadViewFromWhatCB  = FFaDynCB1M(FapUAProperties,this,loadViewFromWhatCB,bool);
  pv.myLoadPickFromPointCB = FFaDynCB0M(FapUAProperties,this,loadPickFromPointCB);

  pv.myLoadViewToPointCB = FFaDynCB1M(FapUAProperties,this,loadViewToPointCB,bool);
  pv.myLoadViewToWhatCB  = FFaDynCB1M(FapUAProperties,this,loadViewToWhatCB,bool);
  pv.myLoadPickToPointCB = FFaDynCB0M(FapUAProperties,this,loadPickToPointCB);

  pv.myBrowseTireFileCB = FFaDynCB0M(FapUAProperties,this,tireDataFileBrowseCB);
  pv.myBrowseRoadFileCB = FFaDynCB0M(FapUAProperties,this,roadDataFileBrowseCB);
  pv.myBrowseRAOFileCB  = FFaDynCB0M(FapUAProperties,this,raoFileBrowseCB);

  pv.mySimEventCB = FFaDynCB0M(FapUAProperties,this,simEventSelectedCB);

  uic->setCBs(&pv);
}


FapUAProperties* FapUAProperties::getPropertiesHandler()
{
  return dynamic_cast<FapUAProperties*>(FapUAExistenceHandler::getFirstOfType(FapUAProperties::getClassTypeID()));
}


FFuaUIValues* FapUAProperties::createValuesObject()
{
  FuaPropertiesValues* vals = new FuaPropertiesValues();

  // Preserve the Global/Local Reference settings when changing the selection,
  // but only if the current model has load objects
  if (myPropertiesUI && FmDB::hasObjectsOfType(FmLoad::getClassTypeID()))
    myPropertiesUI->getLoadRefSettings(vals);

  return vals;
}


////////////////////////////////////////
//
// Getting values from DB :
//
//////////

void FapUAProperties::getDBValues(FFuaUIValues* values)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAProperties::getDBValues(): "
            << (mySelectedFmItem ? mySelectedFmItem->getIdString() :"(NULL)")
            << std::endl;
#endif
  FuaPropertiesValues* pv = dynamic_cast<FuaPropertiesValues*> (values);
  if (!pv) return;

  if ((pv->showStartGuide = (mySelectedFmItem == NULL)))
    return;

  // Lambda function filtering out non-positioned objects.
  auto&& isPositioned = [](FmModelMemberBase* obj)
  {
    if (obj->isOfType(FmAssemblyBase::getClassTypeID())) return true;
    if (!obj->isOfType(FmIsPositionedBase::getClassTypeID())) return false;
    return !obj->isOfType(FmMMJointBase::getClassTypeID());
  };

  if (isPositioned(mySelectedFmItem))
  {
    // Find all selected positioned objects
    pv->objsToPosition.reserve(mySelectedFmItems.size());
    pv->objsToPosition = { mySelectedFmItem };
    for (FmModelMemberBase* item : mySelectedFmItems)
      if (item && item != mySelectedFmItem)
        if (isPositioned(item))
          pv->objsToPosition.push_back(item);
  }
  else
    pv->objsToPosition.clear();

  myTopologyViewList.clear();

  // Heading
  // Set type, id, description and tag fields

  pv->myType = mySelectedFmItem->getUITypeName();
  pv->myId = mySelectedFmItem->getID();
  if (!mySelectedFmItem->isOfType(FmRingStart::getClassTypeID()))
  {
    pv->showHeading = true;
    pv->myDescription = mySelectedFmItem->getUserDescription();
    pv->myTag = mySelectedFmItem->getTag();
  }

  // Damper

  if (mySelectedFmItem->isOfType(FmDamperBase::getClassTypeID()))
    {
      FmDamperBase* item = (FmDamperBase*)mySelectedFmItem;

      pv->showAxialDamper = true;

      static FapUAQuery funcQuery;
      funcQuery.clear();

      for (int ftype : FmFuncAdmin::getAllowableSprDmpFuncTypes())
	funcQuery.typesToFind[ftype] = true;

      FmSpringBase* parallelSpring = NULL;

      if (item->isOfType(FmAxialDamper::getClassTypeID())) {
	funcQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransDamperFunction,bool&,FmModelMemberBase*);

	// Find possible parallel spring
	int dmpTriad1 = ((FmAxialDamper*)item)->getFirstTriad()->getID();
	int dmpTriad2 = ((FmAxialDamper*)item)->getSecondTriad()->getID();

	std::vector<FmAxialSpring*> axialSprings;
	FmDB::getAllAxialSprings(axialSprings);
	for (FmAxialSpring* spring : axialSprings) {
	  int sprTriad1 = spring->getFirstTriad()->getID();
	  int sprTriad2 = spring->getSecondTriad()->getID();
	  if ( (dmpTriad1 == sprTriad1 || dmpTriad1 == sprTriad2) &&
	       (dmpTriad2 == sprTriad1 || dmpTriad2 == sprTriad2) ) {
	    parallelSpring = spring;
	    break;
	  }
	}
      }

      else if (item->isOfType(FmJointDamper::getClassTypeID())) {
	int dof = ((FmJointDamper*)item)->getDOF();
	if (dof < 3)
	  funcQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransDamperFunction,bool&,FmModelMemberBase*);
	else
	  funcQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyRotDamperFunction,bool&,FmModelMemberBase*);

	// Get parallel spring
	parallelSpring = ((FmJointDamper*)item)->getOwnerJoint()->getSpringAtDOF(dof);
      }

      if (parallelSpring)
        pv->myAxialDaForceValues.showDefDamper = parallelSpring->getLengthEngine() ? 1 : -1;
      else
        pv->myAxialDaForceValues.showDefDamper = 0;

      pv->myAxialDaForceValues.isDefDamper = (pv->myAxialDaForceValues.showDefDamper > 0 &&
                                              item->isDefDamper.getValue());

      pv->myAxialDaForceValues.functionQuery    = &funcQuery;
      pv->myAxialDaForceValues.selectedFunction = item->getFunction();
      pv->myAxialDaForceValues.constFunction    = item->getInitDamp();

      pv->myAxialDaForceValues.engineQuery         = FapUAEngineQuery::instance();
      pv->myAxialDaForceValues.selectedScaleEngine = item->getDampEngine();

      // Topology view:

      if (item->isOfType(FmAxialDamper::getClassTypeID()))
        {
          FmTriad* t = ((FmAxialDamper*)item)->getFirstTriad();
          this->addTopologyItem(pv->myTopology,t,0,"First");
          if (t) this->addTopologyItem(pv->myTopology,t->getOwnerLink(0),1,"First");

          t = ((FmAxialDamper*)item)->getSecondTriad();
          this->addTopologyItem(pv->myTopology,t,0,"Second");
          if (t) this->addTopologyItem(pv->myTopology,t->getOwnerLink(0),1,"Second");
        }
      else if (item->isOfType(FmJointDamper::getClassTypeID()))
        {
          this->addTopologyItem(pv->myTopology,((FmJointDamper*)item)->getOwnerJoint());
        }
    }

  // Spring

  else if (mySelectedFmItem->isOfType(FmSpringBase::getClassTypeID()))
    {
      FmSpringBase* item = (FmSpringBase*)mySelectedFmItem;

      pv->showAxialSpring = true;

      // Deflection calculation

      pv->myAxialSpringDefCalcValues.variable             = item->getModelSpringLength();

      pv->myAxialSpringDefCalcValues.useAsDeflection      = item->getInitLengthOrDefl(pv->myAxialSpringDefCalcValues.initLengtOrDeflection);

      pv->myAxialSpringDefCalcValues.engineQuery          = FapUAEngineQuery::instance();
      pv->myAxialSpringDefCalcValues.selectedLengthEngine = item->getLengthEngine();

      // Stiffness

      static FapUAQuery query2;
      query2.clear();

      for (int ftype : FmFuncAdmin::getAllowableSprDmpFuncTypes())
	query2.typesToFind[ftype] = true;
      query2.typesToFind[FmSpringChar::getClassTypeID()] = true;

      if (item->isOfType(FmAxialSpring::getClassTypeID()))
	query2.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransSpringFunction,bool&,FmModelMemberBase*);
      else if (item->isOfType(FmJointSpring::getClassTypeID())) {
	if (((FmJointSpring*)item)->getDOF() < 3)
	  query2.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransSpringFunction,bool&,FmModelMemberBase*);
	else {
	  query2.verifyCB = FFaDynCB2S(FapUAProperties::verifyRotSpringFunction,bool&,FmModelMemberBase*);
	  pv->myAxialSpringDefCalcValues.useAngularLabels = true;
	}
      }

      pv->myAxialSprForceValues.functionQuery       = &query2;
      pv->myAxialSprForceValues.selectedFunction    = item->getSpringCharOrStiffFunction();
      pv->myAxialSprForceValues.constFunction       = item->getInitStiff();

      pv->myAxialSprForceValues.engineQuery         = FapUAEngineQuery::instance();
      pv->myAxialSprForceValues.selectedScaleEngine = item->getScaleEngine();

      // Topology view:

      if (item->isOfType(FmAxialSpring::getClassTypeID()))
        {
          FmTriad* t = ((FmAxialSpring*)item)->getFirstTriad();
          this->addTopologyItem(pv->myTopology,t,0,"First");
          if (t) this->addTopologyItem(pv->myTopology,t->getOwnerLink(0),1,"First");

          t = ((FmAxialSpring*)item)->getSecondTriad();
          this->addTopologyItem(pv->myTopology,t,0,"Second");
          if (t) this->addTopologyItem(pv->myTopology,t->getOwnerLink(0),1,"Second");
        }
      else if (item->isOfType(FmJointSpring::getClassTypeID()))
        {
          this->addTopologyItem(pv->myTopology,((FmJointSpring*)item)->getOwnerJoint());
        }
    }

  // Spring Characteristics

  else if (mySelectedFmItem->isOfType(FmSpringChar::getClassTypeID()))
    {
      FmSpringChar* sprChar = (FmSpringChar*)mySelectedFmItem;

      pv->showSpringChar = true;

      pv->mySpringCharValues.isTranslationalSpring = (sprChar->getSpringCharUse() == FmSpringChar::TRANSLATION);

      static FapUAQuery funcQuery;
      funcQuery.clear();

      for (int ftype : FmFuncAdmin::getAllowableSprDmpFuncTypes())
	funcQuery.typesToFind[ftype] = true;
      if (sprChar->getSpringCharUse() == FmSpringChar::TRANSLATION)
	funcQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransSpringFunction,bool&,FmModelMemberBase*);
      else if (sprChar->getSpringCharUse() == FmSpringChar::ROTATION)
	funcQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyRotSpringFunction,bool&,FmModelMemberBase*);

      pv->mySpringCharValues.springFunctionQuery = &funcQuery;
      pv->mySpringCharValues.springFunction      = sprChar->springFunction.getPointer();
      pv->mySpringCharValues.constantStiffness   = sprChar->springStiffness.getValue();

      pv->mySpringCharValues.deflectionMax    = sprChar->deflectionMax.getValue();
      pv->mySpringCharValues.deflectionMin    = sprChar->deflectionMin.getValue();
      pv->mySpringCharValues.forceMax         = sprChar->forceMax.getValue();
      pv->mySpringCharValues.forceMin         = sprChar->forceMin.getValue();
      pv->mySpringCharValues.useDeflectionMax = sprChar->deflectionMaxIsOn.getValue();
      pv->mySpringCharValues.useDeflectionMin = sprChar->deflectionMinIsOn.getValue();
      pv->mySpringCharValues.useForceMax      = sprChar->forceMaxIsOn.getValue();
      pv->mySpringCharValues.useForceMin      = sprChar->forceMinIsOn.getValue();

      pv->mySpringCharValues.yieldForceEngineQuery = FapUAEngineQuery::instance();
      pv->mySpringCharValues.yieldForceMaxEngine   = sprChar->yieldForceMaxEngine.getPointer();
      pv->mySpringCharValues.yieldForceMinEngine   = sprChar->yieldForceMinEngine.getPointer();
      pv->mySpringCharValues.useYieldForceMax      = sprChar->yieldForceMaxIsOn.getValue();
      pv->mySpringCharValues.useYieldForceMin      = sprChar->yieldForceMinIsOn.getValue();
      pv->mySpringCharValues.constantYieldForceMax = sprChar->yieldForceMax.getValue();
      pv->mySpringCharValues.constantYieldForceMin = sprChar->yieldForceMin.getValue();
      pv->mySpringCharValues.useYieldDeflectionMax = sprChar->yieldDeflectionMaxIsOn.getValue();
      pv->mySpringCharValues.yieldDeflectionMax    = sprChar->yieldDeflectionMax.getValue();

      // Topology view:

      std::vector<FmModelMemberBase*> refs, joints;
      sprChar->getReferringObjs(refs,"mySpringChar");
      for (FmModelMemberBase* refObj : refs)
        if (refObj->isOfType(FmJointSpring::getClassTypeID()))
        {
          // Make sure joints using this in several DOFs are added only once
          FmModelMemberBase* owner = ((FmJointSpring*)refObj)->getOwnerJoint();
          if (std::find(joints.begin(),joints.end(),owner) == joints.end())
          {
            this->addTopologyItem(pv->myTopology,owner);
            joints.push_back(owner);
          }
        }
        else
          this->addTopologyItem(pv->myTopology,refObj);
    }

  // Triad

  else if (mySelectedFmItem->isOfType(FmTriad::getClassTypeID()))
    {
      FmTriad* item = (FmTriad*)mySelectedFmItem;

      pv->showTriadData = true;

      pv->isSlave = item->isSlaveTriad(true);
      pv->isMaster = item->isMasterTriad();
      pv->myTriadIsAttached = item->isAttached();
      pv->myTriadConnector = item->itsConnectorType.getValue();
      pv->myFENodeIdx = pv->myTriadIsAttached ? std::to_string(item->FENodeNo.getValue()) : "N/A";

      int dof, nDOFs = item->getNDOFs(true);
      pv->myTriadVals.resize(nDOFs);
      for (dof = 0; dof < nDOFs; dof++) {
	FuiTriadDOFValues& tv = pv->myTriadVals[dof];
	tv.myMotionType = item->getStatusOfDOF(dof);
	tv.myInitVel = item->getInitVel(dof,true);
	tv.myLoadVals.constValue = 0.0;
	tv.myLoadVals.selectedEngine = NULL;
	if (tv.myMotionType == FmHasDOFsBase::FREE ||
	    tv.myMotionType == FmHasDOFsBase::FREE_DYNAMICS) {
	  if (FmDofLoad* load = item->getLoadAtDOF(dof); load) {
	    tv.myLoadVals.constValue     = load->getInitLoad();
	    tv.myLoadVals.selectedEngine = load->getEngine();
	    tv.freqDomain = load->freqDomain.getValue();
	  }
	  tv.myLoadVals.isConstant = (tv.myLoadVals.selectedEngine ? false : true);
	  tv.myLoadVals.engineQuery = FapUAEngineQuery::instance();
	}
	else if (tv.myMotionType == FmHasDOFsBase::PRESCRIBED) {
	  if (FmDofMotion* pm = item->getMotionAtDOF(dof); pm) {
	    tv.myLoadVals.constValue     = pm->getInitMotion();
	    tv.myLoadVals.selectedEngine = pm->getEngine();
	    tv.freqDomain   = pm->freqDomain.getValue();
	    tv.myMotionType = FuiTriadDOF::PRESCRIBED_DISP + pm->getMotionType();
	  }
	  tv.myLoadVals.isConstant = (tv.myLoadVals.selectedEngine ? false : true);
	  tv.myLoadVals.engineQuery = FapUAEngineQuery::instance();
	}
      }

      pv->mySysDir = item->itsLocalDir.getValue();
      pv->myMass[0] = item->getAddMass();
      for (dof = 1; dof <= 3; dof++)
        pv->myMass[dof] = item->getAddMass(2+dof);

      pv->myResToggles = item->mySaveVar.getValue();

      // Topology view:

      size_t i = 0;
      FmPart* owner;
      while ((owner = item->getOwnerPart(i++)))
	this->addTopologyItem(pv->myTopology,owner);

      std::vector<FmLink*> elms;
      item->getElementBinding(elms);
      for (i = 0; i < elms.size(); i++)
	this->addTopologyItem(pv->myTopology,elms[i]);

      std::vector<FmJointBase*> joints;
      item->getJointBinding(joints);
      for (i = 0; i < joints.size(); i++)
	this->addTopologyItem(pv->myTopology,joints[i]);

      std::vector<FmAxialSpring*> springs;
      item->getSpringBinding(springs);
      for (i = 0; i < springs.size(); i++)
	this->addTopologyItem(pv->myTopology,springs[i]);

      std::vector<FmAxialDamper*> dampers;
      item->getDamperBinding(dampers);
      for (i = 0; i < dampers.size(); i++)
	this->addTopologyItem(pv->myTopology,dampers[i]);

      std::vector<FmLoad*> loads;
      item->getLoadBinding(loads);
      for (i = 0; i < loads.size(); i++)
	this->addTopologyItem(pv->myTopology,loads[i]);

      std::vector<FmDofLoad*> dloads;
      item->getLoadBinding(dloads);
      for (i = 0; i < dloads.size(); i++)
	this->addTopologyItem(pv->myTopology,dloads[i]);

      std::vector<FmDofMotion*> motions;
      item->getMotionBinding(motions);
      for (i = 0; i < motions.size(); i++)
	this->addTopologyItem(pv->myTopology,motions[i]);

      std::vector<FmSticker*> stickers;
      item->getStickers(stickers);
      for (i = 0; i < stickers.size(); i++)
	this->addTopologyItem(pv->myTopology,stickers[i]);
    }

  // Pipe Surface

  else if (mySelectedFmItem->isOfType(FmPipeSurface::getClassTypeID()))
    {
      FmPipeSurface* item = (FmPipeSurface*)mySelectedFmItem;

      pv->showPipeSurfaceData = true;

      pv->pipeSurfaceRadius = item->getPipeRadius();

      // Topology view:

      std::vector<FmTriad*> triads;
      item->getTriads(triads);
      for (FmTriad* triad : triads)
	this->addTopologyItem(pv->myTopology,triad);
    }

  // Load

  else if (mySelectedFmItem->isOfType(FmLoad::getClassTypeID()))
    {
      FmLoad* item = (FmLoad*)mySelectedFmItem;

      pv->showLoadData = true;

      pv->myLoadMagnitude = item->getInitLoad();
      pv->myLoadEngineQuery = FapUAEngineQuery::instance();
      pv->mySelectedLoadMagnitudeEngine = item->getEngine();

      if (pv->myAttackPointIsGlobal)
        pv->myAttackPoint = item->getOwnerTriad()->getGlobalTranslation();
      else
        pv->myAttackPoint = item->getOwnerTriad()->getLocalTranslation();

      pv->myAttackObjectText = item->getOwnerTriad()->getLinkIDString(true);

      if (pv->myFromPointIsGlobal)
        pv->myFromPoint = item->getGlobalFromPoint();
      else
        pv->myFromPoint = item->getLocalFromPoint();

      if (item->getFromRef())
        pv->myFromPointObjectText = item->getFromRef()->getLinkIDString(true);

      if (pv->myToPointIsGlobal)
        pv->myToPoint = item->getGlobalToPoint();
      else
        pv->myToPoint = item->getLocalToPoint();

      if (item->getToRef())
        pv->myToPointObjectText = item->getToRef()->getLinkIDString(true);

      // Topology view:

      FmTriad* t = item->getOwnerTriad();
      this->addTopologyItem(pv->myTopology,t);
      if (t) this->addTopologyItem(pv->myTopology,t->getOwnerLink(0),1);
    }

  // Higher Pairs

  else if (mySelectedFmItem->isOfType(FmHPBase::getClassTypeID()))
    {
      FmHPBase* item = (FmHPBase*)mySelectedFmItem;

      pv->showHPRatio = true;

      pv->myHPRatio = item->getTransmissionRatio();

      // Topology view:

      FmJointBase* rj = item->getInputJoint();
      this->addTopologyItem(pv->myTopology,rj,0,"Input:");
      this->addJointDescendantTopology(pv->myTopology,rj,1);

      FmJointBase* pj = item->getOutputJoint();
      this->addTopologyItem(pv->myTopology,pj,0,"Output:");
      this->addJointDescendantTopology(pv->myTopology,pj,1);
    }

  // Joints

  else if (mySelectedFmItem->isOfType(FmJointBase::getClassTypeID()))
    {
      FmJointBase* item = (FmJointBase*)mySelectedFmItem;

      if (item->getValidFrictionType() > 0)
        {
          pv->showFriction = (item->isOfType(FmBallJoint::getClassTypeID()) ||
                              item->isOfType(FmFreeJoint::getClassTypeID())) ? 2 : 1;
          pv->myFrictionDof = item->getFrictionDof();
          int validFriction = item->getValidFrictionType(pv->myFrictionDof);

          static FapUAQuery query;
          query.clear();

          query.typesToFind[validFriction] = true;
          if (!item->isOfType(FmRevJoint::getClassTypeID()))
            query.typesToFind[FmBearingFriction::getClassTypeID()] = false;
          if (!item->isOfType(FmPrismJoint::getClassTypeID()))
            query.typesToFind[FmPrismaticFriction::getClassTypeID()] = false;
          if (!item->isOfType(FmCamJoint::getClassTypeID()))
            query.typesToFind[FmCamFriction::getClassTypeID()] = false;

          pv->myFrictionQuery = &query;

          FmFrictionBase* selectedFriction = item->getFriction();
          if (selectedFriction)
            if (!selectedFriction->isOfType(validFriction))
              selectedFriction = NULL;

          pv->mySelectedFriction = selectedFriction;
        }

      if (item->isOfType(FmCylJoint::getClassTypeID()))
        {
          pv->showScrew = true;
          pv->myIsScrewConnection = ((FmCylJoint*)item)->isScrewTransmission();
          pv->myScrewRatio = ((FmCylJoint*)item)->getScrewRatio();
        }

      if (item->isOfType(FmCamJoint::getClassTypeID()))
        {
          pv->showCamData = true;
          pv->myCamThickness = ((FmCamJoint*)item)->getThickness();
          pv->myCamWidth = ((FmCamJoint*)item)->getWidth();
          pv->IAmRadialContact = ((FmCamJoint*)item)->isUsingRadialContact();
        }
      else if (item->isOfType(FmMMJointBase::getClassTypeID()))
        {
          FmPart* masterPart = item->getMasterPart();
          if (masterPart)
            pv->showAddMasterButton = !masterPart->isEarthLink();
          pv->showReverseMasterButton = item->isMasterAttachedToLink(true);
        }
      else if (item->isOfType(FmSMJointBase::getClassTypeID()))
	{
	  pv->showSwapTriadButton = true;
	}

      if (item->isOfType(FmFreeJoint::getClassTypeID()))
	{
	  pv->showJointData = 2;
	  pv->showRotFormulation = true;
	  pv->showSpringCpl = true;
	  const Strings& rotFormulationTypes = FmJointBase::getRotFormulationUINames();
	  pv->myRotFormulationTypes = rotFormulationTypes;
	  pv->mySelectedRotFormulation = item->rotFormulation.getValue();

	  const Strings& rotSequenceTypes = FmJointBase::getRotSequenceUINames();
	  pv->myRotSequenceTypes = rotSequenceTypes;
	  pv->mySelectedRotSequence = item->rotSequence.getValue();

	  const Strings& springCplTypes = FmJointBase::getSpringCplUINames();
	  pv->mySpringCplTypes = springCplTypes;
	  pv->mySelectedRotSpringCpl = item->rotSpringCpl.getValue();
	  pv->mySelectedTranSpringCpl = item->tranSpringCpl.getValue();
	}
      else if (item->isOfType(FmBallJoint::getClassTypeID()))
	{
	  pv->showJointData = 2;
	  pv->showRotFormulation = true;
	  pv->showSpringCpl = 2;
	  const Strings& rotFormulationTypes = FmJointBase::getRotFormulationUINames();
	  pv->myRotFormulationTypes = rotFormulationTypes;
	  pv->mySelectedRotFormulation = item->rotFormulation.getValue();

	  const Strings& rotSequenceTypes = FmJointBase::getRotSequenceUINames();
	  pv->myRotSequenceTypes = rotSequenceTypes;
	  pv->mySelectedRotSequence = item->rotSequence.getValue();

	  const Strings& springCplTypes = FmJointBase::getSpringCplUINames();
	  pv->mySpringCplTypes = springCplTypes;
	  pv->mySelectedRotSpringCpl = item->rotSpringCpl.getValue();
	  pv->mySelectedTranSpringCpl = item->tranSpringCpl.getValue();
	}
      else if (item->isOfType(FmRevJoint::getClassTypeID()))
	{
	  pv->showJointData = 2;
	  pv->showDOF_TZ_Toggle = true;
	  pv->myIsDOF_TZ_legal = item->isLegalDOF(FmJointBase::Z_TRANS);
	}
      else if (item->isOfType(FmPrismJoint::getClassTypeID()))
        pv->showJointData = 3;
      else if (item->isOfType(FmCylJoint::getClassTypeID()))
        pv->showJointData = 4;
      else
        pv->showJointData = 1;

      this->getDBJointVariables(item, pv->myJointVals);

      bool isSprDmp = false;
      IntVec dofs;
      item->getDOFs(dofs);
      for (int dof : dofs)
        if (item->getStatusOfDOF(dof) >= FmHasDOFsBase::SPRING_CONSTRAINED)
          isSprDmp = true;

      pv->myResToggles = item->mySaveVar.getValue();
      if (!isSprDmp) pv->myResToggles.resize(5);

      // Topology view:

      this->addJointDescendantTopology(pv->myTopology,item);
    }

  // Beam

  else if (mySelectedFmItem->isOfType(FmBeam::getClassTypeID()))
    {
      FmBeam* item = (FmBeam*)mySelectedFmItem;

      pv->showBeamData = true;

      pv->mySelectedCS   = item->getProperty();
      pv->myCrossSectionQuery = FapUABeamPropQuery::instance();
      pv->myTotalLength  = item->getLength();
      pv->myTotalMass    = item->getMass();
      pv->myVisualize3D  = item->myVisualize3D.getValue();
      pv->myVisualize3DAngles = item->myVisualize3DAngles.getValue();
      pv->myMassPropDamp = item->alpha1.getValue();
      pv->myStifPropDamp = item->alpha2.getValue();
      pv->myScaleMass    = item->massScale.getValue();
      pv->myScaleStiff   = item->stiffnessScale.getValue();
      pv->myOrientation  = item->myLocalZaxis.getValue();
      pv->myBlankFieldsFlags = 0x0000; // always show all

      // Blade property? Hide cross-section combo box.
      pv->myHideCrossSection = dynamic_cast<FmBladeProperty*>(pv->mySelectedCS) != NULL;

      // Topology view:

      this->addTriadTopology(pv->myTopology,item);

      if (item->getProperty())
	this->addTopologyItem(pv->myTopology,item->getProperty(),0,"Element property");
    }

  // Part

  else if (mySelectedFmItem->isOfType(FmPart::getClassTypeID()))
    {
      FmPart* item = (FmPart*)mySelectedFmItem;

      pv->showLinkData = true;

      pv->myLinkValues.locked = item->lockLevel.getValue() == FmPart::FM_DENY_ALL_LINK_MOD;

      pv->myLinkValues.reducedVersionNumber = FapLinkReducer::isReduced(item) ? item->myRSD.getValue().getTaskVer() : 0;

      pv->myLinkValues.massDamping  = item->alpha1.getValue();
      pv->myLinkValues.stiffDamping = item->alpha2.getValue();
      pv->myLinkValues.massScale    = item->massScale.getValue();
      pv->myLinkValues.stiffScale   = item->stiffnessScale.getValue();

      pv->myLinkValues.ignoreCheckSum   = item->overrideChecksum.getValue();
      pv->myLinkValues.expandModeShapes = item->expandModeShapes.getValue();
      pv->myLinkValues.compModes        = item->nGenModes.getValue();
      pv->myLinkValues.consistentMassMx = item->useConsistentMassMatrix.getValue();
      pv->myLinkValues.factorStiffMx    = !item->factorizeMassMxEigSol.getValue();
      pv->myLinkValues.recoveryMxPrec   = (int)item->recoveryMatrixSavePrecision.getValue();
      pv->myLinkValues.singularityCrit  = item->tolFactorize.getValue();
      pv->myLinkValues.eigValTolerance  = item->tolEigenval.getValue();

      pv->myLinkValues.importedFile = item->originalFEFile.getValue();
      pv->myLinkValues.allowChange  = item->originalFEFile.getValue().empty() || FapLicenseManager::isProEdition();

      if (item->importConverter.getValue().isValid())
        pv->myLinkValues.unitConversion = item->importConverter.getValue().getName();
      else
        pv->myLinkValues.unitConversion = "No unit conversion";

      if (!item->usesRepository())
	pv->myLinkValues.repositoryFile = "[internal] ";
      else if (!item->myRepository.getValue().empty())
	pv->myLinkValues.repositoryFile = "[part specific] ";
      else
	pv->myLinkValues.repositoryFile = "[external] ";

      pv->myLinkValues.repositoryFile  += item->getBaseFTLFile();

      pv->myLinkValues.suppressInSolver = item->suppressInSolver.getValue();

      // Distributed loads

      pv->myLinkValues.loadCases = item->myLoadCases.getValue();
      pv->myLinkValues.loadFactors = item->myLoadFactors.getValue();
      pv->myLinkValues.loadDelays = item->myLoadDelays.getValue();
      pv->myLinkValues.loadEngineQuery = FapUAEngineQuery::instance();
      pv->myLinkValues.editLoadEngineCB = FFaDynCB1M(FapUAProperties,this,onQIFieldButtonCB,FuiQueryInputFieldValues&);
      pv->myLinkValues.selectedLoadEngines.clear();
      for (size_t l = 0; l < item->myLoadEngines.size(); l++)
	pv->myLinkValues.selectedLoadEngines.push_back(item->myLoadEngines.getPtr(l));

      // GP properties

      pv->myLinkValues.modelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath();
      pv->myLinkValues.useGenericPart = item->useGenericProperties.getValue();
      pv->myLinkValues.usingFEModelViz = item->useFEModelAsVisualization();
      if ((pv->myLinkValues.allowChangeViz = item->baseCadFileName.getValue().empty()))
        pv->myLinkValues.vizFile = item->visDataFile.getValue();
      else
        pv->myLinkValues.vizFile = FFaFilePath::getRelativeFilename(pv->myLinkValues.modelFilePath,
                                                                    item->getBaseCadFile());
      pv->myLinkValues.genericPartNoCGTriad = item->condenseOutCoG.getValue();
      pv->myLinkValues.genericPartStiffType = (int)item->myGenericPartStiffType.getValue();
      pv->myLinkValues.genericPartKT        = item->kt.getValue();
      pv->myLinkValues.genericPartKR        = item->kr.getValue();
      pv->myLinkValues.genericPartMass      = item->mass.getValue();
      pv->myLinkValues.genericPartInertia   = item->inertia.getValue();
      pv->myLinkValues.inertiaRef           = item->myInertiaRef.getValue();
      pv->myLinkValues.useCalculatedMass    = item->myCalculateMass.getValue();
      if (item->getLinkHandler())
        pv->myLinkValues.canCalculateMass   = pv->myLinkValues.vizFile.empty() ? 'F' : 'B';
      else
        pv->myLinkValues.canCalculateMass   = pv->myLinkValues.vizFile.empty() ? '\0' : 'G';

      pv->myLinkValues.materialQuery = FapUAMaterialPropQuery::instance();
      pv->myLinkValues.selectedMaterial = item->material.getPointer();

#ifdef USE_INVENTOR
      if (!pv->myLinkValues.useGenericPart)
        if ((pv->myLinkValues.feNode = FdPickedPoints::getSelectedNode()) > 0)
        {
          // Picked FE node
          double* X = pv->myLinkValues.feNodePos.getPt();
          item->getNodePos(pv->myLinkValues.feNode,X,X+1,X+2);
        }
#endif

      // Hydrodynamics options

      pv->myLinkValues.buoyancy = item->hasBuoyancy.getValue();

      // Meshing options

      pv->myLinkValues.minSize = item->minSize.getValue();
      pv->myLinkValues.quality = item->quality.getValue();
      pv->myLinkValues.enableMeshing = item->isMeshable() && FapLicenseManager::hasFeature("FA-MSH");
      if (pv->myLinkValues.enableMeshing)
        item->getMeshParams(&pv->myLinkValues.noNodes,
                            &pv->myLinkValues.noElms,
                            &pv->myLinkValues.parabolic);

      // Advanced options

      pv->myLinkValues.coordSysOption = item->myCSOption.getValue();
      pv->myLinkValues.centripOption  = item->myCentripOption.getValue();
      pv->myLinkValues.recoveryOption = item->recoveryDuringSolve.getValue();
      pv->myLinkValues.ignoreRecovery = item->ignoreInRecovery.getValue();
      if (item->hasStrainRosettes()) pv->myLinkValues.recoveryOption += 10;
      pv->myLinkValues.modelFilePath += FFaFilePath::getPathSeparator();
      if (FapLicenseManager::hasFeature("FA-EXR")) {
        pv->myLinkValues.extResFileName = item->externalResFileName.getValue();
        pv->myLinkValues.extResSwitch = item->useExternalResFile.getValue();
      }
      else // hide toggle, feature not available
        pv->myLinkValues.extResSwitch = -1;

      // Nonlinear options

      pv->myLinkValues.useNonlinearSwitch = item->useNonlinearReduction.getValue();
      pv->myLinkValues.numNonlinear       = item->numberOfNonlinearSolutions.getValue();
      pv->myLinkValues.nonlinearInputFile = item->nonlinearDataFileName.getValue();

      // Topology view:

      this->addTriadTopology(pv->myTopology,item);

      std::vector<FmStrainRosette*> rosettes;
      item->getReferringObjs(rosettes,"rosetteLink");

      if (!rosettes.empty()) {
        this->addTopologyItem(pv->myTopology,NULL,0,"Strain rosettes");
        for (FmStrainRosette* rosette : rosettes)
          this->addTopologyItem(pv->myTopology,rosette,1);
      }
    }

  // User-defined elements

  else if (mySelectedFmItem->isOfType(FmUserDefinedElement::getClassTypeID()))
    {
      FmUserDefinedElement* item = (FmUserDefinedElement*)mySelectedFmItem;

      pv->showUDEData = true;

      pv->myTotalMass    = item->getMass();
      pv->myMassPropDamp = item->alpha1.getValue();
      pv->myStifPropDamp = item->alpha2.getValue();
      pv->myScaleMass    = item->massScale.getValue();
      pv->myScaleStiff   = item->stiffnessScale.getValue();

      // Topology view:

      this->addTriadTopology(pv->myTopology,item);
    }

  // Reference plane

  else if (mySelectedFmItem->isOfType(FmRefPlane::getClassTypeID()))
    {
      FmRefPlane* ref = (FmRefPlane*)mySelectedFmItem;

      pv->showRefPlane = true;

      pv->myRefPlaneWidth  = ref->getWidth();
      pv->myRefPlaneHeight = ref->getHeight();

      // Topology view:

      this->addTriadTopology(pv->myTopology,FmDB::getEarthLink());
    }

  // File reference

  else if (mySelectedFmItem->isOfType(FmFileReference::getClassTypeID()))
    {
      FmFileReference* item = (FmFileReference*)mySelectedFmItem;

      pv->showFileReference = true;

      pv->myFileReferenceName = item->fileName.getValue();
      pv->myModelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath() + FFaFilePath::getPathSeparator();

      // Topology view:

      std::vector<FmModelMemberBase*> strMembs;
      item->getReferringObjs(strMembs);
      for (FmModelMemberBase* obj : strMembs)
	this->addTopologyItem(pv->myTopology,obj);
    }

  // Sticker

  else if (mySelectedFmItem->isOfType(FmSticker::getClassTypeID()))
    {
      FmSticker* item = (FmSticker*)mySelectedFmItem;

      // Topology view:

      this->addTopologyItem(pv->myTopology,item->getStuckObject());
    }

  // Generic object

  else if (mySelectedFmItem->isOfType(FmGenericDBObject::getClassTypeID()))
    {
      FmGenericDBObject* item = (FmGenericDBObject*)mySelectedFmItem;

      pv->showGenDBObj = true;

      pv->myGenDBObjType = item->objectType.getValue();
      pv->myGenDBObjDef  = item->objectDefinition.getValue();

      // Topology view:

      std::vector<FmModelMemberBase*> strMembs;
      item->getReferringObjs(strMembs);
      for (FmModelMemberBase* obj : strMembs)
	this->addTopologyItem(pv->myTopology,obj);
    }

  // Sensor

  else if (mySelectedFmItem->isOfType(FmSensorBase::getClassTypeID()))
    {
      FmSensorBase* item = (FmSensorBase*)mySelectedFmItem;

      // Topology view:

      std::vector<FmIsMeasuredBase*> measured;
      item->getMeasured(measured);
      this->addTopologyItem(pv->myTopology,NULL,0,"Measuring:");
      if (measured.size() == 1)
        this->addTopologyItem(pv->myTopology,item->getMeasured(),1);
      else {
        const char* pfx[2] = { "First", "Second" };
        for (size_t i = 0; i < measured.size() && i < 2; i++)
          this->addTopologyItem(pv->myTopology,measured[i],1,pfx[i]);
      }

      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      std::vector<FmEngine*> engines;
      item->getEngines(engines);
      for (FmEngine* engine : engines)
        this->addTopologyItem(pv->myTopology,engine,1);
    }

  // Tire

  else if (mySelectedFmItem->isOfType(FmTire::getClassTypeID()))
    {
      FmTire* item = (FmTire*)mySelectedFmItem;

      pv->showTireData = true;

      static FapUAQuery roadQuery;

      roadQuery.clear();
      roadQuery.typesToFind[FmRoad::getClassTypeID()] = true;

      pv->myTireDataFileRefQuery = FapUAFileRefQuery::instance();
      pv->myRoadQuery = &roadQuery;
      pv->myTireTypes = { "MF-TYRE", "SWIFT", "FTIRE", "JD-TIRE" };
      pv->mySpindelTriadOffset = item->spindelTriadOffset.getValue();
      pv->mySelectedTireModelType = item->tireType.getValue();
      pv->mySelectedRoad = item->road;
      pv->mySelectedTireDataFileRef = item->tireDataFileRef;
      pv->myTireDataFileName = item->tireDataFileName.getValue();

      // Topology view:

      this->addTopologyItem(pv->myTopology,item->road);
      this->addTopologyItem(pv->myTopology,item->bearingJoint,0,"Bearing");
#ifdef FAP_DEBUG
      std::multimap<std::string,FFaFieldContainer*> reffingObjs;
      std::multimap<std::string,FFaFieldContainer*>::iterator it;
      item->getReferringObjs(reffingObjs);
      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      for (it = reffingObjs.begin(); it != reffingObjs.end(); ++it)
        this->addTopologyItem(pv->myTopology,dynamic_cast<FmModelMemberBase*>(it->second),1);

      item->getReferredObjs(reffingObjs);
      this->addTopologyItem(pv->myTopology,NULL,0,"Using:");
      for (it = reffingObjs.begin(); it != reffingObjs.end(); ++it)
        this->addTopologyItem(pv->myTopology,dynamic_cast<FmModelMemberBase*>(it->second),1);
#endif
    }

  // Road

  else if (mySelectedFmItem->isOfType(FmRoad::getClassTypeID()))
    {
      FmRoad* item = (FmRoad*)mySelectedFmItem;

      pv->showRoadData = true;

      pv->iAmUsingExtRoadData = item->useExtRoadDataFile.getValue();

      pv->myRoadDataFileRefQuery = FapUAFileRefQuery::instance();
      pv->mySelectedRoadDataFileRef = item->roadDataFileRef;
      pv->myRoadDataFileName = item->roadDataFileName.getValue();

      pv->myRoadFunctionQuery = FapUARoadFuncQuery::instance();
      pv->mySelectedRoadFunc = item->roadFunction;

      pv->myRoadZShift = item->roadZShift.getValue();
      pv->myRoadXOffset = item->roadXOffset.getValue();
      pv->myRoadZRotation = item->roadZRotation.getValue();

      // Topology view:

      this->addTopologyItem(pv->myTopology,NULL,0,"Function:");
      this->addTopologyItem(pv->myTopology,item->roadFunction,1);

      std::vector<FmTire*> tires;
      item->getReferringObjs(tires,"road");
      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      for (FmTire* tire : tires)
        this->addTopologyItem(pv->myTopology,tire,1);
   }

  // Material properties

  else if (mySelectedFmItem->isOfType(FmMaterialProperty::getClassTypeID()))
    {
      FmMaterialProperty* item = (FmMaterialProperty*)mySelectedFmItem;

      pv->showMatPropData = true;

      pv->myMatPropRho = item->Rho.getValue();
      pv->myMatPropE   = item->E.getValue();
      pv->myMatPropNu  = item->nu.getValue();
      pv->myMatPropG   = item->G.getValue();

      // Topology view:

      std::vector<FmModelMemberBase*> objs;
      item->getReferringObjs(objs,"material");
      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      for (FmModelMemberBase* obj : objs)
        this->addTopologyItem(pv->myTopology,obj,1);
   }

  // Sea state

  else if (mySelectedFmItem->isOfType(FmSeaState::getClassTypeID()))
  {
    FmSeaState* item = (FmSeaState*)mySelectedFmItem;

    pv->showSeaStateData = true;

    pv->mySeaStateWidth = item->xLength.getValue();
    pv->mySeaStateHeight = item->yLength.getValue();

    pv->mySeaStateWidthPos = item->getX();
    pv->mySeaStateHeightPos = item->getY();

    pv->mySeaStateShowGrid = item->getShowGrid();
    pv->mySeaStateShowSolid = item->getShowSolid();

    pv->mySeaStateNumPoints = item->getQuantization();
  }

  // Beam properties

  else if (mySelectedFmItem->isOfType(FmBeamProperty::getClassTypeID()))
    {
      FmBeamProperty* item = (FmBeamProperty*)mySelectedFmItem;

      pv->showBeamPropData = true;

      pv->myBeamCrossSectionType = item->crossSectionType.getValue();
      pv->mySelectedMaterial = item->material.getPointer();
      pv->myMaterialQuery = FapUAMaterialPropQuery::instance();

      pv->myBeamBreakDependence = item->breakDependence.getValue();
      pv->myBeamHydroToggle = item->hydroToggle.getValue();

      pv->myBeamProp = {
        item->Do.getValue(),
        item->Di.getValue(),
        item->EA.getValue(),
        item->EI.getValue().first,
        item->EI.getValue().second,
        item->GIt.getValue(),
        item->Mass.getValue(),
        item->RoIp.getValue(),
        item->A.getValue(),
        item->Iy.getValue(),
        item->Iz.getValue(),
        item->Ip.getValue(),
        item->ShrRed.getValue().first,
        item->ShrRed.getValue().second,
        item->GAs.getValue().first,
        item->GAs.getValue().second,
        item->ShrCentre.getValue().first,
        item->ShrCentre.getValue().second
      };

      pv->myHydroProp = {
        item->Db.getValue(),
        item->Dd.getValue(),
        item->Cd.getValue(),
        item->Ca.getValue(),
        item->Cm.getValue(),
        item->Cd_axial.getValue(),
        item->Ca_axial.getValue(),
        item->Cm_axial.getValue(),
        item->Cd_spin.getValue(),
        item->Di_hydro.getValue()
      };

      // Topology view:

      std::vector<FmBeam*> beams;
      item->getReferringObjs(beams,"myProp");
      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      for (FmBeam* beam : beams)
        this->addTopologyItem(pv->myTopology,beam,1);
   }

  // Engine

  else if (mySelectedFmItem->isOfType(FmEngine::getClassTypeID()))
    {
      FmEngine* item = (FmEngine*)mySelectedFmItem;

      pv->showFunctionData = true;

      // FapUAFunctionProperties does most of the work here.

      // Topology view:

      this->addEngineArgumentTopology(pv->myTopology,item,"Argument:");
      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      this->addEngineUsedByTopology(pv->myTopology,item,1);
    }

  // Strain Rosette

  else if (mySelectedFmItem->isOfType(FmStrainRosette::getClassTypeID()))
    {
      FmStrainRosette* item = (FmStrainRosette*)mySelectedFmItem;

      pv->showStrainRosetteData = true;

      pv->myStrRosIsEditable = FpPM::isModelTouchable() && !FpRDBExtractorManager::instance()->hasResults(item);

      FmPart* rosettePart = item->getTopology(pv->myStrRosNodes);
      pv->myStrRosAngle = item->angle.getValue() * 180.0/M_PI;

      const Strings& rosetteTypes = FmStrainRosette::getRosetteUINames();

      pv->myStrainRosetteTypes = rosetteTypes;
      pv->mySelectedRosetteType = rosetteTypes[item->rosetteType.getValue()];
      pv->myStrRosHeight     = item->getZPos();
      pv->IAmUsingFEHeight   = item->useFEThickness.getValue();
      pv->myStrRosEMod       = item->getEMod();
      pv->myStrRosNu         = item->getNu();
      pv->IAmUsingFEMaterial = item->useFEMaterial.getValue();
      pv->IAmResettingStartStrains = item->removeStartStrains.getValue();

      // Topology view:

      this->addTopologyItem(pv->myTopology,rosettePart);
    }

  // Element Group

  else if (mySelectedFmItem->isOfType(FmElementGroupProxy::getClassTypeID()))
    {
      FmElementGroupProxy* item = (FmElementGroupProxy*)mySelectedFmItem;

      pv->showGroupData = true;

      pv->doFatigue = item->doFatigue();
      pv->mySNCurve = item->myFatigueSNCurve.getValue();
      pv->mySNStd = item->myFatigueSNStd.getValue();
      pv->mySCF = item->myFatigueSCF.getValue();

      // Topology view:

      this->addTopologyItem(pv->myTopology,item->getOwner());
    }

  // Function or Friction

  else if (mySelectedFmItem->isOfType(FmParamObjectBase::getClassTypeID()))
    {
      pv->showFunctionData = true;

      // FapUAFunctionProperties does the job

      // Topology view:

      FmSimulationModelBase* refObj;
      std::vector<FmModelMemberBase*> joints;
      std::multimap<std::string,FFaFieldContainer*> refs;
      std::multimap<std::string,FFaFieldContainer*>::iterator rit;
      mySelectedFmItem->getReferringObjs(refs);
      for (rit = refs.begin(); rit != refs.end(); ++rit)
	if ((refObj = dynamic_cast<FmSimulationModelBase*>(rit->second)))
	{
	  if (refObj->isOfType(FmEngine::getClassTypeID()) && !refObj->isListable())
	  {
	    this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
	    this->addEngineUsedByTopology(pv->myTopology,(FmEngine*)refObj,1);
	    continue;
	  }
	  else if (refObj->isOfType(FmJointSpring::getClassTypeID()))
	    refObj = ((FmJointSpring*)refObj)->getOwnerJoint();
	  else if (refObj->isOfType(FmJointDamper::getClassTypeID()))
	    refObj = ((FmJointDamper*)refObj)->getOwnerJoint();

	  // Make sure joints using this in several DOFs are added only once
	  if (refObj && refObj->isOfType(FmJointBase::getClassTypeID())) {
	    if (std::find(joints.begin(),joints.end(),refObj) == joints.end())
	      joints.push_back(refObj);
	    else
	      continue;
	  }

	  this->addTopologyItem(pv->myTopology,refObj);
	}
    }

  // Control element

  else if (mySelectedFmItem->isOfType(FmcInput::getClassTypeID()))
    {
      FmcInput* item = (FmcInput*)mySelectedFmItem;

      pv->showCtrlInOut = true;
      pv->showFunctionData = true;

      // Topology view:

      if (FmEngine* engine = item->getEngine(); engine)
	this->addEngineArgumentTopology(pv->myTopology,engine,"Input:");

      std::vector<FmCtrlLine*> outputLines;
      item->getLines(outputLines);
      this->addTopologyItem(pv->myTopology,NULL,0,"Output lines:");
      for (FmCtrlLine* line : outputLines)
	this->addTopologyItem(pv->myTopology,line,1);
    }
  else if (mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
    {
      FmcOutput* item = (FmcOutput*)mySelectedFmItem;

      pv->showCtrlInOut = true;
      pv->showFunctionData = true;

      // Topology view:

      this->addTopologyItem(pv->myTopology,NULL,0,"Input line:");
      this->addTopologyItem(pv->myTopology,item->getLine(),1);
    }
  else if (mySelectedFmItem->isOfType(FmCtrlOutputElementBase::getClassTypeID()))
    {
      FmCtrlOutputElementBase* item = (FmCtrlOutputElementBase*)mySelectedFmItem;

      pv->showCtrlData = true;

      // Topology view:

      this->addTopologyItem(pv->myTopology,NULL,0,"Input lines:");
      for (int port = 1; port <= item->getNumInputPorts(); port++)
	this->addTopologyItem(pv->myTopology,item->getLine(port),1);

      std::vector<FmCtrlLine*> outputLines;
      item->getLines(outputLines);
      this->addTopologyItem(pv->myTopology,NULL,0,"Output lines:");
      for (FmCtrlLine* line : outputLines)
	this->addTopologyItem(pv->myTopology,line,1);
    }

  // Control line

  else if (mySelectedFmItem->isOfType(FmCtrlLine::getClassTypeID()))
    {
      FmCtrlLine* item = (FmCtrlLine*)mySelectedFmItem;

      pv->showCtrlData = true;

      // Topology view:

      this->addTopologyItem(pv->myTopology,NULL,0,"Start block:");
      this->addTopologyItem(pv->myTopology,item->getStartElement(),1);
      this->addTopologyItem(pv->myTopology,NULL,0,"End block:");
      this->addTopologyItem(pv->myTopology,item->getEndElement(),1);
    }

#ifdef FT_HAS_EXTCTRL
  else if (mySelectedFmItem->isOfType(FmExternalCtrlSys::getClassTypeID()))
    {
      pv->showExtCtrlSysData = true;
    }
#endif
  else if (mySelectedFmItem->isOfType(FmAnimation::getClassTypeID()))
    {
      pv->showAnimationData = true;
    }
#ifdef FT_HAS_GRAPHVIEW
  else if (mySelectedFmItem->isOfType(FmGraph::getClassTypeID()))
    {
      pv->showGraphData = true;
    }
#endif

  // RAO vessel motion

  else if (mySelectedFmItem->isOfType(FmVesselMotion::getClassTypeID()))
    {
      FmVesselMotion* item = (FmVesselMotion*) mySelectedFmItem;

      pv->showRAOData = true;

      pv->myRAOFileRefQuery   = FapUAFileRefQuery::instance();
      pv->myWaveFunctionQuery = FapUAWaveFuncQuery::instance();
      pv->myMotionScaleQuery  = FapUAEngineQuery::instance();

      pv->myRAOFileName = item->raoFile.getValue();
      pv->mySelectedRAOFileRef = item->raoFileRef.getPointer();
      pv->mySelectedWaveFunc = item->waveFunction.getPointer();
      pv->mySelectedWaveDir = item->waveDir.getValue();
      pv->mySelectedScale = item->motionScale.getPointer();
      item->getWaveAngles(pv->myWaveDirections);

      // Topology view:

      this->addTopologyItem(pv->myTopology,NULL,0,"Vessel Triad:");
      this->addTopologyItem(pv->myTopology,item->getVesselTriad(),1);

      std::vector<FmEngine*> objs;
      item->motionEngine.getPtrs(objs);
      this->addTopologyItem(pv->myTopology,NULL,0,"Motion functions:");
      for (FmEngine* engine : objs)
	this->addTopologyItem(pv->myTopology,engine,1);
    }

  // Simulation event

  else if (mySelectedFmItem->isOfType(FmSimulationEvent::getClassTypeID()))
    {
      FmSimulationEvent* item = (FmSimulationEvent*)mySelectedFmItem;

      pv->showSimEventData = true;

      pv->showActiveEvent = FapSimEventHandler::getActiveEvent() == item;
      pv->mySimEventProbability = item->getProbability();
      pv->allowSimEventChange = FapSolutionProcessManager::instance()->empty();

      // Topology view:

      std::vector<FmSimulationModelBase*> objs;
      item->getObjects(objs);
      this->addTopologyItem(pv->myTopology,NULL,0,"Altered objects:");
      for (FmSimulationModelBase* obj : objs)
	this->addTopologyItem(pv->myTopology,obj,1);
    }

  // Shaft

  else if (dynamic_cast<FmShaft*>(mySelectedFmItem) != NULL)
    {
      FmShaft* item = (FmShaft*)mySelectedFmItem;

      pv->showShaftData = true;

      // Get total mass and length
      pv->myTotalLength = item->Length.getValue();
      pv->myTotalMass = item->getTotalMass();

      // Get visualize 3D value
      pv->myVisualize3Dts = item->visualize3Dts.getValue();

      // Get the properties of all subordinate beams
      std::vector<FmBeam*> beams;
      FmDB::getAllBeams(beams,item);
      if (beams.empty())
        pv->myBlankFieldsFlags = 0xffff; // show blank fields if no links
      else
      {
        // Get values from first item
        pv->myMassPropDamp = beams.front()->alpha1.getValue();
        pv->myStifPropDamp = beams.front()->alpha2.getValue();
        pv->myScaleMass    = beams.front()->massScale.getValue();
        pv->myScaleStiff   = beams.front()->stiffnessScale.getValue();
        pv->mySelectedCS   = beams.front()->getProperty();
        pv->myCrossSectionQuery = FapUABeamPropQuery::instance();
        pv->myHideCrossSection = false;

        // Different values on other items? Show blank fields
        pv->myBlankFieldsFlags = 0x0000;
        for (size_t i = 1; i < beams.size(); i++) {
	  if (beams[i]->getProperty() != pv->mySelectedCS)
	    pv->myBlankFieldsFlags |= 0x0001;
	  if (beams[i]->alpha1.getValue() != pv->myMassPropDamp)
	    pv->myBlankFieldsFlags |= 0x0002;
	  if (beams[i]->alpha2.getValue() != pv->myStifPropDamp)
	    pv->myBlankFieldsFlags |= 0x0004;
	  if (beams[i]->massScale.getValue() != pv->myScaleMass)
	    pv->myBlankFieldsFlags |= 0x0008;
	  if (beams[i]->stiffnessScale.getValue() != pv->myScaleStiff)
	    pv->myBlankFieldsFlags |= 0x0010;
	}
      }
    }

  // Turbine

  else if (dynamic_cast<FmTurbine*>(mySelectedFmItem) != NULL)
    {
      FmTurbine* item = (FmTurbine*)mySelectedFmItem;

      pv->showTurbineData = true;
      pv->showSubassPos = true;

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);

      // Get topology
      pv->myTurbineWindRefTriadDefQuery = FapUATriadQuery::instance();
      pv->myTurbineWindRefTriadDefSelected = item->topology[0];
      pv->myTurbineWindVertOffset = item->ptfmRef.getValue();
      pv->myTurbineYawPointTriadDefQuery = FapUATriadQuery::instance();
      pv->myTurbineYawPointTriadDefSelected = item->topology[1];
      pv->myTurbineHubApexTriadDefQuery = FapUATriadQuery::instance();
      pv->myTurbineHubApexTriadDefSelected = item->topology[2];
      pv->myTurbineHubPartDefQuery = FapUALinkQuery::instance();
      pv->myTurbineHubPartDefSelected = item->topology[3];
      pv->myTurbineFirstBearingDefQuery = FapUARevJointQuery::instance();
      pv->myTurbineFirstBearingDefSelected = item->topology[4];
    }

  // Tower

  else if (dynamic_cast<FmTower*>(mySelectedFmItem) != NULL)
    {
      FmTower* item = (FmTower*)mySelectedFmItem;

      pv->showTowerData = true;

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);

      // Determine if visualize 3D is enabled
      std::vector<FmPart*> parts;
      FmDB::getAllParts(parts,item);
      pv->myVisualize3DEnabled = parts.empty();

      // Get visualize 3D value
      pv->myVisualize3Dts = item->visualize3Dts.getValue();
    }

  // Nacelle

  else if (dynamic_cast<FmNacelle*>(mySelectedFmItem) != NULL)
    {
      FmNacelle* item = (FmNacelle*)mySelectedFmItem;

      pv->showNacelleData = true;

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);
    }

  // Generator

  else if (dynamic_cast<FmGenerator*>(mySelectedFmItem) != NULL)
    {
      FmGenerator* item = (FmGenerator*)mySelectedFmItem;

      pv->showGeneratorData = true;

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);

      // Get the subordinate joint properties
      std::vector<FmModelMemberBase*> elmJoints;
      FmDB::getAllOfType(elmJoints, FmJointBase::getClassTypeID(), item);
      if (!elmJoints.empty())
      {
        FmJointBase* pJoint = static_cast<FmJointBase*>(elmJoints.front());

        // Get Rz DOF status
        pv->myGeneratorTorqueControl = (pJoint->getStatusOfDOF(5) == FmHasDOFsBase::FREE ||
                                        pJoint->getStatusOfDOF(5) >= FmHasDOFsBase::FREE_DYNAMICS);

        // Get torque control values
	if (FmDofLoad* load = pJoint->getLoadAtDOF(5); load) {
	  pv->myGeneratorTorqueControlConstValue = load->getInitLoad();
	  pv->myGeneratorTorqueControlSelectedEngine = load->getEngine();
	}
	else {
	  pv->myGeneratorTorqueControlConstValue = 0.0;
	  pv->myGeneratorTorqueControlSelectedEngine = NULL;
	}
	pv->myGeneratorTorqueControlIsConstant = (pv->myGeneratorTorqueControlSelectedEngine ? false : true);
	pv->myGeneratorTorqueControlEngineQuery = FapUAEngineQuery::instance();

        // Get velocity control values
	if (FmDofMotion* pm = pJoint->getMotionAtDOF(5); pm) {
	  pv->myGeneratorVelocityControlConstValue  = pm->getInitMotion();
	  pv->myGeneratorVelocityControlSelectedEngine = pm->getEngine();
	}
	else {
	  pv->myGeneratorVelocityControlConstValue = 0.0;
	  pv->myGeneratorVelocityControlSelectedEngine = NULL;
	}
	pv->myGeneratorVelocityControlIsConstant = (pv->myGeneratorVelocityControlSelectedEngine ? false : true);
	pv->myGeneratorVelocityControlEngineQuery = FapUAEngineQuery::instance();
      }
    }

  // Gearbox

  else if (dynamic_cast<FmGearBox*>(mySelectedFmItem) != NULL)
    {
      FmGearBox* item = (FmGearBox*)mySelectedFmItem;

      pv->showGearboxData = true;

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);
    }

  // Rotor

  else if (dynamic_cast<FmRotor*>(mySelectedFmItem) != NULL)
    {
      FmRotor* item = (FmRotor*)mySelectedFmItem;

      pv->showRotorData = true;

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);
    }

  // Blade

  else if (dynamic_cast<FmBlade*>(mySelectedFmItem) != NULL)
    {
      FmBlade* item = (FmBlade*)mySelectedFmItem;

      pv->showBladeData = true;

      // Get total mass and length
      pv->myTotalMass = item->getTotalMass();
      pv->myTotalLength = item->getTotalLength();

      // Get ice layer data
      pv->myBladeIceLayer = item->IceLayer.getValue();
      pv->myBladeIceThickness = item->IceThickness.getValue();

      // Get visualize 3D value
      pv->myVisualize3Dts = item->visualize3Dts.getValue();

      // Get pitch joint
      if (FmJointBase* pJoint = item->getPitchJoint(); pJoint) {

        // Get Rz DOF status
        pv->myBladePitchIsFixed = pJoint->getStatusOfDOF(5) == FmHasDOFsBase::FIXED;

        // Get deflection control values
	if (FmDofMotion* pm = pJoint->getMotionAtDOF(5); pm) {
	  pv->myBladePitchControlConstValue = pm->getInitMotion();
	  pv->myBladePitchControlSelectedEngine = pm->getEngine();
	}
	else {
	  pv->myBladePitchControlConstValue = 0.0;
	  pv->myBladePitchControlSelectedEngine = NULL;
	}
	pv->myBladePitchControlIsConstant = (pv->myBladePitchControlSelectedEngine ? false : true);
	pv->myBladePitchControlEngineQuery = FapUAEngineQuery::instance();
      }

      // Get the properties of all subordinate links (for SD & DP)
      std::vector<FmLink*> beams;
      FmDB::getAllLinks(beams,item);
      if (beams.empty())
        pv->myBlankFieldsFlags = 0xffff; // show blank fields if no links
      else
      {
        // Get values from first item
        pv->myMassPropDamp = beams.front()->alpha1.getValue();
        pv->myStifPropDamp = beams.front()->alpha2.getValue();
        pv->myScaleMass    = beams.front()->massScale.getValue();
        pv->myScaleStiff   = beams.front()->stiffnessScale.getValue();

        // Different values on other items? Show blank fields
	pv->myBlankFieldsFlags = 0x0000;
        for (size_t i = 1; i < beams.size(); i++) {
	  if (beams[i]->alpha1.getValue() != pv->myMassPropDamp)
	    pv->myBlankFieldsFlags |= 0x0002;
	  if (beams[i]->alpha2.getValue() != pv->myStifPropDamp)
	    pv->myBlankFieldsFlags |= 0x0004;
	  if (beams[i]->massScale.getValue() != pv->myScaleMass)
	    pv->myBlankFieldsFlags |= 0x0008;
	  if (beams[i]->stiffnessScale.getValue() != pv->myScaleStiff)
	    pv->myBlankFieldsFlags |= 0x0010;
	}
      }
    }

  // Riser

  else if (dynamic_cast<FmRiser*>(mySelectedFmItem) != NULL)
    {
      FmRiser* item = (FmRiser*)mySelectedFmItem;

      pv->showRiserData = true;
      pv->showSubassPos = true;

      pv->mySubAssemblyFile = item->myModelFile.getValue();
      pv->myModelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath() + FFaFilePath::getPathSeparator();

      // Get total mass and length
      double length = 0.0;
      pv->myTotalMass = item->getTotalMass(&length);
      pv->myTotalLength = length;

      // Get internal fluid properties
      pv->myIntFluid = item->internalMud.getValue();
      pv->myIntFluidDensity = item->mudDensity.getValue();
      pv->myIntFluidLevel = item->mudLevel.getValue();

      // Get visualize 3D value
      pv->myVisualize3Dts = item->visualize3Dts.getValue();
      pv->myVisualize3DAngles = item->visualize3DAngles.getValue();

      // Get the properties of all subordinate links (for SD & DP)
      std::vector<FmLink*> beams;
      FmDB::getAllLinks(beams,item);
      if (beams.empty())
        pv->myBlankFieldsFlags = 0xffff; // show blank fields if no links
      else
      {
        // Get values from first item
        pv->myMassPropDamp = beams.front()->alpha1.getValue();
        pv->myStifPropDamp = beams.front()->alpha2.getValue();
        pv->myScaleMass    = beams.front()->massScale.getValue();
        pv->myScaleStiff   = beams.front()->stiffnessScale.getValue();
        // Note: We will not show cross sections on riser parts.

        // Different values on other items? Show blank fields
	pv->myBlankFieldsFlags = 0x0000;
        for (size_t i = 1; i < beams.size(); i++) {
	  if (beams[i]->alpha1.getValue() != pv->myMassPropDamp)
	    pv->myBlankFieldsFlags |= 0x0002;
	  if (beams[i]->alpha2.getValue() != pv->myStifPropDamp)
	    pv->myBlankFieldsFlags |= 0x0004;
	  if (beams[i]->massScale.getValue() != pv->myScaleMass)
	    pv->myBlankFieldsFlags |= 0x0008;
	  if (beams[i]->stiffnessScale.getValue() != pv->myScaleStiff)
	    pv->myBlankFieldsFlags |= 0x0010;
	}
      }
    }

  // Jacket

  else if (dynamic_cast<FmJacket*>(mySelectedFmItem) != NULL)
    {
      FmJacket* item = (FmJacket*)mySelectedFmItem;

      pv->showJacketData = true;
      pv->showSubassPos = true;

      pv->mySubAssemblyFile = item->myModelFile.getValue();
      pv->myModelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath() + FFaFilePath::getPathSeparator();

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);

      // Get visualize 3D value
      pv->myVisualize3Dts = item->visualize3Dts.getValue();

      // Get the properties of all subordinate links (for SD & DP)
      std::vector<FmLink*> beams;
      FmDB::getAllLinks(beams,item);
      if (beams.empty())
        pv->myBlankFieldsFlags = 0xffff; // show blank fields if no links
      else
      {
        // Get values from first item
        pv->myMassPropDamp = beams.front()->alpha1.getValue();
        pv->myStifPropDamp = beams.front()->alpha2.getValue();
        pv->myScaleMass    = beams.front()->massScale.getValue();
        pv->myScaleStiff   = beams.front()->stiffnessScale.getValue();
        // Note: We will not show cross sections on riser parts.

        // Different values on other items? Show blank fields
	pv->myBlankFieldsFlags = 0x0000;
        for (size_t i = 1; i < beams.size(); i++) {
	  if (beams[i]->alpha1.getValue() != pv->myMassPropDamp)
	    pv->myBlankFieldsFlags |= 0x0002;
	  if (beams[i]->alpha2.getValue() != pv->myStifPropDamp)
	    pv->myBlankFieldsFlags |= 0x0004;
	  if (beams[i]->massScale.getValue() != pv->myScaleMass)
	    pv->myBlankFieldsFlags |= 0x0008;
	  if (beams[i]->stiffnessScale.getValue() != pv->myScaleStiff)
	    pv->myBlankFieldsFlags |= 0x0010;
	}
      }
    }

  // Soil Pile

  else if (dynamic_cast<FmSoilPile*>(mySelectedFmItem) != NULL)
    {
      FmSoilPile* item = (FmSoilPile*)mySelectedFmItem;

      pv->showSoilPileData = true;
      pv->showSubassPos = true;

      pv->mySubAssemblyFile = item->myModelFile.getValue();
      pv->myModelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath() + FFaFilePath::getPathSeparator();

      // Get total mass
      double length = 0.0;
      pv->myTotalMass = item->getTotalMass(&length);
      pv->myTotalLength = length;

      // Get internal soil properties
      pv->myIntFluid = item->internalSoil.getValue();
      pv->myIntFluidDensity = item->soilDensity.getValue();

      // Get visualize 3D value
      pv->myVisualize3Dts = item->visualize3Dts.getValue();
      pv->myVisualize3DAngles = item->visualize3DAngles.getValue();

      // Get the properties of all subordinate links (for SD & DP)
      std::vector<FmLink*> beams;
      FmDB::getAllLinks(beams,item);
      if (beams.empty())
        pv->myBlankFieldsFlags = 0xffff; // show blank fields if no links
      else
      {
        // Get values from first item
        pv->myMassPropDamp = beams.front()->alpha1.getValue();
        pv->myStifPropDamp = beams.front()->alpha2.getValue();
        pv->myScaleMass    = beams.front()->massScale.getValue();
        pv->myScaleStiff   = beams.front()->stiffnessScale.getValue();
        // Note: We will not show cross sections on riser parts.

        // Different values on other items? Show blank fields
	pv->myBlankFieldsFlags = 0x0000;
        for (size_t i = 1; i < beams.size(); i++) {
	  if (beams[i]->alpha1.getValue() != pv->myMassPropDamp)
	    pv->myBlankFieldsFlags |= 0x0002;
	  if (beams[i]->alpha2.getValue() != pv->myStifPropDamp)
	    pv->myBlankFieldsFlags |= 0x0004;
	  if (beams[i]->massScale.getValue() != pv->myScaleMass)
	    pv->myBlankFieldsFlags |= 0x0008;
	  if (beams[i]->stiffnessScale.getValue() != pv->myScaleStiff)
	    pv->myBlankFieldsFlags |= 0x0010;
	}
      }
    }

  // Other subassembly objects

  else if (mySelectedFmItem->isOfType(FmSubAssembly::getClassTypeID()))
    {
      FmSubAssembly* item = (FmSubAssembly*)mySelectedFmItem;

      pv->showSubassemblyData = true;
      pv->showSubassPos = dynamic_cast<FmAssemblyBase*>(mySelectedFmItem);

      pv->mySubAssemblyFile = item->myModelFile.getValue();
      pv->myModelFilePath = FmDB::getMechanismObject()->getAbsModelFilePath() + FFaFilePath::getPathSeparator();

      // Get mass properties
      pv->myTotalMass = item->getMass(pv->myCoG);
    }

  // Turbine blade properties

  else if (mySelectedFmItem->isOfType(FmBladeProperty::getClassTypeID()) ||
	   mySelectedFmItem->isOfType(FmBladeDesign::getClassTypeID()))
    {
      // Topology view:

      std::vector<FmModelMemberBase*> strMembs;
      mySelectedFmItem->getReferringObjs(strMembs);
      for (FmModelMemberBase* obj : strMembs)
	this->addTopologyItem(pv->myTopology,obj);
    }

  // Curve

#ifdef FT_HAS_GRAPHVIEW
  else if (mySelectedFmItem->isOfType(FmCurveSet::getClassTypeID()))
    {
      FmCurveSet* item = (FmCurveSet*)mySelectedFmItem;

      pv->showCurveData = true;

      // Topology view:

      switch (item->usingInputMode()) {
      case FmCurveSet::TEMPORAL_RESULT:
	if (item->getResultObj(FmCurveSet::XAXIS))
	  {
	    this->addTopologyItem(pv->myTopology,NULL,0,"X-Axis:");
	    this->addTopologyItem(pv->myTopology,item->getResultObj(FmCurveSet::XAXIS),1);
	  }
	else
	  this->addTopologyItem(pv->myTopology,NULL,0,"X-Axis: " +
				item->getResult(FmCurveSet::XAXIS).getText());

	if (item->getResultObj(FmCurveSet::YAXIS))
	  {
	    this->addTopologyItem(pv->myTopology,NULL,0,"Y-Axis:");
	    this->addTopologyItem(pv->myTopology,item->getResultObj(FmCurveSet::YAXIS),1);
	  }
	else
	  this->addTopologyItem(pv->myTopology,NULL,0,"Y-Axis: " +
				item->getResult(FmCurveSet::YAXIS).getText());
	break;

      case FmCurveSet::SPATIAL_RESULT:
	{
	  std::vector<FmIsPlottedBase*> objs;
	  item->getSpatialObjs(objs);
	  this->addTopologyItem(pv->myTopology,NULL,0,"Y-Axis:");
	  for (FmIsPlottedBase* obj : objs)
	    this->addTopologyItem(pv->myTopology,obj,1);
	}
	break;

      case FmCurveSet::EXT_CURVE:
	this->addTopologyItem(pv->myTopology,NULL,0,"File: " + item->getFilePath());
	break;

      case FmCurveSet::INT_FUNCTION:
      case FmCurveSet::PREVIEW_FUNC:
	{
	  std::vector<FmEngine*> engines;
	  if (FmMathFuncBase* f = item->getFunctionRef(); f)
	    switch (f->getFunctionUse())
	    {
	    case FmMathFuncBase::GENERAL:
	      f->getEngines(engines);
	      for (FmEngine* engine : engines)
		this->addTopologyItem(pv->myTopology,engine);
	      break;
	    default:
	      this->addTopologyItem(pv->myTopology,f);
	    }
	}

      default:
	break;
      }
    }
#endif

  //////
  //
  // Sensor and engine topology additions
  //

  if (mySelectedFmItem->isOfType(FmIsMeasuredBase::getClassTypeID()))
  {
    FmIsMeasuredBase* item = (FmIsMeasuredBase*)mySelectedFmItem;

    if (item->isOfType(FmcOutput::getClassTypeID()))
    {
      if (FmEngine* engine = static_cast<FmcOutput*>(item)->getEngine(); engine)
      {
        this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
        this->addEngineUsedByTopology(pv->myTopology,engine,1);
      }
    }
#ifdef FT_HAS_EXTCTRL
    else if (item->isOfType(FmExternalCtrlSys::getClassTypeID()))
    {
      std::vector<FmEngine*> engines;
      ((FmExternalCtrlSys*)item)->getEngines(engines);
      this->addTopologyItem(pv->myTopology,NULL,0,"Input Functions:");
      for (FmEngine* engine : engines)
        this->addTopologyItem(pv->myTopology,engine,1);

      this->addTopologyItem(pv->myTopology,NULL,0,"Output Sensor:");
      this->addTopologyItem(pv->myTopology,item->getSimpleSensor(),1);
    }
#endif
    else if (!item->isOfType(FmEngine::getClassTypeID()) && item->hasSensors())
    {
      std::vector<FmSensorBase*> sensors;
      item->getReferringObjs(sensors);
      this->addTopologyItem(pv->myTopology,NULL,0,"Used by:");
      for (FmSensorBase* sensor : sensors)
      {
        std::vector<FmEngine*> engines;
        sensor->getEngines(engines);
        for (FmEngine* engine : engines)
          this->addTopologyItem(pv->myTopology,engine,1);
      }
    }
  }

  //////
  //
  // Curve topology additions
  //

  if (mySelectedFmItem->isOfType(FmIsPlottedBase::getClassTypeID()))
    if (FmIsPlottedBase* item = (FmIsPlottedBase*)mySelectedFmItem;
        item->hasCurveSets())
    {
      std::vector<FmCurveSet*> curves;
      item->getCurveSets(curves);
      this->addTopologyItem(pv->myTopology,NULL,0,"Plotted by:");
      for (FmCurveSet* curve : curves)
        this->addTopologyItem(pv->myTopology,curve,1);
    }
}


void FapUAProperties::addTopologyItem(std::vector<FuiTopologyItem>& topology,
                                      FmModelMemberBase* item, int level,
                                      const std::string& typeAddString)
{
  if (!item && (typeAddString.empty() || level > 0)) return;

  FuiTopologyItem ti{ typeAddString, "", "", level < 0 ? -level : level };

  if (item)
  {
    if (ti.type.empty())
      ti.type = item->getUITypeName();
    else if (!item->isOfType(FmGenericDBObject::getClassTypeID()))
      ti.type += std::string(" ") + item->getUITypeName();
    ti.id = std::to_string(item->getID());
    ti.description = item->getUserDescription(128);

    // Since the user IDs are unique only within a sub-assembly,
    // we need to add the sub-assembly path to the item identification.
    IntVec assID;
    item->getAssemblyID(assID);
    // Write the path in bottom-up direction
    for (IntVec::reverse_iterator it = assID.rbegin(); it != assID.rend(); ++it)
      ti.id += "," + std::to_string(*it);
  }

  myTopologyViewList.push_back(item);
  topology.push_back(ti);
}


void FapUAProperties::addEngineArgumentTopology(std::vector<FuiTopologyItem>& topology,
						FmEngine* item, const std::string& arg)
{
  bool measuresTime = false;
  size_t nArg = item->getNoArgs();

  std::vector<FmIsMeasuredBase*> allMeasured;
  for (size_t i = 0; i < nArg; i++)
  {
    FmSensorBase* sensor = item->getSensor(i);
    if (sensor) {
      if (sensor->isTime())
	measuresTime = true;
      else {
	std::vector<FmIsMeasuredBase*> measured;
	sensor->getMeasured(measured);
	allMeasured.insert(allMeasured.end(),measured.begin(),measured.end());
      }
    }
  }

  if (measuresTime)
    this->addTopologyItem(topology,NULL,0,arg+" Time");
  else if (!allMeasured.empty())
    this->addTopologyItem(topology,NULL,0,arg);
  for (FmIsMeasuredBase* obj : allMeasured)
    this->addTopologyItem(topology,obj,1);
}


void FapUAProperties::addEngineUsedByTopology(std::vector<FuiTopologyItem>& topology,
                                              FmEngine* item, int level)
{
  std::vector<FmModelMemberBase*> joints;
  std::vector<FmModelMemberBase*> controlledList;
  item->getUsers(controlledList);
  for (FmModelMemberBase* obj : controlledList)
    if (obj->isOfType(FmHasDOFsBase::getClassTypeID())) {
      // Make sure joints/triads using this in several DOFs are added only once
      if (std::find(joints.begin(),joints.end(),obj) == joints.end()) {
        this->addTopologyItem(topology,obj,level);
        joints.push_back(obj);
      }
    }
    else if (!obj->isOfType(FmCurveSet::getClassTypeID()))
      // Curves are listed separately under "Plotted by"
      this->addTopologyItem(topology,obj,level);
}


void FapUAProperties::addTriadTopology(std::vector<FuiTopologyItem>& topol,
                                       FmLink* item, int level)
{
  std::vector<FmTriad*> triads;
  if (item) item->getTriads(triads);
  if (triads.empty()) return;

  this->addTopologyItem(topol,NULL,level,"Triads");

  for (FmTriad* triad : triads)
  {
    this->addTopologyItem(topol,triad,level+1);

    std::vector<FmJointBase*> joints;
    triad->getJointBinding(joints);

    for (FmJointBase* joint : joints) {
      this->addTopologyItem(topol,joint,level+2);
      this->addTopologyItem(topol,joint->getOtherLink(item),level+3);
    }
  }
}


void FapUAProperties::addJointDescendantTopology(std::vector<FuiTopologyItem>& topol,
                                                 FmJointBase* item, int level)
{
  if (!item) return;

  std::vector<FmTriad*> triads;
  item->getMasterTriads(triads);
  triads.insert(triads.begin(),item->getSlaveTriad());

  FmLink* owner;
  for (size_t i = 0; i < triads.size(); i++)
    if (triads[i])
    {
      this->addTopologyItem(topol,triads[i],level,i?"Master":"Slave");
      for (size_t j = 0; (owner = triads[i]->getOwnerLink(j)); j++)
	this->addTopologyItem(topol,owner,level+1);
    }

  IntVec dofs;
  item->getDOFs(dofs);
  const char* labels[] = {"Tx: ","Ty: ","Tz: ","Rx: ","Ry: ","Rz: "};

  bool firstDof = true;
  for (int d : dofs)
    if (item->getStatusOfDOF(d) >= FmHasDOFsBase::SPRING_CONSTRAINED)
    {
      if (firstDof) this->addTopologyItem(topol,NULL,-level,"Joint springs");
      this->addTopologyItem(topol,item->getSpringAtDOF(d),level+1,labels[d]);
      firstDof = false;
    }

  firstDof = true;
  for (int d : dofs)
    if (item->getStatusOfDOF(d) >= FmHasDOFsBase::SPRING_CONSTRAINED)
    {
      if (firstDof) this->addTopologyItem(topol,NULL,-level,"Joint dampers");
      this->addTopologyItem(topol,item->getDamperAtDOF(d),level+1,labels[d]);
      firstDof = false;
    }

  if (!item->isOfType(FmCamJoint::getClassTypeID()))
  {
    // Default cam formulation (contact) cannot have loads nor prescribed motions
    firstDof = true;
    for (int d : dofs)
      if (item->getStatusOfDOF(d) == FmHasDOFsBase::FREE ||
	  item->getStatusOfDOF(d) >= FmHasDOFsBase::FREE_DYNAMICS)
      {
	if (firstDof) this->addTopologyItem(topol,NULL,-level,"Joint loads");
	this->addTopologyItem(topol,item->getLoadAtDOF(d),level+1,labels[d]);
	firstDof = false;
      }

    firstDof = true;
    for (int d : dofs)
      if (item->getStatusOfDOF(d) == FmHasDOFsBase::PRESCRIBED)
      {
	if (firstDof) this->addTopologyItem(topol,NULL,-level,"Joint motions");
	this->addTopologyItem(topol,item->getMotionAtDOF(d),level+1,labels[d]);
	firstDof = false;
      }
  }

  if (level > 0) return;

  std::multimap<std::string,FFaFieldContainer*> referringObjs;
  std::multimap<std::string,FFaFieldContainer*>::iterator it;
  item->getReferringObjs(referringObjs);

  firstDof = true;
  for (it = referringObjs.begin(); it != referringObjs.end(); ++it)
    if (it->first.find("Joint") < it->first.size())
    {
      if (firstDof) this->addTopologyItem(topol,NULL,0,"Used by:");
      this->addTopologyItem(topol,dynamic_cast<FmModelMemberBase*>(it->second),1);
      firstDof = false;
    }
}


void FapUAProperties::getDBJointVariables(FmJointBase* item,
					  std::vector<FuiJointDOFValues>& uiVars)
{
  if (!item) return;

  uiVars.clear();

  const IntVec& allowableFuncTypes = FmFuncAdmin::getAllowableSprDmpFuncTypes();

  static FapUAQuery JvTransSpringQuery;
  static FapUAQuery JvRotSpringQuery;
  static FapUAQuery JvTransDamperQuery;
  static FapUAQuery JvRotDamperQuery;

  IntVec dofs;
  item->getDOFs(dofs);

  JvTransSpringQuery.clear();
  for (int ftype : allowableFuncTypes)
    JvTransSpringQuery.typesToFind[ftype] = true;
  JvTransSpringQuery.typesToFind[FmSpringChar::getClassTypeID()] = true;
  JvTransSpringQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransSpringFunction,bool&,FmModelMemberBase*);

  JvRotSpringQuery.clear();
  for (int ftype : allowableFuncTypes)
    JvRotSpringQuery.typesToFind[ftype] = true;
  JvRotSpringQuery.typesToFind[FmSpringChar::getClassTypeID()] = true;
  JvRotSpringQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyRotSpringFunction,bool&,FmModelMemberBase*);

  JvTransDamperQuery.clear();
  for (int ftype : allowableFuncTypes)
    JvTransDamperQuery.typesToFind[ftype] = true;
  JvTransDamperQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyTransDamperFunction,bool&,FmModelMemberBase*);

  JvRotDamperQuery.clear();
  for (int ftype : allowableFuncTypes)
    JvRotDamperQuery.typesToFind[ftype] = true;
  JvRotDamperQuery.verifyCB = FFaDynCB2S(FapUAProperties::verifyRotDamperFunction,bool&,FmModelMemberBase*);

  for (int dof : dofs)
  {
    FmJointMotion*  pm = dynamic_cast<FmJointMotion*>(item->getMotionAtDOF(dof));
    FmJointSpring* spr = item->getSpringAtDOF(dof);
    FmJointDamper* dmp = item->getDamperAtDOF(dof);

    FuiJointDOFValues jv;

    jv.myDofNo = dof;
    jv.myMotionType = item->getStatusOfDOF(dof);
    jv.myInitVel = item->getInitVel(dof,true);

    if (FmDofLoad* load = item->getLoadAtDOF(dof); load) {
      jv.myLoadVals.selectedEngine = load->getEngine();
      jv.myLoadVals.constValue     = load->getInitLoad();
      jv.freqDomain                = load->freqDomain.getValue();
    }
    jv.myLoadVals.isConstant       = (jv.myLoadVals.selectedEngine ? false : true);
    jv.myLoadVals.engineQuery      = FapUAEngineQuery::instance();

    jv.mySpringDCVals.useSpringLabels = (jv.myMotionType >= FmHasDOFsBase::SPRING_CONSTRAINED);
    jv.mySpringDCVals.fixedOrFree     = (jv.myMotionType <= FmHasDOFsBase::FIXED ||
					 jv.myMotionType == FmHasDOFsBase::FREE_DYNAMICS);

    jv.mySpringDCVals.variable = item->getJointVariable(dof);
    jv.mySpringDCVals.variableIsEditable = dof > 2 && !item->isContactElement();
    jv.mySpringDCVals.useAngularLabels = dof > 2;

    if (spr && (jv.myMotionType >= FmHasDOFsBase::SPRING_CONSTRAINED || !pm)) {
      jv.mySpringDCVals.useAsDeflection = spr->getInitLengthOrDefl(jv.mySpringDCVals.initLengtOrDeflection);
      jv.mySpringDCVals.selectedLengthEngine = spr->getLengthEngine();
    }
    else if (pm) {
      jv.mySpringDCVals.useAsDeflection = pm->getInitLengthOrDefl(jv.mySpringDCVals.initLengtOrDeflection);
      jv.mySpringDCVals.selectedLengthEngine = pm->getEngine();
      if (jv.myMotionType == FmHasDOFsBase::PRESCRIBED) {
        jv.myMotionType = FuiJointDOF::PRESCRIBED_DISP + pm->getMotionType();
        jv.mySpringDCVals.useAngularLabels = dof > 2 ? 1+pm->getMotionType() : -pm->getMotionType();
        jv.freqDomain = pm->freqDomain.getValue();
      }
    }
    jv.mySpringDCVals.engineQuery = FapUAEngineQuery::instance();
    jv.mySpringFSVals.functionQuery = dof < 3 ? &JvTransSpringQuery : &JvRotSpringQuery;

    if (spr) {
      jv.mySpringFSVals.constFunction       = spr->getInitStiff();
      jv.mySpringFSVals.selectedFunction    = spr->getSpringCharOrStiffFunction();
      jv.mySpringFSVals.selectedScaleEngine = spr->getScaleEngine();
    }
    jv.mySpringFSVals.engineQuery           = FapUAEngineQuery::instance();

    // Hide deformational damper toggle if no prescribed length engine on spring
    if (jv.mySpringDCVals.selectedLengthEngine) {
      jv.myDamperFCVals.showDefDamper = 1;
      jv.myDamperFCVals.isDefDamper = dmp ? dmp->isDefDamper.getValue() : false;
    }
    else {
      jv.myDamperFCVals.showDefDamper = -1;
      jv.myDamperFCVals.isDefDamper = false;
    }

    jv.myDamperFCVals.functionQuery = dof < 3 ? &JvTransDamperQuery : &JvRotDamperQuery;

    if (dmp) {
      jv.myDamperFCVals.constFunction       = dmp->getInitDamp();
      jv.myDamperFCVals.selectedFunction    = dmp->getFunction();
      jv.myDamperFCVals.selectedScaleEngine = dmp->getDampEngine();
    }
    jv.myDamperFCVals.engineQuery           = FapUAEngineQuery::instance();

    uiVars.push_back(jv);
  }
}


////////////////////////////////////////
//
// Setting values in DB :
//
//////////

void FapUAProperties::setDBValues(FFuaUIValues* values)
{
  if (!mySelectedFmItem) return;

  FuaPropertiesValues* pv = dynamic_cast<FuaPropertiesValues*> (values);
  if (!pv) return;

  if (FapUAProperties::setDBValues(mySelectedFmItem,pv))
    return this->updateUI();

  int selectedTab = pv->selectedTab;
  FmPart* part = dynamic_cast<FmPart*>(mySelectedFmItem);
  if (part && part->useGenericProperties.getValue())
    // Remap the selected tab index for Generic Parts:
    // Mass: 2->8, Stiffness: 3->7, CoG: 4->6, Hydrodynamics: 5, Advanced: 6->4
    if (selectedTab > 1 && selectedTab < 7) selectedTab = 10-selectedTab;

  // Now update the other items of current selection. Only the
  // currently selected tab will be updated for items with multiple tabs.
  for (FmModelMemberBase* item : mySelectedFmItems)
    FapUAProperties::setDBValues(item,pv,selectedTab);
}


bool FapUAProperties::setDBValues(FmModelMemberBase* fmItem,
                                  FuaPropertiesValues* pv, int selectedTab)
{
#ifdef FAP_DEBUG
  std::cout <<"FapUAProperties::setDBValues(): "
            << fmItem->getIdString() <<" "<< selectedTab << std::endl;
#endif

  bool changedDescr = false;

  if (pv->showHeading)
  {
    if (selectedTab < 0) // Don't update description for multi-selection
      if ((changedDescr = fmItem->setUserDescription(pv->myDescription)))
        fmItem->onChanged(); // for updating the object list view

    // Replace all non-alphanumeric characters in the tag by "_"
    std::string newTag(pv->myTag);
    for (char& c : newTag)
      if (!isalnum(c)) c = '_';

    if (fmItem->setTag(newTag))
    {
      if (newTag != pv->myTag)
        FFaMsg::dialog("Only alphanumeric characters (and '_') are allowed in tags."
                       "\nTherefore, the tag you specified is changed to \"" + newTag + "\".",
                       FFaMsg::OK);

      // Assign same tag to the underlying Engine for Control In/Out elements
      FmEngine* engine = NULL;
      if (fmItem->isOfType(FmcInput::getClassTypeID()))
        engine = static_cast<FmcInput*>(fmItem)->getEngine();
      else if (fmItem->isOfType(FmcOutput::getClassTypeID()))
        engine = static_cast<FmcOutput*>(fmItem)->getEngine();
      if (engine) engine->setTag(newTag);
    }
  }

  bool update = false;
  if (fmItem->isOfType(FmDamperBase::getClassTypeID()))
    {
      FmDamperBase* item = (FmDamperBase*)fmItem;

      item->isDefDamper = pv->myAxialDaForceValues.isDefDamper;

      if (pv->myAxialDaForceValues.isConstant)
        item->setInitDamp(pv->myAxialDaForceValues.constFunction);

      item->setFunction(static_cast<FmMathFuncBase*>(pv->myAxialDaForceValues.selectedFunction));

      item->setDampEngine(static_cast<FmEngine*>(pv->myAxialDaForceValues.selectedScaleEngine));
    }

  else if (fmItem->isOfType(FmSpringBase::getClassTypeID()))
    {
      FmSpringBase* item = (FmSpringBase*)fmItem;

      item->setInitLengthOrDefl(pv->myAxialSpringDefCalcValues.initLengtOrDeflection,
				pv->myAxialSpringDefCalcValues.useAsDeflection);

      item->setEngine(static_cast<FmEngine*>(pv->myAxialSpringDefCalcValues.selectedLengthEngine));

      if (pv->myAxialSprForceValues.isConstant)
        item->setInitStiff(pv->myAxialSprForceValues.constFunction);

      item->setSpringCharOrStiffFunction(pv->myAxialSprForceValues.selectedFunction);

      item->setScaleEngine(static_cast<FmEngine*>(pv->myAxialSprForceValues.selectedScaleEngine));
    }

  else if (fmItem->isOfType(FmSpringChar::getClassTypeID()))
    {
      FmSpringChar* item = (FmSpringChar*)fmItem;

      item->springStiffness.setValue(pv->mySpringCharValues.constantStiffness);
      item->springFunction.setPointer(static_cast<FmMathFuncBase*>(pv->mySpringCharValues.springFunction));

      item->deflectionMax.setValue(pv->mySpringCharValues.deflectionMax);
      item->deflectionMin.setValue(pv->mySpringCharValues.deflectionMin);
      item->forceMax.setValue(pv->mySpringCharValues.forceMax);
      item->forceMin.setValue(pv->mySpringCharValues.forceMin);

      item->deflectionMaxIsOn.setValue(pv->mySpringCharValues.useDeflectionMax);
      item->deflectionMinIsOn.setValue(pv->mySpringCharValues.useDeflectionMin);
      item->forceMaxIsOn.setValue(pv->mySpringCharValues.useForceMax);
      item->forceMinIsOn.setValue(pv->mySpringCharValues.useForceMin);

      item->yieldForceMaxEngine.setPointer(static_cast<FmEngine*>(pv->mySpringCharValues.yieldForceMaxEngine));
      item->yieldForceMinEngine.setPointer(static_cast<FmEngine*>(pv->mySpringCharValues.yieldForceMinEngine));
      item->yieldForceMaxIsOn.setValue(pv->mySpringCharValues.useYieldForceMax);
      item->yieldForceMinIsOn.setValue(pv->mySpringCharValues.useYieldForceMin);
      item->yieldForceMax.setValue(pv->mySpringCharValues.constantYieldForceMax);
      item->yieldForceMin.setValue(pv->mySpringCharValues.constantYieldForceMin);
      item->yieldDeflectionMaxIsOn.setValue(pv->mySpringCharValues.useYieldDeflectionMax);
      item->yieldDeflectionMax.setValue(pv->mySpringCharValues.yieldDeflectionMax);
    }

  else if (fmItem->isOfType(FmTriad::getClassTypeID()))
    {
      FmTriad* item = (FmTriad*)fmItem;

      if (pv->myTriadConnector > 0)
	item->updateConnector((FmTriad::ConnectorType)pv->myTriadConnector);

      bool updateIcon = false;
      int  dof, nDOFs = item->getNDOFs(true);

      if (selectedTab <= 0) {
	item->itsLocalDir.setValue((FmTriad::LocalDirection)pv->mySysDir);

	bool hadAddedMass = item->hasAddMass();
	for (dof = 0; dof < 3; dof++) {
	  item->setAddMass(dof,pv->myMass[0]);
	  item->setAddMass(dof+3,pv->myMass[1+dof]);
	}
	if (item->hasAddMass() != hadAddedMass)
	  updateIcon = true;
      }

      FmHasDOFsBase::DOFStatus motionType;
      for (dof = 0; dof < nDOFs && (size_t)dof < pv->myTriadVals.size(); dof++) {
        if (!(selectedTab < 0 || selectedTab == 2+dof)) continue;

	const FuiTriadDOFValues& tv = pv->myTriadVals[dof];
	item->setInitVel(dof,tv.myInitVel);
	if (tv.myMotionType > FuiTriadDOF::FREE_DYNAMICS)
	  motionType = FmHasDOFsBase::PRESCRIBED;
	else
	  motionType = (FmHasDOFsBase::DOFStatus)tv.myMotionType;
	if (item->setDOFStatus(dof,motionType))
	  updateIcon = true;
	if (motionType == FmHasDOFsBase::PRESCRIBED)
	{
	  // Set prescribed motion
	  bool forceCreate = ((tv.myLoadVals.isConstant &&
			       tv.myLoadVals.constValue != 0.0) ||
			      tv.myLoadVals.selectedEngine);

	  if (FmDofMotion* pm = item->getMotionAtDOF(dof,forceCreate); pm)
	  {
	    if (tv.myMotionType > FuiTriadDOF::PRESCRIBED_DISP)
	      pm->setMotionType(tv.myMotionType-FuiTriadDOF::PRESCRIBED_DISP);
	    else
	      pm->setMotionType(FmDofMotion::DEFLECTION);

	    if (tv.myLoadVals.isConstant)
	      pm->setInitMotion(tv.myLoadVals.constValue);
	    else
	      pm->freqDomain.setValue(tv.freqDomain);

	    pm->setEngine(static_cast<FmEngine*>(tv.myLoadVals.selectedEngine));
	  }
	}
	else if (motionType != FmHasDOFsBase::FIXED)
	{
	  // Set DOF load
	  bool hadLoad = item->hasLoad(dof);
	  bool forceCreate = ((tv.myLoadVals.isConstant &&
			       tv.myLoadVals.constValue != 0.0) ||
			      tv.myLoadVals.selectedEngine);

	  if (FmDofLoad* load = item->getLoadAtDOF(dof,forceCreate); load)
	  {
	    if (tv.myLoadVals.isConstant)
	      load->setInitLoad(tv.myLoadVals.constValue);
	    else
	      load->freqDomain.setValue(tv.freqDomain);

	    load->setEngine(static_cast<FmEngine*>(tv.myLoadVals.selectedEngine));
	  }
	  if (item->hasLoad(dof) != hadLoad)
	    updateIcon = true;
	}
      }

      item->mySaveVar.setValue(pv->myResToggles);

      if (updateIcon)
	fmItem->onChanged(); // for updating the triad icons
    }

  else if (fmItem->isOfType(FmPipeSurface::getClassTypeID()))
    {
      FmPipeSurface* item = (FmPipeSurface*)fmItem;

      if (item->getPipeRadius() != pv->pipeSurfaceRadius)
        item->setPipeRadius(pv->pipeSurfaceRadius);
    }

  else if (fmItem->isOfType(FmLoad::getClassTypeID()))
    {
      FmLoad* item = (FmLoad*)fmItem;

      item->setInitLoad(pv->myLoadMagnitude);
      item->setEngine(static_cast<FmEngine*>(pv->mySelectedLoadMagnitudeEngine));

      if (pv->myFromPointIsGlobal)
        item->setGlobalFromPoint(pv->myFromPoint);
      else
        item->setLocalFromPoint(pv->myFromPoint);

      if (pv->myToPointIsGlobal)
        item->setGlobalToPoint(pv->myToPoint);
      else
        item->setLocalToPoint(pv->myToPoint);
    }

  else if (fmItem->isOfType(FmHPBase::getClassTypeID()))
    {
      FmHPBase* item = (FmHPBase*)fmItem;

      item->setTransmissionRatio(pv->myHPRatio);
    }

  else if (fmItem->isOfType(FmJointBase::getClassTypeID()))
    {
      FmJointBase* item = (FmJointBase*)fmItem;

      if (selectedTab < 0)
	FapUAProperties::setDBJointVariables(item, pv->myJointVals);
      else if (selectedTab > 0)
      {
        int dofTab = selectedTab -
          (fmItem->isOfType(FmSMJointBase::getClassTypeID()) ? 2 : 1);
        if (dofTab >= 0)
          FapUAProperties::setDBJointVariables(item, pv->myJointVals, dofTab);
      }

      if (selectedTab <= 0) {
	if (item->getValidFrictionType() > 0) {
	  item->setFriction(static_cast<FmFrictionBase*>(pv->mySelectedFriction));
	  item->setFrictionDof(pv->myFrictionDof);
	}

	if (item->isOfType(FmCylJoint::getClassTypeID()))
        {
          if (pv->myIsScrewConnection)
            ((FmCylJoint*)item)->setAsScrewTransmission();
          else
            ((FmCylJoint*)item)->unsetAsScrewTransmission();
          ((FmCylJoint*)item)->setScrewRatio(pv->myScrewRatio);
        }
	else if (item->isOfType(FmCamJoint::getClassTypeID()))
        {
          ((FmCamJoint*)item)->setThickness(pv->myCamThickness);
          ((FmCamJoint*)item)->setWidth(pv->myCamWidth);
          ((FmCamJoint*)item)->setUsingRadialContact(pv->IAmRadialContact);
        }
	else if (item->isOfType(FmRevJoint::getClassTypeID()))
        {
          ((FmRevJoint*)item)->setHasTzDOF(pv->myIsDOF_TZ_legal);
        }
      }

      if (selectedTab < 0 || selectedTab == 2+(int)pv->myJointVals.size()) {
        if (item->isOfType(FmFreeJoint::getClassTypeID()))
        {
          item->rotFormulation = (FmJointBase::RotFormulationType)pv->mySelectedRotFormulation;
          item->rotSequence    = (FmJointBase::RotSequenceType)pv->mySelectedRotSequence;
          item->rotSpringCpl   = (FmJointBase::SpringCplType)pv->mySelectedRotSpringCpl;
          item->tranSpringCpl  = (FmJointBase::SpringCplType)pv->mySelectedTranSpringCpl;
        }
        else if (item->isOfType(FmBallJoint::getClassTypeID()))
        {
          item->rotFormulation = (FmJointBase::RotFormulationType)pv->mySelectedRotFormulation;
          item->rotSequence    = (FmJointBase::RotSequenceType)pv->mySelectedRotSequence;
          item->rotSpringCpl   = (FmJointBase::SpringCplType)pv->mySelectedRotSpringCpl;
        }
      }

      item->mySaveVar.setValue(pv->myResToggles);
    }

  else if (fmItem->isOfType(FmBeam::getClassTypeID()))
    {
      FmBeam* item = (FmBeam*)fmItem;

      // Detect visualization change
      char fdChange = false;

      FmBeamProperty* newP = dynamic_cast<FmBeamProperty*>(pv->mySelectedCS);
      FmBeamProperty* oldP = dynamic_cast<FmBeamProperty*>(item->getProperty());
      if (newP != oldP && (oldP || !item->getProperty())) {
        item->setProperty(newP);
	fdChange = 'y';
      }
      else if (!pv->myOrientation.equals(item->myLocalZaxis.getValue(),1.0e-6))
	fdChange = 'y';

      if (item->myVisualize3D.getValue() != pv->myVisualize3D)
	fdChange = 'P';
      if (item->myVisualize3DAngles.getValue() != pv->myVisualize3DAngles)
	fdChange = 'P';

      // Set values
      item->alpha1         = pv->myMassPropDamp;
      item->alpha2         = pv->myStifPropDamp;
      item->massScale      = pv->myScaleMass;
      item->stiffnessScale = pv->myScaleStiff;
      item->myLocalZaxis   = pv->myOrientation;
      item->myVisualize3D  = pv->myVisualize3D;
      item->myVisualize3DAngles = pv->myVisualize3DAngles;

      // Visualization change
      if (fdChange == 'P') {
        std::vector<FmBeam*> beams(1,item);

        // Force parent shaft into tristate (if any)
        FmShaft* parentShaft = dynamic_cast<FmShaft*>(item->getParentAssembly());
        if (parentShaft != NULL) {
          parentShaft->visualize3Dts = 1;
          FmDB::getAllBeams(beams,parentShaft);
        }

        // Force parent blade into tristate (if any)
        FmBlade* parentBlade = dynamic_cast<FmBlade*>(item->getParentAssembly());
        if (parentBlade != NULL) {
          parentBlade->visualize3Dts = 1;
          FmDB::getAllBeams(beams,parentBlade);
        }

        // Force parent tower into tristate (if any)
        FmTower* parentTower = dynamic_cast<FmTower*>(item->getParentAssembly());
        if (parentTower != NULL) {
          parentTower->visualize3Dts = 1;
          FmDB::getAllBeams(beams,parentTower);
        }

        // Force parent riser into tristate (if any)
        FmRiser* parentRiser = dynamic_cast<FmRiser*>(item->getParentAssembly());
        if (parentRiser != NULL) {
          parentRiser->visualize3Dts = 1;
          FmDB::getAllBeams(beams,parentRiser);
        }

        // Force parent jacket into tristate (if any)
        FmJacket* parentJacket = dynamic_cast<FmJacket*>(item->getParentAssembly());
        if (parentJacket != NULL) {
          parentJacket->visualize3Dts = 1;
          FmDB::getAllBeams(beams,parentJacket);
        }

        // Force parent soil pile into tristate (if any)
        FmSoilPile* parentSoilPile = dynamic_cast<FmSoilPile*>(item->getParentAssembly());
        if (parentSoilPile != NULL) {
          parentSoilPile->visualize3Dts = 1;
          FmDB::getAllBeams(beams,parentSoilPile);
        }

        // Then, remake the visualization of all subordinate beams of the parent assembly
        for (FmBeam* beam : beams) beam->draw();
      }
      else if (fdChange) // Only update the selected beam
	item->draw();
    }

  else if (fmItem->isOfType(FmPart::getClassTypeID()))
    {
      FmPart* item = (FmPart*)fmItem;

      bool isGenPart = pv->myLinkValues.useGenericPart;
      bool updateRFB = false;

      // Damping and dynamic properties

      if (selectedTab <= 0) {
	item->alpha1.setValue(pv->myLinkValues.massDamping);
	item->alpha2.setValue(pv->myLinkValues.stiffDamping);
	item->massScale.setValue(pv->myLinkValues.massScale);
	item->stiffnessScale.setValue(pv->myLinkValues.stiffScale);
      }

      // Reducer options

      if (selectedTab < 0 || (!isGenPart && selectedTab == 3)) {
	item->overrideChecksum.setValue(pv->myLinkValues.ignoreCheckSum);
	item->expandModeShapes.setValue(pv->myLinkValues.expandModeShapes);
	if (item->nGenModes.setValue(pv->myLinkValues.compModes)) updateRFB = true;
	if (item->useConsistentMassMatrix.setValue(pv->myLinkValues.consistentMassMx)) updateRFB = true;
	if (item->factorizeMassMxEigSol.setValue(!pv->myLinkValues.factorStiffMx)) updateRFB = true;
	item->recoveryMatrixSavePrecision.setValue((FmPart::FloatPrecision)pv->myLinkValues.recoveryMxPrec);
	if (item->tolFactorize.setValue(pv->myLinkValues.singularityCrit)) updateRFB = true;
	if (item->tolEigenval.setValue(pv->myLinkValues.eigValTolerance)) updateRFB = true;
      }

      // Distributed loads (not for multi-selection)

      if (selectedTab < 0) {
	item->myLoadFactors.setValue(pv->myLinkValues.loadFactors);
	item->myLoadDelays.setValue(pv->myLinkValues.loadDelays);
	item->myLoadEngines.clear();
	for (FmModelMemberBase* engine : pv->myLinkValues.selectedLoadEngines)
	  item->myLoadEngines.push_back(static_cast<FmEngine*>(engine),true);
      }

      // Visualization only (not for multi-selection)

      if (selectedTab < 0 && item->suppressInSolver.setValue(pv->myLinkValues.suppressInSolver))
      {
	updateRFB = true;
	item->updateTriadTopologyRefs(false,2); // Bugfix #195: No warning when no FE model yet
      }

      // FE <-> GP toggling (not for multi-selection)

      bool needMassPropUpdate = false;
      if (selectedTab < 0 && item->useGenericProperties.setValue(isGenPart))
      {
	updateRFB = true;
	needMassPropUpdate = isGenPart;

	// If switching from FE to GP, ask if we should use the FE mass properties for the GP

	if (isGenPart && item->myCalculateMass.getValue() == FmPart::EXPLICIT && item->isFELoaded())
	  if (FFaMsg::dialog("Do you want to use the mass properties "
			     "from the FE model on the Generic part?",
			     FFaMsg::YES_NO))
	    pv->myLinkValues.useCalculatedMass = FmPart::FROM_FEM;

	// Load and create visualization

	if (isGenPart && !item->isCADLoaded())
	{
	  Fui::noUserInputPlease();
	  FFaMsg::pushStatus("Loading CAD model");
	  item->openCadData();
	  FFaMsg::popStatus();
	  Fui::okToGetUserInput();
	}
	else if (!isGenPart && !item->isFELoaded())
	{
	  Fui::noUserInputPlease();
	  FFaMsg::pushStatus("Loading FE model");
	  item->openFEData();
	  FFaMsg::popStatus();
	  Fui::okToGetUserInput();
	}

	item->updateTriadTopologyRefs(false,2); // Bugfix #195: No warning when no FE model yet

	Fui::noUserInputPlease();
	FFaMsg::pushStatus("Creating Visualization");
	item->draw();
	FFaMsg::popStatus();
	Fui::okToGetUserInput();
      }
      else
        isGenPart = item->useGenericProperties.getValue();

      // GP properties

      if (selectedTab < 0 || (isGenPart && selectedTab == 8)) {
	item->mass.setValue(pv->myLinkValues.genericPartMass);
	item->inertia.setValue(pv->myLinkValues.genericPartInertia);
	item->myInertiaRef.setValue((FmPart::InertiaRef)pv->myLinkValues.inertiaRef);
      }

      if (selectedTab < 0 || (isGenPart && selectedTab == 7)) {
	item->myGenericPartStiffType.setValue((FmPart::GenericPartStiffType)pv->myLinkValues.genericPartStiffType);
	item->kt.setValue(pv->myLinkValues.genericPartKT);
	item->kr.setValue(pv->myLinkValues.genericPartKR);
      }

      if (selectedTab < 0 || (isGenPart && selectedTab == 6))
        item->condenseOutCoG.setValue(pv->myLinkValues.genericPartNoCGTriad);

      // Mass calculation properties (not for multi-selection)

      if (selectedTab < 0) {
	if (item->myCalculateMass.getValue() != pv->myLinkValues.useCalculatedMass)
	  needMassPropUpdate = true;
	else if (item->material.getPointer() != pv->myLinkValues.selectedMaterial)
	  needMassPropUpdate = true;

	item->myCalculateMass.setValue(static_cast<FmPart::MassCalculation>(pv->myLinkValues.useCalculatedMass));
	item->material = static_cast<FmMaterialProperty*>(pv->myLinkValues.selectedMaterial);
	if (needMassPropUpdate) item->updateMassProperties();
      }

      // Hydrodynamics options

      if (selectedTab < 0 || (isGenPart && selectedTab == 5))
	item->hasBuoyancy.setValue(pv->myLinkValues.buoyancy);

      // Meshing options (not for multi-selection)

      if (selectedTab < 0) {
	item->minSize.setValue(pv->myLinkValues.minSize);
	item->quality.setValue(pv->myLinkValues.quality);
      }

      // Advanced options

      if (selectedTab < 0 || selectedTab == 4) {
        item->myCSOption.setValue((FmLink::CoordSysOption)pv->myLinkValues.coordSysOption);
        item->myCentripOption.setValue((FmPart::CentripOption)pv->myLinkValues.centripOption);
        if (selectedTab < 0 || !isGenPart)
          item->recoveryDuringSolve.setValue(pv->myLinkValues.recoveryOption);
        if (!isGenPart)
          item->ignoreInRecovery.setValue(pv->myLinkValues.ignoreRecovery);
      }
      if (selectedTab < 0 && pv->myLinkValues.extResSwitch >= 0) {
        item->useExternalResFile.setValue(pv->myLinkValues.extResSwitch);
        item->externalResFileName.setValue(pv->myLinkValues.extResFileName);
      }

      // Nonlinear options (not for multi-selection)

      if (selectedTab < 0) {
	item->useNonlinearReduction.setValue(pv->myLinkValues.useNonlinearSwitch);
	item->numberOfNonlinearSolutions.setValue(pv->myLinkValues.numNonlinear);
	item->nonlinearDataFileName.setValue(pv->myLinkValues.nonlinearInputFile);
      }

      if (updateRFB)
	fmItem->onChanged(); // for updating the result file browser
    }

  else if (fmItem->isOfType(FmUserDefinedElement::getClassTypeID()))
    {
      FmUserDefinedElement* item = (FmUserDefinedElement*)fmItem;

      // Damping and dynamic properties
      bool changed = changedDescr;
      changed |= item->alpha1.setValue(pv->myMassPropDamp);
      changed |= item->alpha2.setValue(pv->myStifPropDamp);
      changed |= item->massScale.setValue(pv->myScaleMass);
      changed |= item->stiffnessScale.setValue(pv->myScaleStiff);

      // Clear the work array if properties has changed
      // to trigger reinitialization
      if (changed) item->clearWork();
    }

  else if (fmItem->isOfType(FmRefPlane::getClassTypeID()))
    {
      FmRefPlane* item = (FmRefPlane*)fmItem;
      item->setWidth(pv->myRefPlaneWidth);
      item->setHeight(pv->myRefPlaneHeight);
    }

  else if (fmItem->isOfType(FmGenericDBObject::getClassTypeID()))
    {
      FmGenericDBObject* item = (FmGenericDBObject*)fmItem;
      item->objectType = pv->myGenDBObjType;
      item->objectDefinition = pv->myGenDBObjDef;
    }

  else if (fmItem->isOfType(FmFileReference::getClassTypeID()))
    {
      FmFileReference* item = (FmFileReference*)fmItem;

      if (!changedDescr)
	// Description hasn't changed, update with current file name if
	// only using the file name as description (which is the default)
	if (pv->myDescription == FFaFilePath::getFileName(item->fileName.getValue()))
	{
	  fmItem->setUserDescription(FFaFilePath::getFileName(pv->myFileReferenceName));
	  fmItem->onChanged(); // for updating the object list view
	}

      item->fileName = pv->myFileReferenceName;
    }

  else if (fmItem->isOfType(FmTire::getClassTypeID()))
    {
      FmTire* item = (FmTire*)fmItem;

      item->spindelTriadOffset = pv->mySpindelTriadOffset;
      item->tireType = pv->mySelectedTireModelType;
      // Choose API based on the selected tire model type
      item->tireAPI = item->tireType.getValue() == "FTIRE" ? "CTI" : "STI";

      item->tireDataFileName = pv->myTireDataFileName;
      item->tireDataFileRef = static_cast<FmFileReference*>(pv->mySelectedTireDataFileRef);
      item->road = static_cast<FmRoad*>(pv->mySelectedRoad);
      item->draw();
    }

  else if (fmItem->isOfType(FmRoad::getClassTypeID()))
    {
      FmRoad* item = (FmRoad*)fmItem;

      item->roadFunction = static_cast<FmMathFuncBase*>(pv->mySelectedRoadFunc);
      item->roadZShift = pv->myRoadZShift;
      item->roadXOffset = pv->myRoadXOffset;
      item->roadZRotation = pv->myRoadZRotation;
      item->useExtRoadDataFile = pv->iAmUsingExtRoadData;
      item->roadDataFileName = pv->myRoadDataFileName;
      item->roadDataFileRef = static_cast<FmFileReference*>(pv->mySelectedRoadDataFileRef);
    }

  // Material property

  else if (fmItem->isOfType(FmMaterialProperty::getClassTypeID()))
    {
      FmMaterialProperty* item = (FmMaterialProperty*)fmItem;

      if (!item->updateProperties(pv->myMatPropRho,pv->myMatPropE,pv->myMatPropG,pv->myMatPropNu))
	FFaMsg::dialog("Invalid material parameters.\nPoisson ratio (v) "
		       "must be in range [0,0.5>.",FFaMsg::OK);
    }

  // Sea State

  else if (fmItem->isOfType(FmSeaState::getClassTypeID()))
    {
      FmSeaState* item = (FmSeaState*)fmItem;
      bool redraw = false;

      redraw |= item->setLengthX(pv->mySeaStateWidth,false);
      redraw |= item->setLengthY(pv->mySeaStateHeight,false);

      redraw |= item->setX(pv->mySeaStateWidthPos,false);
      redraw |= item->setY(pv->mySeaStateHeightPos,false);

      redraw |= item->setShowGrid(pv->mySeaStateShowGrid,false);
      redraw |= item->setShowSolid(pv->mySeaStateShowSolid,false);

      redraw |= item->setQuantization(pv->mySeaStateNumPoints,false);

      if (redraw) item->draw();
    }

  // Beam property

  else if (fmItem->isOfType(FmBeamProperty::getClassTypeID()))
    {
      FmBeamProperty* item = (FmBeamProperty*)fmItem;

      item->crossSectionType = static_cast<FmBeamProperty::FmSection>(pv->myBeamCrossSectionType);
      item->material = static_cast<FmMaterialProperty*>(pv->mySelectedMaterial);
      item->Do = pv->myBeamProp[0];
      item->Di = pv->myBeamProp[1];
      item->EA = pv->myBeamProp[2];
      item->EI = std::make_pair(pv->myBeamProp[3],pv->myBeamProp[4]);
      item->GAs = std::make_pair(pv->myBeamProp[14],pv->myBeamProp[15]);
      item->GIt = pv->myBeamProp[5];
      item->Mass = pv->myBeamProp[6];
      item->breakDependence = pv->myBeamBreakDependence;
      if (item->crossSectionType.getValue() == FmBeamProperty::GENERIC)
	item->RoIp = pv->myBeamProp[7];
      else if (pv->myBeamBreakDependence) {
	item->A  = pv->myBeamProp[8];
	item->Iy = pv->myBeamProp[9];
	item->Iz = pv->myBeamProp[10];
	item->Ip = pv->myBeamProp[11];
      }
      else
	item->updateDependentValues();
      item->ShrRed = std::make_pair(pv->myBeamProp[12],pv->myBeamProp[13]);
      item->ShrCentre = std::make_pair(pv->myBeamProp[16],pv->myBeamProp[17]);
      item->hydroToggle = pv->myBeamHydroToggle;
      item->Db = pv->myHydroProp[0];
      item->Dd = pv->myHydroProp[1];
      item->Cd = pv->myHydroProp[2];
      item->Ca = pv->myHydroProp[3];
      item->Cm = pv->myHydroProp[4];
      item->Cd_axial = pv->myHydroProp[5];
      item->Ca_axial = pv->myHydroProp[6];
      item->Cm_axial = pv->myHydroProp[7];
      item->Cd_spin = pv->myHydroProp[8];
      item->Di_hydro = pv->myHydroProp[9];

      // Update visualization of all beams that uses this cross-section
      std::vector<FmBeam*> beams;
      item->getReferringObjs(beams,"myProp");
      for (FmBeam* beam : beams) beam->draw();
    }

  else if (fmItem->isOfType(FmStrainRosette::getClassTypeID()))
    {
      FmStrainRosette* item = (FmStrainRosette*)fmItem;

      int rosType = 0;
      for (const std::string& rosName : FmStrainRosette::getRosetteUINames())
        if (pv->mySelectedRosetteType == rosName) {
	  item->rosetteType = (FmStrainRosette::RosetteType)rosType;
	  break;
	}
	else
	  rosType++;

      item->angle = pv->myStrRosAngle * M_PI/180.0;
      item->removeStartStrains = pv->IAmResettingStartStrains;

      // Resync is needed if toggeling on using data from FE model
      bool needReSync = ((pv->IAmUsingFEHeight && !item->useFEThickness.getValue()) ||
			 (pv->IAmUsingFEMaterial && !item->useFEMaterial.getValue()));

      // Edit the user data only when the "use FE data" toggles are off
      if (!pv->IAmUsingFEHeight && !item->useFEThickness.getValue())
        item->zPos = pv->myStrRosHeight;

      if (!pv->IAmUsingFEMaterial && !item->useFEMaterial.getValue()) {
        item->EMod = pv->myStrRosEMod;
        item->nu = pv->myStrRosNu;
      }

      item->useFEThickness = pv->IAmUsingFEHeight;
      item->useFEMaterial = pv->IAmUsingFEMaterial;

      if (needReSync)
        item->syncWithFEModel();

      item->draw();
    }

  else if (fmItem->isOfType(FmElementGroupProxy::getClassTypeID()))
    {
      FmElementGroupProxy* item = (FmElementGroupProxy*)fmItem;

      item->toggleFatigue(pv->doFatigue);
      item->myFatigueSNCurve = pv->mySNCurve;
      item->myFatigueSNStd = pv->mySNStd;
      item->myFatigueSCF = pv->mySCF;
    }

  else if (fmItem->isOfType(FmVesselMotion::getClassTypeID()))
    {
      FmVesselMotion* item = (FmVesselMotion*)fmItem;

      char updateWave = 0;
      std::string oldRAOfile = item->getActualRAOFileName();
      item->raoFile = pv->myRAOFileName;
      item->raoFileRef = static_cast<FmFileReference*>(pv->mySelectedRAOFileRef);
      if (oldRAOfile != item->getActualRAOFileName())
        updateWave = 'R';

      if (item->setWaveDir(pv->mySelectedWaveDir))
	updateWave = 'R';

      if (pv->mySelectedWaveFunc != item->waveFunction.getPointer()) {
	item->waveFunction = static_cast<FmMathFuncBase*>(pv->mySelectedWaveFunc);
	updateWave = 'W';
      }

      if (updateWave == 'W') // Regenerate the wave function
	item->onWaveChanged();
      else if (updateWave == 'R') // Only the RAO file or wave direction has changed
	item->onRAOChanged();

      update = item->scaleMotions(static_cast<FmEngine*>(pv->mySelectedScale));
    }

  else if (fmItem->isOfType(FmSimulationEvent::getClassTypeID()))
    {
      FmSimulationEvent* item = (FmSimulationEvent*)fmItem;

      item->setProbability(pv->mySimEventProbability);
    }

  else if (dynamic_cast<FmShaft*>(fmItem) != NULL)
    {
      FmShaft* item = (FmShaft*)fmItem;

      // Detect visualization change
      bool fdChange = item->visualize3Dts.getValue() != pv->myVisualize3Dts;

      // Set value
      item->visualize3Dts = pv->myVisualize3Dts;

      // Set the properties on all subordinate beams
      std::vector<FmBeam*> beams;
      FmDB::getAllBeams(beams,item);
      for (FmBeam* beam : beams) {
        // Set properties
        if ((pv->myBlankFieldsFlags & 0x0001) == 0) {
          FmBeamProperty* pCS = static_cast<FmBeamProperty*>(pv->mySelectedCS);
          if (pCS != beam->getProperty())
            fdChange = true;
	  beam->setProperty(pCS);
        }
	if ((pv->myBlankFieldsFlags & 0x0002) == 0)
	  beam->alpha1.setValue(pv->myMassPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0004) == 0)
	  beam->alpha2.setValue(pv->myStifPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0008) == 0)
	  beam->massScale.setValue(pv->myScaleMass);
	if ((pv->myBlankFieldsFlags & 0x0010) == 0)
	  beam->stiffnessScale.setValue(pv->myScaleStiff);
        // Visualization change.
        // Note: The visualization scene graph needs to be "remade".
        if (fdChange)
          beam->draw();
      }
    }

  else if (dynamic_cast<FmTurbine*>(fmItem) != NULL)
    {
      FmTurbine* item = (FmTurbine*)fmItem;

      // Set topology
      item->topology[0] = pv->myTurbineWindRefTriadDefSelected;
      item->topology[1] = pv->myTurbineYawPointTriadDefSelected;
      item->topology[2] = pv->myTurbineHubApexTriadDefSelected;
      item->topology[3] = pv->myTurbineHubPartDefSelected;
      item->topology[4] = pv->myTurbineFirstBearingDefSelected;

      // Set vert offset
      item->ptfmRef.setValue(pv->myTurbineWindVertOffset);
    }

  else if (dynamic_cast<FmGenerator*>(fmItem) != NULL)
    {
      FmGenerator* item = (FmGenerator*)fmItem;

      // Get the subordinate joint properties
      std::vector<FmModelMemberBase*> elmJoints;
      FmDB::getAllOfType(elmJoints, FmJointBase::getClassTypeID(), item);
      if (!elmJoints.empty())
      {
        FmJointBase* pJoint = static_cast<FmJointBase*>(elmJoints.front());

        // Update the Rz DOF status
        bool torque = pv->myGeneratorTorqueControl;
	if (pJoint->getStatusOfDOF(5) != FmHasDOFsBase::FREE_DYNAMICS &&
	    pJoint->getStatusOfDOF(5) != FmHasDOFsBase::PRESCRIBED)
	  ListUI <<" ==> WARNING: Changing DOF status for "<< pJoint->getIdString(true)
		 <<" from "<< FmHasDOFsBase::DOFStatusEnum(pJoint->getStatusOfDOF(5)).getText()
		 <<" to "<< (torque ? "FREE_DYNAMICS" : "PRESCRIBED") <<".\n";
	pJoint->setStatusForDOF(5, torque ? FmHasDOFsBase::FREE_DYNAMICS : FmHasDOFsBase::PRESCRIBED);

        // Set Torque Control load value
	bool forceCreate = ((pv->myGeneratorTorqueControlIsConstant &&
			     pv->myGeneratorTorqueControlConstValue != 0.0) ||
			    pv->myGeneratorTorqueControlSelectedEngine);

	if (FmDofLoad* load = pJoint->getLoadAtDOF(5,forceCreate); load) {
	  if (pv->myGeneratorTorqueControlIsConstant)
	    load->setInitLoad(pv->myGeneratorTorqueControlConstValue);
	  load->setEngine(static_cast<FmEngine*>(pv->myGeneratorTorqueControlSelectedEngine));
	}

        // Set Velocity Control load value
	forceCreate = ((pv->myGeneratorVelocityControlIsConstant &&
			pv->myGeneratorVelocityControlConstValue != 0.0) ||
		       pv->myGeneratorVelocityControlSelectedEngine);

	if (FmDofMotion* pm = pJoint->getMotionAtDOF(5,forceCreate); pm) {
	  pm->setMotionType(FmDofMotion::VELOCITY);
	  if (pv->myGeneratorVelocityControlIsConstant)
	   pm->setInitMotion(pv->myGeneratorVelocityControlConstValue);
	  pm->setEngine(static_cast<FmEngine*>(pv->myGeneratorVelocityControlSelectedEngine));
	}
      }
    }

  else if (dynamic_cast<FmTower*>(fmItem) != NULL)
    {
      FmTower* item = (FmTower*)fmItem;

      // Detect visualization change
      bool fdChange = item->visualize3Dts.getValue() != pv->myVisualize3Dts;

      // Set value
      item->visualize3Dts = pv->myVisualize3Dts;

      if (fdChange) {
	// Update visualization all subordinate links.
        // Note: The visualization scene graph needs to be "remade".
	std::vector<FmLink*> links;
	FmDB::getAllLinks(links,item);
	for (FmLink* link : links) link->draw();
      }
    }

  else if (dynamic_cast<FmBlade*>(fmItem) != NULL)
    {
      FmBlade* item = (FmBlade*)fmItem;

      // Detect visualization change
      bool fdChange = item->visualize3Dts.getValue() != pv->myVisualize3Dts;

      // Set values
      item->visualize3Dts.setValue(pv->myVisualize3Dts);
      item->IceLayer.setValue(pv->myBladeIceLayer);
      item->IceThickness.setValue(pv->myBladeIceThickness);

      // Get pitch joint
      if (FmJointBase* pJoint = item->getPitchJoint(); pJoint) {

        // Update the Rz DOF status
        bool fixed = pv->myBladePitchIsFixed;
        if (pJoint->getStatusOfDOF(5) != FmHasDOFsBase::FIXED &&
            pJoint->getStatusOfDOF(5) != FmHasDOFsBase::PRESCRIBED)
          ListUI <<" ==> WARNING: Changing DOF status for "<< pJoint->getIdString(true)
                 <<" from "<< FmHasDOFsBase::DOFStatusEnum(pJoint->getStatusOfDOF(5)).getText()
                 <<" to "<< (fixed ? "FIXED" : "PRESCRIBED") <<".\n";
        pJoint->setStatusForDOF(5, fixed ? FmHasDOFsBase::FIXED : FmHasDOFsBase::PRESCRIBED);

        // Set Deflection Control load value
	bool forceCreate = ((pv->myBladePitchControlIsConstant &&
			     pv->myBladePitchControlConstValue != 0.0) ||
			    pv->myBladePitchControlSelectedEngine);

	if (FmDofMotion* pm = pJoint->getMotionAtDOF(5,forceCreate); pm) {
	  pm->setMotionType(FmDofMotion::DEFLECTION);
	  if (pv->myBladePitchControlIsConstant)
	    pm->setInitMotion(pv->myBladePitchControlConstValue);
	  pm->setEngine(static_cast<FmEngine*>(pv->myBladePitchControlSelectedEngine));
	}
      }

      // Set the properties on all subordinate links
      std::vector<FmBeam*> beams;
      FmDB::getAllBeams(beams,item);
      for (FmBeam* beam : beams) {
        // Set properties
	if ((pv->myBlankFieldsFlags & 0x0002) == 0)
	  beam->alpha1.setValue(pv->myMassPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0004) == 0)
	  beam->alpha2.setValue(pv->myStifPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0008) == 0)
	  beam->massScale.setValue(pv->myScaleMass);
	if ((pv->myBlankFieldsFlags & 0x0010) == 0)
	  beam->stiffnessScale.setValue(pv->myScaleStiff);
        // Visualization change.
        // Note: The visualization scene graph needs to be "remade".
        if (fdChange)
          beam->draw();
      }
    }

  else if (dynamic_cast<FmRiser*>(fmItem) != NULL)
    {
      FmRiser* item = (FmRiser*)fmItem;

      item->myModelFile = pv->mySubAssemblyFile;

      // Detect visualization change
      bool fdChange = item->visualize3Dts.getValue() != pv->myVisualize3Dts;
      fdChange |= item->visualize3DAngles.getValue() != pv->myVisualize3DAngles;

      // Set values
      item->visualize3Dts = pv->myVisualize3Dts;
      item->visualize3DAngles = pv->myVisualize3DAngles;
      item->internalMud = pv->myIntFluid;
      item->mudDensity = pv->myIntFluidDensity;
      item->mudLevel = pv->myIntFluidLevel;

      // Set the properties on all subordinate links
      std::vector<FmLink*> links;
      FmDB::getAllLinks(links,item);
      for (FmLink* link : links) {
        // Set properties
	if ((pv->myBlankFieldsFlags & 0x0002) == 0)
	  link->alpha1.setValue(pv->myMassPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0004) == 0)
	  link->alpha2.setValue(pv->myStifPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0008) == 0)
	  link->massScale.setValue(pv->myScaleMass);
	if ((pv->myBlankFieldsFlags & 0x0010) == 0)
	  link->stiffnessScale.setValue(pv->myScaleStiff);
        // Visualization change.
        // Note: The visualization scene graph needs to be "remade".
        if (fdChange)
          link->draw();
      }
    }

  else if (dynamic_cast<FmJacket*>(fmItem) != NULL)
    {
      FmJacket* item = (FmJacket*)fmItem;

      item->myModelFile = pv->mySubAssemblyFile;

      // Detect visualization change
      bool fdChange = item->visualize3Dts.getValue() != pv->myVisualize3Dts;

      // Set value
      item->visualize3Dts = pv->myVisualize3Dts;

      // Set the properties on all subordinate links
      std::vector<FmLink*> links;
      FmDB::getAllLinks(links,item);
      for (FmLink* link : links) {
        // Set properties
	if ((pv->myBlankFieldsFlags & 0x0002) == 0)
	  link->alpha1.setValue(pv->myMassPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0004) == 0)
	  link->alpha2.setValue(pv->myStifPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0008) == 0)
	  link->massScale.setValue(pv->myScaleMass);
	if ((pv->myBlankFieldsFlags & 0x0010) == 0)
	  link->stiffnessScale.setValue(pv->myScaleStiff);
        // Visualization change.
        // Note: The visualization scene graph needs to be "remade".
        if (fdChange)
          link->draw();
      }
    }

  else if (dynamic_cast<FmSoilPile*>(fmItem) != NULL)
    {
      FmSoilPile* item = (FmSoilPile*)fmItem;

      item->myModelFile = pv->mySubAssemblyFile;

      // Detect visualization change
      bool fdChange = item->visualize3Dts.getValue() != pv->myVisualize3Dts;
      fdChange |= item->visualize3DAngles.getValue() != pv->myVisualize3DAngles;

      // Set values
      item->visualize3Dts = pv->myVisualize3Dts;
      item->visualize3DAngles = pv->myVisualize3DAngles;
      item->internalSoil = pv->myIntFluid;
      item->soilDensity = pv->myIntFluidDensity;

      // Set the properties on all subordinate links
      std::vector<FmLink*> links;
      FmDB::getAllLinks(links,item);
      for (FmLink* link : links) {
        // Set properties
	if ((pv->myBlankFieldsFlags & 0x0002) == 0)
	  link->alpha1.setValue(pv->myMassPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0004) == 0)
	  link->alpha2.setValue(pv->myStifPropDamp);
	if ((pv->myBlankFieldsFlags & 0x0008) == 0)
	  link->massScale.setValue(pv->myScaleMass);
	if ((pv->myBlankFieldsFlags & 0x0010) == 0)
	  link->stiffnessScale.setValue(pv->myScaleStiff);
        // Visualization change.
        // Note: The visualization scene graph needs to be "remade".
        if (fdChange)
          link->draw();
      }
    }

  else if (fmItem->isOfType(FmSubAssembly::getClassTypeID()))
    {
      FmSubAssembly* item = (FmSubAssembly*)fmItem;

      item->myModelFile = pv->mySubAssemblyFile;
    }

  else
    return false;

  // Indicate that the model has changed (for the necessity for save, etc.)
  FpPM::touchModel();
  return update;
}


void FapUAProperties::setDBJointVariables(FmJointBase* item,
                                          const std::vector<FuiJointDOFValues>& uiVars,
					  int dofToUpdate)
{
  if (!item) return;

  IntVec dofs;
  item->getDOFs(dofs);

  int    idof = -1;
  bool   updateRot = false;
  FaVec3 angles;
  for (const FuiJointDOFValues& jdof : uiVars)
  {
    if (dofToUpdate >= 0 && ++idof != dofToUpdate) continue;
    if (std::find(dofs.begin(),dofs.end(),jdof.myDofNo) == dofs.end()) continue;

    if (jdof.myDofNo >= 3)
    {
      angles[jdof.myDofNo-3] = jdof.mySpringDCVals.variable;
      updateRot = true;
    }

    FmJointSpring* spr    = item->getSpringAtDOF(jdof.myDofNo);
    FmJointDamper* dmp    = item->getDamperAtDOF(jdof.myDofNo);
    FmJointMotion* motion = dynamic_cast<FmJointMotion*>(item->getMotionAtDOF(jdof.myDofNo));
    bool hadLengthControl = spr != NULL || motion != NULL;

    FmHasDOFsBase::DOFStatus motionType = (FmHasDOFsBase::DOFStatus)jdof.myMotionType;
    if (motionType > FmHasDOFsBase::SPRING_DYNAMICS)
      motionType = FmHasDOFsBase::PRESCRIBED;

    std::string usedAsFuncArg;
    if (motionType < FmHasDOFsBase::SPRING_CONSTRAINED)
    {
      if (spr && spr->isMeasured())
        usedAsFuncArg = "spring";

      if (dmp && dmp->isMeasured())
      {
        if (usedAsFuncArg.empty())
          usedAsFuncArg = "damper";
        else
          usedAsFuncArg += " and damper";
      }
    }

    if (usedAsFuncArg.empty())
      item->setStatusForDOF(jdof.myDofNo,motionType);
    else
      FFaMsg::dialog("Can not disable " + usedAsFuncArg +
                     " used as argument in Functions.",FFaMsg::OK);

    item->setInitVel(jdof.myDofNo,jdof.myInitVel);

    if (motionType != FmHasDOFsBase::FIXED &&
        motionType != FmHasDOFsBase::PRESCRIBED)
    {
      // Set joint DOF load
      FmDofLoad* load = item->getLoadAtDOF(jdof.myDofNo,true);
      if (jdof.myLoadVals.isConstant)
        load->setInitLoad(jdof.myLoadVals.constValue);
      else
        load->freqDomain.setValue(jdof.freqDomain);

      load->setEngine(static_cast<FmEngine*>(jdof.myLoadVals.selectedEngine));
    }

    if (motionType >= FmHasDOFsBase::SPRING_CONSTRAINED)
    {
      // Set spring motion
      spr = item->getSpringAtDOF(jdof.myDofNo,true);
      if (hadLengthControl) // Bugfix #508: Use default DB values if no length control yet
      {
        spr->setInitLengthOrDefl(jdof.mySpringDCVals.initLengtOrDeflection,
                                 jdof.mySpringDCVals.useAsDeflection);
        spr->setEngine(static_cast<FmEngine*>(jdof.mySpringDCVals.selectedLengthEngine));
      }

      // Also update the prescribed motion, if any
      if (motion && motion->getMotionType() == FmDofMotion::DEFLECTION)
      {
        motion->setInitLengthOrDefl(jdof.mySpringDCVals.initLengtOrDeflection,
                                    jdof.mySpringDCVals.useAsDeflection);

        motion->setEngine(static_cast<FmEngine*>(jdof.mySpringDCVals.selectedLengthEngine));
      }

      // Set spring properties
      if (jdof.mySpringFSVals.isConstant)
        spr->setInitStiff(jdof.mySpringFSVals.constFunction);

      // Set spring characteristics pointer
      spr->setSpringCharOrStiffFunction(jdof.mySpringFSVals.selectedFunction);
      spr->setScaleEngine(static_cast<FmEngine*>(jdof.mySpringFSVals.selectedScaleEngine));

      // Set damper properties
      dmp = item->getDamperAtDOF(jdof.myDofNo,true);
      dmp->isDefDamper = jdof.myDamperFCVals.isDefDamper;
      if (jdof.myDamperFCVals.isConstant)
        dmp->setInitDamp(jdof.myDamperFCVals.constFunction);

      dmp->setFunction(static_cast<FmMathFuncBase*>(jdof.myDamperFCVals.selectedFunction));
      dmp->setDampEngine(static_cast<FmEngine*>(jdof.myDamperFCVals.selectedScaleEngine));
    }

    if (motionType == FmHasDOFsBase::PRESCRIBED)
    {
      // Set prescribed motion

      motion = static_cast<FmJointMotion*>(item->getMotionAtDOF(jdof.myDofNo,true));
      if (jdof.myMotionType > FuiJointDOF::PRESCRIBED_DISP)
        motion->setMotionType(jdof.myMotionType-FuiJointDOF::PRESCRIBED_DISP);
      else
        motion->setMotionType(FmDofMotion::DEFLECTION);

      motion->setInitLengthOrDefl(jdof.mySpringDCVals.initLengtOrDeflection,
                                  jdof.mySpringDCVals.useAsDeflection);
      motion->setEngine(static_cast<FmEngine*>(jdof.mySpringDCVals.selectedLengthEngine));
      motion->freqDomain.setValue(jdof.freqDomain);

      // Also update the spring motion, if any
      if (spr && motion->getMotionType() == FmDofMotion::DEFLECTION)
      {
        spr->setInitLengthOrDefl(jdof.mySpringDCVals.initLengtOrDeflection,
                                 jdof.mySpringDCVals.useAsDeflection);

        spr->setEngine(static_cast<FmEngine*>(jdof.mySpringDCVals.selectedLengthEngine));
      }
    }
  }

  if (updateRot)
    item->setRotJointVariables(angles);
}


////////////////////////////////
//
// Mode State Updating :
//
///////

void FapUAProperties::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
					     const std::vector<FFaViewItem*>&,
					     const std::vector<FFaViewItem*>&)
{
  if (IAmIgnoringPickNotify)
    return;
  else if (IAmIgnoringPickNotifyNotCurves) {
    if (totalSelection.empty()) return;
    if (!dynamic_cast<FmCurveSet*>(totalSelection.back())) return;
  }

  // Find the (first) selected model member object
  // and then all other selected objects of same type
  mySelectedFmItem = NULL;
  mySelectedFmItems.clear();
  for (FFaViewItem* item : totalSelection)
    if (!mySelectedFmItem)
      mySelectedFmItem = dynamic_cast<FmModelMemberBase*>(item);
    else if (FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
             mmb && mmb->getTypeID() == mySelectedFmItem->getTypeID())
      mySelectedFmItems.push_back(mmb);

  // If multi-selection, show the last selected object
  // which is of same type as the first selected object
  if (!mySelectedFmItems.empty())
    std::swap(mySelectedFmItems.back(),mySelectedFmItem);

  FmMMJointBase::editedMaster = NULL;
  FmLoad::editedLoad = NULL;
  myTopologyViewList.clear();
  this->updateUI();
}


void FapUAProperties::onModelMemberChanged(FmModelMemberBase* changedObj)
{
  if (changedObj->isOfType(FmMathFuncBase::getClassTypeID()))
  {
    FmSeaState* sea = FmDB::getSeaStateObject(false);
    if (sea && changedObj == sea->waveFunction.getPointer())
      sea->draw(); // Updates visualization of sea when the wave function is changed
  }
  else if (changedObj == mySelectedFmItem)
  {
    if (changedObj->isOfType(FmCurveSet::getClassTypeID()))
      this->updateUI();
    else if (changedObj->isOfType(FmPart::getClassTypeID()))
      this->updateUI(); // for sensitivity updates during reduction
  }
}


//////////////////////////
//
// Callbacks :
//
////

void FapUAProperties::topologyHighlightCB(int i, bool onOrOff)
{
  if (i < 0 || (size_t)i >= myTopologyViewList.size()) return;

  FmIsRenderedBase* item = dynamic_cast<FmIsRenderedBase*>(myTopologyViewList[i]);
  if (item) item->highlight(onOrOff);
}


void FapUAProperties::onTopViewRightClick(int i, std::vector<FFuaCmdItem*>& cmds)
{
  if (i < 0 || (size_t)i >= myTopologyViewList.size()) return;
  if (!myTopologyViewList[i]) return;

  FapEventManager::tmpSelect(myTopologyViewList[i]);
  cmds.push_back(FFuaCmdItem::getCmdItem("cmdId_viewCtrl_zoomTo"));
  cmds.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_selectTempSelection"));
  cmds.push_back(FFuaCmdItem::getCmdItem("cmdId_edit_selectTempSelectionAll"));
}


namespace
{
  bool selectObject(FmModelMemberBase* item)
  {
    if (!item) return false;

    if (FmPart* link = dynamic_cast<FmPart*>(item); link && link->isEarthLink())
      return false; // Don't select the earth link

    switch (FuiModes::getMode()) {
    case FuiModes::EXAM_MODE:
    case FuiModes::ERASE_MODE:
      FapEventManager::permTotalSelect(item);
    default:
      break;
    }

    return true;
  }
}


void FapUAProperties::topologyActivatedCB(int i)
{
  if (i >= 0 && i > static_cast<int>(myTopologyViewList.size()))
    selectObject(myTopologyViewList[i]);
}


void FapUAProperties::onQIFieldButtonCBS(FuiQueryInputFieldValues& v)
{
  if (FmEngine* item = dynamic_cast<FmEngine*>(v.selectedRef);
      item && item->isControlOutEngine() &&
      selectObject(dynamic_cast<FmcOutput*>(item->getSensor()->getMeasured())))
    return;

  selectObject(v.selectedRef);
}


void FapUAProperties::onQIFieldButtonCB(FuiQueryInputFieldValues& v)
{
  FapUAProperties::onQIFieldButtonCBS(v);
}


void FapUAProperties::addMasterCB()
{
  FmMMJointBase* item = dynamic_cast<FmMMJointBase*>(mySelectedFmItem);
  if (!item) return;

  IAmIgnoringPickNotify = true;
  FmMMJointBase::editedMaster = item->getMaster();
  FuiModes::setMode(FuiModes::ADDMASTERINLINJOINT_MODE);
}


void FapUAProperties::swapMasterSlaveCB()
{
  FmSMJointBase* item = dynamic_cast<FmSMJointBase*>(mySelectedFmItem);
  if (!item) return;

  if (!item->swapMasterAndSlave())
    return;

  item->draw();
  this->updateUIValues();
}


void FapUAProperties::reverseMasterCB()
{
  FmMMJointBase* item = dynamic_cast<FmMMJointBase*>(mySelectedFmItem);
  if (!item) return;

  Fm1DMaster* master = item->getMaster();
  if (!master) return;

  master->reverseTriads();

  item->draw();
  this->updateUIValues();
}


void FapUAProperties::swapJointCB()
{
  FmJointBase* newJoint = NULL;
  if (mySelectedFmItem->isOfType(FmPrismJoint::getClassTypeID()))
    newJoint = new FmCylJoint();
  else if (mySelectedFmItem->isOfType(FmCylJoint::getClassTypeID()))
    newJoint = new FmPrismJoint();
  else
    return;

  FmJointBase* oldJoint = static_cast<FmJointBase*>(mySelectedFmItem);
  newJoint->clone(mySelectedFmItem,FmBase::DEEP_REPLACE);
  newJoint->connect();
  oldJoint->eraseKeepDOFs();
  FapEventManager::permTotalSelect(newJoint);

  newJoint->draw();
  this->updateUIValues();
}


void FapUAProperties::setAllDofs(bool fixed)
{
  FmHasDOFsBase* item = dynamic_cast<FmHasDOFsBase*>(mySelectedFmItem);
  if (!item) return;

  std::vector<FmHasDOFsBase*> changed;
  changed.reserve(1+mySelectedFmItems.size());
  // Lambda function updating the DOF status for an item.
  auto&& setDofStatus = [&changed](FmHasDOFsBase* item, bool fixed)
  {
    if (!item->setStatusForAllDOFs(fixed))
      return;
    else if (item->isOfType(FmTriad::getClassTypeID()))
      changed.push_back(item);
    else
      FpPM::touchModel();
  };

  setDofStatus(item,fixed);

  // Update the multi-selected items, if any
  for (FmModelMemberBase* obj : mySelectedFmItems)
    if ((item = dynamic_cast<FmHasDOFsBase*>(obj)))
      setDofStatus(item,fixed);

  // Issue #120: Delay the onChanged() call since it may change
  // the mySelectedFmItems list before all items are processed
  for (FmHasDOFsBase* item : changed)
    item->onChanged(); // to update triad icon

  this->updateUIValues();
}


void FapUAProperties::linkMeshCB(bool parabolic)
{
  FmPart* part = dynamic_cast<FmPart*>(mySelectedFmItem);
  if (!part) return;

  // Check that we have some geometry to mesh
  if (part->baseCadFileName.getValue().empty() &&
      part->visDataFile.getValue().empty()) return;

  if (!FapLicenseManager::checkLicense("FA-MSH"))
    return;

  // Check if we already have an FE data file for this part
  // and prompt user before overwriting it
  std::string ftlFile = part->getBaseFTLFile();
  if (!ftlFile.empty() && FpFileSys::isFile(ftlFile))
    if (!FFaMsg::dialog("An FE mesh already exists for this part.\n"
			"Do you want to replace it with a new mesh?",
			FFaMsg::YES_NO)) return;

  // Go ahead and create a new mesh
  Fui::noUserInputPlease();
  FFaMsg::pushStatus("Generating FE mesh");
  bool meshed = part->createFEData(parabolic);
  FFaMsg::popStatus();
  Fui::okToGetUserInput();

  if (meshed)
  {
    part->updateTriadTopologyRefs(true,2);

    Fui::noUserInputPlease();
    FFaMsg::pushStatus("Creating Visualization");
    part->draw();
    FFaMsg::popStatus();
    Fui::okToGetUserInput();
  }

  part->onChanged();
}


void FapUAProperties::linkChangeCB()
{
  FapFileCmds::changeLink();
  this->updateUI();
}


namespace
{
  using FilterItem = std::pair<const std::string,Strings>;
  using FileFilter = std::vector<FilterItem>;

  bool browseDataFile(std::string& fName, const std::string& type,
                      const FileFilter& filter, bool allFilesFilter = false)
  {
    const std::string& path = FmDB::getMechanismObject()->getAbsModelFilePath();
    FFuFileDialog* dialog = FFuFileDialog::create(path,"FileDialog",
                                                  FFuFileDialog::FFU_OPEN_FILE);

    dialog->setTitle(("Select " + type + " file").c_str());
    dialog->addAllFilesFilter(allFilesFilter);
    bool lFirst = true;
    for (const FilterItem& ff : filter)
    {
      if (ff.second.size() == 1)
        dialog->addFilter(ff.first,ff.second.front(),lFirst);
      else if (ff.second.size() > 1)
        dialog->addFilter(ff.first,ff.second,lFirst);
      lFirst = false;
    }

    bool isRelativeNameWanted = FFaFilePath::isRelativePath(fName);

    if (!fName.empty())
    {
      // Set the default file name to the original file, if the directory of
      // that file does not exist, assume current model file directory instead
      FFaFilePath::checkName(fName);
      FFaFilePath::makeItAbsolute(fName,path);
      if (!FpFileSys::verifyDirectory(FFaFilePath::getPath(fName),false))
        FFaFilePath::setPath(fName,path);
      dialog->setDefaultName(fName);
    }

    dialog->addUserToggle("relToggle",
                          "Use path relative to model-file location",
                          isRelativeNameWanted);
    dialog->remember((type + "BrowseField").c_str());

    Strings files = dialog->execute();
    bool useRelativePath = dialog->getUserToggleSet("relToggle");
    delete dialog;

    if (files.empty()) return false;

    if (useRelativePath)
      fName = FFaFilePath::getRelativeFilename(path,files.front());
    else
      fName = files.front();

    return true;
  }
}


void FapUAProperties::linkChangeVizCB(const std::string& file, bool promptFile)
{
  FmPart* part = dynamic_cast<FmPart*>(mySelectedFmItem);
  if (!part) return;

  std::string fileName;
  if (promptFile)
  {
    fileName = part->visDataFile.getValue();

    static FileFilter filter = {
      { "VRML model",                 { "vrml", "vrl", "wrl", "wrz" } },
      { "Fedem Technology CAD model", { "ftc" } },
      { "Wavefront OBJ model",        { "obj" } }
    };
    if (!browseDataFile(fileName,"visualization",filter)) return;
  }
  else if (!file.empty())
  {
    fileName = file;
    std::string tmp(FFaFilePath::checkName(fileName));
    FFaFilePath::makeItAbsolute(tmp,FmDB::getMechanismObject()->getAbsModelFilePath());
    if (!FpFileSys::isReadable(tmp))
    {
      FFaMsg::dialog("Non-existing visualization file:\n" + fileName);
      this->updateUIValues();
      return;
    }
  }

  Fui::noUserInputPlease();
  bool changed = part->setVisualizationFile(fileName);
  Fui::okToGetUserInput();
  this->updateUIValues();

  if (changed) FpPM::touchModel();
}


void FapUAProperties::tireDataFileBrowseCB()
{
  FmTire* tire = dynamic_cast<FmTire*>(mySelectedFmItem);
  if (!tire) return;

  std::string fileName = tire->tireDataFileName.getValue();

  static FileFilter filter = {
    { "TNO Tire file",      { "tpf" } },
    { "JD Tire file",       { "jdt" } },
    { "Tire property file", { "tir" } }
  };
  if (!browseDataFile(fileName,"tire",filter,true)) return;

  tire->tireDataFileName.setValue(fileName);
  tire->tireDataFileRef.setPointerToNull();
  tire->tireType.setValue("");
  tire->updateFromFile();
  tire->draw();

  this->updateUIValues();
  FpPM::touchModel();
}


void FapUAProperties::roadDataFileBrowseCB()
{
  FmRoad* road = dynamic_cast<FmRoad*>(mySelectedFmItem);
  if (!road) return;

  std::string fileName = road->roadDataFileName.getValue();

  static FileFilter filter = {
    { "Road property file",    { "rdf" } },
    { "JD Road property file", { "jdr" } }
  };
  if (!browseDataFile(fileName,"road",filter,true)) return;

  road->roadDataFileName.setValue(fileName);
  road->roadDataFileRef.setPointerToNull();

  this->updateUIValues();
  FpPM::touchModel();
}


void FapUAProperties::raoFileBrowseCB()
{
  FmVesselMotion* vm = dynamic_cast<FmVesselMotion*>(mySelectedFmItem);
  if (!vm) return;

  std::string fileName = vm->raoFile.getValue();

  static FileFilter filter = { { "RAO file", { "rao" } } };
  if (!browseDataFile(fileName,"RAO",filter,true)) return;

  vm->raoFile.setValue(fileName);
  vm->raoFileRef.setPointerToNull();

  this->updateUIValues();
  FpPM::touchModel();
}


void FapUAProperties::simEventSelectedCB()
{
#ifdef FT_HAS_GRAPHVIEW
  FapUACurveDefine::clearCachedDBCurve();
#endif

  FmSimulationEvent* event = dynamic_cast<FmSimulationEvent*>(mySelectedFmItem);
  if (!event) return;

  if (FapSimEventHandler::activate(event))
    FFaMsg::list("===> Switching to " + event->getIdString(true) + "\n");
  else if (FapSimEventHandler::activate(NULL))
    FFaMsg::list("===> Reverted to master event.\n");
}


void FapUAProperties::dofStatusToggledCB(int dof, int stat)
{
  FmHasDOFsBase* item = dynamic_cast<FmHasDOFsBase*>(mySelectedFmItem);
  if (!item || dof < 0) return;

  const size_t idof = static_cast<size_t>(dof);

  std::vector<FmHasDOFsBase*> changed;
  changed.reserve(1+mySelectedFmItems.size());
  // Lambda function updating the DOF status for an item.
  auto&& setDofStatus = [&changed](FmHasDOFsBase* item, int dof, int stat)
  {
    if (!item->setStatusForDOF(dof,stat))
      return;
    else if (item->isOfType(FmTriad::getClassTypeID()))
      changed.push_back(item);
    else
      FpPM::touchModel();
  };

  IntVec dofs;
  item->getDOFs(dofs);
  if (dofs.size() > idof)
    setDofStatus(item,dofs[idof],stat);

  // Update the multi-selected items, if any
  for (FmModelMemberBase* obj : mySelectedFmItems)
    if ((item = dynamic_cast<FmHasDOFsBase*>(obj)))
      if (item->getDOFs(dofs); dofs.size() > idof)
        setDofStatus(item,dofs[idof],stat);

  // Issue #120: Delay the onChanged() call since it may change the
  // mySelectedFmItems list before all items are processed
  for (FmHasDOFsBase* item : changed)
    item->onChanged(); // to update triad icon
}


////////////////////////////////////
//
// Load Callbacks :
//
//////

void FapUAProperties::loadViewAttackPointCB(bool doHighlight)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (!item) return;

#ifdef USE_INVENTOR
  static long highlightID = -1;
  if (doHighlight)
    highlightID = FdPickedPoints::add2DPoint(item->getOwnerTriad()->getGlobalTranslation());
  else
    FdPickedPoints::remove2DPoint(highlightID);
#else
  std::cout <<"FapUAProperties::loadViewAttackPointCB("
            << std::boolalpha << doHighlight <<") does nothing."<< std::endl;
#endif
}


void FapUAProperties::loadViewAttackWhatCB(bool doHighlight)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (!item) return;

  FmTriad* attackTriad = item->getOwnerTriad();
  if (attackTriad && attackTriad->getOwnerLink())
    attackTriad->getOwnerLink()->highlight(doHighlight);
}


void FapUAProperties::loadPickAttackPointCB()
{
  if ((FmLoad::editedLoad = dynamic_cast<FmLoad*>(mySelectedFmItem)))
  {
    IAmIgnoringPickNotify = true;
    FuiModes::setMode(FuiModes::PICKLOADATTACKPOINT_MODE);
  }
}


void FapUAProperties::applyAttackPointCB(bool isGlobal, FaVec3 point)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (!item) return;

  if (isGlobal)
    item->moveAttackPointGlobal(point);
  else
    item->moveAttackPointLocal(point);

  this->updateUI();
}


void FapUAProperties::loadViewFromPointCB(bool doHighlight)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (!item) return;

#ifdef USE_INVENTOR
  static long highlightID = -1;
  if (doHighlight)
    highlightID = FdPickedPoints::add2DPoint(item->getGlobalFromPoint());
  else
    FdPickedPoints::remove2DPoint(highlightID);
#else
  std::cout <<"FapUAProperties::loadViewFromPointCB("
            << std::boolalpha << doHighlight <<") does nothing."<< std::endl;
#endif
}


void FapUAProperties::loadViewFromWhatCB(bool doHighlight)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (item && item->getFromRef())
    item->getFromRef()->highlight(doHighlight);
}


void FapUAProperties::loadPickFromPointCB()
{
  if ((FmLoad::editedLoad = dynamic_cast<FmLoad*>(mySelectedFmItem)))
  {
    IAmIgnoringPickNotify = true;
    FuiModes::setMode(FuiModes::PICKLOADFROMPOINT_MODE);
  }
}


void FapUAProperties::loadViewToPointCB(bool doHighlight)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (!item) return;

#ifdef USE_INVENTOR
  static long highlightID = -1;
  if (doHighlight)
    highlightID = FdPickedPoints::add2DPoint(item->getGlobalToPoint());
  else
    FdPickedPoints::remove2DPoint(highlightID);
#else
  std::cout <<"FapUAProperties::loadViewToPointCB("
            << std::boolalpha << doHighlight <<") does nothing."<< std::endl;
#endif
}


void FapUAProperties::loadViewToWhatCB(bool doHighlight)
{
  FmLoad* item = dynamic_cast<FmLoad*>(mySelectedFmItem);
  if (item && item->getToRef())
    item->getToRef()->highlight(doHighlight);
}


void FapUAProperties::loadPickToPointCB()
{
  if ((FmLoad::editedLoad = dynamic_cast<FmLoad*>(mySelectedFmItem)))
  {
    IAmIgnoringPickNotify = true;
    FuiModes::setMode(FuiModes::PICKLOADTOPOINT_MODE);
  }
}


//! CB to verify that \a item is translational damper function of allowable type

void FapUAProperties::verifyTransDamperFunction(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  if (FmMathFuncBase* func = dynamic_cast<FmMathFuncBase*>(item); func)
    if (func->isLegalSprDmpFunc())
      isOK = (func->getFunctionUse() == FmMathFuncBase::DA_TRA_COEFF ||
              func->getFunctionUse() == FmMathFuncBase::DA_TRA_FORCE);
}


//! CB to verify that \a item is rotational damper function of allowable type

void FapUAProperties::verifyRotDamperFunction(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  if (FmMathFuncBase* func = dynamic_cast<FmMathFuncBase*>(item); func)
    if (func->isLegalSprDmpFunc())
      isOK = (func->getFunctionUse() == FmMathFuncBase::DA_ROT_COEFF ||
              func->getFunctionUse() == FmMathFuncBase::DA_ROT_TORQUE);
}


//! CB to verify that \a item is translational spring function of allowable type

void FapUAProperties::verifyTransSpringFunction(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  if (FmMathFuncBase* func = dynamic_cast<FmMathFuncBase*>(item); func)
  {
    if (func->isLegalSprDmpFunc())
      if (func->getFunctionUse() == FmMathFuncBase::SPR_TRA_STIFF ||
	  func->getFunctionUse() == FmMathFuncBase::SPR_TRA_FORCE)
	isOK = true;
  }
  else if (FmSpringChar* sprc = dynamic_cast<FmSpringChar*>(item); sprc)
    if (sprc->getSpringCharUse() == FmSpringChar::TRANSLATION)
      isOK = true;
}


//! CB to verify that \a item is rotational spring function of allowable type

void FapUAProperties::verifyRotSpringFunction(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  if (FmMathFuncBase* func = dynamic_cast<FmMathFuncBase*>(item); func)
  {
    if (func->isLegalSprDmpFunc())
      if (func->getFunctionUse() == FmMathFuncBase::SPR_ROT_STIFF ||
	  func->getFunctionUse() == FmMathFuncBase::SPR_ROT_TORQUE)
	isOK = true;
  }
  else if (FmSpringChar* sprc = dynamic_cast<FmSpringChar*>(item); sprc)
    if (sprc->getSpringCharUse() == FmSpringChar::ROTATION)
      isOK = true;
}
