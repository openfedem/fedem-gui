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
#include <array>
#include <vector>

class FFuLabelFrame;
class FFuLabel;
class FFuIOField;
class FFuOptionMenu;
class FFuToggleButton;
class FFaViewItem;
class FuiQueryInputField;
class FmModelMemberBase;


class FuiPositionData : public virtual FFuMultUIComponent
{
public:
  FuiPositionData();

  void setEditedObj(FmModelMemberBase* obj);
  FmModelMemberBase* getEditedObj() { return myEditedObj; }
  void editLinkCG(bool onOff) { IAmEditingLinkCG = onOff; }

  static void cancelActiveUI();

  void updateUI();
  void setSensitivity(bool onOff);

  void setPosLabel(const char* text);
  void setRotLabel(const char* text);

  void popUpRotUI(bool onOff);
  void popUpPosUI(bool onOff);

  void finishRefCSSelection();

  FuiQueryInputField* myPosRefCSField;
  FuiQueryInputField* myRotRefCSField;

  FFuLabelFrame* myTranslationFrame;
  FFuLabelFrame* myRotationFrame;
  FFuOptionMenu* myPosViewTypeMenu;
  FFuOptionMenu* myRotViewTypeMenu;

  std::array<FFuLabel*,9>   myLabels;
  std::array<FFuIOField*,9> myFields;

  FFuLabelFrame*   myTriadPosFollowFrame;
  FFuToggleButton* myMasterFollowToggle;
  FFuToggleButton* mySlaveFollowToggle;

protected:
  void initWidgets();

  FmModelMemberBase* myEditedObj;

  virtual void onPoppedUp();
  virtual void onPoppedDown();

private:
  bool IAmEditingLinkCG;
  bool IAmEditable;
  bool ImAwareOfPosRefSelections;
  bool ImAwareOfRotRefSelections;

  void prepareRefCSSelection();

  void onPosTypeChanged(int option);
  void onRotTypeChanged(int option);
  void onFieldAccepted(double d);
  void onPosRefChanged(FmModelMemberBase* obj);
  void onRotRefChanged(FmModelMemberBase* obj);
  void onRotRefButtonPressed();
  void onPosRefButtonPressed();
  void onTriadsFollowToggled(bool val);

  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                              const std::vector<FFaViewItem*>&,
                              const std::vector<FFaViewItem*>&);

  FapPermSelChangedReceiver<FuiPositionData> signalConnector;
  friend class FapPermSelChangedReceiver<FuiPositionData>;

  static FuiPositionData* ourActiveUI;
};

#endif
