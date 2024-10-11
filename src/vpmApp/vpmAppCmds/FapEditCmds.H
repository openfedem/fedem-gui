// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_EDIT_CMDS_H
#define FAP_EDIT_CMDS_H

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"

class FFaViewItem;
class FmModelMemberBase;


class FapEditCmds : public FapCmdsBase
{
private:
  static void init();

  static void erase();
  static void getEraseSensitivity(bool& isSensitive);
  static void selectTempSelection();
  static void selectTempSelectionAll();

  static void copyObject();
  static void getCopyObjectSensitivity(bool& isSensitive);

  static void copyLink();
  static void getCopyLinkSensitivity(bool& isSensitive);

  static void copySubassembly();
  static void getCopySubassemblySensitivity(bool& isSensitive);

  static void mergeParts();
  static void getMergePartsSensitivity(bool& isSensitive);

  static void splitBeam(int nseg);
  static void getSplitBeamSensitivity(bool& isSensitive);
  static void onSplitBeamDone(int button);

  static void reorientCam();
  static void getReorientCamSensitivity(bool& isSensitive);

  static bool checkUsedByCurves(FFaViewItem* obj);
  static bool isEraseAble(FFaViewItem* obj);

public:
  static bool isBeingErased(FmModelMemberBase* item) { return item == objBeingErased; }

private:
  static FmModelMemberBase* objBeingErased;
  friend class FFaInitialisation<FapEditCmds>;
};

static FFaInitialisation<FapEditCmds> FapEditCmds_Init;

#endif
