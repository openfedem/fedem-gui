// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FapCmdsBase.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/FapEventManager.H"
#ifdef FT_HAS_GRAPHVIEW
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#endif
#include "vpmUI/vpmUITopLevels/FuiModeller.H"
#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmElementGroupProxy.H"
#include "vpmDB/FmRingStart.H"

#include <algorithm>


FuiModeller* FapCmdsBase::getActiveModeller()
{
  return dynamic_cast<FuiModeller*>(FapEventManager::getActiveWindow());
}


FuiCtrlModeller* FapCmdsBase::getActiveCtrlModeller()
{
  return dynamic_cast<FuiCtrlModeller*>(FapEventManager::getActiveWindow());
}


FuiGraphView* FapCmdsBase::getActiveGraphView()
{
#ifdef FT_HAS_GRAPHVIEW
  FuiGraphViewTLS* tls = dynamic_cast<FuiGraphViewTLS*>(FapEventManager::getActiveWindow());
  return tls ? tls->getGraphViewComp() : NULL;
#else
  return NULL;
#endif
}


FFaViewItem* FapCmdsBase::findSelectedSingleObject()
{
  FFaViewItem* tmpSelection = FapEventManager::getTmpSelection();
  if (tmpSelection)
    return tmpSelection;

  std::vector<FFaViewItem*> permSelection = FapEventManager::getPermSelection();
  if (permSelection.size() == 1)
    return permSelection.front();

  return NULL;
}


bool FapCmdsBase::getCurrentSelection(std::vector<FFaViewItem*>& data)
{
  FFaViewItem* tmpSelection = FapEventManager::getTmpSelection();
  if (tmpSelection)
    data = std::vector<FFaViewItem*>(1,tmpSelection);
  else
    data = FapEventManager::getPermSelection();

  return !data.empty();
}


bool FapCmdsBase::getCurrentSelection(std::vector<FmModelMemberBase*>& data,
                                      bool noRingStart)
{
  FmModelMemberBase* tmpSelection = FapEventManager::getTmpMMBSelection();
  if (tmpSelection)
    data = std::vector<FmModelMemberBase*>(1,tmpSelection);
  else
    data = FapEventManager::getPermMMBSelection();

  if (noRingStart)
  {
    // Replace any ring start objects by their respective ring members
    for (size_t i = 0; i < data.size();)
      if (data[i]->isOfType(FmRingStart::getClassTypeID()))
      {
        std::vector<FmModelMemberBase*> members;
        FmRingStart* rs = static_cast<FmRingStart*>(data[i]);
        data.erase(data.begin()+i);
        rs->getMembers(members);
        std::vector<FmModelMemberBase*>::const_iterator jt;
        for (jt = members.begin(); jt != members.end(); ++jt)
          if (std::find(data.begin(),data.end(),*jt) == data.end())
            data.push_back(*jt);
      }
      else
        ++i;
  }

  return !data.empty();
}


bool FapCmdsBase::hasResultsCheck()
{
  return FpPM::hasResults();
}


void FapCmdsBase::isProEdition(bool& isPro)
{
  isPro = FapLicenseManager::isProEdition();
}


void FapCmdsBase::isModellerActive(bool& active)
{
  active = FapCmdsBase::getActiveModeller() ? true : false;
}


void FapCmdsBase::isCtrlModellerActive(bool& active)
{
  active = FapCmdsBase::getActiveCtrlModeller() ? true : false;
}


void FapCmdsBase::isGraphViewActive(bool& active)
{
  active = FapCmdsBase::getActiveGraphView() ? true : false;
}


void FapCmdsBase::isModellerOrGraphViewActive(bool& active)
{
  active = FapCmdsBase::getActiveModeller() || FapCmdsBase::getActiveGraphView();
}


void FapCmdsBase::isCtrlModellerEditable(bool& isEditable)
{
  isEditable = FapCmdsBase::getActiveCtrlModeller() ? FpPM::isModelEditable() : false;
}


void FapCmdsBase::isModellerEditable(bool& isEditable)
{
  isEditable = FapCmdsBase::getActiveModeller() ? FpPM::isModelEditable() : false;
}


void FapCmdsBase::isModellerTouchable(bool& isTouchable)
{
  isTouchable = FapCmdsBase::getActiveModeller() ? FpPM::isModelTouchable() : false;
}


void FapCmdsBase::isModelEditable(bool& isEditable)
{
  isEditable = FpPM::isModelEditable();
}


void FapCmdsBase::isModelTouchable(bool& isTouchable)
{
  isTouchable = FpPM::isModelTouchable();
}


/*!
  Returns the selected part, if only one is selected.
*/

FmPart* FapCmdsBase::getTheSelectedPart()
{
  FmModelMemberBase* tmpSelection = FapEventManager::getTmpMMBSelection();
  if (tmpSelection)
    return dynamic_cast<FmPart*>(tmpSelection);

  std::vector<FmModelMemberBase*> selection = FapEventManager::getPermMMBSelection();
  if (selection.size() == 1)
    return dynamic_cast<FmPart*>(selection.front());

  return NULL;
}


/*!
  Returns a vector with the selected parts.
*/

bool FapCmdsBase::getSelectedParts(std::vector<FmPart*>& data)
{
  data.clear();

  std::vector<FmModelMemberBase*> selection;
  if (!FapCmdsBase::getCurrentSelection(selection))
    return false;

  bool partsOnly = true;
  for (size_t i = 0; i < selection.size(); i++)
    if (selection[i]->isOfType(FmPart::getClassTypeID()))
      data.push_back((FmPart*)selection[i]);
    else
      partsOnly = false;

  return partsOnly;
}


/*!
  Returns a map with the selected parts and the optionally selected groups.
*/

bool FapCmdsBase::getSelectedPartsWithGroups(FapPartGroupsMap& data)
{
  data.clear();

  std::vector<FmModelMemberBase*> selection;
  if (!FapCmdsBase::getCurrentSelection(selection))
    return false;

  bool partsAndGroupsOnly = true;
  for (size_t i = 0; i < selection.size(); i++)
    if (selection[i]->isOfType(FmPart::getClassTypeID()))
      data[(FmPart*)selection[i]];
    else if (selection[i]->isOfType(FmElementGroupProxy::getClassTypeID()))
      data[((FmElementGroupProxy*)selection[i])->getOwner()].push_back((FmElementGroupProxy*)selection[i]);
    else
      partsAndGroupsOnly = false;

#ifdef FAP_DEBUG
  FapPartGroupsMap::const_iterator mit;
  for (mit = data.begin(); mit != data.end(); mit++) {
    std::cout << mit->first->getUserDescription() << std::endl;
    for (size_t i = 0; i < mit->second.size(); i++)
      std::cout <<"\t"<< mit->second[i]->getUserDescription() << std::endl;
  }
#endif

  return partsAndGroupsOnly;
}
