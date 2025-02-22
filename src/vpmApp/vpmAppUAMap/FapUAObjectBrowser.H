// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_OBJECT_BROWSER_H
#define FAP_UA_OBJECT_BROWSER_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/FapEventManager.H"
#include "vpmDB/FmModelMemberConnector.H"

class FuiObjectBrowser;


class FapUAObjectBrowser : public FapUAExistenceHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAObjectBrowser(FuiObjectBrowser* ui);
  virtual ~FapUAObjectBrowser() {}

private:
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>&,
			      const std::vector<FFaViewItem*>&);
  void onModelMemberChanged(FmModelMemberBase* changedObj);

  FuiObjectBrowser* myUI;

  FapPermSelChangedReceiver<FapUAObjectBrowser> permSignalConnector;
  FmModelMemberChangedReceiver<FapUAObjectBrowser> modelMemberChangedConnector;

  friend class FapPermSelChangedReceiver<FapUAObjectBrowser>;
  friend class FmModelMemberChangedReceiver<FapUAObjectBrowser>;
};

#endif
