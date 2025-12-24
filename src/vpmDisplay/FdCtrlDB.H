// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 * CLASS: FdCtrlDB
 * AUTHOR: Magnus Svanberg
 * LAST MODIFIED: 6 Okt 1997
 **********************************************************************/

#ifndef FD_CTRL_DB_H
#define FD_CTRL_DB_H

class SoSeparator;
class SbVec2f;
class SbVec3f;

class FdObject;
class FdCtrlKit;
class FdQtViewer;

class FmCtrlElementBase;
class FmIsRenderedBase;

struct ctrlViewData;


namespace FdCtrlDB
{
  void init();
  void start(bool useGUI);
  void openCtrl();
  void closeCtrl();

  ctrlViewData getView();
  void setView(const ctrlViewData& cvData);

  FdCtrlKit*   getCtrlKit();
  SoSeparator* getCtrlExtraGraphicsRoot();
  FdObject*    getPickedObject();
  void         setCreatedElem(FmCtrlElementBase* createElem);
  FdQtViewer*  getCtrlViewer();

  SbVec3f getNewVec(const SbVec2f& currPos, bool ignoreSnap = false);

  void updateMode(int newMode, int oldMode);
  void updateNeutralType(int newNeutralType, int oldNeutralMode);
  void zoomTo(FmIsRenderedBase* elmOrLine);
}

#endif
