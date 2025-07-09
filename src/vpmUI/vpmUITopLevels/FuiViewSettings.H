// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_VIEW_SETTINGS_H
#define FUI_VIEW_SETTINGS_H

#include <array>
#include <map>

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include "FFuLib/FFuColor.H"

class FFuIOField;
class FFuPushButton;
class FFuToggleButton;
class FFuFrame;
class FFuLabelFrame;
class FFuScale;
class FFuColorDialog;
class FuaViewSettingsValues;


class FuiViewSettings : virtual public FFuTopLevelShell,
			public FFuUAExistenceHandler,
			public FFuUADataHandler
{
  Fmd_HEADER_INIT();

public:
  FuiViewSettings();
  virtual ~FuiViewSettings();

  static FuiViewSettings* create(int xpos=100, int ypos=100,
				 int width=100,int height=100,
				 const char* title = "General Appearance",
				 const char* name  = "FuiViewSettings");

  virtual void setUIValues(const FFuaUIValues* values);
  bool getValues(FuaViewSettingsValues* values) const;

  // Symbol fields

  enum {
    JOINT,
    SPR_DA,
    GEAR,
    LOAD,
    SENSOR,
    STRAIN_ROSETTE,
    STICKER,
    TIRES,
    TRIADS,
    NSYMBOLS
  };

  enum {
    REVOLUTE,
    BALL,
    RIGID,
    FREE,
    PRISMATIC,
    CYLINDRIC,
    CAM,
    NJOINTS
  };

protected:

  void initWidgets();

  virtual void onPoppedUp();
  virtual void onPoppedDown();
  virtual bool onClose();

private:
  void onColorEditClicked(FFuComponentBase* dlg);
  void onColDlgChanged(const FFuColor& col, FFuColorDialog* dlg);
  void onColDlgClosed(const FFuColor& col, FFuColorDialog* dlg);

  void toggleValueChanged(int,bool) { this->updateDBValues(); }
  void sliderValueChanged(int)      { this->updateDBValues(); }
  void fieldValueChanged(double)    { this->updateDBValues(); }
  void onCloseButtonClicked()       { this->onClose(); }

  virtual bool updateDBValues(bool = true);

protected:
  // Main frames

  FFuLabelFrame* mySymbolsFrame;
  FFuLabelFrame* myDefaultColorFrame;
  FFuLabelFrame* myViewerOptionsFrame;

  // Dialog button:

  FFuPushButton* myCloseButton;

  // Symbols:

  std::array<FFuToggleButton*,NJOINTS> myJointSymbolToggles;
  std::array<FFuToggleButton*,3>       myPartSymbolToggles;

  FFuToggleButton* myBeamTriadsToggle;
  FFuToggleButton* myBeamsToggle;
  FFuToggleButton* myBeamCSToggle;
  FFuToggleButton* myPartsToggle;
  FFuToggleButton* myRefPlaneToggle;
  FFuToggleButton* mySeaStateToggle;
  FFuToggleButton* myWavesToggle;

  FFuIOField* mySymbolSizeField;
  FFuScale*   myLineWidthScale;

  std::array<FFuToggleButton*,NSYMBOLS> mySymbolToggles;
  std::array<FFuFrame*       ,NSYMBOLS> myColorFrames;
  std::array<FFuPushButton*  ,NSYMBOLS> myColorEditButtons;
  std::array<FFuColorDialog* ,NSYMBOLS> myColorDialog;

  // Default colors:

  FFuFrame* myGroundedColFrame;
  FFuFrame* myUnattachedColFrame;
  FFuFrame* myBackgroundColFrame;

  FFuPushButton* myGroundedColEditButton;
  FFuPushButton* myUnattachedColEditButton;
  FFuPushButton* myBackgroundColEditButton;

  FFuColorDialog* myGroundedColDialog;
  FFuColorDialog* myUnattachedColDialog;
  FFuColorDialog* myBackgroundColDialog;

  // Viewer options

  FFuToggleButton* myFogToggle;
  FFuScale*        myFogScale;
  FFuToggleButton* myTransparencyToggle;
  FFuToggleButton* myAntialiazingToggle;

  // Toggle groups
  FFuLogicButtonGroup myToggleButtonGroup;
  FFuLogicButtonGroup myColorEditButtonGroup;

  // Mapping of the color dialogs and frames
  std::map<FFuColorDialog*,FFuFrame*> myColorDialogMap;
};


class FuaViewSettingsValues : public FFuaUIValues
{
  // toggles, symbol size, line width and fog visibility are transfered.
  // Colors are set directly from each color-selector.

public:
  // Viewer options
  bool isFogOn;
  bool isAntialiazingOn;
  bool isSimpleTransparencyOn;
  double myFogVisibility;

  // symbols
  std::array<bool,FuiViewSettings::NSYMBOLS> isSymbolOn;

  bool isRevoluteJointsOn;
  bool isBallJointsOn;
  bool isRigidJointsOn;
  bool isFreeJointsOn;
  bool isPrismaticJointsOn;
  bool isCylindricJointsOn;
  bool isCamJointsOn;

  bool isBeamTriadsOn;
  bool isBeamsOn;
  bool isBeamCSOn;
  bool isPartsOn;
  bool isPartCSOn;
  bool isInternalPartCSsOn;
  bool isPartCoGCSOn;
  bool isRefPlaneOn;
  bool isSeaStateOn;
  bool isWavesOn;
  double mySymbolSize;
  int myLineWidth;

  // colors
  std::array<FFuColor,FuiViewSettings::NSYMBOLS> mySymbolColor;
  FFuColor myGroundedColor;
  FFuColor myUnattachedColor;
  FFuColor myBackgroundColor;

  bool showTireSettings;
};

#endif
