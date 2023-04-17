// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_PARAMETER_VIEW_H
#define FUI_QT_PARAMETER_VIEW_H

#include "FFuLib/FFuQtBaseClasses/FFuQtMultUIComponent.H"
#include "vpmUI/vpmUIComponents/FuiParameterView.H"

class QGridLayout;


class FuiQtParameterView : public virtual FFuQtMultUIComponent,
                           public FuiParameterView
{
public:
  FuiQtParameterView(QWidget* parent, const char* name = "FuiQtParameterView");

  virtual void clear();
  virtual void setFields(const std::vector<std::string>& names);
  virtual void setValues(const std::vector<double>& values);
  virtual void getValues(std::vector<double>& values) const;
  virtual void setSensitivity(bool isSensitive);

private:
  bool IAmSensitive;

  QGridLayout* myLayout;
};

#endif
