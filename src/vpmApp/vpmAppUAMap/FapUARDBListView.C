// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUARDBListView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"

#include "vpmPM/FpExtractor.H"
#include "FFrLib/FFrVariableReference.H"
#include "FFaLib/FFaDefinitions/FFaResultDescription.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"
#include "FFaLib/FFaString/FFaStringExt.H"


//----------------------------------------------------------------------------

FapUARDBListView::FapUARDBListView(FuiItemsListView* ui) : FapUAItemsListView(ui)
{
  this->extractor = NULL;
  this->topLevelVarsOnly = false;
  this->hasApplIndependentSelection = true;
  this->sortMode = FapUAItemsListView::NONE;
}
//----------------------------------------------------------------------------

void FapUARDBListView::setExtractor(FFrExtractor* ex)
{
  this->extractor = dynamic_cast<FpExtractor*>(ex);

  this->updateSession();
}
//----------------------------------------------------------------------------

void FapUARDBListView::setTopLevelItemDescr(const FFaResultDescription& item,
					    bool includeMyself)
{
  if (!this->extractor) return;

  this->FapUAItemsListView::setTopLevelItem(this->extractor->search(item),
					    includeMyself);
}
//----------------------------------------------------------------------------

FFrEntryBase* FapUARDBListView::findItem(const FFaResultDescription& item)
{
  if (!this->extractor) return NULL;

  return this->extractor->search(item);
}
//----------------------------------------------------------------------------

void FapUARDBListView::showTopLevelVarsOnly()
{
  this->topLevelVarsOnly = true;
  this->updateSession();
  this->topLevelVarsOnly = false;
}
//----------------------------------------------------------------------------

bool FapUARDBListView::verifyItem(FFaListViewItem* item)
{
  if (!this->FapUAItemsListView::verifyItem(item)) return false;

  // Filter out the Free-free modes and Eigenvalues for the Component modes
  // These are results from link reduction and contain typically only a
  // single data point each (not much to plot, really...)

  FFrEntryBase* ffritem = (FFrEntryBase*)item;
  if (!ffritem->isIG())
    return true;

  if (ffritem->getType() == "Free-free reduced modes")
    return false;

  if (ffritem->getType() != "Eigenvalues")
    return true;

  FFrEntryBase* parent = ffritem->getOwner();
  if (parent->isIG())
    return parent->getType() != "Component modes";

  return true;
}
//----------------------------------------------------------------------------

void FapUARDBListView::getChildren(FFaListViewItem* parent,
				   std::vector<FFaListViewItem*>& children) const
{
  children.clear();
  if (!this->extractor) return;

  FFrFieldEntryBase* ffrparent = dynamic_cast<FFrFieldEntryBase*>(parent);
  if (ffrparent)
    for (FFrEntryBase* field : ffrparent->dataFields)
      children.push_back(field);
  else if (!parent)
  {
    this->extractor->getTopLevelVars(children);
    if (!this->topLevelVarsOnly)
      this->extractor->getSuperObjectGroups(children);
  }
}
//----------------------------------------------------------------------------

std::string FapUARDBListView::getItemText(FFaListViewItem* item)
{
  FFrEntryBase* ffr = static_cast<FFrEntryBase*>(item);
  if (!ffr->isOG()) return ffr->getDescription();

  std::string name = ffr->getType();
  if (ffr->hasUserID())
    name += FFaNumStr("[%d]",ffr->getUserID());
  else if (ffr->hasBaseID())
    name += FFaNumStr("{%d}",ffr->getBaseID());
  name += " " + ffr->getDescription();
  return name;
}
//----------------------------------------------------------------------------

const char** FapUARDBListView::getItemPixmap(FFaListViewItem* item)
{
  if (dynamic_cast<FFrVariableReference*>(item))
    this->ui->setItemDragable(this->getMapItem(item),true);

  return (const char**)NULL;
}
//----------------------------------------------------------------------------

FFuaUICommands* FapUARDBListView::getCommands()
{
  // No commands in this list view yet
  return new FuaItemsLVCommands();
}
//----------------------------------------------------------------------------
