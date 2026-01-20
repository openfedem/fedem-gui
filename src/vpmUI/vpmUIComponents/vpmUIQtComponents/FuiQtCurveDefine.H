// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_CURVE_DEFINE_H
#define FUI_QT_CURVE_DEFINE_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiCurveDefine.H"


class FuiQtCurveDefine : public FFuQtWidget, public FuiCurveDefine
{
public:
  FuiQtCurveDefine(QWidget* parent, const char* name = "FuiQtCurveDefine");

  virtual void setVisible(bool visible);
  
private:
  virtual void setSensitivity(bool) {}
};
/////////////////////////////////////////////////////////////////////////////

class FuiQtCurveDefSheet : public FFuQtWidget, public FuiCurveDefSheet
{
public:
  FuiQtCurveDefSheet(const char* name = "FuiQtCurveDefSheet");

  virtual void setVisible(bool visible);

protected:
  virtual void setNoComps(unsigned int nc);

private:
  virtual void setSensitivity(bool) {}
};
/////////////////////////////////////////////////////////////////////////////

class FuiQtCurveAnalysisSheet : public FFuQtWidget, public FuiCurveAnalysisSheet
{
public:
  FuiQtCurveAnalysisSheet(const char* name = "FuiQtCurveAnaylsisSheet");

private:
  virtual void setSensitivity(bool) {}
};
/////////////////////////////////////////////////////////////////////////////

class FuiQtCurveAppearanceSheet : public FFuQtWidget,
                                  public FuiCurveAppearanceSheet
{
public:
  FuiQtCurveAppearanceSheet(const char* name = "FuiQtCurveAppearance");

private:
  virtual void setSensitivity(bool) {}
};
/////////////////////////////////////////////////////////////////////////////

class FuiQtCurveInfoSheet : public FFuQtWidget, public FuiCurveInfoSheet
{
public:
  FuiQtCurveInfoSheet(const char* name = "FuiQtCurveInfo");

private:
  virtual void setSensitivity(bool) {}
};
/////////////////////////////////////////////////////////////////////////////

class FuiQtCurveScaleSheet : public FFuQtWidget, public FuiCurveScaleSheet
{
public:
  FuiQtCurveScaleSheet(const char* name = "FuiQtcurveScaleSheet");

private:
  virtual void setSensitivity(bool) {}
};
/////////////////////////////////////////////////////////////////////////////

class FuiQtCurveFatigueSheet : public FFuQtWidget, public FuiCurveFatigueSheet
{
public:
  FuiQtCurveFatigueSheet(const char* name = "FuiQtFatigueInfo");

private:
  virtual void setSensitivity(bool) {}
};

#endif
