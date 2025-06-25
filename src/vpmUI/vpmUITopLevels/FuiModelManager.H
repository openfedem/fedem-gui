// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MODEL_MANAGER_H
#define FUI_MODEL_MANAGER_H

#include "FFuLib/FFuBase/FFuComponentBase.H"

class FuiSimModelListView;
class FuiResultListView;


class FuiModelManager : virtual public FFuComponentBase
{
protected:
  FuiSimModelListView* modelManager = NULL;
  FuiResultListView* resultManager = NULL;

  friend class FuiMainWindow;
};

#endif
