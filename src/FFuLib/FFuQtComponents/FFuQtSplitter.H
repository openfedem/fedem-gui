// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_SPLITTER_H
#define FFU_QT_SPLITTER_H

#include <QSplitter>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


class FFuQtSplitter : public QSplitter, public FFuQtComponentBase
{
public:
  FFuQtSplitter(Qt::Orientation o, QWidget* parent = NULL);
  virtual ~FFuQtSplitter() {}
};

#endif
