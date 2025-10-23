// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiGraphDefine.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuSpinBox.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuLabelField.H"

//----------------------------------------------------------------------------

FuiGraphDefine::FuiGraphDefine()
{
  titleField = subtitleField = xAxisField = yAxisField = NULL;

  fontSizeBox.fill(NULL);

  gridMenu = NULL;

  legendButton = autoscaleButton = timeButton = NULL;

  tminField = tmaxField = NULL;
}
//----------------------------------------------------------------------------

void FuiGraphDefine::initWidgets()
{
  titleField->setLabel("Title");
  subtitleField->setLabel("Sub-title");
  xAxisField->setLabel("X-axis label");
  yAxisField->setLabel("Y-axis label");

  titleField->myField->setAcceptedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,char*));
  subtitleField->myField->setAcceptedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,char*));
  xAxisField->myField->setAcceptedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,char*));
  yAxisField->myField->setAcceptedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,char*));

  for (FFuSpinBox* box : this->fontSizeBox) {
    box->setMinMax(5,50);
    box->setMinWidth(40);
    box->setValueChangedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,int));
  }

  this->gridMenu->setOptionSelectedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,int));
  this->legendButton->setToggleCB(FFaDynCB1M(FuiGraphDefine,this,onButtonToggled,bool));
  this->autoscaleButton->setToggleCB(FFaDynCB1M(FuiGraphDefine,this,onButtonToggled,bool));

  this->timeButton->setToggleCB(FFaDynCB1M(FuiGraphDefine,this,onButtonToggled,bool));
  this->tminField->setAcceptedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,double));
  this->tmaxField->setAcceptedCB(FFaDynCB1M(FuiGraphDefine,this,onFieldValueChanged,double));

  this->gridMenu->addOption("None");   // 0 refer FFu2DPlotter enum
  this->gridMenu->addOption("Coarse"); // 1
  this->gridMenu->addOption("Fine");   // 2

  this->legendButton->setLabel("Legend");
  this->autoscaleButton->setLabel("Autoscale");

  const char* timeTip =
    "Note: The time interval fields are used only for curves that plot results from the RDB, and\n"
    "are used to limit the amount of data read for a large result database with many time steps.\n"
    "They do NOT affect curves plotting internal functions or imported curves, unless the\n"
    "RDB-curves all have Time as their X-axis defintion. In the latter case, it is assumed that\n"
    "the imported curves also have Time as X-axis definition and are clipped accordingly.";
  this->timeButton->setLabel("Use time interval");
  this->timeButton->setToolTip(timeTip);
  this->tminField->setLabel("Start time");
  this->tminField->setToolTip(timeTip);
  this->tmaxField->setLabel("Stop time");
  this->tmaxField->setToolTip(timeTip);

  // This feature is (not yet) supported with Qwt 6.1.2, so hide this widget
  this->subtitleField->popDown();

  // create ui's UA object
  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//-----------------------------------------------------------------------------

FFuaUIValues* FuiGraphDefine::createValuesObject()
{
  return new FuaGraphDefineValues();
}
//----------------------------------------------------------------------------

void FuiGraphDefine::setUIValues(const FFuaUIValues* values)
{
  FuaGraphDefineValues* graphValues = (FuaGraphDefineValues*) values;

  this->titleField->setValue(graphValues->title);
  this->subtitleField->setValue(graphValues->subtitle);
  this->xAxisField->setValue(graphValues->xAxisLabel);
  this->yAxisField->setValue(graphValues->yAxisLabel);

  for (size_t i = 0; i < this->fontSizeBox.size() && i < graphValues->fontSize.size(); i++)
    this->fontSizeBox[i]->setIntValue(graphValues->fontSize[i]);

  this->gridMenu->selectOption(graphValues->gridType);
  this->legendButton->setValue(graphValues->showLegend);
  this->autoscaleButton->setValue(graphValues->autoScaleOnLoadCurve);

  if (graphValues->useTimeInterval < -1) {
    this->timeButton->popDown();
    this->tminField->popDown();
    this->tmaxField->popDown();
  }
  else {
    this->timeButton->popUp();
    this->tminField->popUp();
    this->tmaxField->popUp();

    this->timeButton->setValue(graphValues->useTimeInterval > 0);
    this->tminField->setValue(graphValues->tmin);
    this->tmaxField->setValue(graphValues->tmax);

    this->timeButton->setSensitivity(graphValues->useTimeInterval >= 0);
    this->tminField->setSensitivity(graphValues->useTimeInterval > 0);
    this->tmaxField->setSensitivity(graphValues->useTimeInterval > 0);
  }
}
//----------------------------------------------------------------------------

void FuiGraphDefine::getUIValues(FFuaUIValues* values)
{
  FuaGraphDefineValues* graphValues = (FuaGraphDefineValues*) values;

  graphValues->title      = titleField->getText();
  graphValues->subtitle   = subtitleField->getText();
  graphValues->xAxisLabel = xAxisField->getText();
  graphValues->yAxisLabel = yAxisField->getText();

  for (size_t i = 0; i < this->fontSizeBox.size() && i < graphValues->fontSize.size(); i++)
    graphValues->fontSize[i] = this->fontSizeBox[i]->getIntValue();

  graphValues->gridType = this->gridMenu->getSelectedOption();
  graphValues->showLegend = this->legendButton->getValue();
  graphValues->autoScaleOnLoadCurve = this->autoscaleButton->getValue();

  graphValues->useTimeInterval = this->timeButton->getValue();
  graphValues->tmin = this->tminField->getValue();
  graphValues->tmax = this->tmaxField->getValue();
}
//----------------------------------------------------------------------------
