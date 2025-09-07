// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_ANIMATION_DEFINE_H
#define FUI_QT_ANIMATION_DEFINE_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiAnimationDefine.H"


class FuiQtAnimationDefine : public FFuQtWidget, public FuiAnimationDefine
{
public:
  FuiQtAnimationDefine(QWidget* parent,
                       const char* name = "FuiQtAnimationDefine");
};


class FuiQtFringeSheet : public FFuQtWidget, public FuiFringeSheet
{
public:
  FuiQtFringeSheet(QWidget* parent, const char* name = "FuiQtFringeSheet");
};


class FuiQtTimeSheet : public FFuQtWidget,  public FuiTimeSheet
{
public:
  FuiQtTimeSheet(QWidget* parent, const char* name = "FuiQtTimeSheet");
};


class FuiQtModesSheet : public FFuQtWidget, public FuiModesSheet
{
public:
  FuiQtModesSheet(QWidget* parent, const char* name = "FuiQtModesSheet");
};

#endif
