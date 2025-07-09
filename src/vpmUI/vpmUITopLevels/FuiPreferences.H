// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_PREFERENCES_H
#define FUI_PREFERENCES_H

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include <array>
#include <map>

class FFuLabel;
class FFuLabelFrame;
class FFuIOField;
class FFuToggleButton;
class FFuRadioButton;


class FuiPreferences : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiPreferences();

  // Static create method to use instead of constructor
  // Implementation in GUILib-dependent implementation file
  static FuiPreferences* create(int xpos = 100, int ypos  = 100,
				int width= 100, int height= 100,
				const char* title= "Additional Solver Options",
				const char* name = "FuiPreferences");

  enum { //fields
    REDUCER,
    SOLVER,
    STRESS,
    MODES,
    GAGE,
    FPP,
    MAX_CONC_PROC,
    EQ_SOLVER_SWAP,
    REC_MATRIX_SWAP,
    SOLVER_PREFIX,
    SOLVER_PATH,
    NFIELDS
  };

  virtual void setSensitivity(bool sens);

protected:
  virtual void initWidgets();

private:
  virtual void setUIValues(const FFuaUIValues* values);
  virtual bool updateDBValues(bool);

  void onEqSolverToggeled(bool);
  void onEqSolverGroupToggeled(int,bool);
  void onRecMatrixToggeled(bool);
  void onRecMatrixGroupToggeled(int,bool);
  void onSolvePrefixToggeled(bool);
  void onSolvePathToggeled(bool);

public:
  void onRestartToggeled(bool); // invoked from FuiAdvAnalysisOptions

protected:
  std::array<FFuIOField*,NFIELDS> optFields;
  std::array<FFuLabel*,NFIELDS>   labels;

  FFuLabelFrame* reducerFrame;
  FFuToggleButton* eqSolverToggle;
  FFuRadioButton* eqSolverAutomatic;
  FFuRadioButton* eqSolverManual;
  FFuLogicButtonGroup eqSolverBtnGroup;
  FFuToggleButton* recMatrixToggle;
  FFuRadioButton* recMatrixAutomatic;
  FFuRadioButton* recMatrixManual;
  FFuLogicButtonGroup recMatrixBtnGroup;

  FFuLabelFrame* remoteSolveFrame;
  FFuToggleButton* solverPrefixToggle;
  FFuToggleButton* solverPathToggle;

private:
  bool isRestartActive;
  bool isSensitive;
};
////////////////////////////////////////////////////////////////////////////////

class FuaPreferencesValues : public FFuaUIValues
{
public:
  std::array<std::string,FuiPreferences::NFIELDS> options;
  std::map<int,bool> optionStatus;

  bool useEqSolverBuffer = false;
  bool autoEqSolverBuffer = true;
  bool useRecMatrixBuffer = false;
  bool autoRecMatrixBuffer = true;

  bool useSolverPrefix = false;
  bool useSolverPath = false;
  bool isRestarted = false;
  bool isSensitive = true;
  bool isTouchable = true;
};
////////////////////////////////////////////////////////////////////////////////

#endif
