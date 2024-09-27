// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAFunctionProperties.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmApp/vpmAppUAMap/FapUACurveDefine.H"
#endif
#include "vpmApp/vpmAppUAMap/FapUAProperties.H"
#include "vpmApp/vpmAppUAMap/FapUAQuery.H"
#ifdef FT_HAS_PREVIEW
#include "vpmApp/vpmAppCmds/FapGraphCmds.H"
#endif
#include "vpmApp/vpmAppCmds/FapCreateSensorCmd.H"
#include "vpmApp/FapEventManager.H"
#include "vpmApp/FapLicenseManager.H"

#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"

#include "vpmUI/vpmUIComponents/FuiFunctionProperties.H"
#include "FFuLib/FFuFileDialog.H"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmEngine.H"
#if defined(FT_HAS_GRAPHVIEW) || defined(FT_HAS_PREVIEW)
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmGraph.H"
#endif
#include "vpmDB/FmFuncAdmin.H"
#include "vpmDB/FmFrictionBase.H"
#include "vpmDB/FmfMultiVarBase.H"
#include "vpmDB/FmfLinVelVar.H"
#include "vpmDB/FmfMathExpr.H"
#include "vpmDB/FmfDeviceFunction.H"
#include "vpmDB/FmfExternalFunction.H"
#include "vpmDB/FmfWaveSinus.H"
#include "vpmDB/FmfUserDefined.H"
#include "vpmDB/FmVesselMotion.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmFileReference.H"
#include "vpmDB/FmcInput.H"
#include "vpmDB/FmcOutput.H"

#include "vpmPM/FpPM.H"
#include "vpmPM/FpFileSys.H"


Fmd_SOURCE_INIT(FAPUAFUNCTIONPROPERTIES, FapUAFunctionProperties, FapUAExistenceHandler);


FapUAFunctionProperties::FapUAFunctionProperties(FuiFunctionProperties* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUAFunctionProperties);

  mySelectedFmItem = NULL;
  myFunctionPropertiesUI = uic;
  FuaFunctionPropertiesValues pv;

  pv.myFuncTypeSwitchedCB = FFaDynCB1M(FapUAFunctionProperties,this,funcTypeSwitchedCB,int);

#ifdef FT_HAS_PREVIEW
  pv.myPreviewCB = FFaDynCB0M(FapUAFunctionProperties,this,previewCB);
#endif

  pv.myAddNumberCB  = FFaDynCB1M(FapUAFunctionProperties,this,addNumberCB,double);
  pv.myAddNumbersCB = FFaDynCB2M(FapUAFunctionProperties,this,addNumbersCB,double,double);
  pv.myPasteCB      = FFaDynCB1M(FapUAFunctionProperties,this,pasteCB,const std::string&);
  pv.myDeleteCB     = FFaDynCB1M(FapUAFunctionProperties,this,deleteNumberCB,int);
  pv.myClearAllCB   = FFaDynCB0M(FapUAFunctionProperties,this,clearNumbersCB);

  pv.myGetChannelListCB = FFaDynCB0M(FapUAFunctionProperties,this,getChannelListCB);

  pv.myEditLinkFunctionCB = FFaDynCB1S(FapUAProperties::onQIFieldButtonCBS,FuiQueryInputFieldValues&);

  pv.myPickSensorCB = FFaDynCB1M(FapUAFunctionProperties,this,pickSensorCB,int);

  pv.myAcceptExprCB = FFaDynCB0M(FapUAFunctionProperties,this,acceptExprCB);

  pv.myFilesBrowseCB = FFaDynCB0M(FapUAFunctionProperties,this,onFileBrowseActivated);

  uic->setCBs(&pv);
}


FFuaUIValues* FapUAFunctionProperties::createValuesObject()
{
  return new FuaFunctionPropertiesValues();
}


void FapUAFunctionProperties::getDBValues(FFuaUIValues* values)
{
  if (!mySelectedFmItem) return;

  FuaFunctionPropertiesValues* pv = dynamic_cast<FuaFunctionPropertiesValues*> (values);
  if (!pv) return;

  FmEngine* e = this->getMyEngine();
  FmMathFuncBase* f = e ? NULL : dynamic_cast<FmMathFuncBase*>(mySelectedFmItem);

  // Engine

  if (e) {

    if (e->isFunctionLinked()) {
      pv->showLinkToFields = true;

      // Linked to function from other engine
      FmEngine* linkedFrom = e->getEngineToLinkFunctionFrom();
      if (linkedFrom == e)
	linkedFrom = NULL;
      else if (linkedFrom)
	f = linkedFrom->getFunction();

      static FapUAQuery query2;
      query2.clear();
      query2.typesToFind[FmEngine::getClassTypeID()] = true;
      query2.verifyCB = FFaDynCB2M(FapUAFunctionProperties,this,
				   verifyGeneralFunctionCB,bool&,FmModelMemberBase*);

      pv->myLinkFunctionQuery = &query2;
      pv->mySelectedLinkFunction = linkedFrom;
    }
    else
      f = e->getFunction();

    // Argument(s)

    unsigned int nArgs = 0;
    if (e->isDriveFile() || mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
      pv->myArgumentValues.clear();
    else if (f && f->isOfType(FmfExternalFunction::getClassTypeID()))
      pv->myArgumentValues.clear();
    else {

      static FapUAQuery query;
      query.clear();
      query.typesToFind[FmSensorBase::getClassTypeID()] = true;
      query.verifyCB = FFaDynCB2M(FapUAFunctionProperties,this,
				  verifyArgumentSensor,bool&,FmModelMemberBase*);

      nArgs = f ? f->getNoArgs() : 1;

      pv->showArgument = true;
      pv->myArgumentValues.resize(nArgs);
      for (unsigned int j = 0; j < nArgs; j++) {
	FmSensorBase* sensor = e->getSensor(j);
	pv->myArgumentValues[j].objectSelectorQuery = &query;
	pv->myArgumentValues[j].selectedObject = sensor;
	if (sensor) {

	  std::vector<FmSensorChoice> choices;
          size_t i;

	  // Dof choice :

	  sensor->getSensorDofs(choices);
	  pv->myArgumentValues[j].showDofList = !choices.empty();
	  pv->myArgumentValues[j].selectedDofIdx = 0;
	  pv->myArgumentValues[j].dofList.clear();
	  for (i = 0; i < choices.size(); i++) {
	    pv->myArgumentValues[j].dofList.push_back(choices[i].second);
	    if (e->getDof(j) == choices[i].first)
	      pv->myArgumentValues[j].selectedDofIdx = i;
	  }

	  // Variable choice :

	  sensor->getSensorEntities(choices,e->getDof(j));
	  pv->myArgumentValues[j].showVariableList = !choices.empty();
	  pv->myArgumentValues[j].selectedVariableIdx = 0;
	  pv->myArgumentValues[j].variableList.clear();
	  for (i = 0; i < choices.size(); i++) {
	    pv->myArgumentValues[j].variableList.push_back(choices[i].second);
	    if (sensor->isExternalCtrlSys()) {
	      if (e->getEntityName(j) == choices[i].second)
		pv->myArgumentValues[j].selectedVariableIdx = i;
	    }
	    else
	      if (e->getEntity(j) == choices[i].first)
		pv->myArgumentValues[j].selectedVariableIdx = i;
	  }
	}
      }
    }

    // Output indicator threshold

    if ((nArgs > 0 && !mySelectedFmItem->isOfType(FmcInput::getClassTypeID())) ||
        mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
      if (FapLicenseManager::hasFeature("FA-SAP"))
      {
        pv->useAsOutputSensor = e->myOutput.getValue();
        const FmThreshold& dtt = e->myThreshold.getValue();
        if (dtt.isDefined())
        {
          pv->myThreshold.active = dtt.isActive();
          pv->myThreshold.threshold = dtt.threshold;
          pv->myThreshold.min = dtt.min;
          pv->myThreshold.skip = dtt.skip;
          pv->myThreshold.severity = dtt.severity-1;
          pv->myThreshold.description = dtt.description;
        }
      }

    // Disallow topology modification for objects referred by simulation events
    if (e->isEventModified()) pv->allowTopolChange = false;
  }

  // Set Help pixmap for function or friction

  FmParamObjectBase* pObj = e || f ? f : dynamic_cast<FmParamObjectBase*>(mySelectedFmItem);
  if (pObj && !pObj->isOfType(FmfExternalFunction::getClassTypeID())) {
    pv->showParameterView = pObj->isOfType(FmFrictionBase::getClassTypeID());
#ifdef FT_HAS_PREVIEW
    pv->showCurvePreview = pObj->isOfType(FmMathFuncBase::getClassTypeID());
#endif
    pv->myHelpPixmap = pObj->getPixmap();
  }

  if (pv->useAsOutputSensor > 0 && (pv->myHelpPixmap || pv->showCurvePreview))
    pv->useAsOutputSensor = 2; // Showing threshold fields in separate tab

  // Do the function things, either as an embedded or "native",
  // As long as we actually have an engine or a function:

  if (e || f) {

    bool isFunctionLinked = false;
    bool isFunctionLinkedTo = false;
    if (e) {
      isFunctionLinked = e->isFunctionLinked();
      std::vector<FFaFieldContainer*> fLinkingToThis;
      e->getReferringObjs(fLinkingToThis,"myFunctionOwner");
      if (fLinkingToThis.size() == 1 && fLinkingToThis.front() == e)
	isFunctionLinkedTo = false;
      else
	isFunctionLinkedTo = !fLinkingToThis.empty();
    }

    // Get function type list
    std::vector<FmFuncTypeInfo> funcTypes;
    FmFuncAdmin::getCompatibleFunctionTypes(funcTypes, isFunctionLinked ? NULL : f);

    if (funcTypes.size() > 1) pv->showTypeSwitch = true;

    int selectedFuncType = FFaTypeCheck::NO_TYPE_ID;
    if (f) selectedFuncType = f->getTypeID();

    // Build list of function names and find the current function type list index

    pv->mySelectedFunctionTypeIdx = -1;
    int idxSkip = 0;
    for (size_t i = 0; i < funcTypes.size(); i++)  {

      // Add the texts in same order as given by getCompatibleFunctionTypes.
      // Leave the reference one, if this function is used as a shape reference.
      if (isFunctionLinkedTo && funcTypes[i].funcMenuEnum == FmFuncAdmin::REFERENCE)
	idxSkip = 1;
      else
	pv->myFunctionTypes.push_back(funcTypes[i].listName);

      if (pv->mySelectedFunctionTypeIdx >= 0) continue;

      if (isFunctionLinked) {
	if (funcTypes[i].funcMenuEnum == FmFuncAdmin::REFERENCE)
	  pv->mySelectedFunctionTypeIdx = i-idxSkip;
      }
      else if (!f) {
	if (funcTypes[i].funcMenuEnum == FmFuncAdmin::NONE)
	  pv->mySelectedFunctionTypeIdx = i-idxSkip;
      }
      else if (funcTypes[i].getFuncType() == selectedFuncType) {
	pv->mySelectedFunctionTypeIdx = i-idxSkip;
	FmfUserDefined* udf = dynamic_cast<FmfUserDefined*>(f);
	if (udf && udf->getFunctionUse() != FmMathFuncBase::WAVE_FUNCTION) // assuming there is only one user-defined wave function in each plugin
	  pv->mySelectedFunctionTypeIdx += udf->getFuncNo()-1;
      }
    }

    if (!f) return; // no function

    // External file view

    if (f->isOfType(FmfDeviceFunction::getClassTypeID())) {

      pv->showFileView = true;

      FmfDeviceFunction* df = static_cast<FmfDeviceFunction*>(f);

      pv->fileRefQuery    = FapUAFileRefQuery::instance();
      pv->selectedFileRef = df->getFileReference();
      pv->isMultiChannel  = df->getDevice(pv->fileName,pv->myChannelName);
      pv->myScaleFactor   = df->scaleFactor.getValue();
      pv->myVerticalShift = df->verticalShift.getValue();
      pv->myZeroAdjust    = df->zeroAdjust.getValue();
      pv->myJonswapHs     = df->getHs();
      pv->myJonswapTp     = df->getTz();
      pv->myJonswapRandomSeed = df->randomSeed.getValue();
    }

    // External function view

    else if (f->isOfType(FmfExternalFunction::getClassTypeID())) {

      FmfExternalFunction* xf = static_cast<FmfExternalFunction*>(f);

      pv->myExtFunc       = xf->channel.getValue();
      pv->myScaleFactor   = xf->scale.getValue();
      pv->myVerticalShift = xf->shift.getValue();
    }

    // Scrolled list of values

    else if (f->isOfType(FmfMultiVarBase::getClassTypeID())) {

      FmfMultiVarBase* mvf = static_cast<FmfMultiVarBase*>(f);
      pv->showParameterList = mvf->getBlockSize();

      mvf->getFirstValues(pv->myXValues);
      mvf->getSecondValues(pv->myYValues);
      pv->myExtrapolationType = mvf->getExtrapolationType();
    }

    // User defined mathematical expression

    else if (f->isOfType(FmfMathExpr::getClassTypeID())) {

      pv->showMathExpr = true;

      pv->myExpression = static_cast<FmfMathExpr*>(f)->getExpressionString();
      if (!e) pv->myArgumentValues.resize(f->getNoArgs());
    }

    // Jonswap

    else if (f->isOfType(FmfWaveSpectrum::getClassTypeID())) {

      pv->showJonswapView = true;

      FmfWaveSpectrum* ws = static_cast<FmfWaveSpectrum*>(f);

      pv->myJonswapHs = ws->myHs.getValue();
      pv->myJonswapTp = ws->myTp.getValue();
      pv->myJonswapRange = ws->myTrange.getValue();
      pv->myJonswapAutoCalcCutoff = ws->autoCalcTrange.getValue();
      pv->myJonswapAutoCalcSpectralPeakedness = ws->autoCalcPeakedness.getValue();
      pv->myJonswapSpectralPeakedness = ws->myPeakedness.getValue();
      pv->myJonswapNComp = ws->nComp.getValue();
      pv->myJonswapRandomSeed = ws->myRandomSeed.getValue();
      pv->myJonswapNDir = ws->nDir.getValue();
      pv->myJonswapSprExp = ws->sprExp.getValue();
    }

    // Parameter view

    else
      pv->showParameterView = true;

    // Disallow topology modification for objects referred by simulation events
    if (f->isEventModified()) pv->allowTopolChange = false;
  }

  if (pv->showParameterView) {
    std::vector<FmParameter> params;
    if (pObj) pObj->getParameters(params);
    pv->myParameterNames.clear();
    pv->myParameterValues.clear();
    for (const FmParameter& param : params) {
      pv->myParameterNames.push_back(param.fpName);
      pv->myParameterValues.push_back(param.getFcn(pObj));
    }
  }

  // Preview curve

#ifdef FT_HAS_PREVIEW
  if (!f) return; // no function to preview

  FmCurveSet* curve = f->getPreviewCurve();
  if (curve) {
    pv->previewDomain.X = curve->getFuncDomain();
    pv->previewDomain.dX = curve->getIncX();
  }
  if (f->hasSmartPoints())
    pv->previewDomain.autoInc = curve ? curve->getUseSmartPoints() : 1;
#endif
}


void FapUAFunctionProperties::setDBValues(FFuaUIValues* values)
{
  if (!mySelectedFmItem) return;

  FuaFunctionPropertiesValues* pv = dynamic_cast<FuaFunctionPropertiesValues*> (values);
  if (!pv) return;

  FmEngine* e = this->getMyEngine();
  FmMathFuncBase* fn = e ? e->getFunction() : dynamic_cast<FmMathFuncBase*>(mySelectedFmItem);
  FmParamObjectBase* f = fn ? fn : dynamic_cast<FmParamObjectBase*>(mySelectedFmItem);
  bool functionLinked = e ? e->isFunctionLinked() : false;

  // First update the engine we are possibly editing

  if (e) {

    // Check license for control system editing
    if (mySelectedFmItem->isOfType(FmcInput::getClassTypeID()) ||
        mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
      if (!FapLicenseManager::hasCtrlLicense()) return;

    // Function to link to

    if (functionLinked) {

      FmEngine* linkFromEng = static_cast<FmEngine*>(pv->mySelectedLinkFunction);
      if (linkFromEng)
	e->setEngineToLinkFunctionFrom(linkFromEng);
      else
	e->setEngineToLinkFunctionFrom(e);
    }

    // Argument(s)

    if (fn && !functionLinked)
      fn->setNoArgs(pv->myArgumentValues.size());

    if (!e->isDriveFile() && !mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
      for (size_t j = 0; j < pv->myArgumentValues.size(); j++) {
	FmSensorBase* sensor = static_cast<FmSensorBase*>(pv->myArgumentValues[j].selectedObject);
	FmSensorBase* oldSensor = e->getSensor(j);
	if (sensor && sensor == oldSensor) {
	  std::vector<FmSensorChoice> choices;
	  sensor->getSensorDofs(choices);
	  int selectionIdx = pv->myArgumentValues[j].selectedDofIdx;
	  if (selectionIdx >= 0 && selectionIdx < (int)choices.size())
	    e->setDof(choices[selectionIdx].first,j);

	  sensor->getSensorEntities(choices,e->getDof(j));
	  selectionIdx = pv->myArgumentValues[j].selectedVariableIdx;
	  if (selectionIdx >= 0 && selectionIdx < (int)choices.size()) {
	    e->setEntity(choices[selectionIdx].first,j);
	    if (sensor->isExternalCtrlSys())
	      e->setEntityName(choices[selectionIdx].second,j);
	  }
	}
	else if (sensor)
	  e->setSensor(sensor,j);
	else if (!oldSensor && fn && j == 0)
	  e->setSensor(FmDB::getTimeSensor());
	else
	  e->setSensor(NULL,j);
      }

    // Output indicator threshold

    if (pv->useAsOutputSensor >= 0 && !e->isDriveFile())
      if (FapLicenseManager::hasFeature("FA-SAP"))
      {
        if (pv->useAsOutputSensor > 0)
        {
          FmThreshold& dtt = e->myThreshold.getValue();
          dtt.active = pv->myThreshold.active ? FmThreshold::ACTIVE : FmThreshold::INACTIVE;
          dtt.threshold = pv->myThreshold.threshold;
          dtt.min = pv->myThreshold.min;
          dtt.skip = pv->myThreshold.skip;
          dtt.severity = pv->myThreshold.severity+1;
          dtt.description = pv->myThreshold.description;
        }

        if (e->myOutput.setValue(pv->useAsOutputSensor > 0) &&
            mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
          mySelectedFmItem->onChanged();
      }

    e->onChanged();
  }

  // Do the real function things :

  if (f && !functionLinked) {

    // File view

    if (f->isOfType(FmfDeviceFunction::getClassTypeID())) {
      FmfDeviceFunction* df = static_cast<FmfDeviceFunction*>(f);
      bool newFile = df->setDevice(pv->fileName,pv->myChannelName);
      bool newFref = df->setFileReference(static_cast<FmFileReference*>(pv->selectedFileRef));
      if ((newFile || newFref) && df->isUsedAs(FmMathFuncBase::WAVE_FUNCTION))
        df->initGetValue(); // Recalculate Hs and Tz
      df->scaleFactor.setValue(pv->myScaleFactor);
      df->verticalShift.setValue(pv->myVerticalShift);
      df->zeroAdjust.setValue(pv->myZeroAdjust);
      df->randomSeed.setValue(pv->myJonswapRandomSeed);
      if (!pv->myChannelName.empty() && pv->myChannelName != "Not set") {
        if (e && e->getUserDescription().empty() &&
            df->getFunctionUse() != FmMathFuncBase::DRIVE_FILE) {
          e->setUserDescription(pv->myChannelName);
          e->onChanged();
        }
        else if (!e && f->getUserDescription().empty())
          f->setUserDescription(pv->myChannelName);
      }
    }

    // External function

    else if (f->isOfType(FmfExternalFunction::getClassTypeID())) {
      FmfExternalFunction* xf = static_cast<FmfExternalFunction*>(f);
      xf->scale.setValue(pv->myScaleFactor);
      xf->shift.setValue(pv->myVerticalShift);
    }

    // Jonswap

    else if (f->isOfType(FmfWaveSpectrum::getClassTypeID())) {
      FmfWaveSpectrum* ws = static_cast<FmfWaveSpectrum*>(f);
      ws->myHs.setValue(pv->myJonswapHs);
      ws->myTp.setValue(pv->myJonswapTp);
      ws->nComp.setValue(pv->myJonswapNComp);
      ws->myRandomSeed.setValue(pv->myJonswapRandomSeed);
      ws->nDir.setValue(pv->myJonswapNDir);
      ws->sprExp.setValue(pv->myJonswapSprExp);

      // Note: Must auto-calculate the spectral peakedness before the T-range,
      // because the latter also depends on the former
      ws->autoCalcPeakedness.setValue(pv->myJonswapAutoCalcSpectralPeakedness);
      if (pv->myJonswapAutoCalcSpectralPeakedness)
	ws->deriveSpectralPeakedness();
      else if (pv->myJonswapSpectralPeakedness > 0.0 &&
	       pv->myJonswapSpectralPeakedness <= 30.0)
	ws->myPeakedness.setValue(pv->myJonswapSpectralPeakedness);
      else
	FFaMsg::dialog(FFaNumStr("Illegal spectral peakedness value (%g).",
				 pv->myJonswapSpectralPeakedness) +
		       "\nIt must be in the range <0,30].",FFaMsg::ERROR);

      ws->autoCalcTrange.setValue(pv->myJonswapAutoCalcCutoff);
      if (pv->myJonswapAutoCalcCutoff)
	ws->deriveTrange();
      else
	ws->myTrange.setValue(pv->myJonswapRange);
    }

    // Math expression

    else if (f->isOfType(FmfMathExpr::getClassTypeID()))
      static_cast<FmfMathExpr*>(f)->setExpressionString(pv->myExpression);

    // Parameter list (linear derivative, piecewise linear, spline)

    else if (f->isOfType(FmfMultiVarBase::getClassTypeID()))
      static_cast<FmfMultiVarBase*>(f)->setExtrapolationType(pv->myExtrapolationType);

    // Parameter view

    else {
      std::vector<FmParameter> params;
      f->getParameters(params);
      for (size_t i = 0; i < params.size() && i < pv->myParameterValues.size(); i++)
	params[i].setFcn(f,pv->myParameterValues[i]);
    }

    // Update the associated vessel motions for RAO wave functions, if any

    std::vector<FmVesselMotion*> vms;
    f->getReferringObjs(vms,"waveFunction");
    if (vms.empty())
      f->onChanged();
    else for (size_t i = 0; i < vms.size(); i++)
      if (i == 0)
	vms[i]->onWaveChanged();
      else
	vms[i]->onRAOChanged();
  }

  // Update the curve previewing this function, if any

#ifdef FT_HAS_PREVIEW
  FmCurveSet* curve = fn ? fn->getPreviewCurve() : NULL;
  if (curve) {
    curve->setFuncDomain(pv->previewDomain.X);
    curve->setIncX(pv->previewDomain.dX);
    curve->setUseSmartPoints(fn->hasSmartPoints() && pv->previewDomain.autoInc);
    if (curve->hasXYDataChanged()) {
#ifdef FT_HAS_GRAPHVIEW
      FapUACurveDefine::clearCachedDBCurve(curve);
#endif
      curve->onDataChanged();
    }
  }
#endif

  if (e) // To get the topology view to handle argument change
    FapUAExistenceHandler::doUpdateUI(FapUAProperties::getClassTypeID());
}


void FapUAFunctionProperties::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                                                     const std::vector<FFaViewItem*>&,
                                                     const std::vector<FFaViewItem*>&)
{
  // Show the last selected one, if Function, Friction or Control In/Out

  for (size_t i = totalSelection.size(); i > 0; i--)
    if ((mySelectedFmItem = dynamic_cast<FmModelMemberBase*>(totalSelection[i-1])) &&
        (mySelectedFmItem->isOfType(FmParamObjectBase::getClassTypeID()) ||
         mySelectedFmItem->isOfType(FmEngine::getClassTypeID()) ||
         mySelectedFmItem->isOfType(FmcInput::getClassTypeID()) ||
         mySelectedFmItem->isOfType(FmcOutput::getClassTypeID())))
    {
      FuaFunctionPropertiesValues values;
      this->getDBValues(&values);
      myFunctionPropertiesUI->buildDynamicWidgets(&values);
      myFunctionPropertiesUI->setUIValues(&values);
      myFunctionPropertiesUI->setSensitivity(FpPM::isModelEditable() &&
                                             (!mySelectedFmItem->isOfType(FmCtrlElementBase::getClassTypeID()) ||
                                              FapLicenseManager::checkCtrlLicense(false)));
      return;
    }

  mySelectedFmItem = NULL;
}


// Callbacks distributed to the creating ui :

void FapUAFunctionProperties::previewCB()
{
#ifdef FT_HAS_PREVIEW
  FmMathFuncBase* f = this->getMyFunction();
  if (!f) return;

  FmCurveSet* curve = f->getPreviewCurve();
  FmGraph*    graph = NULL;

  if (curve)
    graph = curve->getOwnerGraph();
  else {
    graph = new FmGraph();
    curve = new FmCurveSet(FmCurveSet::PREVIEW_FUNC);
    curve->setOwnerGraph(graph);
    graph->connect();
    curve->connect();
    curve->setFunctionRef(f);
  }

  FuaFunctionDomain domain;
  myFunctionPropertiesUI->getPreviewDomain(domain);
  curve->setFuncDomain(domain.X);
  curve->setIncX(domain.dX);
  curve->setUseSmartPoints(domain.autoInc);

  if (f->isOfType(FmfDeviceFunction::getClassTypeID()))
    curve->reload(); // Bugfix #458: Force reload of file on each "Show" push
#ifdef FT_HAS_GRAPHVIEW
  else if (curve->hasXYDataChanged())
    FapUACurveDefine::clearCachedDBCurve(curve);
#endif

  FapGraphCmds::show(graph);
#endif
}


void FapUAFunctionProperties::getChannelListCB()
{
  FmfDeviceFunction* f = dynamic_cast<FmfDeviceFunction*>(this->getMyFunction());
  if (!f) return;

  std::vector<std::string> channels;
  if (!f->getChannelList(channels))
    FFaMsg::dialog("File: " + f->getActualDeviceName(true) +
		   "\nCould not read channel list. Check file header.",
		   FFaMsg::DISMISS_ERROR);

  else if (!channels.empty())
  {
    myFunctionPropertiesUI->setChannelList(channels);
    myFunctionPropertiesUI->popUpChannelList();
  }
}


void FapUAFunctionProperties::addNumberCB(double nr)
{
  FmfLinVelVar* f = dynamic_cast<FmfLinVelVar*>(this->getMyFunction());
  if (f) f->addIntervalBreak(nr);

  this->updateUIValues();

  FuaFunctionPropertiesValues values;
  myFunctionPropertiesUI->getValues(values);
  this->setDBValues(&values);
}


void FapUAFunctionProperties::addNumbersCB(double nr1, double nr2)
{
  FmfMultiVarBase* f = dynamic_cast<FmfMultiVarBase*>(this->getMyFunction());
  if (f) f->addXYset(nr1, nr2);

  this->updateUIValues();

  FuaFunctionPropertiesValues values;
  myFunctionPropertiesUI->getValues(values);
  this->setDBValues(&values);
}


void FapUAFunctionProperties::pasteCB(const std::string& data)
{
  if (data.empty()) return;

  std::vector<std::string> words = { "" };
  std::vector<double> numbers;
  bool wasSpace = true;
  for (const char& c : data)
    if (!isspace(c))
    {
      words.back() += c;
      wasSpace = false;
    }
    else if (!wasSpace)
    {
      words.push_back("");
      wasSpace = true;
    }

  for (std::string& word : words)
  {
    size_t kommaPos = 0;
    while (kommaPos < word.size())
    {
      kommaPos = word.find_first_of(',', kommaPos);
      if (kommaPos < word.size())
        word[kommaPos] = '.';
    }

    float x;
    if (sscanf(word.c_str(), "%f",&x) == 1)
      numbers.push_back(x);
  }

  FmfMultiVarBase* mvf = dynamic_cast<FmfMultiVarBase*>(this->getMyFunction());
  FmfLinVelVar*    lvf = mvf ? NULL : dynamic_cast<FmfLinVelVar*>(this->getMyFunction());

  if (mvf)
    for (size_t i = 0; i+1 < numbers.size(); i += 2)
      mvf->addXYset(numbers[i],numbers[i+1]);
  else if (lvf)
    for (double number : numbers)
      lvf->addIntervalBreak(number);

  this->updateUIValues();

  FuaFunctionPropertiesValues values;
  myFunctionPropertiesUI->getValues(values);
  this->setDBValues(&values);
}


void FapUAFunctionProperties::deleteNumberCB(int idx)
{
  if (idx >= 0)
  {
    FmfMultiVarBase* mvf = dynamic_cast<FmfMultiVarBase*>(this->getMyFunction());
    FmfLinVelVar*    lvf = mvf ? NULL : dynamic_cast<FmfLinVelVar*>(this->getMyFunction());

    if (mvf)
      mvf->removeXYset(idx);
    else if (lvf)
      lvf->removeIntervalBreak(idx);
  }

  this->updateUIValues();

  FuaFunctionPropertiesValues values;
  myFunctionPropertiesUI->getValues(values);
  this->setDBValues(&values);
}


void FapUAFunctionProperties::clearNumbersCB()
{
  FmfMultiVarBase* f = dynamic_cast<FmfMultiVarBase*>(this->getMyFunction());
  if (f) f->getData().clear();

  this->updateUIValues();

  FuaFunctionPropertiesValues values;
  myFunctionPropertiesUI->getValues(values);
  this->setDBValues(&values);
}


void FapUAFunctionProperties::acceptExprCB()
{
  FmfMathExpr* f = dynamic_cast<FmfMathExpr*>(this->getMyFunction());
  if (f && !f->initGetValueNoRecursion())
    FFaMsg::dialog("Flawed expression: '" + f->getExpressionString() +
		   "'.\nPlease inspect, fix and re-apply.",
		   FFaMsg::DISMISS_ERROR);
}


FmEngine* FapUAFunctionProperties::getMyEngine()
{
  if (!mySelectedFmItem) return NULL;

  if (mySelectedFmItem->isOfType(FmEngine::getClassTypeID()))
    return static_cast<FmEngine*>(mySelectedFmItem);

  if (mySelectedFmItem->isOfType(FmcInput::getClassTypeID()))
    return static_cast<FmcInput*>(mySelectedFmItem)->getEngine();

  if (mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
    return static_cast<FmcOutput*>(mySelectedFmItem)->getEngine();

  return NULL;
}


FmMathFuncBase* FapUAFunctionProperties::getMyFunction()
{
  if (!mySelectedFmItem) return NULL;

  // Get actual function shown
  FmEngine* e = this->getMyEngine();
  if (e) return e->getFunction();

  return dynamic_cast<FmMathFuncBase*>(mySelectedFmItem);
}


void FapUAFunctionProperties::funcTypeSwitchedCB(int idx)
{
  FmMathFuncBase* f = this->getMyFunction();
  FmEngine* e = this->getMyEngine();
  bool functionLinked = e ? e->isFunctionLinked() : false;

  std::vector<FmFuncTypeInfo> funcTypes;
  FmFuncAdmin::getCompatibleFunctionTypes(funcTypes, functionLinked ? NULL : f);

  if (idx < 0 || idx >= (int)funcTypes.size())
    return;

  if (funcTypes[idx].funcMenuEnum == FmFuncAdmin::EXTERNAL)
    if (mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
    {
      // External functions are meaningless for control outputs
      this->updateUIValues();
      return;
    }

  if (functionLinked) {
    // Handle function linking

    if (funcTypes[idx].funcMenuEnum == FmFuncAdmin::REFERENCE)
      return; // Refer to other function -> Refer to other function

    // Engine had a linked function that is turned off, so remove the linkage
    e->setEngineToLinkFunctionFrom(NULL);
    e->setFunction(f = NULL);
  }
  else {
    // Engine did not have a linked function

    if (f && funcTypes[idx].getFuncType() == f->getTypeID()) {
      FmfUserDefined* udf = dynamic_cast<FmfUserDefined*>(f);
      int newFuncNo = funcTypes[idx].funcMenuEnum - FmFuncAdmin::USER_HEADING;
      if (udf && newFuncNo != udf->getFuncNo()) // To another user function
      {
	udf->setFuncNo(newFuncNo);
        udf->onChanged();
	this->updateUI();
      }
      return; // To same function type, don't do anything
    }

    if (!f && funcTypes[idx].funcMenuEnum == FmFuncAdmin::NONE)
      return; // 1:1 to 1:1

    // If function linking is turned on, set the link temporarily to itself
    if (e && funcTypes[idx].funcMenuEnum == FmFuncAdmin::REFERENCE)
      e->setEngineToLinkFunctionFrom(e);
  }

  // Have to switch function type ==> create a new function of the new type

  FmMathFuncBase* newF = FmFuncAdmin::createFunction(funcTypes[idx].getFuncType());
  if (newF) {

    // Now we've got a function of right type (newF)

    // Copy all fields that are present in both function types
    if (f) newF->clone(f,FmBase::SHALLOW);

    // Set the function use (i.e. under which list view node should it appear)
    if (e)
      newF->setFunctionUse(FmMathFuncBase::GENERAL);
    else if (f) {
      // Reset the use even if it is unchanged, to check for userID uniqueness
      newF->setID(f->getID()); // the clone method does not copy the userID
      newF->setFunctionUse(FmMathFuncBase::NONE);
      newF->setFunctionUse(f->getFunctionUse(),true);
    }

    if (newF->isOfType(FmfMathExpr::getClassTypeID()))
      // Set default number of arguments depending on function use
      switch (newF->getFunctionUse()) {
      case FmMathFuncBase::ROAD_FUNCTION:
	newF->setNoArgs(2); // x, y
	break;
      case FmMathFuncBase::CURR_FUNCTION:
	newF->setNoArgs(4); // x, y, z, t
	break;
      default:
	break;
      }

    FmfUserDefined* udf = dynamic_cast<FmfUserDefined*>(newF);
    if (udf)
      udf->setFuncNo(funcTypes[idx].funcMenuEnum-FmFuncAdmin::USER_HEADING);

    if (e)
      newF->setParentAssembly(e->getParentAssembly());
    else if (f)
      newF->setParentAssembly(f->getParentAssembly());
    newF->connect();

    if (f) {
#ifdef FT_HAS_GRAPHVIEW
      // Update all curves that plot the old function
      std::vector<FmCurveSet*> curves;
      f->getReferringObjs(curves,"myFunction");
      for (FmCurveSet* curve : curves)
        if (curve->usingInputMode() >= FmCurveSet::INT_FUNCTION) {
          curve->setFunctionRef(newF);
          curve->setUseSmartPoints(curve->getUseSmartPoints() && newF->hasSmartPoints());
          FapUACurveDefine::clearCachedDBCurve(curve);
          curve->onDataChanged();
        }
#endif
      // Update all other objects referring to the old function
      f->releaseReferencesToMe("",newF);
    }
    else if (e)
      e->setFunction(newF);

    if (mySelectedFmItem->isOfType(FmCtrlElementBase::getClassTypeID()))
      mySelectedFmItem->onChanged();
    else if (e)
      e->onChanged();

    // Update the associated vessel motions for RAO wave functions, if any
    std::vector<FmVesselMotion*> vms;
    newF->getReferringObjs(vms,"waveFunction");
    if (vms.empty())
      newF->onChanged();
    else for (size_t i = 0; i < vms.size(); i++)
      if (i == 0)
	vms[i]->onWaveChanged();
      else
	vms[i]->onRAOChanged();
  }

  // We did not select a new function type, either we hit a heading (do nothing)
  // or we hit 1:1 or Refer to other function (delete the old function)
  else if (funcTypes[idx].funcMenuEnum != FmFuncAdmin::NONE &&
	   funcTypes[idx].funcMenuEnum != FmFuncAdmin::REFERENCE)
    f = NULL;

  // If we were looking at the pure function, change the selection too
  if (mySelectedFmItem)
    if (mySelectedFmItem->isOfType(FmMathFuncBase::getClassTypeID())) {
      if (newF) mySelectedFmItem = newF;
      FapEventManager::permTotalSelect(mySelectedFmItem);
    }

  // Erase the old function of wrong type
  if (f) f->erase();

  this->updateUI();
}


void FapUAFunctionProperties::pickSensorCB(int iArg)
{
  FapCreateSensorCmd::instance()->createSensor(iArg);
}


void FapUAFunctionProperties::onFileBrowseActivated()
{
  FmfDeviceFunction* f = dynamic_cast<FmfDeviceFunction*>(this->getMyFunction());
  if (!f) return;

  std::string absModelfilePath = FmDB::getMechanismObject()->getAbsModelFilePath();
  FFuFileDialog* aDialog = FFuFileDialog::create(absModelfilePath, "Dialog",
                                                 FFuFileDialog::FFU_OPEN_FILE,
                                                 true);
  aDialog->setTitle("Select function file");
  aDialog->addFilter("ASCII File", std::vector<std::string>{ "asc", "txt" });
  if (f->getFunctionUse() == FmMathFuncBase::GENERAL ||
      f->getFunctionUse() == FmMathFuncBase::DRIVE_FILE) {
    aDialog->addFilter("nCode DAC File", "dac");
    aDialog->addFilter("MTS RPC Time History File", std::vector<std::string>{ "rsp", "drv", "tim" });
  }
  aDialog->addAllFilesFilter(true);

  std::string origFile = f->deviceName.getValue();
  bool isRelativeNameWanted = FFaFilePath::isRelativePath(origFile);

  if (!origFile.empty())
  {
    // Set the default file name to the original file, if the directory of
    // that file does not exist, assume current model file directory instead
    FFaFilePath::checkName(origFile);
    FFaFilePath::makeItAbsolute(origFile,absModelfilePath);
    if (!FpFileSys::verifyDirectory(FFaFilePath::getPath(origFile),false))
      FFaFilePath::setPath(origFile,absModelfilePath);
    aDialog->setDefaultName(origFile);
  }

  aDialog->addUserToggle("relToggle",
                         "Use path relative to model-file location",
                         isRelativeNameWanted);
  aDialog->remember("FunctionBrowseField");

  std::vector<std::string> retFile = aDialog->execute();
  if (!retFile.empty())
  {
    std::string& fileName = retFile.front();
    if (!absModelfilePath.empty() && aDialog->getUserToggleSet("relToggle"))
      fileName = FFaFilePath::getRelativeFilename(absModelfilePath,fileName);

    f->setDevice(fileName);
    f->setFileReference(NULL);
    this->updateUIValues();
  }

  delete aDialog;
}


void FapUAFunctionProperties::verifyGeneralFunctionCB(bool& isOK, FmModelMemberBase* item)
{
  // Filter out engines already using a linked function
  // and this engine itself
  if (item && item->isOfType(FmEngine::getClassTypeID()) &&
      !static_cast<FmEngine*>(item)->isFunctionLinked() &&
      item != mySelectedFmItem)
    isOK = true;
  else
    isOK = false;
}


void FapUAFunctionProperties::verifyArgumentSensor(bool& isOK, FmModelMemberBase* item)
{
  isOK = false;
  FmSensorBase* sensor = dynamic_cast<FmSensorBase*>(item);
  if (!sensor)
    isOK = false;
  else if (sensor->isTime())
    isOK = true;
  else if (!sensor->getMeasured())
    std::cerr <<"FapUAFunctionProperties::verifyArgumentSensor() No measured object!"<< std::endl;
  else if (this->getMyEngine() == dynamic_cast<FmEngine*>(sensor->getMeasured()))
    isOK = false;
  else if (sensor->getMeasured()->isOfType(FmcOutput::getClassTypeID()))
    isOK = false;
  else
    isOK = true;
}
