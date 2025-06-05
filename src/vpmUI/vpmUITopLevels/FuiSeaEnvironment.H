// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SEA_ENVIRONMENT_H
#define FUI_SEA_ENVIRONMENT_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuLabelFrame;
class FFuIOField;
class FFuDialogButtons;
class Fui3DPoint;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FFuaQueryBase;
class FmModelMemberBase;


class FuiSeaEnvironment : virtual public FFuTopLevelShell,
			  public FFuUAExistenceHandler,
			  public FFuUADataHandler,
			  public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiSeaEnvironment();

  static FuiSeaEnvironment* create(int xpos = 100, int ypos = 100,
				   int width = 100, int height = 100,
				   const char* title= "Sea Environment",
				   const char* name = "FuiSeaEnvironment");

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();
  virtual bool onClose();

private:
  virtual void onPoppedUp() { this->updateUIValues(); }

  void onCurrentSelected(FmModelMemberBase*);
  void onWaveSelected(FmModelMemberBase*);
  void onDialogButtonClicked(int button);

  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuIOField*    waterDensityField;
  FFuIOField*    meanSeaLevelField;
  FFuIOField*    seaDepthField;

  FFuLabelFrame* gravitationFrame;
  Fui3DPoint*    gravitationVector;

  FFuLabelFrame* waveDirectionFrame;
  Fui3DPoint*    waveDirectionVector;

  FFuLabelFrame* marineGrowthFrame;
  FFuIOField*    marineGrowthDensityField;
  FFuIOField*    marineGrowthThicknessField;
  FFuIOField*    marineGrowthUpperLimitField;
  FFuIOField*    marineGrowthLowerLimitField;

  FuiQueryInputField* waveQueryField;
  FuiQueryInputField* currQueryField;
  FuiQueryInputField* currDirQueryField;
  FuiQueryInputField* currScaleQueryField;
  FuiQueryInputField* hdfScaleQueryField;

  FFuDialogButtons* dialogButtons;
};


class FuaSeaEnvironmentValues : public FFuaUIValues
{
public:
  FFuaQueryBase* waveQuery  = NULL;
  FFuaQueryBase* currQuery  = NULL;
  FFuaQueryBase* scaleQuery = NULL;

  FmModelMemberBase* waveFunction    = NULL;
  FmModelMemberBase* currFunction    = NULL;
  FmModelMemberBase* currDirFunction = NULL;
  FmModelMemberBase* currScaleEngine = NULL;
  FmModelMemberBase* hdfScaleEngine  = NULL;

  FFaDynCB1<FuiQueryInputFieldValues&> editCB;

  double waterDensity  = 0.0;
  double seaLevelValue = 0.0;
  double seaDepth      = 0.0;
  FaVec3 gravitation;
  FaVec3 waveDirection;

  double marineGrowthDensity    = 0.0;
  double marineGrowthThickness  = 0.0;
  double marineGrowthUpperLimit = 0.0;
  double marineGrowthLowerLimit = 0.0;

  bool waveDirSens = true;
  bool isSensitive = true;
};

#endif
