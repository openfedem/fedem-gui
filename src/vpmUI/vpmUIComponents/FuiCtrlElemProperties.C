// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiCtrlElemProperties.H"
#include "vpmUI/vpmUIComponents/FuiParameterView.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelField.H"
#include "FFuLib/FFuLabelFrame.H"


void FuiCtrlElemProperties::initWidgets()
{
  myParameterFrame->setLabel("Parameters");

  myParameterView = new FuiParameterView();
  myParameterView->setAcceptedCB(FFaDynCB1M(FuiCtrlElemProperties,this,
                                            onValueChanged,double));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


void FuiCtrlElemProperties::placeWidgets(int width, int height)
{
  int glh1 = getGridLinePos(height, 3, FROM_START);
  int glh2 = myParameterFrame->getFontHeigth();
  int glh3 = getGridLinePos(height, 3, FROM_END);

  int glv1 = getGridLinePos(width, 3, FROM_START);
  int glv2 = getGridLinePos(width, 300);
  int glv3 = glv2 + 3;

  myElemPixmap->setEdgeGeometry(glv1,glv2,glh1,glh3);
  myParameterFrame->setEdgeGeometry(glv3,width,0,height);
  myParameterView->setEdgeGeometry(glv3+5,width-5,glh2,glh3);
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

  if (data->parameters.empty())
  {
    myParameterFrame->popDown();
    myParameterView->popDown();
  }
  else
  {
    myParameterFrame->popUp();

    std::vector<std::string> fields;
    fields.reserve(data->parameters.size());
    for (const std::pair<std::string,double>& param : data->parameters)
      fields.push_back(param.first);
    myParameterView->setFields(data->ctrlTypeIdx,fields,this);
  }

  this->placeWidgets(this->getWidth(), this->getHeight());
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
