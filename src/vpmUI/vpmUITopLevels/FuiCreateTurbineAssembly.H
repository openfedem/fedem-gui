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

class FFuOptionMenu;
class FFuLabel;
class FFuLabelFrame;
class FFuLabelField;
class FFuFileBrowseField;
class FFuSpinBox;
class FFuDialogButtons;
class FFuPushButton;
class FFuToggleButton;
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
  virtual ~FuiCreateTurbineAssembly() {}

  static FuiCreateTurbineAssembly* create(int xpos = 100, int ypos = 100,
					  int width = 100, int height = 100,
					  const char* title= "Fedem Turbine Definition",
					  const char* name = "FuiCreateTurbineAssembly");

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();
  void setMainImage(int drivelineType, int bearings);
  virtual bool onClose();

private:
  virtual void placeWidgets(int width, int height);
  virtual void onPoppedUp();

protected:
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

  void createOrUpdateTurbine();

protected:
  FFuLabel*           headerImage;
  FFuLabel*           modelImage;
  FFuLabel*           drivelineTypeLabel;
  FFuOptionMenu*      drivelineTypeMenu;
  FFuLabel*           bearingsLabel;
  FFuOptionMenu*      bearingsMenu;
  FFuLabel*           nameLabel;
  FFuLabelFrame*      towerBaseFrame;
  FFuLabelFrame*      hubFrame;
  FFuLabelFrame*      drivelineFrame;
  FFuLabelFrame*      nacelleFrame;
  FFuLabelField*      nameField;
  std::array<FFuLabelField*,FWP::NUM_FIELDS> myFields;
  FFuPushButton*      copyButton;
  FFuPushButton*      pasteButton;
  FFuLabel*           bladesNumLabel;
  FFuSpinBox*         bladesNumField;
  FFuLabel*           bladesDesignLabel;
  FFuFileBrowseField* bladesDesignField;
  FFuToggleButton*    incCtrlSysToggle;
  FFuLabel*           notesImage;
  FFuLabel*           notesLabel;
  FFuLabelFrame*      notesSeparator;
  FFuLabel*           notesText;
  FFuDialogButtons*   dialogButtons;

private:
  bool haveTurbine;
  FmBladeDesign* currentBladeDesign;
};
////////////////////////////////////////////////////////////////////////////////

class FuaCreateTurbineAssemblyValues : public FFuaUIValues
{
public:
  FuaCreateTurbineAssemblyValues() : geom{} {
    haveTurbine = incCtrlSys = false;
    drivelineType = bearings = bladesNum = 0;
    bladesDesign = NULL;
    isSensitive = true;
  }
  virtual ~FuaCreateTurbineAssemblyValues() {}

  char               haveTurbine;   //!< 0: no turbine, 1=unattached, 2=attached
  int                drivelineType; //!< Drive line type (=gearbox 1=direct)
  int                bearings;      //!< Number of bearings (0, 1 or 2)
  int                bladesNum;     //!< Number of blades (3 or 4)
  bool               incCtrlSys;    //!< Include control system?
  FmModelMemberBase* bladesDesign;  //!< Pointer to turbine blade design object
  std::string        name;          //!< Turbine name
  std::array<double,FWP::NUM_FIELDS> geom; //!< Geometry parameters

  bool isSensitive;
};
////////////////////////////////////////////////////////////////////////////////

#endif
