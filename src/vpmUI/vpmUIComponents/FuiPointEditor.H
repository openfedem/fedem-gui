// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_POINT_EDITOR_H
#define FUI_POINT_EDITOR_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class Fui3DPoint;
class FFuLabelField;
class FFuPushButton;
class FaVec3;


class FuiPointEditor : public virtual FFuComponentBase
{
public:
  void setViewWhatCB (const FFaDynCB1<bool>& cb) { myViewWhatCB  = cb; }
  void setViewPointCB(const FFaDynCB1<bool>& cb) { myViewPointCB = cb; }

  void setPickCB        (const FFaDynCB0& aDynCB);
  void setPointChangedCB(const FFaDynCB2<const FaVec3&,bool>& aDynCB);
  void setRefChangedCB  (const FFaDynCB1<bool>& aDynCB);

  // Setting values:

  void setOnWhatText(const std::string& text);
  void setValue(const FaVec3& point);
  void setGlobal(bool isGlobal);

  // Getting values:

  FaVec3 getValue() const;
  bool   isGlobal() const;

  // Look

  void hideOnWhatDisplay(bool doHide);
  void setReadOnly(bool readOnly) { IAmEditable = !readOnly; }

  virtual void setSensitivity(bool isSensitive);

protected:
  void initWidgets();

  // Shadow Pt's from Subclass,
  // Subclass is responsible to set them up:

  FFuLabelField* myIdField;
  FFuPushButton* myViewWhatButton;
  Fui3DPoint*    myPointUI;
  FFuPushButton* myPickButton;
  FFuPushButton* myViewPointButton;

private:
  // Internal methods for rerouting of Callbacks
  void onMyViewWhatTouched();
  void onMyViewPointTouched();

  // Callback objects

  FFaDynCB1<bool> myViewWhatCB;  // Highlight when True
  FFaDynCB1<bool> myViewPointCB; // Highlight when True

  bool IAmEditable = true;
};

#endif
