// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "FFuLib/FFuQtComponents/FFuQtSplitter.H"


FFuQtSplitter::FFuQtSplitter(Qt::Orientation o, QWidget* parent) : QSplitter(o, parent)
{
  this->setWidget(this);
}
