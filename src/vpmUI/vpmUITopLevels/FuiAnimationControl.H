// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_ANIMATION_CONTROL_H
#define FUI_ANIMATION_CONTROL_H

#include <vector>
#include <string>

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"

class FFuLabelFrame;
class FFuToggleButton;
class FFuRadioButton;
class FFuIOField;
class FFuOptionMenu;
class FFuDialogButtons;
class FuaQueryBase;
class FuiQueryInputField;
class FFuaQueryBase;
class FmModelMemberBase;


class FuiAnimationControl : virtual public FFuTopLevelShell,
                            public FFuUAExistenceHandler,
                            public FFuUADataHandler,
                            public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiAnimationControl();

  static FuiAnimationControl* create(int xpos = 100, int ypos  = 100,
                                     int width= 100, int height= 100,
                                     const char* title= "Animation Control",
                                     const char* name = "FuiQtAnimationControl");

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();
  virtual bool onClose();

private:
  virtual void onPoppedUp();

  // Inside callback functions
  void onButtonToggled(bool)      { this->updateDBValues(); }
  void onMenuSelected(int)        { this->updateDBValues(); }
  void onFieldValueChanged(char*) { this->updateDBValues(); }

  void onTickMarkCountToggled(bool toggle);
  void onTickMarkSpaceToggled(bool toggle);
  void onDialogButtonClicked(int button);

  // From datahandler
  virtual FFuaUIValues* createValuesObject();
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

protected:
  FFuLabelFrame*      cameraFrame;
  FuiQueryInputField* cameraMenu;

  FFuLabelFrame*   motionFrame;
  FFuToggleButton* linkMotionToggle;
  FFuToggleButton* triadMotionToggle;

  FFuLabelFrame*   defFrame;
  FFuToggleButton* defToggle;
  FFuIOField*      defScaleField;

  FFuLabelFrame*   legendFrame;
  FFuToggleButton* fringeToggle;
  FFuToggleButton* legendToggle;

  FFuOptionMenu* legendColorsMenu;
  FFuOptionMenu* legendMappingMenu;
  FFuOptionMenu* legendLookMenu;

  FFuIOField* legendMaxField;
  FFuIOField* legendMinField;

  FFuLabelFrame*  tickMarkFrame;
  FFuRadioButton* tickMarkCountRadio;
  FFuIOField*     tickMarkCountField;
  FFuRadioButton* tickMarkSpaceRadio;
  FFuIOField*     tickMarkSpaceField;

  FFuDialogButtons* dialogButtons;
};


class FuaAnimationControlValues : public FFuaUIValues
{
public:
  // Follow-me camera
  FFuaQueryBase*     myLinkQuery    = NULL;
  FmModelMemberBase* myLinkToFollow = NULL;

  // rigid body motion
  bool linkMotion  = false;
  bool triadMotion = false;

  // deformation
  bool   def              = false;
  double defScale         = 0.0;
  bool   defScalePossible = false;

  // legend
  bool fringe = false;
  bool legend = false;

  std::vector<std::string> legendColors;   std::string selLegendColor;
  std::vector<std::string> legendMappings; std::string selLegendMapping;
  std::vector<std::string> legendLooks;    std::string selLegendLook;

  std::pair<double,double> legendRange = { 0.0,0.0 };

  // tick marks
  bool   tickMarkCount      = false;
  int    tickMarkCountVal   = 0;
  double tickMarkSpacingVal = 0.0;
};

#endif
