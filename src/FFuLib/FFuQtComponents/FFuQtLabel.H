// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_LABEL_H
#define FFU_QT_LABEL_H

#include <QLabel>

#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


class FFuQtLabel : public QLabel, public FFuLabel,
		   public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQtLabel(QWidget* parent = NULL);

  virtual void setPixMap(const char** pixmap, bool stretch);
  virtual void setLabel(const char* label);

  virtual std::string getLabel() const;

public slots:
  void fwdLinkActivated(const QString&);
};


class FFuQtNotesLabel : public QWidget, public FFuQtComponentBase
{
public:
  FFuQtNotesLabel(QWidget* parent = NULL);
};

#endif
