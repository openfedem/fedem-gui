// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>

#include "vpmUI/vpmUITopLevels/FuiDutyCycleOptions.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuTable.H"
#include "FFaLib/FFaString/FFaStringExt.H"

// Enum for columns in event table
enum { EVENT, DESCR, REPEATS, MASTER, NCOLS };


Fmd_SOURCE_INIT(FUI_DUTYCYCLEOPTIONS, FuiDutyCycleOptions, FFuTopLevelShell);


FuiDutyCycleOptions::FuiDutyCycleOptions()
{
  Fmd_CONSTRUCTOR_INIT(FuiDutyCycleOptions);

  this->eventTable = 0;
  this->linkTable = 0;
  this->addEventButton = 0;
  this->delEventButton = 0;
  this->equivUnitField = 0;
  this->equivUnitScaleField = 0;
  this->equivUnitScaleLabel = 0;
  this->equivUnitLabel = 0;
  this->eventFrame = 0;
  this->linkFrame = 0;
  this->checkNoneButton = 0;
  this->checkAllButton = 0;
}


void FuiDutyCycleOptions::setUIValues(const FFuaUIValues* values)
{
  FuaDutyCycleOptionsValues* v = (FuaDutyCycleOptionsValues*) values;

  this->setLinks(v->links);
  this->setEvents(v->events);
  this->equivUnitScaleField->setValue(v->equivUnit.first);
  this->equivUnitField->setValue(v->equivUnit.second);
}


void FuiDutyCycleOptions::getUIValues(FFuaUIValues* values)
{
  FuaDutyCycleOptionsValues* v = (FuaDutyCycleOptionsValues*) values;

  // links
  v->links.clear();
  int row;
  for (row = 0; row < linkTable->getNumberRows(); row++)
    v->links.push_back(std::make_pair(this->linkTable->getText(row,0),
				      this->linkTable->getCheckBoxItemToggle(row,0)));

  // events
  v->events.clear();
  for (row = 0; row < this->eventTable->getNumberRows(); row++) {
    FuiEventData data;
    data.fmmPath = this->eventTable->getText(row, EVENT);
    data.name = this->eventTable->getText(row, DESCR);
    data.repeats = atof(this->eventTable->getText(row, REPEATS).c_str());
    data.master = this->eventTable->getCheckBoxItemToggle(row, MASTER);
    v->events.push_back(data);
  }

  // eq unit
  v->equivUnit.first = this->equivUnitScaleField->getDouble();
  v->equivUnit.second = this->equivUnitField->getValue();
}


void FuiDutyCycleOptions::initWidgets()
{
  this->eventTable->setSelectionPolicy(FFuTable::SINGLE_ROW);
  this->eventTable->showTableGrid(true);
  this->eventTable->showColumnHeader(true);
  this->eventTable->showRowHeader(false);
  this->eventTable->setNumberColumns(NCOLS);
  this->eventTable->setColumnLabel(EVENT, "Event");
  this->eventTable->setColumnLabel(DESCR, "Description");
  this->eventTable->setColumnLabel(REPEATS, "Repeats");
  this->eventTable->setColumnLabel(MASTER, "Master");
  this->eventTable->setColumnSensitivity(EVENT, false); // we will handle this
  this->eventTable->setColumnSensitivity(MASTER, false); // internally
  this->eventTable->setCellClickedCB(FFaDynCB2M(FuiDutyCycleOptions, this,
 						onEventCellClicked,
						int, int));

  this->linkTable->setSelectionPolicy(FFuTable::NO_SELECTION);
  this->linkTable->showRowHeader(false);
  this->linkTable->showColumnHeader(false);
  this->linkTable->setNumberColumns(1);
  this->linkTable->setCellClickedCB(FFaDynCB2M(FuiDutyCycleOptions, this,
					       onLinkCellClicked,
					       int, int));

  this->addEventButton->setLabel("Add event");
  this->addEventButton->setActivateCB(FFaDynCB0M(FuiDutyCycleOptions,this,onAddEventButtonClicked));

  this->delEventButton->setLabel("Delete event");
  this->delEventButton->setActivateCB(FFaDynCB0M(FuiDutyCycleOptions,this,onDelEventButtonClicked));

  this->checkAllButton->setLabel("Check all");
  this->checkAllButton->setActivateCB(FFaDynCB0M(FuiDutyCycleOptions,this,onCheckAllButtonClicked));

  this->checkNoneButton->setLabel("Check none");
  this->checkNoneButton->setActivateCB(FFaDynCB0M(FuiDutyCycleOptions,this,onCheckNoneButtonClicked));

  this->equivUnitLabel->setLabel("Equivalent unit");
  this->equivUnitScaleLabel->setLabel("Equivalent unit scale");
  this->eventFrame->setLabel("Events");
  this->linkFrame->setLabel("Parts");

  this->equivUnitScaleField->setInputCheckMode(FFuIOField::DOUBLECHECK);

  this->equivUnitField->setInputCheckMode(FFuIOField::NOCHECK);

  this->FuiTopLevelDialog::initWidgets();
}


void FuiDutyCycleOptions::placeWidgets(int width,int height)
{
  int border = this->getBorder();

  int fieldHeight = this->equivUnitField->getHeightHint();
  int btnHeight = this->addEventButton->getHeightHint();
  int btnWidth = this->delEventButton->getWidthHint();
  int frameTop = border;
  int frameBottom = height - this->getDialogButtonsHeightHint() - 2*border - 2*fieldHeight;
  int eventFrameLeft = border;
  int eventFrameRight = 2*width/3 - border/2;
  int linkFrameLeft = eventFrameRight + border;
  int linkFrameRight = width - border;
  int tableTop = frameTop + 3*border/2;
  int btnBottom = frameBottom - border;
  int btnTop = btnBottom - btnHeight;
  int tableBottom = btnTop - border;

  int equivUnit1Center = frameBottom + border + fieldHeight/2;
  int equivUnit2Center = equivUnit1Center + fieldHeight + border;

  // frames
  this->eventFrame->setEdgeGeometry(eventFrameLeft, eventFrameRight, frameTop, frameBottom);
  this->linkFrame->setEdgeGeometry(linkFrameLeft, linkFrameRight, frameTop, frameBottom);

  // tables
  this->eventTable->setEdgeGeometry(eventFrameLeft + border, eventFrameRight - border,
				    tableTop, tableBottom);
  this->linkTable->setEdgeGeometry(linkFrameLeft + border, linkFrameRight - border,
				   tableTop, tableBottom);

  // "Internal" table geometry
  this->eventTable->setColumnWidth(EVENT, this->eventTable->getWidth()/2);
  this->eventTable->setColumnWidth(REPEATS, this->eventTable->getWidth()/7);
  this->eventTable->setColumnWidth(MASTER, this->eventTable->getWidth()/7);
  this->eventTable->stretchColWidth(DESCR, true);
  this->linkTable->stretchColWidth(0, true);

  // buttons
  this->addEventButton->setEdgeGeometry(eventFrameLeft + border, eventFrameLeft + border + btnWidth,
					btnTop, btnBottom);
  this->delEventButton->setEdgeGeometry(this->addEventButton->getXRightPos() + border,
					this->addEventButton->getXRightPos() + border + btnWidth,
					btnTop, btnBottom);
  this->checkAllButton->setEdgeGeometry(linkFrameLeft + border, linkFrameLeft + border + btnWidth,
					btnTop, btnBottom);
  this->checkNoneButton->setEdgeGeometry(this->checkAllButton->getXRightPos() + border,
					 this->checkAllButton->getXRightPos() + border + btnWidth,
					 btnTop, btnBottom);

  // equivalent unit
  this->equivUnitScaleLabel->setCenterYGeometrySizeHint(border, equivUnit1Center);
  this->equivUnitLabel->setCenterYGeometrySizeHint(border, equivUnit2Center);
  int fieldLeft = this->equivUnitScaleLabel->getXRightPos() + border;
  this->equivUnitScaleField->setCenterYGeometry(fieldLeft, equivUnit1Center, width/4, fieldHeight);
  this->equivUnitField->setCenterYGeometry(fieldLeft, equivUnit2Center, width/4, fieldHeight);

  this->FuiTopLevelDialog::placeWidgets(width,height);
}


FFuaUIValues* FuiDutyCycleOptions::createValuesObject()
{
  return new FuaDutyCycleOptionsValues();
}


void FuiDutyCycleOptions::onAddEventButtonClicked()
{
  this->addEventCB.invoke();
}


void FuiDutyCycleOptions::onDelEventButtonClicked()
{
  int row = this->eventTable->getCurrentRow();
  if (row < 0) return;

  std::string toBeDeleted = this->eventTable->getText(row,0);
  this->eventTable->deleteRow(row);
  myEvents.erase(toBeDeleted);
}


void FuiDutyCycleOptions::onCheckAllButtonClicked()
{
  this->toggleAllLinks(true);
}


void FuiDutyCycleOptions::onCheckNoneButtonClicked()
{
  this->toggleAllLinks(false);
}


void FuiDutyCycleOptions::toggleAllLinks(bool on)
{
  for (int row = 0; row < this->linkTable->getNumberRows(); row++)
    this->linkTable->setCheckBoxItemToggle(row, 0, on);
}


void FuiDutyCycleOptions::onLinkCellClicked(int row, int col)
{
  // HACK
  // To aid easier toggelig of links (flaw in Qt 3.0.4)
  // Is fixed in Qt 3.0.5, so this is temporary!
  this->linkTable->setCheckBoxItemToggle(row, col,
					 !this->linkTable->getCheckBoxItemToggle(row,col));
  // END HACK
}


void FuiDutyCycleOptions::onEventCellClicked(int row, int col)
{
  if (col == MASTER) // Ignoring everything not related to master col
    for (int r = 0; r < this->eventTable->getNumberRows(); r++)
      this->eventTable->setCheckBoxItemToggle(r, col, (r == row));
}


void FuiDutyCycleOptions::setAddEventCB(const FFaDynCB0& dynCB)
{
  this->addEventCB = dynCB;
}


void FuiDutyCycleOptions::setLinks(const std::vector< std::pair<std::string,bool> >& links)
{
  this->linkTable->clearTableItems();
  this->linkTable->setNumberRows(links.size());
  size_t i = 0;
  for (const std::pair<std::string,bool>& link : links)
    this->linkTable->insertCheckBoxItem(i++, 0, link.first, link.second);
}


void FuiDutyCycleOptions::setEvents(const std::vector<FuiEventData>& events)
{
  myEvents.clear();
  this->eventTable->clearTableItems();
  this->eventTable->setNumberRows(events.size());
  size_t i = 0;
  for (const FuiEventData& event : events)
    this->addEvent(event, i++);
}


void FuiDutyCycleOptions::addEvent(const FuiEventData& event, int row)
{
  if (row == -1 || row >= this->eventTable->getNumberRows()) {
    row = this->eventTable->getNumberRows();
    this->eventTable->setNumberRows(row + 1);
  }

  this->eventTable->insertText(row, EVENT, event.fmmPath);
  this->eventTable->insertText(row, DESCR, event.name);
  this->eventTable->insertText(row, REPEATS, FFaNumStr(event.repeats));
  this->eventTable->insertCheckBoxItem(row, MASTER, "", event.master);
  myEvents.insert(event.fmmPath);
}


bool FuiDutyCycleOptions::eventPresent(const std::string& file)
{
  return (myEvents.find(file) != myEvents.end());
}
