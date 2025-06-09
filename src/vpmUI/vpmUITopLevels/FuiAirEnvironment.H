// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_AIR_ENVIRONMENT_H
#define FUI_AIR_ENVIRONMENT_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include <string>

class FFuOptionMenu;
class FFuToggleButton;
class FFuPushButton;
class FFuLabelFrame;
class FFuLabelField;
class FFuIOField;
class FFuFileBrowseField;
class FFuDialogButtons;


class FuiAirEnvironment : virtual public FFuTopLevelShell,
			  public FFuUAExistenceHandler,
			  public FFuUADataHandler,
			  public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiAirEnvironment();

  static FuiAirEnvironment* create(int xpos = 100, int ypos = 100,
				   int width = 100, int height = 100,
				   const char* title= "Aerodynamic Setup",
				   const char* name = "FuiAirEnvironment");

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();
  virtual bool onClose();

private:
  virtual void onPoppedUp();

  void onStallChanged(int);
  void onInflowChanged(int);
  void onInductionChanged(int);
  void onTLossChanged(int);
  void onHLossChanged(int);
  void onWindToggled(bool);
  void onWindTurbClicked(void);
  void onTowerToggled(bool);
  void onDtToggled(bool);
  void onDialogButtonClicked(int button);

  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuOptionMenu*      stallModMenu;
  FFuToggleButton*    useCMToggle;
  FFuOptionMenu*      infModMenu;
  FFuOptionMenu*      indModMenu;
  FFuIOField*         aTolerField;
  FFuOptionMenu*      tipLossMenu;
  FFuOptionMenu*      hubLossMenu;
  FFuLabelFrame*      windFrame;
  FFuToggleButton*    windToggle;
  FFuPushButton*      windTurbButton;
  FFuLabelField*      windSpeedField;
  FFuLabelField*      windDirField;
  FFuFileBrowseField* windField;
  FFuLabelFrame*      towerFrame;
  FFuToggleButton*    towerPotToggle;
  FFuToggleButton*    towerShdToggle;
  FFuFileBrowseField* towerField;
  FFuLabelField*      airDensField;
  FFuLabelField*      kinViscField;
  FFuLabelField*      dtAeroField;
  FFuToggleButton*    dtAeroToggle;
  FFuDialogButtons*   dialogButtons;

private:
  double myAeroDt;
  double windTurbHubHeight;
  double windTurbGridSize;
  double windTurbDuration;
  double windTurbTimeInc;
  std::string windTurbOutputPath;
};


class FuaAirEnvironmentValues : public FFuaUIValues
{
public:
  int         stallMod = 0;  //!< Dynamic stall model
  bool        useCM = false; //!< Use aerodynamic pitching moment model?
  int         infMod = 0;    //!< Inflow model
  int         indMod = 0;    //!< Induction-factor model
  double      aToler = 0.0;  //!< Induction-factor tolerance
  int         tlMod = 0;     //!< Tip-loss model (EQUIL only)
  int         hlMod = 0;     //!< Hub-loss model (EQUIL only)
  bool        constWnd = false; //!< Use constant wind?
  double      windVel = 0.0; //!< Constant wind speed
  double      windDir = 0.0; //!< Constant wind direction
  std::string windFile;      //!< Name of wind data file
  double      windTurbHubHeight = 0.0; //!< Used by TurbWind.exe
  double      windTurbGridSize  = 0.0; //!< Used by TurbWind.exe
  double      windTurbDuration  = 0.0; //!< Used by TurbWind.exe
  double      windTurbTimeInc   = 0.0; //!< Used by TurbWind.exe
  bool        twrPot  = false; //!< Calculate tower potential flow?
  bool        twrShad = false; //!< Calculate tower shadow?
  std::string twrFile;         //!< Name of tower drag file
  double      airDens = 0.0;   //!< Air density (kg/m^3)
  double      kinVisc = 0.0;   //!< Kinematic air viscosity
  double      dtAero  = 0.0;   //!< Time interval for aerodynamic calculations
  bool        useDSdt = false; //!< Use dynamics solver time interval?

  std::string modelFilePath; //!< Path to current model file
  bool        isSensitive = true;
};

#endif
