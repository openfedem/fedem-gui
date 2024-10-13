// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_EXPORT_CMDS_H
#define FAP_EXPORT_CMDS_H

#include <string>

#include "FapCmdsBase.H"
#include "FFaLib/FFaPatterns/FFaInitialisation.H"

class FmGraph;
class FmCurveSet;
class FmModelExpOptions;


class FapExportCmds : public FapCmdsBase
{
  static void init();

public:
  static std::string autoExportCurves(const std::string& exportPath,
                                      int format = 0,
                                      bool exportSingleGraph = true);
  static void autoExportToVTF(const std::string& exportDir, int format = 0,
			      bool asFirstOrder = true);
  static void exportApps();

private:
  static void exportObject();
  static void getExportObjectSensitivity(bool& sensitivity);
  static void getExportLinkSensitivity(bool& sensitivity);

  static void exportCurves();
  static void exportGraphs();
  static void exportGraph();
  static void getExportCurveSensitivity(bool& sensitivity);

  static void exportCurvesAuto();
  static void getAutoExportCurveSensitivity(bool& sensitivity);

  static void findSelectedCurves(std::vector<FmCurveSet*>& curves);

  static void exportMultCurves(const std::vector<FmCurveSet*>& curves);
  static void exportMultGraphs(const std::vector<FmCurveSet*>& curves,
			       const std::vector<FmGraph*>& graphs,
			       bool exportAsOneGraph = false);

  static void exportCurves(const std::vector<FmCurveSet*>& curves,
			   const std::string& dirPath, int format, std::string& message);
  static bool exportGraph(const std::vector<FmCurveSet*>& curves,
			  const std::string& fileName, int format, std::string& message, bool noHeader = false);

  static void exportGraphStatistics();
  static void getExportStatisticsSensitivity(bool& sensitivity);

  static void exportCurveFatigue();
  static void getExportFatigueSensitivity(bool& sensitivity);

  static void exportView();
  static void getExportViewSensitivity(bool& sensitivity);

  static void exportAnimation();
  static void onAnimationExportSetupDone(int button);
  static void getExportAnimationSensitivity(bool& sensitivity);

  static void exportVTF();
  static void getExportVTFSensitivity(bool& sensitivity);

  static void exportCGeo();
  static void exportDTSApp(FmModelExpOptions* options);
  static void exportDTSBatchApp(FmModelExpOptions* options);
  static void exportFMUApp(FmModelExpOptions* options);
  static void exportDigitalTwin();
  static void exportPipeWear();

  friend class FFaInitialisation<FapExportCmds>;
};

static FFaInitialisation<FapExportCmds> FapExportCmds_Init;

#endif
