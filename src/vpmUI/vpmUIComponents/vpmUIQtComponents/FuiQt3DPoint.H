// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_3D_POINT_H
#define FUI_QT_3D_POINT_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/Fui3DPoint.H"


class FuiQt3DPoint : public FFuQtWidget, public Fui3DPoint
{
public:
  FuiQt3DPoint(QWidget* parent = NULL, const char* name = "D3Point",
               int refMenu = 1, bool nodeField = false);

  virtual void setSensitivity(bool sensitive)
  {
    this->Fui3DPoint::setSensitivity(sensitive);
  }

  virtual void showNodeField(int ID);

private:
  QWidget* myNodeField;
};

#endif
