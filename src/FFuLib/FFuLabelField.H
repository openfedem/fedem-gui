// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_LABEL_FIELD_H
#define FFU_LABEL_FIELD_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"

class FFuLabel;
class FFuIOField;


class FFuLabelField : virtual public FFuComponentBase
{
public:
  void setLabel(const char* label);
  void setValue(const std::string& value);
  void setValue(double value);

  std::string getText() const;
  double      getValue() const;

  // Field Accept :

  void setAcceptedCB(const FFaDynCB1<double>& aDynCB);

  // Label geometry :

  int  getLabelWidth(bool useHint = false);
  void setLabelWidth(int width);

  // To prevent editing :

  virtual void setSensitivity(bool makeSensitive);
  virtual bool getSensitivity();

  // Overriding parent toolTip
  virtual void setToolTip(const char* tip);

  // Public members to ease access. Do Not Delete ! ;-)
  FFuLabel*   myLabel;
  FFuIOField* myField;
};

#endif
