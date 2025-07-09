// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_MODEL_MANAGER_H
#define FUI_QT_MODEL_MANAGER_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUITopLevels/FuiModelManager.H"


class FuiQtModelManager : public FFuQtWidget, public FuiModelManager
{
public:
  FuiQtModelManager(QWidget* parent);
};

#endif
