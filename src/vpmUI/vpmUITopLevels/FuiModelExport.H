// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MODEL_EXPORT_H
#define FUI_MODEL_EXPORT_H

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFuLib/FFuBase/FFuUAFinishHandler.H"
#include <string>
#include <vector>
#include <tuple>


class FuiModelExport : virtual public FFuTopLevelShell,
                       public FFuUAExistenceHandler,
                       public FFuUADataHandler,
                       public FFuUAFinishHandler
{
  Fmd_HEADER_INIT();

public:
  FuiModelExport();

  static FuiModelExport* create(int xpos = 100, int ypos = 100,
                                int width = 100, int height = 100,
                                const char* title = "Model Export",
                                const char* name = "FuiModelExport");

  void setExportCB(const FFaDynCB0& cb) { exportCB = cb; }

protected:
  virtual bool onClose();

private:
  virtual void onPoppedUp();

  virtual FFuaUIValues* createValuesObject();

protected:
  FFaDynCB0 exportCB;
};
/////////////////////////////////////////////////////////////////////////////

using DTInput  = std::tuple<std::string,std::string>;
using DTOutput = std::tuple<std::string,std::string,std::string>;

class FuaModelExportValues : public FFuaUIValues
{
public:
  //Stream
  std::string streamFilename;
  std::string streamInputIndGroup;
  std::string streamOutputIndGroup;
  double streamWindowSize = 10;
  bool streamTransferState = false;
  bool streamAppExport = false;
  //Batch
  std::string batchFilename;
  std::string batchInputIndGroup;
  bool batchSurfaceOnly = true;
  bool batchStressRecovery = true;
  bool batchAllFEParts = false;
  bool batchAppExport = false;
  //FMU
  std::string fmuFilename;
  bool fmuAppExport = false;
  bool includeExtFuncFile = false;
  // Indicators
  std::vector<DTInput>  inputs;
  std::vector<DTOutput> outputs;
};

#endif
