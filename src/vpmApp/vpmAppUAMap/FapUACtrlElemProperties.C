// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUACtrlElemProperties.H"
#include "vpmApp/FapLicenseManager.H"
#include "vpmUI/vpmUIComponents/FuiCtrlElemProperties.H"
#include "vpmDB/FmCtrlLine.H"
#include "vpmDB/FmcInput.H"
#include "vpmDB/FmcOutput.H"
#include "vpmPM/FpPM.H"


Fmd_SOURCE_INIT(FAPUACTRLELEMPROPERTIES, FapUACtrlElemProperties, FapUAExistenceHandler);


FapUACtrlElemProperties::FapUACtrlElemProperties(FuiCtrlElemProperties* ctrlProp)
  : FapUAExistenceHandler(ctrlProp), FapUADataHandler(ctrlProp),
    signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUACtrlElemProperties);

  mySelectedFmItem = NULL;
  myUI = ctrlProp;
}


FFuaUIValues* FapUACtrlElemProperties::createValuesObject()
{
  return new FuaCtrlElemPropertiesValues();
}


void FapUACtrlElemProperties::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
                                                     const std::vector<FFaViewItem*>&,
                                                     const std::vector<FFaViewItem*>&)
{
  if (totalSelection.empty())
    mySelectedFmItem = NULL;
  else
    mySelectedFmItem = dynamic_cast<FmModelMemberBase*>(totalSelection.back());

  if (!mySelectedFmItem) return;

  if (mySelectedFmItem->isOfType(FmcInput::getClassTypeID()) ||
      mySelectedFmItem->isOfType(FmcOutput::getClassTypeID()))
    return; // these are now handled by FapUAFunctionProperties

  if (!mySelectedFmItem->isOfType(FmCtrlLine::getClassTypeID()) &&
      !mySelectedFmItem->isOfType(FmCtrlElementBase::getClassTypeID()))
    return; // not a control element

  FuaCtrlElemPropertiesValues values;
  this->getDBValues(&values);
  myUI->buildDynamicWidgets(&values);
  myUI->setUIValues(&values);
  myUI->setSensitivity(FpPM::isModelEditable() && FapLicenseManager::checkCtrlLicense(false));
}


void FapUACtrlElemProperties::getDBValues(FFuaUIValues* values)
{
  if (!mySelectedFmItem) return;

  if (!mySelectedFmItem->isOfType(FmCtrlElementBase::getClassTypeID())) return;

  FuaCtrlElemPropertiesValues* DBVals = dynamic_cast<FuaCtrlElemPropertiesValues*>(values);
  if (!DBVals) return;

  FmCtrlElementBase* item = static_cast<FmCtrlElementBase*>(mySelectedFmItem);
  DBVals->ctrlTypeIdx = item->getTypeID();
  DBVals->pixmap = item->getPixmap();
  std::vector<ctrlVars> fmParams;
  item->getElementVariables(fmParams);
  DBVals->parameters.reserve(fmParams.size());
  for (const ctrlVars& var : fmParams)
    DBVals->parameters.push_back({ var.myString, var.getFcn(item) });
}


void FapUACtrlElemProperties::setDBValues(FFuaUIValues* values)
{
  if (!mySelectedFmItem) return;

  if (!mySelectedFmItem->isOfType(FmCtrlElementBase::getClassTypeID())) return;

  FuaCtrlElemPropertiesValues* UIVals = dynamic_cast<FuaCtrlElemPropertiesValues*>(values);
  if (!UIVals || UIVals->parameters.empty()) return;

  FmCtrlElementBase* item = static_cast<FmCtrlElementBase*>(mySelectedFmItem);
  std::vector<ctrlVars> fmParams;
  item->getElementVariables(fmParams);
  if (fmParams.empty()) return;

  // Check license for control system editing
  if (!FapLicenseManager::hasCtrlLicense()) return;

  for (size_t i = 0; i < fmParams.size() && i < UIVals->parameters.size(); i++)
    fmParams[i].setFcn(item,UIVals->parameters[i].second);

  item->onChanged();
}
