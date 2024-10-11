// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_POSITION_DATA_H
#define FUI_POSITION_DATA_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include "vpmApp/FapEventManager.H"
#include <string>
#include <vector>

class FuiQueryInputField;
class FFuOptionMenu;
class FFuIOField;
class FFuLabel;
class FFaViewItem;
class FmModelMemberBase;
class FFuLabelFrame;
class FFuToggleButton;


class FuiPositionData : public virtual FFuMultUIComponent
{
public:
  FuiPositionData();
  virtual ~FuiPositionData() {}

  void setEditedObj(FmModelMemberBase* obj);
  FmModelMemberBase* getEditedObj() { return myEditedObj; }

  static void cancelActiveUI();

  void updateUI();
  void editLinkCG(bool yep);

  void setSensitivity(bool onOff) { IAmEditable = onOff; this->updateSensitivity(); }

  virtual void setPosLabel(const std::string& text) = 0;
  virtual void setRotLabel(const std::string& text) = 0;

  virtual void popUpRotUI(bool onOff) = 0;
  virtual void popUpPosUI(bool onOff) = 0;

  void finishRefCSSelection();

  FuiQueryInputField* myPosRefCSField;
  FuiQueryInputField* myRotRefCSField;

  FFuOptionMenu* myPosViewTypeMenu;
  FFuOptionMenu* myRotViewTypeMenu;

  std::vector<FFuIOField*> myFields;
  std::vector<FFuLabel*>   myLabels;

  FFuLabelFrame*   myTriadPosFollowFrame;
  FFuToggleButton* myMasterFollowToggle;
  FFuToggleButton* mySlaveFollowToggle;

protected:
  void initWidgets();
  void setRotSensitivity(bool isSensitive);
  void setPosSensitivity(bool isSensitive);

  bool isUsing6RotParams();

  FmModelMemberBase* myEditedObj;

  void onPosTypeChanged(int option);
  void onRotTypeChanged(int option);
  void onFieldAccepted(double d);
  void onPosRefChanged(FmModelMemberBase* obj);
  void onRotRefChanged(FmModelMemberBase* obj);
  void onRotRefButtonPressed();
  void onPosRefButtonPressed();
  void onTriadsFollowToggled(bool val);
  void updateSensitivity();

  virtual void onPoppedUp();
  virtual void onPoppedDown();

private:
  bool IAmEditingLinkCG;
  bool IAmEditable;
  bool ImAwareOfPosRefSelections;
  bool ImAwareOfRotRefSelections;

  void prepareRefCSSelection();

  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                              const std::vector<FFaViewItem*>& selectedSinceLast,
                              const std::vector<FFaViewItem*>& unselectedSinceLast);

  FapPermSelChangedReceiver<FuiPositionData> signalConnector;
  friend class FapPermSelChangedReceiver<FuiPositionData>;

  static FuiPositionData* ourActiveUI;
};

#endif
