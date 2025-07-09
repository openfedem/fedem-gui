// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MODEL_PREFERENCES_H
#define FUI_MODEL_PREFERENCES_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>

class FFuLabelFrame;
class FFuMemo;
class FFuIOField;
class FFuFileBrowseField;
class FFuOptionMenu;
class FFuPushButton;
class FFuToggleButton;
class FFuDialogButtons;
class Fui3DPoint;


class FuiModelPreferences : virtual public FFuTopLevelShell,
			    public FFuUAExistenceHandler,
			    public FFuUADataHandler,
			    public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiModelPreferences();

  static FuiModelPreferences* create(int xpos = 100, int ypos = 100,
				     int width = 100, int height = 100,
				     const char* title= "Model Preferences",
				     const char* name = "FuiModelPreferences");

public:
  virtual void setSensitivity(bool isSensitive);

  void setUpdateDescriptionCB(const FFaDynCB1<const std::string&>& dynCB);
  void setChangeRepositoryCB(const FFaDynCB1<bool>& dynCB);

protected:
  void initWidgets();
  virtual bool onClose();

private:
  virtual void onPoppedUp();

  void updateDescriptionOnly();
  void onChangeButtonPushed();
  void onSwitchButtonPushed();
  void onDialogButtonClicked(int button);

  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuLabelFrame* descriptionFrame;
  FFuMemo*       descriptionMemo;

  FFuLabelFrame*   repositoryFrame;
  FFuIOField*      repositoryField;
  FFuPushButton*   changeButton;
  FFuPushButton*   switchButton;
  FFuToggleButton* overwriteButton;

  FFuLabelFrame* unitsFrame;
  FFuOptionMenu* unitsMenu;

  FFuLabelFrame* modelingTolFrame;
  FFuIOField*    modelingTolField;

  FFuLabelFrame* gravitationFrame;
  Fui3DPoint*    gravitationVector;

  FFuLabelFrame* initialVelFrame;
  Fui3DPoint*    initialVelVector;

  FFuLabelFrame*      extFuncFileFrame;
  FFuToggleButton*    extFuncFileButton;
  FFuFileBrowseField* extFuncFileField;

  FFuDialogButtons* dialogButtons;

private:
  bool                          IAmInternalRepository;
  FFaDynCB1<bool>               changeRepositoryCB;
  FFaDynCB1<const std::string&> updateDescriptionCB;
};


class FuaModelPreferencesValues : public FFuaUIValues
{
public:
  std::string modelFilePath;
  std::string description;
  std::string repository;
  double      modelingTol = 0.0;
  std::vector<std::string> units;
  std::string selectedUnits;
  std::string extFuncFileName;
  char        useFuncFile = -1;

  FaVec3 gravitation;
  FaVec3 velocity;

  bool overwriteFE = false;
  bool isSensitive = true;
};

#endif
