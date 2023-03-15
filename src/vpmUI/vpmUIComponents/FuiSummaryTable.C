// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "FuiSummaryTable.H"
#include "FFuLib/FFuTable.H"
#include "FFuLib/FFuLabel.H"


void FuiSummaryTable::initWidgets()
{
  mySummaryTable->setTableReadOnly(true);
  mySummaryTable->setSelectionPolicy(FFuTable::NO_SELECTION);

/* Does not work with Qt4 due to unexpected recursion
  mySummaryTable->setColumnResizedCB(FFaDynCB3M(FuiSummaryTable,this,
						columnSizeChanged,int,int,int));
*/
  mySummaryTable->setCellDoubleClicked(FFaDynCB3M(FuiSummaryTable,this,
						  tableDoubleClicked,int,int,int));

  myAddBCLabel->setLabel(" *) Fixed in initial equilibrium analysis");
  myAddBCLabel->setToolTip("Also fixed in eigenmode analysis\n"
                           "if activated in the \"Dynamics Solver\" dialog");
}


void FuiSummaryTable::updateColumnWidths()
{
  size_t i;
  double totalRelWidth = 0;
  for (i = 0; i < myRelColWidths.size(); i++)
    totalRelWidth += myRelColWidths[i];

  int totColumnWidth = mySummaryTable->getWidth() - mySummaryTable->getRowHeaderWidth() - 4;
  double columnUnitWidth = totColumnWidth/totalRelWidth;

  double leftovers = 0; double dummy;
  for (i = 0; i < myRelColWidths.size(); i++)
    leftovers += modf(columnUnitWidth*myRelColWidths[i], &dummy);

  for (i = 0; i < myRelColWidths.size()-1; i++)
    {
      double colW = columnUnitWidth*myRelColWidths[i];
      if (modf(colW,&dummy) >= 0.5 && leftovers > 0)
        {
          mySummaryTable->setColumnWidth(i, (int)(colW + 0.5));
          --leftovers;
        }
      else
        mySummaryTable->setColumnWidth(i, (int)(colW));
    }
  mySummaryTable->setColumnWidth(i, (int)((columnUnitWidth*myRelColWidths[i]) + leftovers));
}


void FuiSummaryTable::columnSizeChanged(int col, int oldSize, int newSize)
{
  int columnWidthsBeforeChangedOne = 0;
  for (int j = 0; j < col; j++)
    columnWidthsBeforeChangedOne += mySummaryTable->getColumnWidth(j);

  int totColumnWidth = this->getWidth() - mySummaryTable->getRowHeaderWidth() - 4;
  if (newSize + columnWidthsBeforeChangedOne >= totColumnWidth - 5)
    newSize = oldSize;

  double newRelColWidth = newSize * myRelColWidths[col]/oldSize;
  double difference = newRelColWidth - myRelColWidths[col];
  myRelColWidths[col] = newRelColWidth;

  size_t i;
  double totRelColWidthRest = 0;
  for (i = col+1; i < myRelColWidths.size(); i++)
    totRelColWidthRest += myRelColWidths[i];

  for (i = col+1; i < myRelColWidths.size(); i++)
    myRelColWidths[i] -= myRelColWidths[i]*difference/totRelColWidthRest;

  this->updateColumnWidths();
}


void FuiSummaryTable::tableDoubleClicked(int, int, int)
{
  this->updateColumnWidths();
}
