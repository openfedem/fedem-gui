// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAModelExport.H"
#include "vpmApp/vpmAppCmds/FapExportCmds.H"
#include "vpmUI/vpmUITopLevels/FuiModelExport.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmEngine.H"
#include "vpmDB/FmModelExpOptions.H"


Fmd_SOURCE_INIT(FAPUAMODELEXPORT, FapUAModelExport, FapUAExistenceHandler);


//----------------------------------------------------------------------------

FapUAModelExport::FapUAModelExport(FuiModelExport* uic)
  : FapUAExistenceHandler(uic), FapUADataHandler(uic), FapUAFinishHandler(uic)
{
  Fmd_CONSTRUCTOR_INIT(FapUAModelExport);

  this->ui = uic;

  this->ui->setExportCB(FFaDynCB0S(FapExportCmds::exportApps));
}
//----------------------------------------------------------------------------

FFuaUIValues* FapUAModelExport::createValuesObject()
{
  return new FuaModelExportValues();
}
//----------------------------------------------------------------------------

void FapUAModelExport::setDBValues(FFuaUIValues* values)
{
  FuaModelExportValues* exportValues = (FuaModelExportValues*) values;

  FmModelExpOptions* exp = FmDB::getModelExportOptions();

  //Stream
  bool changed = exp->streamFilename.setValue(exportValues->streamFilename);
  changed |= exp->streamInputIndGroup.setValue(exportValues->streamInputIndGroup);
  changed |= exp->streamOutputIndGroup.setValue(exportValues->streamOutputIndGroup);
  changed |= exp->streamWindowSize.setValue(exportValues->streamWindowSize);
  changed |= exp->streamTransferState.setValue(exportValues->streamTransferState);
  changed |= exp->streamAppExport.setValue(exportValues->streamAppExport);

  //Batch
  changed |= exp->batchFilename.setValue(exportValues->batchFilename);
  changed |= exp->batchInputIndGroup.setValue(exportValues->batchInputIndGroup);
  changed |= exp->batchSurfaceOnly.setValue(exportValues->batchSurfaceOnly);
  changed |= exp->batchStressRecovery.setValue(exportValues->batchStressRecovery);
  changed |= exp->batchAllFEParts.setValue(exportValues->batchAllFEParts);
  changed |= exp->batchAppExport.setValue(exportValues->batchAppExport);

  //FMU
  changed |= exp->fmuFilename.setValue(exportValues->fmuFilename);
  changed |= exp->inclExtFuncFile.setValue(exportValues->includeExtFuncFile);
  changed |= exp->fmuExport.setValue(exportValues->fmuAppExport);

  if (changed) FpPM::touchModel(); // Indicate that the model needs save
}
//----------------------------------------------------------------------------

void FapUAModelExport::getDBValues(FFuaUIValues* values)
{
  FuaModelExportValues* exportValues = (FuaModelExportValues*) values;

  FmModelExpOptions* exp = FmDB::getModelExportOptions(false);
  // Create a temporary object to get default values
  if (!exp) exp = new FmModelExpOptions();

  //Stream
  exportValues->streamFilename = exp->streamFilename.getValue();
  exportValues->streamInputIndGroup = exp->streamInputIndGroup.getValue();
  exportValues->streamOutputIndGroup = exp->streamOutputIndGroup.getValue();
  exportValues->streamWindowSize = exp->streamWindowSize.getValue();
  exportValues->streamTransferState = exp->streamTransferState.getValue();
  exportValues->streamAppExport = exp->streamAppExport.getValue();

  //Batch
  exportValues->batchFilename = exp->batchFilename.getValue();
  exportValues->batchInputIndGroup = exp->batchInputIndGroup.getValue();
  exportValues->batchSurfaceOnly = exp->batchSurfaceOnly.getValue();
  exportValues->batchStressRecovery = exp->batchStressRecovery.getValue();
  exportValues->batchAllFEParts = exp->batchAllFEParts.getValue();
  exportValues->batchAppExport = exp->batchAppExport.getValue();

  //FMU
  exportValues->fmuFilename = exp->fmuFilename.getValue();
  exportValues->includeExtFuncFile = exp->inclExtFuncFile.getValue();
  exportValues->fmuAppExport = exp->fmuExport.getValue();

  // Delete the temporary object
  if (!FmDB::getModelExportOptions(false)) exp->erase();

  // Get input and output indicator definitions
  exportValues->inputs.clear();
  exportValues->outputs.clear();
  std::vector<FmEngine*> engines;
  FmDB::getAllEngines(engines);
  for (FmEngine* e : engines)
    if (e->isExternalFunc())
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_input_" + std::to_string(exportValues->inputs.size() + 1);
      else for (char& c : name)
        if (!isalnum(c)) c = '_';
      exportValues->inputs.push_back(std::make_tuple(name, e->getItemDescr()));
    }
    else if (e->myOutput.getValue())
    {
      std::string name = e->getTag();
      if (name.empty())
        name = "dt_output_" + std::to_string(exportValues->outputs.size() + 1);
      else for (char& c : name)
        if (!isalnum(c)) c = '_';
      exportValues->outputs.push_back(std::make_tuple(name, e->getItemDescr(),
                                                      e->myThreshold.getValue().description));
    }
}
//----------------------------------------------------------------------------

void FapUAModelExport::finishUI()
{
  Fui::modelExportUI(false,true);
}
//----------------------------------------------------------------------------
