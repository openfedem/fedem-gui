// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_MAIN_WINDOW_H
#define FAP_UA_MAIN_WINDOW_H

#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAExistenceHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUACommandHandler.H"
#include "vpmApp/vpmAppUAMap/vpmAppUAMapHandlers/FapUAFinishHandler.H"

class FuiMainWindow;
class FFuaCmdHeaderItem;

class FapUAMainWindow : public FapUAExistenceHandler,
                        public FapUACommandHandler,
                        public FapUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FapUAMainWindow(FuiMainWindow* ui);
  virtual ~FapUAMainWindow() {}

private:
  virtual FFuaUICommands* getCommands();
  virtual void finishUI();

private:
  FuiMainWindow* ui;

  FFuaCmdHeaderItem fileHeader;
  FFuaCmdHeaderItem fileExportHeader;
  FFuaCmdHeaderItem fileRecentHeader;
  FFuaCmdHeaderItem editHeader;
  FFuaCmdHeaderItem viewHeader;
  FFuaCmdHeaderItem viewZoomHeader;
  FFuaCmdHeaderItem viewShiftHeader;
  FFuaCmdHeaderItem viewPredefHeader;
  FFuaCmdHeaderItem toolsHeader;
  FFuaCmdHeaderItem windpowerHeader;
  FFuaCmdHeaderItem marineHeader;
  FFuaCmdHeaderItem mechHeader;
  FFuaCmdHeaderItem ctrlHeader;
  FFuaCmdHeaderItem solveMainHeader;
  FFuaCmdHeaderItem resultHeader;
  FFuaCmdHeaderItem addonsHeader;
  FFuaCmdHeaderItem windowsHeader;
  FFuaCmdHeaderItem helpHeader;

  FFuaCmdHeaderItem createSpiderHeader;
  FFuaCmdHeaderItem createRigidSpiderHeader;
  FFuaCmdHeaderItem createFlexibleSpiderHeader;
  FFuaCmdHeaderItem createJointHeader;
  FFuaCmdHeaderItem createFrictionHeader;
  FFuaCmdHeaderItem createSpringFunctionHeader;
  FFuaCmdHeaderItem createDamperFunctionHeader;
  FFuaCmdHeaderItem createWellHeader;
  FFuaCmdHeaderItem createUDEHeader;

  //solveToolBar
  FFuaCmdHeaderItem stressHeader;
  FFuaCmdHeaderItem modesHeader;
  FFuaCmdHeaderItem rosetteHeader;
  FFuaCmdHeaderItem strainCoatHeader;

  //mechToolBar
  FFuaCmdHeaderItem mechCreateSingleJointHeader;
  FFuaCmdHeaderItem mechCreateMultiJointHeader;
  FFuaCmdHeaderItem mechCreateGearHeader;
  FFuaCmdHeaderItem mechCreateSpringHeader;
  FFuaCmdHeaderItem mechCreateTireHeader;
  FFuaCmdHeaderItem mechCreateForceHeader;
  FFuaCmdHeaderItem mechCreateSensorHeader;
  FFuaCmdHeaderItem mechStickerHeader;
  FFuaCmdHeaderItem mechMoveHeader;

  //ctrlToolBar
  FFuaCmdHeaderItem ctrlCreateInputHeader;
  FFuaCmdHeaderItem ctrlCreateAmplifierHeader;
  FFuaCmdHeaderItem ctrlCreateComparatorHeader;
  FFuaCmdHeaderItem ctrlCreateIntegratorHeader;
  FFuaCmdHeaderItem ctrlCreateTimeDelayHeader;
  FFuaCmdHeaderItem ctrlCreateLogicalSwitchHeader;
  FFuaCmdHeaderItem ctrlCreateDeadZoneHeader;
  FFuaCmdHeaderItem ctrlCreatePidHeader;
  FFuaCmdHeaderItem ctrlCreatePlimIlimDHeader;
  FFuaCmdHeaderItem ctrlCreateRealPoleHeader;
  FFuaCmdHeaderItem ctrlCreate1ordTFHeader;

  //threedViewBar
  FFuaCmdHeaderItem threedSymbolSizeHeader;
};

#endif
