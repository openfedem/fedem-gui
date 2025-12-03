// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppDisplay/FapGraphDataMap.H"
#include "vpmApp/vpmAppDisplay/FapReadCurveData.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmDB.H"

#include "vpmPM/FpRDBExtractorManager.H"
#include "FFpLib/FFpFatigue/FFpSNCurve.H"
#include "FFpLib/FFpCurveData/FFpGraph.H"
#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include <algorithm>


/*!
  Builds the \a dataMap with operations and then reads data from the RDB,
  external curve data files, and/or internal functions for a set of \a curves.
  Error messages (if any) are given in a dialog box and output list if the
  the pointer \a errMsg is null. Otherwise, they are returned in \a *errMsg.
*/

bool FapGraphDataMap::findPlottingData(const std::vector<FmCurveSet*>& curves,
				       std::string* errMsg, bool isAppending)
{
  if (curves.empty()) return true;

  // First, resolve the combined curves (if any) such that we
  // only try to read the basic curves (RDB, external and function)
  std::vector<FmCurveSet*> bCurves(curves);
  replaceCombinedCurves(bCurves);
#ifdef FAP_DEBUG
  if (bCurves != curves)
    std::cout <<"FapGraphDataMap: Resolved combined curves "
              << curves.size() <<" --> "<< bCurves.size() << std::endl;
#endif

  std::string  msg1, msg2;
  std::string& message = (errMsg ? *errMsg : msg1); // Dialog box messages
  std::string& listMsg = (errMsg ? *errMsg : msg2); // Output list messages

  FFpGraph rdbCurves;

  // Set the load time interval from the owner graph of the first RDB curve
  for (FmCurveSet* curve : curves)
    if (curve->usingInputMode() == FmCurveSet::TEMPORAL_RESULT ||
        curve->usingInputMode() == FmCurveSet::COMB_CURVES)
      if (FmGraph* graph = curve->getOwnerGraph();
          graph && graph->getUseTimeRange())
      {
	double tmin, tmax;
	graph->getTimeRange(tmin,tmax);
	rdbCurves.setTimeInterval(tmin,tmax);
	break;
      }

  int rdbType = -1;
  std::map<const FmCurveSet*,FFpCurve>::iterator cit;
  for (FmCurveSet* curve : bCurves)
  {
    if (curve->usingInputMode() == FmCurveSet::SPATIAL_RESULT)
    {
      std::vector<FmIsPlottedBase*> spatialObjs;
      curve->getSpatialObjs(spatialObjs);
      if (size_t nPoints = spatialObjs.size(); nPoints > 0)
      {
        // Create result description for each spatial point
        std::vector<FFaResultDescription> spatialDescr, xDescr;
        spatialDescr.reserve(nPoints);
        for (FmIsPlottedBase* obj : spatialObjs)
        {
          spatialDescr.emplace_back(obj->getUITypeName(),
                                    obj->getBaseID(),obj->getID());
          spatialDescr.back().copyResult(curve->getResult(FmCurveSet::YAXIS));
        }

	short int end1 = -1;
	if (spatialDescr.front().isBeamSectionResult())
	{
	  // Special treatment for beam section results (at element ends)
	  if (nPoints > 1)
	  {
	    // Detect direction of traversal
	    FmBeam* b1 = static_cast<FmBeam*>(spatialObjs[0]);
	    FmBeam* b2 = static_cast<FmBeam*>(spatialObjs[1]);
	    if (b1->getSecondTriad() == b2->getFirstTriad())
	      end1 = 0;
	    else if (b1->getFirstTriad() == b2->getSecondTriad())
	      end1 = 1;
	    else
	      std::cerr <<" *** FapGraphDataMap::findPlottingData:"
			<<" Could not detect direction of traversal."
			<< std::endl;
	  }
	  xDescr.reserve(nPoints*2);
	  FmTriad* triads[2];
	  for (FmIsPlottedBase* obj : spatialObjs)
	  {
	    triads[0] = static_cast<FmBeam*>(obj)->getFirstTriad();
	    triads[1] = static_cast<FmBeam*>(obj)->getSecondTriad();
	    if (end1 == 1) std::swap(triads[0],triads[1]);
	    for (int k = 0; k < 2; k++)
              xDescr.emplace_back(triads[k]->getUITypeName(),
                                  triads[k]->getBaseID(),triads[k]->getID());
	  }
	  nPoints *= 2; // two spatial result points per element
	}

	// Initialize the axis definitions for this spatial RDB-curve.
	// Any existing curve data is thrown away.
	FFpCurve& ffpc = dataMap[curve];
	ffpc.resize(nPoints);
	ffpc.initAxes(xDescr,spatialDescr,
		      curve->getResultOper(FmCurveSet::XAXIS),
		      curve->getResultOper(FmCurveSet::YAXIS),
		      curve->getTimeRange(),
		      curve->getTimeOper(),end1);
      }
    }

    if ((cit = dataMap.find(curve)) == dataMap.end())
      cit = dataMap.emplace(curve,FFpCurve()).first; // new RDB-curve
    else if (cit->first->usingInputMode() == FmCurveSet::TEMPORAL_RESULT)
    {
      // Clear the temporal RDB-curve data when ...
      if (isAppending && cit->first->doAnalysis())
        cit->second.clear(); // we are appending and doing curve analysis,
      else if (!isAppending && cit->first->hasXYDataChanged()) // Bugfix #510
        cit->second.clear(); // there is new data while not appending, or
      else if (cit->first->hasDFTOptionsChanged(1))
        // DFT-options have changed while DFT is on, or we are appending
        cit->second.clear(); // while the DFT was switched off
    }

    if (cit->first->usingInputMode() <= FmCurveSet::RDB_RESULT)
    {
      // Ensure all RDB-curves are of the same kind
      if (rdbType < 0)
	rdbType = cit->first->usingInputMode();
      else if (cit->first->usingInputMode() != rdbType)
	continue; // Cannot mix temporal and spatial RDB curves...
      rdbCurves.addCurve(&cit->second);
    }

    switch (cit->first->usingInputMode()) {
    case FmCurveSet::TEMPORAL_RESULT:
      // Initialize the axis definitions for this temporal RDB-curve
      for (int axis = 0; axis < FmCurveSet::NAXES; axis++)
	cit->second.initAxis(cit->first->getResult(axis),
			     cit->first->getResultOper(axis), axis);
      break;

    case FmCurveSet::SPATIAL_RESULT:
      // Beta feature: Check if model configuration should be used for X-axis
      if (FmGraph* graph = cit->first->getOwnerGraph(); graph)
        if (graph->getUserDescription().find("#Model") != std::string::npos)
          rdbCurves.setNoXaxisValues();
      break;

    case FmCurveSet::EXT_CURVE:
      // Find data for "external curves" (i.e. defined via ascii/dac/rpc file)
      if (cit->first->hasXYDataChanged() ||      // the curve data has changed
	  cit->first->hasDFTOptionsChanged(1) || // options for dft has changed
	  cit->second.empty())                   // this is the first read
	if (!findDataFromFile(cit->first, cit->second, listMsg))
	  cit->second.clear();
      break;

    case FmCurveSet::INT_FUNCTION:
    case FmCurveSet::PREVIEW_FUNC:
      // Find data for "internal function" curves
      if (cit->first->hasXYDataChanged() ||      // the curve data has changed
	  cit->first->hasDFTOptionsChanged(1) || // options for dft has changed
	  cit->second.empty())                   // this is the first read
	if (!findDataFromFunc(cit->first, cit->second, listMsg))
	  cit->second.clear();
      break;

    default:
      break;
    }
  }

  // Lambda function defining the X-axis values for a spatial curve.
  auto&& setXvalue = [](FFpCurve& crv, const std::string& xOper)
  {
    crv[FmCurveSet::XAXIS].resize(crv[FmCurveSet::YAXIS].size(),0.0);

    size_t ix = xOper.find("Position");
    if (ix == std::string::npos || ix+9 >= xOper.size()) return;
    char cPos = xOper[ix+9];
    if (cPos < 'X' || cPos > 'Z') return;
    ix = cPos - 'X';

    size_t i = 0;
    for (double& xValue : crv[FmCurveSet::XAXIS])
      if (FmBase* obj = FmDB::findObject(crv.getSpatialXaxisObject(i++)); obj)
        if (FmIsPositionedBase* p = dynamic_cast<FmIsPositionedBase*>(obj); p)
          xValue = p->getGlobalCS().translation()[ix];
  };

  if (!rdbCurves.empty())
  {
    // Actually read the RDB curves from file
#ifdef FAP_DEBUG
    std::cout <<"FapGraphDataMap: Loading curve data from RDB"<< std::endl;
#endif
    bool readOK = true;
    if (!isAppending) FFaMsg::pushStatus("Reading curve data from RDB");
    FFrExtractor* extr = FpRDBExtractorManager::instance()->getModelExtractor();
    if (rdbType == FmCurveSet::TEMPORAL_RESULT)
      readOK = rdbCurves.loadTemporalData (extr,message);
    else
    {
      readOK = rdbCurves.loadSpatialData (extr,message);
      if (rdbCurves.getNoXaxisValues())
        for (cit = dataMap.begin(); cit != dataMap.end(); ++cit)
          if (cit->first->usingInputMode() == FmCurveSet::SPATIAL_RESULT)
            setXvalue(cit->second,cit->first->getResultOper(FmCurveSet::XAXIS));
    }
    if (!isAppending) FFaMsg::popStatus();
    if (readOK && !msg1.empty())
    {
      // We got some messages from the data reader, but no failure status.
      // Redirect the messages to the Output list instead (no pop-up dialog).
      msg2.append(msg1);
      msg1.erase();
    }
  }

  // Process the expressions of the combined curves, if any
  for (FmCurveSet* curve : curves)
    if (curve->usingInputMode() == FmCurveSet::COMB_CURVES)
      findCombinedCurveData(curve,listMsg);

  // Replace the wanted curves by their Derivative, Fourier transform, etc.
  int giveStatus = isAppending ? 0 : 1;
  for (cit = dataMap.begin(); cit != dataMap.end(); ++cit)
    if (cit->first->hasDFTOptionsChanged() || cit->second.hasDataChanged())
    {
      if (cit->first->derivate())
      {
	if (giveStatus == 1)
	{
	  FFaMsg::pushStatus("Differentiating");
	  giveStatus = 2;
	}
#ifdef FAP_DEBUG
        std::cout <<"FapGraphDataMap: Differentiating "
                  << cit->first->getIdString(true) << std::endl;
#endif
	if (!cit->second.replaceByScaledShifted(cit->first->getDFTparameters()))
	  cit->second.clear();
	else if (!cit->second.replaceByDerivative())
	  cit->second.clear();
      }
      else if (cit->first->integrate())
      {
	if (giveStatus == 1)
	{
	  FFaMsg::pushStatus("Integrating");
	  giveStatus = 2;
	}
#ifdef FAP_DEBUG
        std::cout <<"FapGraphDataMap: Integrating "
                  << cit->first->getIdString(true) << std::endl;
#endif
	if (!cit->second.replaceByScaledShifted(cit->first->getDFTparameters()))
	  cit->second.clear();
	else if (!cit->second.replaceByIntegral())
	  cit->second.clear();
      }
      else if (cit->first->doDft())
      {
	if (giveStatus == 1)
	{
	  FFaMsg::pushStatus("Doing DFT transformation");
	  giveStatus = 2;
	}
#ifdef FAP_DEBUG
        std::cout <<"FapGraphDataMap: DFT transforming "
                  << cit->first->getIdString(true) << std::endl;
#endif
	if (!cit->second.replaceByDFT(cit->first->getDFTparameters(),
				      cit->first->getUserDescription(),message))
	  cit->second.clear(); // Don't plot curve if the DFT failed
      }
      else if (cit->first->doRainflow())
      {
	if (giveStatus == 1)
	{
	  FFaMsg::pushStatus("Doing rainflow analysis");
	  giveStatus = 2;
	}
#ifdef FAP_DEBUG
        std::cout <<"FapGraphDataMap: Rainflow transforming "
                  << cit->first->getIdString(true) << std::endl;
#endif
	double ys = cit->first->getYScale();
	RFprm rf(cit->first->getFatigueGateValue()/ys);
	if (!cit->first->getFatigueEntireDomain())
	{
	  rf.start = cit->first->getFatigueDomain().first;
	  rf.stop = cit->first->getFatigueDomain().second;
	}

	// Beta feature: Plotting the peak-and-valley extraction results
	bool pvx = cit->first->getUserDescription().find("#PVX") < std::string::npos;
	if (!cit->second.replaceByRainflow(rf,ys,pvx,
					   cit->first->getUserDescription(),
					   message))
	  cit->second.clear(); // Don't plot curve if the Rainflow failed
      }
    }
  if (giveStatus == 2) FFaMsg::popStatus();

  if (errMsg) return errMsg->empty(); // Error messages are returned in *errMsg

  if (!isAppending)
  {
    // Output error messages, if any
    if (!msg1.empty())
    {
      if (msg1.size() < 700)
        FFaMsg::dialog(msg1,FFaMsg::DISMISS_INFO);
      else if (FFaMsg::dialog("Several curves could not be loaded because "
			      "their data were not present in the RDB.\n"
			      "Do you want a detailed list of the missing "
			      "data items in the Output List?",FFaMsg::YES_NO))
        ListUI << msg1 <<"\n";
    }
    if (!msg2.empty())
      ListUI << msg2 <<"\n";
  }
#ifdef FAP_DEBUG
  else
  {
    if (!msg1.empty()) std::cout << msg1 << std::endl;
    if (!msg2.empty()) std::cout << msg2 << std::endl;
  }
#endif
  return msg1.empty();
}


/*!
  Replaces the combined curves in a vector by their respective curve components.
*/

void FapGraphDataMap::replaceCombinedCurves(std::vector<FmCurveSet*>& curves)
{
  std::vector<FmCurveSet*> eCurves;
  for (size_t j = 0; j < curves.size();)
    if (curves[j]->usingInputMode() == FmCurveSet::COMB_CURVES)
    {
      std::vector<FmCurveSet*> comps;
      curves[j]->getActiveCurveComps(comps);
      for (FmCurveSet* comp : comps)
        if (std::find(curves.begin(),curves.end(),comp) == curves.end())
          if (std::find(eCurves.begin(),eCurves.end(),comp) == eCurves.end())
            curves.push_back(comp);

      eCurves.push_back(curves[j]);
      curves.erase(curves.begin()+j);
    }
    else
      j++;
}


/*!
  Loads curve point data for \a curve from an internal function.
*/

bool FapGraphDataMap::findDataFromFunc(const FmCurveSet* curve,
                                       FFpCurve& curveData,
                                       std::string& message)
{
  if (!curve) return false;
  if (!curve->areAxesComplete()) return false;

  FmMathFuncBase* function = curve->getFunctionRef();
  if (!function) return false;

#ifdef FAP_DEBUG
  std::cout <<"FapGraphDataMap: Loading curve data from Function "
            << function->getInfoString() << std::endl;
#endif

  curveData.setDataChanged();

  int error;
  if (curve->getUseSmartPoints())
    error = function->getSmartPoints(curve->getFuncDomain().first,
				     curve->getFuncDomain().second,
				     curveData[FmCurveSet::XAXIS],
				     curveData[FmCurveSet::YAXIS]);
  else
    error = function->getCurvePoints(curve->getFuncDomain().first,
				     curve->getFuncDomain().second,
				     curve->getIncX(),
				     curveData[FmCurveSet::XAXIS],
				     curveData[FmCurveSet::YAXIS]);
  if (!error) return true;

  message += "\nError in " + curve->getIdString(true) +":\n";
  if (error == -1)
    message += "Invalid domain or increment set for Function "
      + function->getInfoString();
  else if (curve->getUseSmartPoints())
    message += "Could not get the defining points of Function "
      + function->getInfoString()
      + "\nVerify that the function is properly defined and has a valid domain";
  else
  {
    message += "Could not get data from Function " + function->getInfoString()
      + "\nVerify that the function is properly defined and has a valid domain";
    if (std::string(function->getUITypeName()) == "Spline")
      message += "\nA cubic spline needs at least four points";
  }

  return false;
}


/*!
  Loads curve point data for \a curve from an external file.
*/

bool FapGraphDataMap::findDataFromFile(const FmCurveSet* curve,
                                       FFpCurve& curveData,
                                       std::string& message)
{
  if (!curve) return false;

  std::string filePath = curve->getFilePath();
  if (filePath.empty()) return false;

  // Check for relative path, add model file path if neccesary
  FmMechanism* mech = FmDB::getMechanismObject();
  FFaFilePath::makeItAbsolute(filePath,mech->getAbsModelFilePath());

  // Check if a time range applies
  FmRange timeRange(0.0,-1.0);
  curve->getOwnerGraph()->useTimeRange(timeRange);

#ifdef FAP_DEBUG
  std::cout <<"FapGraphDataMap: Loading curve data from "<< filePath
            << std::endl;
#endif
  return curveData.loadFileData (filePath, curve->getChannelName(), message,
				 timeRange.first, timeRange.second);
}


/*!
  Loads curve point data for the combined curve \a ccrv by evaluating the
  mathematical expression defining it at each curve point, where the component
  curves defines the argument values.
*/

bool FapGraphDataMap::findCombinedCurveData(const FmCurveSet* ccrv,
					    std::string& message)
{
  if (ccrv->usingInputMode() != FmCurveSet::COMB_CURVES) return true;

#ifdef FAP_DEBUG
  std::cout <<"FapGraphDataMap: Loading curve data for expression "
            << ccrv->getExpression() << std::endl;
#endif

  std::vector<FmCurveSet*> curves;
  std::vector<bool>        active;
  ccrv->getCurveComps(curves,active);

  static std::vector<const FmCurveSet*> cStack;
  cStack.push_back(ccrv);

  std::vector<FFpCurve>  transCrv;
  std::vector<FFpCurve*> comps(active.size(),NULL);
  transCrv.reserve(active.size());
  for (size_t i = 0; i < active.size(); i++)
    if (active[i])
    {
      if (i >= curves.size() || !curves[i])
        message += "Component " + std::string(FmCurveSet::getCompNames()[i])
          + " is undefined.\n";
      else if (std::find(cStack.begin(),cStack.end(),curves[i]) != cStack.end())
        message += "Looping component curve definition detected.\n";
      else if (curves[i]->doDft() || curves[i]->doRainflow())
        message += "Component " + std::string(FmCurveSet::getCompNames()[i])
          + ": " + curves[i]->getIdString(true) + " is transformed.\n";
      else if (findCombinedCurveData(curves[i],message))
      {
        if (curves[i]->derivate() || curves[i]->integrate() ||
            curves[i]->hasNonDefaultScaleShift())
        {
          // Create a local copy of this curve component and transform it first
          transCrv.push_back(dataMap[curves[i]]);
          transCrv.back().replaceByScaledShifted(curves[i]->getDFTparameters());
          bool transformed = true;
          if (curves[i]->derivate())
            transformed = transCrv.back().replaceByDerivative();
          else if (curves[i]->integrate())
            transformed = transCrv.back().replaceByIntegral();
          if (transformed)
            comps[i] = &transCrv.back();
        }
        else
          comps[i] = this->getFFpCurve(curves[i],false);
      }
    }

  cStack.pop_back();

  bool doClip = ccrv->getUserDescription().find("#noClip") == std::string::npos;
  if (dataMap[ccrv].combineData(ccrv->getBaseID(),ccrv->getExpression(),comps,
                                FmCurveSet::getCompNames(),doClip,message))
    return true;

  message += "Failed to evaluate combined " + ccrv->getIdString(true) + ".\n";
  dataMap[ccrv].clear();
  return false;
}


/*!
  Returns a pointer to the corresponding FFpCurve object for \a curve.
  If scaling and/or offset is defined for the curve, and \a scaleShift is true,
  the curve points in the FFpCurve object are transformed first, such that
  the exported curve is identical to what you see in the graph view.
  \note The returned FFpCurve pointer goes out of scope once this
  FapGraphDataMap object is deleted.
*/

FFpCurve* FapGraphDataMap::getFFpCurve(const FmCurveSet* curve,
				       bool scaleShift, bool createIfNone)
{
  if (!curve) return NULL;

  std::map<const FmCurveSet*,FFpCurve>::iterator cit = dataMap.find(curve);
  if (cit == dataMap.end())
    return createIfNone ? &dataMap[curve] : NULL;

  // Check if the curve data need to be scaled and/or shifted before export.
  // Note that if the curve has been Fourier-transformed, the scale/shift
  // options have already been applied during that process.
  if (scaleShift && !curve->doAnalysis() && curve->hasNonDefaultScaleShift())
    if (!cit->second.replaceByScaledShifted(curve->getDFTparameters()))
      return NULL;

  return &(cit->second);
}


/*!
  Checks whether the loaded plotting data, if any, for the given \a curve
  has been changed recently (since the last graph view update).
  The method will also return \e true if \a curve is currently not loaded.
*/

bool FapGraphDataMap::hasDataChanged(const FmCurveSet* curve) const
{
  std::map<const FmCurveSet*,FFpCurve>::const_iterator c = dataMap.find(curve);
  return c == dataMap.end() ? true : c->second.hasDataChanged();
}


/*!
  Sets the data change flag for the given \a curve, to force a reload
  when (re)-enabling the result container with the actual curve data.
*/

bool FapGraphDataMap::setDataChanged(const FmCurveSet* curve)
{
  std::map<const FmCurveSet*,FFpCurve>::iterator c = dataMap.find(curve);
  if (c == dataMap.end() || c->second.hasDataChanged()) return false;

  c->second.setDataChanged();
  return true;
}


/*!
  Calculates statistical properties of \a curve if currently loaded.
*/

bool FapGraphDataMap::getCurveStatistics(const FmCurveSet* curve,
					 bool scaledShifted, bool wholeDomain,
					 double startT, double stopT,
					 FapCurveStat& stat)
{
  FFpCurve* ffpCurve = this->getFFpCurve(curve,false);
  if (!ffpCurve) return false;

  std::string msg;
  if (ffpCurve->getCurveStatistics(wholeDomain, startT, stopT,
				   scaledShifted, curve->getDFTparameters(),
				   stat.rms, stat.avg, stat.stdDev,
				   stat.integral, stat.min, stat.max, msg))
    return true;

  if (!msg.empty()) FFaMsg::dialog(msg,FFaMsg::DISMISS_ERROR);
  return false;
}


/*!
  Calculates statistical properties for all curves currently loaded.
*/

bool FapGraphDataMap::getCurveStatistics(std::vector<FapCurveSet>& stat,
					 bool scaledShifted, bool wholeDomain,
					 double startT, double stopT) const
{
  stat.resize(dataMap.size());

  std::string msg;
  std::vector<FapCurveSet>::iterator sit = stat.begin();
  std::map<const FmCurveSet*,FFpCurve>::const_iterator cit;

  for (cit = dataMap.begin(); cit != dataMap.end(); ++cit, ++sit)
    if (cit->second.getCurveStatistics(wholeDomain, startT, stopT,
				       scaledShifted,
				       cit->first->getDFTparameters(),
				       sit->second.rms, sit->second.avg,
				       sit->second.stdDev, sit->second.integral,
				       sit->second.min, sit->second.max, msg))
      sit->first = cit->first;
    else
    {
      if (!msg.empty()) FFaMsg::dialog(msg,FFaMsg::DISMISS_ERROR);
      return false;
    }

  return true;
}


/*!
  Calculates damage of \a curve based on given \a gateValue and \a snCurve.
  If \a scaledData is true, the Y-axis scaling factor of the curve, if any,
  is applied before the damage is calculated.
*/

double FapGraphDataMap::getDamageFromCurve(const FmCurveSet* curve,
					   double gateValue,
					   bool scaledData, bool wholeDomain,
					   double startT, double stopT,
					   const FFpSNCurve& snCurve)
{
  if (!snCurve.isValid() || gateValue <= 0.0) return -1.0;

  FFpCurve* ffpCurve = this->getFFpCurve(curve,false);
  if (!ffpCurve || ffpCurve->empty()) return -1.0;

  FmMechanism* mech = FmDB::getMechanismObject();
  double MPa = 1.0e-6; // Stress scaling factor to MPa
  mech->modelDatabaseUnits.getValue().convert(MPa,"FORCE/AREA");

  if (scaledData)
  {
    double scale = curve->getYScale();
    if (scale == 0.0) return -1.0;

    // Apply the scaling factor implicitly through the MPa and gateValue factors
    gateValue /= scale;
    MPa *= scale;
  }

  if (wholeDomain)
    return ffpCurve->getDamage(RFprm(gateValue),MPa,snCurve);
  else if (startT < stopT)
    return ffpCurve->getDamage(RFprm(startT,stopT,gateValue),MPa,snCurve);
  else
    return 0.0;
}


size_t Fap::readCurveData(const std::vector<FmCurveSet*>& curves,
                          std::vector<DoubleVec>& values,
                          double startT, double stopT)
{
  values.clear();
  std::string message;
  FapGraphDataMap graphDataMap(curves,message);
  if (!message.empty())
    ListUI << message <<"\n";

  size_t nCX = 0;
  values.reserve(curves.size());
  for (FmCurveSet* curve : curves)
    if (FFpCurve* myCurve = graphDataMap.getFFpCurve(curve); myCurve)
    {
      myCurve->clipX(startT,stopT);
      if (!myCurve->empty())
      {
        // Consider only the non-empty curves
        values.push_back(myCurve->getAxisData(1));
        if (values.size() == 1 || nCX > values.back().size())
          nCX = values.back().size();
      }
    }

  return nCX;
}
