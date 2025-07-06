// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_WORK_SPACE_H
#define FUI_QT_WORK_SPACE_H

#include <QMdiArea>
#include "vpmUI/vpmUIComponents/FuiWorkSpace.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


class FuiQtWorkSpace : public QMdiArea,
                       public FuiWorkSpace, public FFuQtComponentBase
{
  Q_OBJECT

public:
  FuiQtWorkSpace(QWidget* parent, const char* name="FuiQtWorkSpace");

  virtual void cascadeWins();
  virtual void tileWins();
  virtual void tabWins();
  virtual void subWins();

protected:
  virtual FFuMDIWindow* getActiveWindow();

private slots:
  void fwdWindowActivated(QMdiSubWindow*) { this->sendWindowActivated(); }
};

#endif
