// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_APPERANCE_H
#define FUI_APPERANCE_H

#include <string>
#include <vector>

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include "FFuLib/FFuColor.H"

class FFuLabelFrame;
class FFuOptionMenu;
class FFuColorSelector;
class FFuScale;
class FFuDialogButtons;
class FuaAppearanceValues;


class FuiAppearance : virtual public FFuTopLevelShell,
		      public FFuUAExistenceHandler,
		      public FFuUADataHandler, public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiAppearance();

  // Static create method to use instead of constructor :
  // Implementation in GUILib Dependenet implementation file

  static FuiAppearance* create( int xpos=100, int ypos=100,
				int width=100, int height=100,
				const char *title= "Item Appearance",
				const char *name = "FuiAppearance");

  virtual void setUIValues(const FFuaUIValues* values);
  bool getValues(FuaAppearanceValues* values) const;
  virtual bool updateDBValues(bool = true);

  virtual bool onClose();

  // callbacks
  void onDialogButtonClicked(int button);
  void onTranspSliderChanged(int)    { isTransparencyChanged = true; this->updateDBValues(); isTransparencyChanged = false; }
  void onShininessSliderChanged(int) { isShininessChanged    = true; this->updateDBValues(); isShininessChanged    = false; }
  void onPolygonOptionChanged(int)   { isPolygonsChanged     = true; this->updateDBValues(); isPolygonsChanged     = false; }
  void onLinesOptionChanged(int)     { isLinesChanged        = true; this->updateDBValues(); isLinesChanged        = false; }
  void colorValueChanged(const FFuColor&) { isColorChanged   = true; this->updateDBValues(); isColorChanged        = false; }

  // detail changes
  void disableAdvancedOptions(bool trueOrFalse);
  void setPolygonLODText(const std::vector<std::string>& options);
  void setLinesLODText(const std::vector<std::string>& options);

protected:
  void initWidgets(void);

  FFuLabelFrame* myDetailsFrame;
  FFuLabelFrame* myColorFrame;
  FFuLabelFrame* myMaterialFrame;

  FFuOptionMenu*    myPolygonOptions;
  FFuOptionMenu*    myLinesOptions;
  FFuColorSelector* myColorSelector;
  FFuScale*         myShininessScale;
  FFuScale*         myTransparencyScale;

  FFuDialogButtons* myDialogButtons;

private:
  bool isPolygonsChanged;
  bool isLinesChanged;
  bool isColorChanged;
  bool isShininessChanged;
  bool isTransparencyChanged;
};


class FuaAppearanceValues : public FFuaUIValues
{
public:
  bool isPolygonsChanged = false;
  int  myPolygonsValue = 0;

  bool isLinesChanged = false;
  int  myLinesValue = 0;

  bool isColorChanged = false;
  FFuColor myColor;

  bool isShininessChanged = false;
  int  myShininess = 0;

  bool isTransparencyChanged = false;
  int  myTransparency = 0;
};

#endif
