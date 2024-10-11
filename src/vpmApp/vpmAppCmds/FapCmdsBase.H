// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_CMDS_BASE_H
#define FAP_CMDS_BASE_H

#include <map>
#include <vector>

#include "vpmDB/FmModelMemberBase.H"

class FuiModeller;
class FuiCtrlModeller;
class FuiGraphView;
class FFaViewItem;
class FmPart;
class FmElementGroupProxy;

typedef std::map< FmPart*, std::vector<FmElementGroupProxy*> > FapPartGroupsMap;


class FapCmdsBase
{
protected:
  // views
  static FuiModeller* getActiveModeller();
  static FuiCtrlModeller* getActiveCtrlModeller();
  static FuiGraphView* getActiveGraphView();

  // for setGetSensitivityCB
  static void alwaysSensitive(bool& sens) { sens = true; }
  static void isProEdition(bool& isPro);
  static void isModellerActive(bool& active);
  static void isCtrlModellerActive(bool& active);
  static void isGraphViewActive(bool& active);
  static void isModellerOrGraphViewActive(bool& active);

  static void isCtrlModellerEditable(bool& isEditable);
  static void isModellerEditable(bool& isEditable);
  static void isModellerTouchable(bool& isTouchable);
  static void isModelEditable(bool& isEditable);
  static void isModelTouchable(bool& isTouchable);

  static bool hasResultsCheck();

  // objects
  static FmPart* getTheSelectedPart(); // null if multi-selection
  static bool getSelectedParts(std::vector<FmPart*>& data);
  static bool getSelectedPartsWithGroups(FapPartGroupsMap& data);

  static FFaViewItem* findSelectedSingleObject(); // null if multi-selection
  static bool getCurrentSelection(std::vector<FFaViewItem*>& data);

public:
  static bool getCurrentSelection(std::vector<FmModelMemberBase*>& data,
                                  bool noRingStart = false);

  template <class T> static bool getSelected(std::vector<T*>& data)
  {
    data.clear();
    std::vector<FmModelMemberBase*> allSelected;
    if (!getCurrentSelection(allSelected))
      return false;

    data.reserve(allSelected.size());
    for (FmModelMemberBase* obj : allSelected)
      if (obj->isOfType(T::getClassTypeID()))
        data.push_back(static_cast<T*>(obj));

    return !data.empty();
  }
};

#endif
