// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_CREATE_SENSOR_CMD_H
#define FAP_CREATE_SENSOR_CMD_H

#include "vpmApp/vpmAppCmds/FapCmdsBase.H"
#include "vpmApp/FapEventManager.H"
#include "FFaLib/FFaPatterns/FFaSingelton.H"

class FmSensorBase;
class FmEngine;
class FFaViewItem;


class FapCreateSensorCmd : public FapCmdsBase,
			   public FFaSingelton<FapCreateSensorCmd>
{
public:
  FapCreateSensorCmd() : permSignalConnector(this) { myEngine = NULL; myArg = 0; }

  const char* getTypeIDName() const { return "FapCreateSensorCmd"; }

  void createSensor(unsigned int iArg = 0);
  void setSensor(FmSensorBase* sens);

  static void enterMode();
  static void cancelMode();
  static void done();

private:
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>& selectedSinceLast,
			      const std::vector<FFaViewItem*>& unselectedSinceLast);

  FmEngine* myEngine;
  unsigned int myArg;

  FapPermSelChangedReceiver<FapCreateSensorCmd> permSignalConnector;
  friend class FapPermSelChangedReceiver<FapCreateSensorCmd>;
};

#endif
