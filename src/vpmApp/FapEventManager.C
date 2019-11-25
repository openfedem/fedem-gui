// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include "vpmApp/FapEventManager.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"
#include "FFuLib/FFuBase/FFuMDIWindow.H"
#include "FFaLib/FFaDefinitions/FFaViewItem.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"
#include "vpmDB/FmModelMemberBase.H"
#include "vpmDB/FmIsRenderedBase.H"


FapEventManager::SignalConnector FapEventManager::signalConnector;
std::list<FapEventManager::FFaViewItems>* FapEventManager::permSelectedItems = NULL;
FFaViewItem* FapEventManager::tmpSelectedItem = NULL;
FFuMDIWindow* FapEventManager::activeWindow = NULL;
FmGraph* FapEventManager::loadingGraph = NULL;
bool FapEventManager::isDisconnectingItems = false;
FmAnimation* FapEventManager::activeAnimation = NULL;


//----------------------------------------------------------------------------

void FapEventManager::init()
{
  FapEventManager::permSelectedItems = new std::list<FFaViewItems>();
  FapEventManager::permSelectedItems->push_back(FFaViewItems());
}
//----------------------------------------------------------------------------

void FapEventManager::pushPermSelection()
{
  FFaViewItems empty;

  FapEventManager::highlightCurrentLayer(false);
  FapEventManager::permSelectedItems->push_back(empty);

  FapEventManager::sendPermSelectionStackChanged(true);
  FapEventManager::sendPermSelectionChanged(FapEventManager::permSelectedItems->back(),empty,empty);
}
//----------------------------------------------------------------------------

void FapEventManager::popPermSelection()
{
  if (FapEventManager::permSelectedItems->size() <= 1)
    return;

  FFaViewItems empty;

  FapEventManager::highlightCurrentLayer(false);
  FapEventManager::permSelectedItems->pop_back();
  FapEventManager::highlightCurrentLayer(true);

  FapEventManager::sendPermSelectionChanged(FapEventManager::permSelectedItems->back(),empty,empty);
  FapEventManager::sendPermSelectionStackChanged(false);
}
//----------------------------------------------------------------------------

bool FapEventManager::isInSelectionStack(FFaViewItem* item)
{
  size_t i = 0, nStack = FapEventManager::permSelectedItems->size();
  if (nStack > 1)
    for (const FFaViewItems& selection : *FapEventManager::permSelectedItems)
      if (++i < nStack)
        if (std::find(selection.begin(),selection.end(),item) != selection.end())
          return true;

  return false;
}
//----------------------------------------------------------------------------

bool FapEventManager::hasStackedSelections()
{
  return FapEventManager::permSelectedItems->size() > 1;
}
//----------------------------------------------------------------------------

void FapEventManager::permTotalSelect(FFaViewItem* total)
{FapEventManager::permTotalSelect(FFaViewItems(1,total));}

void FapEventManager::permSelect(FFaViewItem* select)
{FapEventManager::permSelect(FFaViewItems(1,select));}

void FapEventManager::permAddSelect(FFaViewItem* select)
{FapEventManager::permAddSelect(FFaViewItems(1,select));}

void FapEventManager::permUnselect(FFaViewItem* unselect)
{FapEventManager::permUnselect(FFaViewItems(1,unselect));}

//----------------------------------------------------------------------------

void FapEventManager::permTotalSelect(const FFaViewItems& total)
{
  FFaViewItems filtered, superfluous, added, removed;
  FapEventManager::filterNull(total,filtered);
  for (FFaViewItem* item : FapEventManager::permSelectedItems->back())
    if (std::find(filtered.begin(),filtered.end(),item) == filtered.end())
      superfluous.push_back(item);

  if (!superfluous.empty())
    FapEventManager::removePermSelectedItems(superfluous,removed);

  FapEventManager::addPermSelectedItems(filtered,added);

  if (!removed.empty() || !added.empty())
    FapEventManager::sendPermSelectionChanged(FapEventManager::permSelectedItems->back(),added,removed);
}
//----------------------------------------------------------------------------

void FapEventManager::tmpSelect(FFaViewItem* tmp)
{
  FFaViewItem* tmpUnselected = FapEventManager::tmpSelectedItem;
  if (tmp == tmpUnselected) return;

  FapEventManager::tmpSelectedItem = tmp;
  FapEventManager::sendTmpSelectionChanged(FapEventManager::tmpSelectedItem,tmpUnselected,
                                           FapEventManager::permSelectedItems->back());
}

//----------------------------------------------------------------------------

void FapEventManager::permSelect(FFaViewItem* object, int index, bool replace)
{
  FFaViewItems removed;
  removed.reserve(1);

  FFaViewItems& pSel = FapEventManager::permSelectedItems->back();
  if (index >= (int)pSel.size())
  {
    // Select item at index, but we have to
    // pad the selection array to actually reach the index wanted
    size_t npad = index - pSel.size();
    if (npad > 0) // Pad with Null selections
      pSel.insert(pSel.end(),npad,NULL);

    // Put object in place
    pSel.push_back(object);
  }
  else if (index >= 0)
  {
    if (replace)
    {
      // Deselect item at index
      FapEventManager::highlightRendered(pSel[index],false);
      removed.push_back(pSel[index]);
      pSel.erase(pSel.begin()+index);
    }

    // Select item at index
    pSel.insert(pSel.begin()+index,object);
  }
  else
    return;

  FapEventManager::highlightRendered(object,true);
  FapEventManager::sendPermSelectionChanged(pSel,{object},removed);
}

//----------------------------------------------------------------------------

void FapEventManager::permSelectInsert(FFaViewItem* object, int index)
{
  FapEventManager::permSelect(object,index,false);
}

//---------------------------------------------------------------------------

void FapEventManager::permSelect(const FFaViewItems& select)
{
  FFaViewItems filtered, added;
  FapEventManager::filterNull(select,filtered);
  FapEventManager::addPermSelectedItems(filtered,added);

  if (!added.empty())
    FapEventManager::sendPermSelectionChanged(FapEventManager::permSelectedItems->back(),added,FFaViewItems());
}
//----------------------------------------------------------------------------

void FapEventManager::permAddSelect(const FFaViewItems& select)
{
  FFaViewItems filtered;
  FapEventManager::filterNull(select,filtered);
  for (FFaViewItem* item : filtered) {
    FapEventManager::highlightRendered(item,true);
    FapEventManager::permSelectedItems->back().push_back(item);
  }

  if (!filtered.empty())
    FapEventManager::sendPermSelectionChanged(FapEventManager::permSelectedItems->back(),filtered,FFaViewItems());
}
//----------------------------------------------------------------------------

void FapEventManager::permUnselect(const FFaViewItems& unselect)
{
  FFaViewItems filtered, removed;
  FapEventManager::filterNull(unselect,filtered);
  FapEventManager::removePermSelectedItems(filtered,removed);

  if (!removed.empty())
    FapEventManager::sendPermSelectionChanged(FapEventManager::permSelectedItems->back(),FFaViewItems(),removed);
}
//----------------------------------------------------------------------------

/*!
  Deselects the last object in the selection.
  Does nothing with other instances of the object,
  that might be selected.
*/

void FapEventManager::permUnselectLast()
{
  if (!FapEventManager::permSelectedItems->empty())
    FapEventManager::permUnselect(FapEventManager::permSelectedItems->back().size()-1);
}

/*!
  Deselects what is stored at index.
  Does not deselect any other instances of the object index points to.
*/

void FapEventManager::permUnselect(int index)
{
  FFaViewItems& pSel = FapEventManager::permSelectedItems->back();
  if (index < 0 || index >= (int)pSel.size())
    return;

  // Dehighlight the item to deselect if needed
  FapEventManager::highlightRendered(pSel[index],false);

  // Store what was removed, and actually remove item
  FFaViewItems removed = { pSel[index] };
  pSel.erase(pSel.begin()+index);

  // Send signal with changes
  FapEventManager::sendPermSelectionChanged(pSel,FFaViewItems(),removed);
}

//----------------------------------------------------------------------------

void FapEventManager::getSelection(FFaViewItems& permSelection,
				   FFaViewItem*& tmpSelection)
{
  permSelection = FapEventManager::getPermSelection();
  tmpSelection = FapEventManager::getTmpSelection();
}
//----------------------------------------------------------------------------

void FapEventManager::getLVSelection(std::vector<FFaListViewItem*>& permSelection,
				     FFaListViewItem*& tmpSelection)
{
  permSelection = FapEventManager::getPermLVSelection();
  tmpSelection = FapEventManager::getTmpLVSelection();
}
//----------------------------------------------------------------------------

void FapEventManager::getMMBSelection(std::vector<FmModelMemberBase*>& permSelection,
				     FmModelMemberBase*& tmpSelection)
{
  permSelection = FapEventManager::getPermMMBSelection();
  tmpSelection = FapEventManager::getTmpMMBSelection();
}
//----------------------------------------------------------------------------


FapEventManager::FFaViewItems FapEventManager::getPermSelection()
{
  FFaViewItems permSelectionFiltered;
  FapEventManager::filterNull(FapEventManager::permSelectedItems->back(),permSelectionFiltered);//tmp since 0's are being selected internally
  return permSelectionFiltered;
}
//----------------------------------------------------------------------------

std::vector<FFaListViewItem*> FapEventManager::getPermLVSelection()
{
  std::vector<FFaListViewItem*> permLVSelection;
  FapEventManager::filterLVI(FapEventManager::getPermSelection(),permLVSelection);
  return permLVSelection;
}
//----------------------------------------------------------------------------

std::vector<FmModelMemberBase*> FapEventManager::getPermMMBSelection()
{
  std::vector<FmModelMemberBase*> permMMBSelection;
  FapEventManager::filterMMB(FapEventManager::getPermSelection(),permMMBSelection);
  return permMMBSelection;
}
//----------------------------------------------------------------------------

FFaViewItem* FapEventManager::getFirstPermSelectedObject()
{
  if (!FapEventManager::permSelectedItems->back().empty())
    return FapEventManager::permSelectedItems->back().front();
  else
    return NULL;
}
//----------------------------------------------------------------------------

FFaViewItem* FapEventManager::getLastPermSelectedObject()
{
  if (FapEventManager::permSelectedItems->back().size())
    return FapEventManager::permSelectedItems->back().back();
  else
    return NULL;
}

//----------------------------------------------------------------------------

FFaViewItem* FapEventManager::getPermSelectedObject(int index)
{
  if ((int)FapEventManager::permSelectedItems->back().size() > index)
    return FapEventManager::permSelectedItems->back()[index];
  else
    return NULL;
}

//----------------------------------------------------------------------------

FFaViewItem* FapEventManager::getTmpSelection()
{
  return FapEventManager::tmpSelectedItem;
}
//----------------------------------------------------------------------------

FFaListViewItem* FapEventManager::getTmpLVSelection()
{
  return dynamic_cast<FFaListViewItem*>(FapEventManager::tmpSelectedItem);
}
//----------------------------------------------------------------------------

FmModelMemberBase* FapEventManager::getTmpMMBSelection()
{
  return dynamic_cast<FmModelMemberBase*>(FapEventManager::tmpSelectedItem);
}
//----------------------------------------------------------------------------

bool FapEventManager::isPermSelected(FFaViewItem* item)
{
  if (!item) return false;

  const FFaViewItems& permSel = permSelectedItems->back();
  return (std::find(permSel.begin(),permSel.end(),item) != permSel.end());
}
//----------------------------------------------------------------------------

bool FapEventManager::isTmpSelected(FFaViewItem* item)
{
  if (!item) return false;

  return (item == FapEventManager::tmpSelectedItem);
}
//----------------------------------------------------------------------------

bool FapEventManager::isObjectOfTypeSelected(int typeID)
{
  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(FapEventManager::tmpSelectedItem);
  if (mmb)
    return mmb->isOfType(typeID);

  for (FFaViewItem* item : FapEventManager::permSelectedItems->back())
    if ((mmb = dynamic_cast<FmModelMemberBase*>(item)) && mmb->isOfType(typeID))
      return true;

  return false;
}

bool FapEventManager::isOnlyObjectsOfTypeSelected(int typeID)
{
  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(FapEventManager::tmpSelectedItem);
  if (mmb)
    return mmb->isOfType(typeID);
  else if (FapEventManager::permSelectedItems->back().empty())
    return false;

  for (FFaViewItem* item : FapEventManager::permSelectedItems->back())
    if (!(mmb = dynamic_cast<FmModelMemberBase*>(item)) || !mmb->isOfType(typeID))
      return false;

  return true;
}
//----------------------------------------------------------------------------

void FapEventManager::setActiveWindow(FFuMDIWindow* active)
{
  FFuMDIWindow* prevActive = FapEventManager::activeWindow;
  if (active == prevActive) return;

  FapEventManager::activeWindow = active;
  FapEventManager::sendActiveWindowChanged(FapEventManager::activeWindow,prevActive);
}
//----------------------------------------------------------------------------

void FapEventManager::setLoadingGraph(FmGraph* graph)
{
  FapEventManager::loadingGraph = graph;
}
//----------------------------------------------------------------------------

void FapEventManager::setActiveAnimation(FmAnimation* animation)
{
  FmAnimation* prevAnim = FapEventManager::activeAnimation;
  if (animation == prevAnim) return;

#if FAP_DEBUG > 4
  std::cout <<"\nFFaSwitchBoardCall: active animation"<< std::endl;
#endif
  FapEventManager::activeAnimation = animation;
  FFaSwitchBoardCall(&signalConnector,ACTIVE_ANIMATION_CHANGED,
                     FapEventManager::activeAnimation,prevAnim);
}
//----------------------------------------------------------------------------

void FapEventManager::addPermSelectedItems(const FFaViewItems& add,
					   FFaViewItems& added)
{
  added.clear();
  FFaViewItems& permSel = FapEventManager::permSelectedItems->back();
  for (FFaViewItem* item : add)
    // Check if it has been selected already, if not, put it into the array
    if (std::find(permSel.begin(),permSel.end(),item) == permSel.end()) {
      FapEventManager::highlightRendered(item,true);
      permSel.push_back(item);
      added.push_back(item);
    }
}
//----------------------------------------------------------------------------

void FapEventManager::removePermSelectedItems(const FFaViewItems& remove,
					      FFaViewItems& removed)
{
  removed.clear();
  FFaViewItems& pSel = FapEventManager::permSelectedItems->back();
  FFaViewItems::iterator it;
  for (FFaViewItem* item : remove)
    while ((it = std::find(pSel.begin(),pSel.end(),item)) != pSel.end()) {
      FapEventManager::highlightRendered(*it,false);
      pSel.erase(it);
      removed.push_back(item);
    }
}
//----------------------------------------------------------------------------

void FapEventManager::sendPermSelectionChanged(const FFaViewItems& permSelected,
					       const FFaViewItems& permSelectedSinceLast,
					       const FFaViewItems& permUnselectedSinceLast)
{
  FFaViewItems permSelectedFiltered;
  FapEventManager::filterNull(permSelected,permSelectedFiltered);//tmp since 0's are being selected internally
  FFaViewItems permSelectedSinceLastFiltered;
  FapEventManager::filterNull(permSelectedSinceLast,permSelectedSinceLastFiltered);//tmp since 0's are being selected internally
  FFaViewItems permUnselectedSinceLastFiltered;
  FapEventManager::filterNull(permUnselectedSinceLast,permUnselectedSinceLastFiltered);//tmp since 0's are being selected internally

#if FAP_DEBUG > 4
  std::cout <<"\nFFaSwitchBoardCall: permanent selection "<< permSelectedFiltered.size()
            <<"\n  Total:";
  for (FFaViewItem* item : permSelectedFiltered)
    std::cout <<" "<< item->getItemName() <<"("<< item->getItemID() <<")";
  std::cout <<"\n  Removed:";
  for (FFaViewItem* item : permUnselectedSinceLastFiltered)
    std::cout <<" "<< item->getItemName() <<"("<< item->getItemID() <<")";
  std::cout <<"\n  Added:";
  for (FFaViewItem* item : permSelectedSinceLastFiltered)
    std::cout <<" "<< item->getItemName() <<"("<< item->getItemID() <<")";
  std::cout << std::endl;
#endif

  FFaSwitchBoardCall(&signalConnector,PERMSELECTIONCHANGED,permSelectedFiltered,
                     permSelectedSinceLastFiltered,permUnselectedSinceLastFiltered);

  FapUACommandHandler::updateAllUICommandsSensitivity();
}
//----------------------------------------------------------------------------

void FapEventManager::sendTmpSelectionChanged(FFaViewItem* tmpSelected,
					      FFaViewItem* tmpUnselected,
					      const FFaViewItems& permSelected)
{
  FFaViewItems permSelectedFiltered;
  FapEventManager::filterNull(permSelected,permSelectedFiltered);//tmp since 0's are being selected internally

#if FAP_DEBUG > 4
  std::cout <<"\nFFaSwitchBoardCall: temporary selection "<< permSelectedFiltered.size();
  if (tmpSelected)
    std::cout <<"\n  Selected:"<< tmpSelected->getItemName() <<"("<< tmpSelected->getItemID()<<")";
  if (tmpUnselected)
    std::cout <<"\n  Unselected:"<< tmpUnselected->getItemName() <<"("<< tmpUnselected->getItemID()<<")";
  std::cout << std::endl;
#endif

  FFaSwitchBoardCall(&signalConnector,TMPSELECTIONCHANGED,
                     tmpSelected,tmpUnselected,permSelectedFiltered);
}
//----------------------------------------------------------------------------

void FapEventManager::sendPermSelectionStackChanged(bool pushed)
{
#if FAP_DEBUG > 4
  std::cout <<"\nFFaSwitchBoardCall: permanent selection strack "
            << std::boolalpha << pushed << std::endl;
#endif

  FFaSwitchBoardCall(&signalConnector,PERMSELECTIONSTACKCHANGED,pushed);
}
//----------------------------------------------------------------------------

void FapEventManager::sendActiveWindowChanged(FFuMDIWindow* newActive,FFuMDIWindow* prevActive)
{
#if FAP_DEBUG > 4
  std::cout <<"\nFFaSwitchBoardCall: active window";
  if (newActive)
    std::cout <<" "<< newActive->getName();
  std::cout << std::endl;
#endif

  FFaSwitchBoardCall(&signalConnector,ACTIVEWINDOWCHANGED,newActive,prevActive);
}
//----------------------------------------------------------------------------

void FapEventManager::onModelMemberDisconnected(FmModelMemberBase* item)
{
  if (FapEventManager::isPermSelected(item))
    {
      FapEventManager::isDisconnectingItems = true;
      FapEventManager::permUnselect(item);
      FapEventManager::isDisconnectingItems = false;
    }

  if (FapEventManager::isTmpSelected(item))
    {
      FapEventManager::isDisconnectingItems = true;
      FapEventManager::tmpSelect(NULL);
      FapEventManager::isDisconnectingItems = false;
    }
}
//----------------------------------------------------------------------------

void FapEventManager::filterNull(const FFaViewItems& in, FFaViewItems& out)
{
  out.clear();
  for (FFaViewItem* item : in)
    if (item) out.push_back(item);
}
//----------------------------------------------------------------------------

void FapEventManager::filterLVI(const FFaViewItems& in,
				std::vector<FFaListViewItem*>& out)
{
  out.clear();
  for (FFaViewItem* item : in)
    if (dynamic_cast<FFaListViewItem*>(item))
      out.push_back((FFaListViewItem*)item);
}
//----------------------------------------------------------------------------

void FapEventManager::filterMMB(const FFaViewItems& in,
				std::vector<FmModelMemberBase*>& out)
{
  out.clear();
  for (FFaViewItem* item : in)
    if (dynamic_cast<FmModelMemberBase*>(item))
      out.push_back((FmModelMemberBase*)item);
}
//----------------------------------------------------------------------------

void FapEventManager::highlightCurrentLayer(bool highlight)
{
  for (FFaViewItem* item : FapEventManager::permSelectedItems->back())
    FapEventManager::highlightRendered(item,highlight);
}
//----------------------------------------------------------------------------

void FapEventManager::highlightRendered(FFaViewItem* item, bool onOff)
{
  if (FapEventManager::isDisconnectingItems)
    return; // Never highlight while disconnecting

  FmIsRenderedBase* rItem = dynamic_cast<FmIsRenderedBase*>(item);
  if (rItem)
    rItem->highlight(onOff);
}
//----------------------------------------------------------------------------

FapEventManager::SignalConnector::SignalConnector() : FFaSwitchBoardConnector("FapEventManager")
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
                          FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
                          FFaSlot1S(FapEventManager,onModelMemberDisconnected,FmModelMemberBase*));
}
