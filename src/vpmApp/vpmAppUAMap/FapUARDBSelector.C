// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUARDBSelector.H"
#include "vpmApp/vpmAppUAMap/FapUASimModelRDBListView.H"
#include "vpmApp/vpmAppUAMap/FapUARDBListView.H"
#include "vpmApp/FapEventManager.H"
#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmUI/vpmUIComponents/FuiItemsListViews.H"
#include "vpmUI/vpmUITopLevels/FuiRDBSelector.H"
#include "vpmUI/Fui.H"

#include "vpmDB/FmSubAssembly.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmBeam.H"

#include "FFrLib/FFrExtractor.H"
#include "FFrLib/FFrEntryBase.H"
#include "FFrLib/FFrVariableReference.H"
#include "FFrLib/FFrItemGroup.H"
#include "FFrLib/FFrObjectGroup.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaOperation/FFaOpUtils.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"

#include <algorithm>


Fmd_SOURCE_INIT(FcFAPUARDBSELECTOR, FapUARDBSelector, FapUAExistenceHandler)

//----------------------------------------------------------------------------

FapUARDBSelector::FapUARDBSelector(FuiRDBSelector* uic)
  : FapUAExistenceHandler(uic), FapUAFinishHandler(uic),
    signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUARDBSelector);

  this->ui = uic;
  this->myCurve = NULL;
  this->myAxis = 0;
  IAmEditingCurve = false;

  this->ui->setResultAppliedCB(FFaDynCB0M(FapUARDBSelector,this,onResultApplied));
  this->ui->setAppearanceOnScreenCB(FFaDynCB1M(FapUARDBSelector,this,onAppearance,bool));

  const FpRDBListViewFilter* lvFilter = FpRDBExtractorManager::instance()->getRDBListViewFilter();
  this->resUA = dynamic_cast<FapUASimModelRDBListView*>(this->ui->lvRes->getUA());
  this->posUA = dynamic_cast<FapUARDBListView*>(this->ui->lvPos->getUA());

  // listviews settings
  this->resUA->setEnsureItemVisibleOnPermSelection(true);
  this->posUA->setLeavesOnlySelectable(true);

  // Filter settings
  this->resUA->setFreezeTopLevelItem(lvFilter->freezeTopLevelItem);
  this->resUA->setShowModelPermSelectionAsTopLevelItem(lvFilter->showModelPermSelectionAsTopLevelItem);
  this->resUA->setVerifyItemCB(lvFilter->verifyItemCB);
  this->posUA->setVerifyItemCB(lvFilter->verifyItemCB);

  this->resUA->setPermTotUISelectionChangedCB(FFaDynCB1M(FapUARDBSelector,this,
							 onRDBItemSelected,FapUAItemsViewHandler*));
  this->posUA->setPermTotUISelectionChangedCB(FFaDynCB1M(FapUARDBSelector,this,
							 onPosItemSelected,FapUAItemsViewHandler*));

  this->resUA->setExtractor(FpRDBExtractorManager::instance()->getModelExtractor());
  this->posUA->setExtractor(FpRDBExtractorManager::instance()->getPossibilityExtractor());
}
//----------------------------------------------------------------------------

void FapUARDBSelector::edit(FmCurveSet* curve, int axis)
{
  myAxis = axis;
  myCurve = curve;

  if (myCurve)
  {
    if (!IAmEditingCurve && this->isUIPoppedUp())
      FapEventManager::pushPermSelection();
    IAmEditingCurve = true;
    this->selectResult(myCurve->getResult(myAxis));
    this->ui->lvRes->setSglSelectionMode(curve->usingInputMode() != FmCurveSet::SPATIAL_RESULT);
    this->ui->setOkCancelDialog(true);
  }
  else
  {
    this->ui->setOkCancelDialog(false);
    if (IAmEditingCurve && this->isUIPoppedUp())
      FapEventManager::popPermSelection();
    IAmEditingCurve = false;
  }
  this->setAxisText();
}
//----------------------------------------------------------------------------

void FapUARDBSelector::selectResult(const FFaResultDescription& result)
{
  // no result
  if (result.empty())
  {
    FapEventManager::permTotalSelect(NULL);
    posUA->permTotSelectUIItems({});
  }

  FmModelMemberBase* mmb = result.baseId > 0 ? FmDB::findObject(result.baseId) : NULL;
  if (mmb)
  {
    // Fill up frozen top-level item listviews with model member
    if (resUA->getFreezeTopLevelItem()) resUA->setTopLevelItem(mmb,true);
    if (posUA->getFreezeTopLevelItem()) posUA->setTopLevelItem(mmb,true);
  }

  //result only
  if (FFrEntryBase* ffrItem = resUA->findItem(result); ffrItem)
  {
    FapEventManager::permTotalSelect(ffrItem);
    resUA->ensureItemVisible(ffrItem);
  }
  //result + possibility
  else if (mmb)
  {
    //res
    FapEventManager::permTotalSelect(mmb);
    resUA->ensureItemVisible(mmb);
    //pos
    FFaResultDescription pos = result;
    pos.baseId = 0;
    if (FFrEntryBase* ffrItem = posUA->findItem(pos); ffrItem)
    {
      posUA->permTotSelectUIItems({ ffrItem });
      posUA->ensureItemVisible(ffrItem);
    }
  }
  //possibility only -> ie top level var
  else if (FFrEntryBase* ffrItem = posUA->findItem(result); ffrItem)
  {
    FapEventManager::permTotalSelect(NULL);
    posUA->permTotSelectUIItems({ ffrItem });
    posUA->ensureItemVisible(ffrItem);
  }

  this->updateApplyable();
}
//----------------------------------------------------------------------------

void FapUARDBSelector::setAxisText()
{
  std::string txt;
  if (this->myCurve)
    txt = this->myCurve->getUserDescription() + " - ";
  txt += char('X' + this->myAxis) + std::string(" Axis Definition");

  this->ui->setTitle(txt.c_str());
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onRDBItemSelected(FapUAItemsViewHandler* lv)
{
  std::vector<FFaViewItem*> totalSelection = lv->getUISelectedItems();
  if (totalSelection.empty())
    posUA->showTopLevelVarsOnly();
  else
  {
    // If multi-selection, check that all are of the same type
    const char* selected = NULL;
    for (FFaViewItem* item : totalSelection)
      if (dynamic_cast<FmModelMemberBase*>(item))
      {
        if (!selected)
          selected = item->getItemName();
        else if (strcmp(selected,item->getItemName()))
        {
          selected = NULL;
          break;
        }
      }
      else
      {
        selected = NULL;
        break;
      }

    if (selected)
      posUA->setTopLevelItem(posUA->findItem(FFaResultDescription(selected)),false);
    else
      posUA->clearSession();
  }

  this->updateApplyable();
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onPosItemSelected(FapUAItemsViewHandler*)
{
  this->updateApplyable();
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onResultApplied()
{
  if (!myCurve) return;

  bool spatial = myCurve->usingInputMode() == FmCurveSet::SPATIAL_RESULT;

  // Setting result in curve only if this is a new result
  FFaResultDescription result;
  std::vector<FmIsPlottedBase*> spaceObjs;
  this->getSelection(result, spatial ? &spaceObjs : NULL);
  if (result == myCurve->getResult(myAxis)) return;

  if (spatial)
  {
    if (spaceObjs.size() > 1)
    {
      for (size_t i = 1; i < spaceObjs.size(); i++)
        if (spaceObjs[i]->getTypeID() != spaceObjs.front()->getTypeID())
        {
          Fui::okDialog("All objects have to be of the same type.\n"
                        "Axis definition is not updated.",FFuDialog::ERROR);
          return;
        }
      result.baseId = result.userId = 0;
    }
    else if (result.baseId > 0)
    {
      if (FmTriad::traverseBeam(FmDB::findObject(result.baseId),spaceObjs))
	result.baseId = result.userId = 0;
      else if (FmBeam::traverse(FmDB::findObject(result.baseId),spaceObjs))
	result.baseId = result.userId = 0;
      else
      {
	Fui::okDialog("You have to select an end Triad (or Beam element) "
		      "of the beam structure in Beam diagram curves.\n"
		      "Axis definition is not updated.",FFuDialog::ERROR);
	return;
      }
    }

    result.OGType.erase();
    myCurve->setSpatialObjs(spaceObjs);
  }

  myCurve->setResult(myAxis,result);

  std::vector<std::string> allOpers = FFaOpUtils::findOpers(result.varRefType);
  std::vector<std::string>::iterator it = std::find(allOpers.begin(), allOpers.end(),
                                                    myCurve->getResultOper(myAxis));
  if (it == allOpers.end())
    myCurve->setResultOper(myAxis,FFaOpUtils::getDefaultOper(result.varRefType));
  else
    myCurve->setResultOper(myAxis,*it);

  myCurve->onDataChanged();
}
//----------------------------------------------------------------------------

FFaResultDescription FapUARDBSelector::getSelectedResultDescr() const
{
  FFaResultDescription result;
  this->getSelection(result,NULL);
  return result;
}
//----------------------------------------------------------------------------

void FapUARDBSelector::getSelection(FFaResultDescription& result,
                                    std::vector<FmIsPlottedBase*>* spatialObjs) const
{
  // Get selection from the list views

  std::vector<FFaViewItem*> resSel = resUA->getUISelectedItems();
  std::vector<FFaViewItem*> posSel = posUA->getUISelectedItems();

  FFaListViewItem* resViewSel = (FFaListViewItem*)(resSel.empty() ? NULL : resSel.front());
  FFaListViewItem* posViewSel = (FFaListViewItem*)(posSel.empty() ? NULL : posSel.front());

  bool isFromResView = dynamic_cast<FFrVariableReference*>(resViewSel) ? true : false;
  bool isFromPosView = dynamic_cast<FFrVariableReference*>(posViewSel) ? true : false;

  // Check for multi-selection of spatial objects
  if (spatialObjs && resSel.size() > 1)
    for (FFaViewItem* item : resSel)
    {
      FFaListViewItem* vItem = static_cast<FFaListViewItem*>(item);
      if (dynamic_cast<FFrVariableReference*>(vItem))
      {
        resViewSel = vItem;
        isFromResView = true;
        do
          vItem = resUA->getUIParent(vItem);
        while (dynamic_cast<FFrItemGroup*>(vItem));
      }
      else
        while (dynamic_cast<FFrItemGroup*>(vItem))
          vItem = resUA->getUIParent(vItem);

      if (FmIsPlottedBase* obj = dynamic_cast<FmIsPlottedBase*>(vItem); obj)
        spatialObjs->push_back(obj);
    }

  // Build the FFaResultDescription

  if (resViewSel && isFromResView)
  {
    isFromPosView = false;

    // Selection in the result view only
    result.varRefType = ((FFrEntryBase*)resViewSel)->getType();

    // Create varDescrPath by assembling descriptions from selection
    // and parents, up to the owner group item (model object)
    while (dynamic_cast<FFrVariableReference*>(resViewSel) ||
           dynamic_cast<FFrItemGroup*>(resViewSel))
    {
      result.varDescrPath.insert(result.varDescrPath.begin(),
				 ((FFrEntryBase*)resViewSel)->getDescription());
      resViewSel = resUA->getUIParent(resViewSel);
    }

    if (dynamic_cast<FmSimulationModelBase*>(resViewSel) ||
	dynamic_cast<FmSubAssembly*>(resViewSel))
    {
      // Simulation object
      result.OGType = resViewSel->getItemName();
      result.baseId = resViewSel->getItemBaseID();
      result.userId = resViewSel->getItemID();
    }
    else if (dynamic_cast<FFrObjectGroup*>(resViewSel))
    {
      // Result owner group
      result.OGType = ((FFrEntryBase*)resViewSel)->getType();
      result.baseId = ((FFrEntryBase*)resViewSel)->getBaseID();
      result.userId = ((FFrEntryBase*)resViewSel)->getUserID();
    }
  }
  else if (resViewSel && isFromPosView)
  {
    // Selection in both result and the possibility view
    result.OGType = resViewSel->getItemName();
    result.baseId = resViewSel->getItemBaseID();
    result.userId = resViewSel->getItemID();
  }

  if (isFromPosView)
  {
    // Selection from possibility view
    result.varRefType = ((FFrEntryBase*)posViewSel)->getType();

    // Create varDescrPath by assembling descriptions from selection
    // and parents, up to the owner group item (model object)
    while (dynamic_cast<FFrVariableReference*>(posViewSel) ||
	   dynamic_cast<FFrItemGroup*>(posViewSel)) {
      result.varDescrPath.insert(result.varDescrPath.begin(),
				 ((FFrEntryBase*)posViewSel)->getDescription());
      posViewSel = posUA->getUIParent(posViewSel);
    }
  }
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onAppearance(bool popup)
{
  if (IAmEditingCurve)
  {
    if (popup)
      FapEventManager::pushPermSelection();
    else
      FapEventManager::popPermSelection();
  }

  if (popup)
    this->updateApplyable();
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onModelExtrDeleted(FFrExtractor*)
{
  this->doNewModelExtr(NULL);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onModelExtrHeaderChanged(FFrExtractor* extr)
{
  this->doNewModelExtr(extr);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onPosExtrDeleted(FFrExtractor*)
{
  this->doNewPosExtr(NULL);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onPosExtrHeaderChanged(FFrExtractor* extr)
{
  this->doNewPosExtr(extr);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::doNewModelExtr(FFrExtractor* extr)
{
  resUA->setExtractor(extr);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::doNewPosExtr(FFrExtractor* extr)
{
  posUA->setExtractor(extr);
  if (resUA->isUIPoppedUp())
    this->onRDBItemSelected(resUA);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::onModelMemberChanged(FmModelMemberBase* item)
{
  if (myCurve && dynamic_cast<FmCurveSet*>(item) == myCurve)
    if (this->isUIPoppedUp())
      this->setAxisText();
}
//----------------------------------------------------------------------------

void FapUARDBSelector::updateApplyable()
{
  std::vector<FFaViewItem*> selection = resUA->getUISelectedItems();
  for (FFaViewItem* item : selection)
    if (dynamic_cast<FFrVariableReference*>(item))
    {
      ui->setApplyable(true);
      return;
    }

  selection = posUA->getUISelectedItems();
  for (FFaViewItem* item : selection)
    if (dynamic_cast<FFrVariableReference*>(item))
    {
      ui->setApplyable(true);
      return;
    }

  ui->setApplyable(false);
}
//----------------------------------------------------------------------------

void FapUARDBSelector::finishUI()
{
  Fui::rdbSelectorUI(false,true);
}
//----------------------------------------------------------------------------

FapUARDBSelector::SignalConnector::SignalConnector(FapUARDBSelector* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_ABOUT_TO_DELETE,
			  FFaSlot1M(SignalConnector,this,onModelExtrDeleted,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::MODELEXTRACTOR_HEADER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelExtrHeaderChanged,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::POSEXTRACTOR_ABOUT_TO_DELETE,
			  FFaSlot1M(SignalConnector,this,onPosExtrDeleted,FFrExtractor*));
  FFaSwitchBoard::connect(FpRDBExtractorManager::instance(),
			  FpRDBExtractorManager::POSEXTRACTOR_HEADER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onPosExtrHeaderChanged,FFrExtractor*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelMemberChanged,FmModelMemberBase*));
}
//----------------------------------------------------------------------------

FapUARDBSelector::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
//----------------------------------------------------------------------------
