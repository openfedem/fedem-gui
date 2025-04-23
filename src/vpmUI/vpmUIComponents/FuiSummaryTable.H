// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_SUMMARY_TABLE_H
#define FUI_SUMMARY_TABLE_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"

class FFuTable;
class FFuLabel;


class FuiSummaryTable : public virtual FFuMultUIComponent
{
public:
  FuiSummaryTable() { mySummaryTable = NULL; myAddBCLabel = NULL; }
  virtual ~FuiSummaryTable() {}

  int getTableHeight() const;

protected:
  static FFuTable* createTable(FFuComponentBase* parent);

  virtual void initWidgets();

public:
  FFuTable* mySummaryTable;
  FFuLabel* myAddBCLabel;
};

#endif
