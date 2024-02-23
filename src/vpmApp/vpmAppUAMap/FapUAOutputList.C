// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAOutputList.H"
#include "vpmUI/vpmUITopLevels/FuiOutputList.H"
#include "vpmUI/Fui.H"


Fmd_SOURCE_INIT(FAPUAOUTPUTLIST, FapUAOutputList, FapUAExistenceHandler);

//----------------------------------------------------------------------------

FapUAOutputList::FapUAOutputList(FuiOutputList* uic)
  : FapUAExistenceHandler(uic), FapUAFinishHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUAOutputList);

  this->ui = uic;
}
//----------------------------------------------------------------------------

void FapUAOutputList::finishUI()
{
  Fui::outputListUI(false,true);
}
