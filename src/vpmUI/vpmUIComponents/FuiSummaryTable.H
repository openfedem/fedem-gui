// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SUMMARY_TABLE_H
#define FUI_SUMMARY_TABLE_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
#include <vector>

class FFuTable;
class FFuLabel;


class FuiSummaryTable : public virtual FFuMultUIComponent
{
public:
  FuiSummaryTable() { mySummaryTable = NULL; myAddBCLabel = NULL; }
  virtual ~FuiSummaryTable() {}

protected:
  virtual void initWidgets();
  void updateColumnWidths();

private:
  void columnSizeChanged(int col, int oldSize, int newSize);
  void tableDoubleClicked(int,int) { this->updateColumnWidths(); }

public:
  FFuTable* mySummaryTable;
  FFuLabel* myAddBCLabel;

protected:
  std::vector<double> myRelColWidths;
};

#endif
