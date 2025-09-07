// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiCtrlElemProperties.H"
#include "vpmUI/vpmUIComponents/FuiParameterView.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"


void FuiCtrlElemProperties::initWidgets()
{
  myElemFrame->setLabel("Element definition");
  myParameterFrame->setLabel("Parameters");

  myParameterView = new FuiParameterView();
  myParameterView->setAcceptedCB(FFaDynCB1M(FuiCtrlElemProperties,this,
                                            onValueChanged,double));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiCtrlElemProperties::setSensitivity(bool isSensitive)
{
  myParameterView->setSensitivity(isSensitive);
}


void FuiCtrlElemProperties::setUIValues(const FFuaUIValues* values)
{
  const FuaCtrlElemPropertiesValues* data = dynamic_cast<const FuaCtrlElemPropertiesValues*>(values);
  if (!data) return;

  std::vector<double> params;
  params.reserve(data->parameters.size());
  for (const std::pair<std::string,double>& prm : data->parameters)
    params.push_back(prm.second);

  myParameterView->setValues(data->ctrlTypeIdx,params);
  myElemPixmap->setPixMap(data->pixmap);
}


void FuiCtrlElemProperties::buildDynamicWidgets(const FFuaUIValues* values)
{
  const FuaCtrlElemPropertiesValues* data = dynamic_cast<const FuaCtrlElemPropertiesValues*>(values);
  if (!data) return;

  if (data->pixmap)
    myElemFrame->popUp();
  else
    myElemFrame->popDown();

  if (data->parameters.empty())
    myParameterFrame->popDown();
  else
  {
    myParameterFrame->popUp();

    std::vector<std::string> fields;
    fields.reserve(data->parameters.size());
    for (const std::pair<std::string,double>& param : data->parameters)
      fields.push_back(param.first);

    myParameterView->setFields(data->ctrlTypeIdx,fields,myParameterFrame);
  }
}


void FuiCtrlElemProperties::onValueChanged(double)
{
  FuaCtrlElemPropertiesValues values;
  std::vector<double> params;
  values.ctrlTypeIdx = myParameterView->getValues(params);
  values.parameters.reserve(params.size());
  for (double prm : params)
    values.parameters.push_back({ "", prm });

  this->invokeSetAndGetDBValuesCB(&values);
  this->setUIValues(&values);
}
