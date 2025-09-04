// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_LABEL_H
#define FFU_LABEL_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>


class FFuLabel : virtual public FFuComponentBase
{
  using DynCB1 = FFaDynCB1<const std::string&>;

public:
  virtual void setPixMap(const char** pixmap, bool stretch = false) = 0;
  virtual void setLabel(const char* label) = 0;

  void setLabel(const std::string& label) { this->setLabel(label.c_str()); }
  virtual std::string getLabel() const = 0;

  void setLinkActivatedCB(const DynCB1& cb) { myLinkActivatedCB = cb; }

protected:
  DynCB1 myLinkActivatedCB;
};


class FFuNotes : virtual public FFuComponentBase
{
public:
  virtual void setText(const char* text) = 0;
};

#endif
