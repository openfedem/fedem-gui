// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CREATE_TURBINE_TOWER_H
#define FUI_CREATE_TURBINE_TOWER_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuLabel;
class FFuLabelFrame;
class FFuLabelField;
class FFuNotes;
class FFuDialogButtons;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;
class FFuaQueryBase;


class FuiCreateTurbineTower : virtual public FFuTopLevelShell,
			      public FFuUAExistenceHandler,
			      public FFuUADataHandler,
			      public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiCreateTurbineTower();

  static FuiCreateTurbineTower* create(int xpos = 100, int ypos = 100,
				       int width = 100, int height = 100,
				       const char* title= "Fedem Tower Definition",
				       const char* name = "FuiCreateTurbineTower");

  virtual void setSensitivity(bool isSensitive);

  void setCB(const FFaDynCB1<FmModelMemberBase*>& aDynCB);
  void setValues(double rho, double E, double nu, double G);

protected:
  void initWidgets();
  virtual bool onClose();

private:
  virtual void onPoppedUp();

  void onDialogButtonClicked(int button);
  void onHeightChanged(double val);

  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuLabel*           headerImage;
  FFuLabel*           modelImage;
  FFuNotes*           notes;
  FFuLabelFrame*      towerGeometryFrame;
  FFuLabelField*      towerGeometryD1Field;
  FFuLabelField*      towerGeometryD2Field;
  FFuLabelField*      towerGeometryM1Field;
  FFuLabelField*      towerGeometryH1Field;
  FFuLabelField*      towerGeometryH2Field;
  FFuLabelField*      towerWallThicknField;
  FFuLabelFrame*      towerMeshFrame;
  FFuLabelField*      towerMeshN1Field;
  FFuLabelField*      towerMeshN2Field;
  FFuLabelFrame*      towerMaterialFrame;
  FuiQueryInputField* towerMaterialMenu;
  FFuLabelField*      towerMaterialRhoField;
  FFuLabelField*      towerMaterialEField;
  FFuLabelField*      towerMaterialNuField;
  FFuLabelField*      towerMaterialGField;
  FFuDialogButtons*   dialogButtons;
};


class FuaCreateTurbineTowerValues : public FFuaUIValues
{
public:
  double D1 = 1.0;
  double D2 = 0.0;
  double M1 = 68.0;
  double H1 = 68.0;
  double wallThickness = 0.1;
  int    N1 = 10;
  int    N2 = 10;

  FmModelMemberBase* materialObject = NULL;
  FFuaQueryBase*     materialQuery  = NULL;

  FFaDynCB1<FuiQueryInputFieldValues&> editCB;

  bool isSensitive = true;
  bool haveTurbine = false;
};

#endif
