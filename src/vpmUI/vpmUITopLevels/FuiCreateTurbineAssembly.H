// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CREATE_TURBINE_ASSEMBLY_H
#define FUI_CREATE_TURBINE_ASSEMBLY_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include <array>

class FFuLabel;
class FFuLabelFrame;
class FFuLabelField;
class FFuIOField;
class FFuFileBrowseField;
class FFuOptionMenu;
class FFuSpinBox;
class FFuPushButton;
class FFuToggleButton;
class FFuDialogButtons;
class FmModelMemberBase;
class FmBladeDesign;


namespace FWP {
  enum FieldNames {
    TOWER_X,
    TOWER_Y,
    TOWER_Z,
    TOWER_T,
    H1,
    HR,
    ALPHA,
    BETA,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    B1,
    B2,
    S,
    THETA,
    COG_X,
    COG_Y,
    COG_Z,
    NUM_FIELDS
  };
}


class FuiCreateTurbineAssembly : virtual public FFuTopLevelShell,
				 public FFuUAExistenceHandler,
				 public FFuUADataHandler,
				 public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiCreateTurbineAssembly();

  static FuiCreateTurbineAssembly* create(int xpos = 100, int ypos = 100,
					  int width = 100, int height = 100,
					  const char* title= "Fedem Turbine Definition",
					  const char* name = "FuiCreateTurbineAssembly");

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();
  void setMainImage(int drivelineType, int bearings);
  virtual bool onClose();

  void onDrivelineTypeChanged(int);
  void onBearingsChanged(int);

private:
  void onBladeDesignFileSelected(const std::string&,int);
  void onDialogButtonClicked(int button);
  virtual void onCopyButtonClicked() = 0;
  virtual void onPasteButtonClicked() = 0;

  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);
  virtual void onPoppedUp();

  void createOrUpdateTurbine();
  void setApplyButton(bool switchToUpdate = false);

protected:
  FFuLabel*           headerImage;
  FFuLabel*           modelImage;
  FFuLabel*           notesText;
  FFuOptionMenu*      drivelineTypeMenu;
  FFuOptionMenu*      bearingsMenu;
  FFuLabelFrame*      towerBaseFrame;
  FFuLabelFrame*      hubFrame;
  FFuLabelFrame*      drivelineFrame;
  FFuLabelFrame*      nacelleFrame;
  FFuIOField*         nameField;
  std::array<FFuLabelField*,FWP::NUM_FIELDS> myFields;
  FFuPushButton*      copyButton;
  FFuPushButton*      pasteButton;
  FFuSpinBox*         bladesNumField;
  FFuFileBrowseField* bladesDesignField;
  FFuToggleButton*    incCtrlSysToggle;
  FFuDialogButtons*   dialogButtons;

private:
  bool haveTurbine;
  FmBladeDesign* currentBladeDesign;
};


class FuaCreateTurbineAssemblyValues : public FFuaUIValues
{
public:
  char haveTurbine   = 0;     //!< 0: no turbine, 1=unattached, 2=attached
  int  drivelineType = 1;     //!< Drive line type (0=gearbox 1=direct)
  int  bearings      = 0;     //!< Number of bearings (0, 1 or 2)
  int  bladesNum     = 0;     //!< Number of blades (3 or 4)
  bool incCtrlSys    = false; //!< Include control system?

  std::string        name;                   //!< Turbine name
  FmModelMemberBase* bladesDesign = NULL;    //!< Turbine blade design object
  std::array<double,FWP::NUM_FIELDS> geom{}; //!< Geometry parameters

  bool isSensitive = true;
};

#endif
