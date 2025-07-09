// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_EIGEN_OPTIONS_H
#define FUI_EIGEN_OPTIONS_H

#include <vector>
#include <string>

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"

class FFuOptionMenu;
class FFuPushButton;
class FFuListView;
class FFuListViewItem;
class FFuToggleButton;
class FFuFileBrowseField;


struct FuiModeData
{
  bool operator< (const FuiModeData& m) const;
  bool operator==(const FuiModeData& m) const;

  int    mode = 0;
  double time = 0.0;
  int  listId = -1; // id for this objects list view item
};


class FuaEigenOptionsValues : public FFuaUIValues
{
public:
  bool eraseModeData(int idToErase);

  // db -> ui
  std::vector<int>    modes;
  std::vector<double> times;

  // db <-> ui
  std::vector<FuiModeData> selectedModes;

  char autoVTFSwitch = 0;
  int  autoVTFFileType = 0;
  std::string autoVTFFileName;
  std::string modelFilePath;

  bool isSensitive = true;
};


class FuiEigenOptions : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiEigenOptions();

  // Static create method to use instead of constructor
  // Implementation in GUILib-dependent implementation file
  static FuiEigenOptions* create(int xpos = 100, int ypos  = 100,
				 int width= 100, int height= 100,
				 const char* title= "Mode Shape Recovery Setup",
				 const char* name = "FuiEigenOptions");

protected:
  virtual void initWidgets();

private:
  void setUISelectedModes();

  virtual void setUIValues(const FFuaUIValues* values);
  virtual bool updateDBValues(bool);

  void addModeAtTime(int mode, double time);
  void addModeAtAllTimes(int mode);
  void addAllModesAtTime(double time);
  void addAllModes();

  void onAddButtonClicked();
  void onDeleteButtonClicked();
  void onTmpSelectionChanged(FFuListViewItem*);
  void onPermSelectionChanged();

  void setVTFLabel(int);
  void onAutoVTFToggled(bool);
  void onAutoVTFFileChanged(const std::string&, int);

protected:
  FFuOptionMenu* modeMenu;
  FFuOptionMenu* timeMenu;

  FFuPushButton* addButton;
  FFuPushButton* deleteButton;
  FFuListView*   selectedModesView;

  FFuToggleButton*    autoVTFToggle;
  FFuFileBrowseField* autoVTFField;

private:
  FuaEigenOptionsValues myValues;
};

#endif
