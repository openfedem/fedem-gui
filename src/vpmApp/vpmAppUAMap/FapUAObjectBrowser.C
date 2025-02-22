// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAObjectBrowser.H"
#include "vpmUI/vpmUITopLevels/FuiObjectBrowser.H"


Fmd_SOURCE_INIT(FAPUAOBJECTBROWSER, FapUAObjectBrowser, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUAObjectBrowser::FapUAObjectBrowser(FuiObjectBrowser* ui)
  : FapUAExistenceHandler(ui),
    permSignalConnector(this), modelMemberChangedConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUAObjectBrowser);

  myUI = ui;

  // Find selection that created this dialog
  FmModelMemberBase* selectedItem = NULL;
  std::vector<FmModelMemberBase*> objs;
  FapEventManager::getMMBSelection(objs,selectedItem);
  if (!selectedItem && !objs.empty())
    myUI->update(objs.back());
}
//----------------------------------------------------------------------------

void FapUAObjectBrowser::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                                                const std::vector<FFaViewItem*>&,
                                                const std::vector<FFaViewItem*>&)
{
  // Show the last selected model member
  FmModelMemberBase* selectedItem = NULL;
  size_t i = totalSelection.size();
  while (i > 0 && !selectedItem)
    selectedItem = dynamic_cast<FmModelMemberBase*>(totalSelection[--i]);

  myUI->update(selectedItem);
}
//----------------------------------------------------------------------------

void FapUAObjectBrowser::onModelMemberChanged(FmModelMemberBase* changedObj)
{
  myUI->update(changedObj,true);
}
