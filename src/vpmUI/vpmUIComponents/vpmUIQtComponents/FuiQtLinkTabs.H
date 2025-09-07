// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_LINK_TABS_H
#define FUI_QT_LINK_TABS_H

#include "vpmUI/vpmUIComponents/FuiLinkTabs.H"
#include "FFuLib/FFuQtComponents/FFuQtFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"

class FuiQtPositionData;
class QLabel;


class FuiQtReductionFrame : public FFuQtFrame, public FuiReductionFrame
{
public:
  FuiQtReductionFrame();
  void setReducedVersion(int redVer);

private:
  QLabel* myLabel;
};


class FuiQtLinkModelSheet : public FFuQtWidget, public FuiLinkModelSheet
{
public:
  FuiQtLinkModelSheet(QWidget* parent, const char* name = "FuiQtLinkModel");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiLinkModelSheet::setSensitivity(isSensitive);
  }
};


using FuiQtLinkOriginSheet = FuiQtPositionData; //!< Convenience alias


class FuiQtLinkNodeSheet : public FFuQtWidget, public FuiLinkNodeSheet
{
public:
  FuiQtLinkNodeSheet(QWidget* parent, const char* name = "FuiQtLinkNode");
};


class FuiQtLinkRedOptSheet : public FFuQtWidget, public FuiLinkRedOptSheet
{
public:
  FuiQtLinkRedOptSheet(QWidget* parent, const char* name = "FuiQtLinkRedOpt");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiLinkRedOptSheet::setSensitivity(isSensitive);
  }
};


class FuiQtLinkLoadSheet : public FFuQtTable, public FuiLinkLoadSheet
{
public:
  FuiQtLinkLoadSheet(QWidget* parent, const char* name = "FuiQtLinkLoad");

  virtual void buildDynamicWidgets(const FuiLinkValues& values);

protected:
  virtual void setNoLoadCases(unsigned int nlc);
  virtual void resizeEvent(QResizeEvent* e);

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiLinkLoadSheet::setSensitivity(isSensitive);
  }
};


class FuiQtGenericPartCGSheet : public FFuQtWidget, public FuiGenericPartCGSheet
{
public:
  FuiQtGenericPartCGSheet(QWidget* parent, const char* name = "FuiQtGenPartCG");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiGenericPartCGSheet::setSensitivity(isSensitive);
  }
};


class FuiQtGenericPartMassSheet : public FFuQtWidget,
                                  public FuiGenericPartMassSheet
{
public:
  FuiQtGenericPartMassSheet(QWidget* parent,
                            const char* name = "FuiQtGenPartMass");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiGenericPartMassSheet::setSensitivity(isSensitive);
  }
};


class FuiQtGenericPartStiffSheet : public FFuQtWidget,
                                   public FuiGenericPartStiffSheet
{
public:
  FuiQtGenericPartStiffSheet(QWidget* parent,
                             const char* name = "FuiQtGenPartStiff");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiGenericPartStiffSheet::setSensitivity(isSensitive);
  }
};


class FuiQtHydrodynamicsSheet : public FFuQtWidget, public FuiHydrodynamicsSheet
{
public:
  FuiQtHydrodynamicsSheet(QWidget* parent,
                          const char* name = "FuiQtHydrodynamics");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiHydrodynamicsSheet::setSensitivity(isSensitive);
  }
};


class FuiQtMeshingSheet : public FFuQtWidget, public FuiMeshingSheet
{
public:
  FuiQtMeshingSheet(QWidget* parent, const char* name = "FuiQtMeshing");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiMeshingSheet::setSensitivity(isSensitive);
  }
};


class FuiQtAdvancedLinkOptsSheet : public FFuQtWidget,
                                   public FuiAdvancedLinkOptsSheet
{
public:
  FuiQtAdvancedLinkOptsSheet(QWidget* parent,
                             const char* name = "FuiQtAdvancedLinkOpts");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiAdvancedLinkOptsSheet::setSensitivity(isSensitive);
  }
};


class FuiQtNonlinearLinkOptsSheet : public FFuQtWidget,
                                    public FuiNonlinearLinkOptsSheet
{
public:
  FuiQtNonlinearLinkOptsSheet(QWidget* parent,
                              const char* name = "FuiQtNonlinearLinkOpts");

private:
  virtual void setSensitivity(bool isSensitive)
  {
    this->FuiNonlinearLinkOptsSheet::setSensitivity(isSensitive);
  }
};

#endif
