// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_FUNCTION_PROPERTIES_H
#define FAP_UA_FUNCTION_PROPERTIES_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUADataHandler.H"
#include "vpmApp/FapEventManager.H"
#include <vector>
#include <string>

class FuiFunctionProperties;
class FmModelMemberBase;
class FmMathFuncBase;
class FmEngine;
class FFaViewItem;


class FapUAFunctionProperties : public FapUAExistenceHandler,
                                public FapUADataHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAFunctionProperties(FuiFunctionProperties* ui);

  // Mode and state handling
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                              const std::vector<FFaViewItem*>&,
                              const std::vector<FFaViewItem*>&);
  void onFileBrowseActivated();

protected:
  virtual void setDBValues(FFuaUIValues* values);
  virtual void getDBValues(FFuaUIValues* values);

private:
  virtual FFuaUIValues* createValuesObject();

  // Callbacks distributed to the creating ui :

  void addNumberCB(double nr);
  void addNumbersCB(double nr1, double nr2);
  void deleteNumberCB(int idx);
  void clearNumbersCB();
  void funcTypeSwitchedCB(int idx);
  void pasteCB(const std::string&);
  void getChannelListCB();
  void acceptExprCB();
  void previewCB();

  // Stuff for verification
  void verifyGeneralFunctionCB(bool& isOK, FmModelMemberBase* item);
  void verifyArgumentSensor(bool& isOK, FmModelMemberBase* item);

  // Convenience methods for the cb's
  FmMathFuncBase* getMyFunction();
  FmEngine*       getMyEngine();

  // Variables
  FmModelMemberBase*     mySelectedFmItem;
  FuiFunctionProperties* myFunctionPropertiesUI;

  // Signal receiver
  FapPermSelChangedReceiver<FapUAFunctionProperties> signalConnector;
};

#endif
