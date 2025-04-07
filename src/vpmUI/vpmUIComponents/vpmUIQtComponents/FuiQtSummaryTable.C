// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHeaderView>

#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "vpmUI/vpmUIComponents/FuiSummaryTable.H"


FFuTable* FuiSummaryTable::createTable(FFuComponentBase* parent)
{
  FFuQtTable* table = new FFuQtTable(dynamic_cast<QWidget*>(parent),false,true);
  table->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  table->setLineWidth(2);

  int minSize = table->verticalHeader()->minimumSectionSize();
  table->verticalHeader()->setDefaultSectionSize(minSize);
  table->verticalHeader()->setResizeMode(QHeaderView::Fixed);

  // Somehow this is neeeded to set the proper font size
  QFont font = table->font();
  font.setPointSize(font.pointSize());
  table->setFont(font);

  return table;
}
