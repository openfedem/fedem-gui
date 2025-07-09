// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_ANIM_EXPORT_SETUP_H
#define FUI_ANIM_EXPORT_SETUP_H

#include "FFuLib/FFuBase/FFuModalDialog.H"
#include "FFuLib/FFuLogicButtonGroup.H"

class FFuSpinBox;
class FFuRadioButton;
class FFuIOField;
class FFuPushButton;
class FFuDialogButtons;


class FuiAnimExportSetup : virtual public FFuModalDialog
{
  Fmd_HEADER_INIT();

protected:
  FuiAnimExportSetup();

  static FuiAnimExportSetup* create(int xpos = 100, int ypos = 100,
				    int width = 100, int height = 100,
				    const char* title = "Animation Export",
				    const char* name = "FuiAnimExportSetup");
public:
  static FuiAnimExportSetup* getUI(bool onScreen = true);

  void setDialogButtonClickedCB(const FFaDynCB1<int>& cb) { myClickedCB = cb; }

  void getSetupValues(bool& allFrames, bool& realTime,
		      bool& omitSome, bool& onlySome,
		      int& nthToOmit, int& nthToInclude);
  void getFileValues(std::string& fileName, int& format);

  void setDefaultFileName(const std::string& fileName);

protected:
  void initWidgets();

private:
  void onToggleChanged(int,bool);
  void onBrowseButtonClicked();

  virtual void onDialogButtonClicked(int);

protected:
  FFuLogicButtonGroup myButtonGroup;
  FFuRadioButton* myAllBtn;  // All frames
  FFuRadioButton* myRealBtn; // Real time
  FFuRadioButton* myOmitBtn; // Omit every nth
  FFuRadioButton* mySomeBtn; // Export only every nth
  FFuSpinBox* omitNthSpinBox;
  FFuSpinBox* everyNthSpinBox;
  FFuIOField* myFileField;
  FFuPushButton* myBrowseButton;
  FFuDialogButtons* dialogButtons;

private:
  int            selectedFilter;
  FFaDynCB1<int> myClickedCB;
};

#endif
