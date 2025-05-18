// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaFunction.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmAllFunctionHeaders.H"
#include "vpmDB/FmEngine.H"
#include "vpmDB/FmcOutput.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmSimpleSensor.H"
#include "vpmDB/FmRelativeSensor.H"
#include "vpmDB/FmDB.H"

#include "FFaLib/FFaDefinitions/FFaMsg.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaFunction, CCmdTarget)

BEGIN_MESSAGE_MAP(CaFunction, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaFunction, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaFunction, "FunctionType", dispidFunctionType, get_FunctionType, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaFunction, "Parameters", dispidParameters, get_Parameters, put_Parameters, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaFunction, "ArgumentObject", dispidArgumentObject, get_ArgumentObject, SetNotSupported, VT_VARIANT)
  DISP_PROPERTY_EX_ID(CaFunction, "ArgumentDOF", dispidArgumentDOF, get_ArgumentDOF, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaFunction, "ArgumentVariable", dispidArgumentVariable, get_ArgumentVariable, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaFunction, "Usage", dispidUsage, get_Usage, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaFunction, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaFunction, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaFunction, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaFunction, "ChannelID", dispidChannelID, get_ChannelID, SetNotSupported, VT_I4)
  DISP_FUNCTION_ID(CaFunction, "SetConstant", dispidSetConstant, SetConstant, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaFunction, "SetLinear", dispidSetLinear, SetLinear, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaFunction, "SetSin", dispidSetSin, SetSin, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "SetCombinedSin", dispidSetCombinedSin, SetCombinedSin, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "SetPolyline", dispidSetPolyline, SetPolyline, VT_EMPTY, VTS_VARIANT VTS_I4)
  DISP_FUNCTION_ID(CaFunction, "SetPolylineFromFile", dispidSetPolylineFromFile, SetPolylineFromFile, VT_EMPTY, VTS_BSTR VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "SetMathExpression", dispidSetMathExpression, SetMathExpression, VT_EMPTY, VTS_BSTR)
  DISP_FUNCTION_ID(CaFunction, "SetExternal", dispidSetExternal, SetExternal, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaFunction, "SetJonswap", dispidSetJonswap, SetJonswap, VT_EMPTY, VTS_R8 VTS_R8 VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "SetArgumentSingle", dispidSetArgumentSingle, SetArgumentSingle, VT_EMPTY, VTS_I4 VTS_I4 VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "SetArgumentRelative", dispidSetArgumentRelative, SetArgumentRelative, VT_EMPTY, VTS_I4 VTS_I4 VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "SetArgumentScalar", dispidSetArgumentScalar, SetArgumentScalar, VT_EMPTY, VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaFunction, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaFunction, CCmdTarget)
  INTERFACE_PART(CaFunction, IID_IFunction, LocalClass)
END_INTERFACE_MAP()

// {EF4FC756-F2BD-48b4-B63B-43F49A1E2F5C}
IMPLEMENT_OLECREATE(CaFunction, "FEDEM.Function",
0xef4fc756, 0xf2bd, 0x48b4, 0xb6, 0x3b, 0x43, 0xf4, 0x9a, 0x1e, 0x2f, 0x5c);


CaFunction::CaFunction(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  m_nUsage = fuNone;
  signalConnector.Connect(this);
}

CaFunction::~CaFunction(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


static FmfWaveSpectrum* isJonSwap(FmModelMemberBase* ptr)
{
  FmfWaveSpectrum* pWaveFunc = dynamic_cast<FmfWaveSpectrum*>(ptr);
  if (pWaveFunc && pWaveFunc->spectrum.getValue() == FmfWaveSpectrum::JONSWAP)
    return pWaveFunc;
  else
    return NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

FunctionType CaFunction::get_FunctionType()
{
  CA_CHECK(m_ptr);

  FmMathFuncBase* pFunc = NULL;
  FmEngine* pEngine = dynamic_cast<FmEngine*>(m_ptr);
  if (pEngine)
    pFunc = pEngine->getFunction();
  else
    pFunc = dynamic_cast<FmMathFuncBase*>(m_ptr);

  if (pFunc)
    if (pFunc->isOfType(FmfConstant::getClassTypeID()))
      return fConstant;
    else if (pFunc->isOfType(FmfScale::getClassTypeID()))
      return fLinear;
    else if (pFunc->isOfType(FmfSinusoidal::getClassTypeID()))
      return fSin;
    else if (pFunc->isOfType(FmfComplSinus::getClassTypeID()))
      return fCombinedSin;
    else if (pFunc->isOfType(FmfLinVar::getClassTypeID()))
      return fPolyline;
    else if (pFunc->isOfType(FmfDeviceFunction::getClassTypeID()))
      return fPolylineFromFile;
    else if (pFunc->isOfType(FmfUserDefined::getClassTypeID()))
      return fUserDefined;
    else if (pFunc->isOfType(FmfMathExpr::getClassTypeID()))
      return fMathExpr;
    else if (pFunc->isOfType(FmfExternalFunction::getClassTypeID()))
      return fExternal;
    else if (isJonSwap(pFunc))
      return fJonswap;

  // TODO: More types.
  return (FunctionType)-1;
}

// Note: These parameters are function-specific. The parameters must include all
// parameters passed to the SetNN()-method. These parameters should also be first
// and in the same order. Additional parameters can also be used.
BSTR CaFunction::get_Parameters()
{
  CA_CHECK(m_ptr);

  std::string strParams;
  char buf[4096];

  // TODO: More functions!

  FmfWaveSpectrum* pWaveFunc = isJonSwap(m_ptr);
  if (pWaveFunc != NULL) {
    sprintf(buf, "Hs=%g;", pWaveFunc->myHs.getValue());
    strParams.append(buf);
    sprintf(buf, "Tp=%g;", pWaveFunc->myTp.getValue());
    strParams.append(buf);
    sprintf(buf, "Gamma=%g;", pWaveFunc->myPeakedness.getValue());
    strParams.append(buf);
    sprintf(buf, "N=%d;", pWaveFunc->nComp.getValue());
    strParams.append(buf);
    sprintf(buf, "RndSeed=%d;", pWaveFunc->myRandomSeed.getValue());
    strParams.append(buf);
    sprintf(buf, "TRange=%g %g;", pWaveFunc->myTrange.getValue().first, pWaveFunc->myTrange.getValue().second);
    strParams.append(buf);
    sprintf(buf, "AutoCalcGamma=%s;", pWaveFunc->autoCalcPeakedness.getValue() ? "true" : "false");
    strParams.append(buf);
    sprintf(buf, "AutoCalcTRange=%s", pWaveFunc->autoCalcTrange.getValue() ? "true" : "false");
    strParams.append(buf);
  }

  return SysAllocString(CA2W(strParams.c_str()));
}

// See function above for documentation
void CaFunction::put_Parameters(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  if (val == NULL)
    return;

  // Split string
  std::string strParams = CaConvert(val);
  std::vector<std::string> vNames, vValues;
  char* psz = strtok(const_cast<char*>(strParams.c_str()),";");
  for (; psz; psz = strtok(NULL,";")) {
    char* pszEq = strchr(psz,'=');
    if (pszEq != NULL) {
      *pszEq = 0;
      vNames.push_back(psz);
      vValues.push_back(pszEq+1);
    } else {
      vNames.push_back(psz);
      vValues.push_back("");
    }
  }

  FmfWaveSpectrum* pWaveFunc = isJonSwap(m_ptr);
  if (!pWaveFunc) {
    // Get current values
    double Hs = pWaveFunc->myHs.getValue();
    double Tp = pWaveFunc->myTp.getValue();
    VARIANT Gamma;
    Gamma.vt = pWaveFunc->autoCalcPeakedness.getValue() ? VT_EMPTY : VT_R8;
    Gamma.dblVal = pWaveFunc->myPeakedness.getValue();
    VARIANT N;
    N.vt = VT_I4;
    N.lVal = pWaveFunc->nComp.getValue();
    VARIANT RndSeed;
    RndSeed.vt = VT_I4;
    RndSeed.lVal = pWaveFunc->myRandomSeed.getValue();
    VARIANT TRangeMin;
    TRangeMin.vt = pWaveFunc->autoCalcTrange.getValue() ? VT_EMPTY : VT_R8;
    TRangeMin.dblVal = pWaveFunc->myTrange.getValue().first;
    VARIANT TRangeMax;
    TRangeMax.vt = pWaveFunc->autoCalcTrange.getValue() ? VT_EMPTY : VT_R8;
    TRangeMax.dblVal = pWaveFunc->myTrange.getValue().second;
    // Process params
    for (size_t i = 0; i < vNames.size(); i++)
      if (vNames[i].compare("Hs") == 0)
        Hs = atof(vValues[i].c_str());
      else if (vNames[i].compare("Tp") == 0)
        Tp = atof(vValues[i].c_str());
      else if (vNames[i].compare("Gamma") == 0) {
        Gamma.vt = VT_R8;
        Gamma.dblVal = atof(vValues[i].c_str());
      }
      else if (vNames[i].compare("AutoCalcGamma") == 0)
        Gamma.vt = vValues[i].compare("true") == 0 ? VT_EMPTY : VT_R8;
      else if (vNames[i].compare("N") == 0) {
        N.vt = VT_I4;
        N.lVal = atol(vValues[i].c_str());
      }
      else if (vNames[i].compare("RndSeed") == 0) {
        RndSeed.vt = VT_I4;
        RndSeed.lVal = atol(vValues[i].c_str());
      }
      else if (vNames[i].compare("TRange") == 0) {
        // TRange is two space separated double values
        char* pszMin = (char*)vValues[i].c_str();
        char* pszMax = strchr(pszMin, ' ');
        if (pszMax == NULL)
          AfxThrowOleException(E_INVALIDARG);
        *pszMax = 0;
        pszMax++;
        TRangeMin.vt = TRangeMax.vt = VT_R8;
        TRangeMin.dblVal = atof(pszMin);
        TRangeMax.dblVal = atof(pszMax);
      }
      else if (vNames[i].compare("AutoCalcTRange") == 0)
        TRangeMin.vt = TRangeMax.vt = (vValues[i].compare("true") == 0 ? VT_EMPTY : VT_R8);

    SetJonswap(Hs, Tp, Gamma, N, RndSeed, TRangeMin, TRangeMax);
  }
}

VARIANT CaFunction::get_ArgumentObject()
{
  VARIANT vRet;
  vRet.vt = VT_EMPTY;

  // TODO!

  // Currently not supported!
  AfxThrowOleException(E_NOTIMPL);
  return vRet;
}

FunctionArgSingleDOFType CaFunction::get_ArgumentDOF()
{
  // TODO!

  // Currently not supported!
  AfxThrowOleException(E_NOTIMPL);
  return (FunctionArgSingleDOFType)0;
}

FunctionArgVariableType CaFunction::get_ArgumentVariable()
{
  // TODO!

  // Currently not supported!
  AfxThrowOleException(E_NOTIMPL);
  return (FunctionArgVariableType)0;
}

FunctionUsageType CaFunction::get_Usage()
{
  return m_nUsage;
}

BSTR CaFunction::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaFunction::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

long CaFunction::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

ISubAssembly* CaFunction::get_Parent()
{
  CA_CHECK(m_ptr);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_ptr->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

static FmEngine* getEngine(FmModelMemberBase* ptr)
{
  FmEngine* engine = dynamic_cast<FmEngine*>(ptr);
  if (engine) return engine;

  if (ptr) ptr->erase();

  engine = new FmEngine();
  engine->connect();
  return engine;
}

long CaFunction::get_ChannelID()
{
  CA_CHECK(m_ptr);

  FmEngine* engine = getEngine(m_ptr);
  FmfExternalFunction* extFunc = dynamic_cast<FmfExternalFunction*>(engine->getFunction());
  return extFunc ? extFunc->channel.getValue() : -1;
}

void CaFunction::SetConstant(double Value)
{
  FmEngine* engine = getEngine(m_ptr);
  FmfConstant* constFunc = dynamic_cast<FmfConstant*>(engine->getFunction());

  if (!constFunc) {
    if (engine->getFunction() != NULL)
      engine->getFunction()->erase();
    constFunc = new FmfConstant();
  }

  constFunc->setFunctionUse(FmMathFuncBase::GENERAL);
  constFunc->setConstant(Value);
  constFunc->connect();

  engine->setFunction(constFunc);
  m_ptr = engine;

  m_ptr->onChanged();
}

void CaFunction::SetLinear(double Slope)
{
  FmEngine* engine = getEngine(m_ptr);
  FmfScale* linFunc = dynamic_cast<FmfScale*>(engine->getFunction());

  if (!linFunc) {
    if (engine->getFunction() != NULL)
      engine->getFunction()->erase();
    linFunc = new FmfScale();
  }

  linFunc->setFunctionUse(FmMathFuncBase::GENERAL);
  linFunc->setScale(Slope);
  linFunc->connect();

  engine->setFunction(linFunc);
  m_ptr = engine;

  m_ptr->onChanged();
}

void CaFunction::SetSin(double Period, double MeanValue, double Amplitude, const VARIANT FAR& MaxTime)
{
  // Get max time
  double fMaxTime = 0.0;
  if (MaxTime.vt != VT_EMPTY && MaxTime.vt != VT_ERROR) {
    // Convert to R8
    VARIANT varR8;
    VariantInit(&varR8);
    HRESULT hr = VariantChangeType(&varR8, &MaxTime, 0, VT_R8);
    if (FAILED(hr))
      AfxThrowOleException(E_INVALIDARG);
    fMaxTime = varR8.dblVal;
  }

  // Check if function is used as wave function. If so, no engine is needed
  FmEngine* engine = NULL;
  FmfSinusoidal* sinFunc = NULL;
  FmMathFuncBase* pFunc = dynamic_cast<FmMathFuncBase*>(m_ptr);
  if (pFunc && pFunc->getFunctionUse() == FmMathFuncBase::WAVE_FUNCTION) {
    pFunc->erase();
    sinFunc = new FmfSinusoidal();
    sinFunc->setFunctionUse(FmMathFuncBase::WAVE_FUNCTION);
  }
  else {
    engine = getEngine(m_ptr);
    sinFunc = dynamic_cast<FmfSinusoidal*>(engine->getFunction());

    if (!sinFunc) {
      if (engine->getFunction() != NULL)
        engine->getFunction()->erase();
      sinFunc = new FmfSinusoidal();
    }
    sinFunc->setFunctionUse(FmMathFuncBase::GENERAL);
  }

  sinFunc->setFrequency(1.0/Period);
  sinFunc->setAmplitudeDisplacement(MeanValue);
  sinFunc->setAmplitude(Amplitude);
  sinFunc->setMaxTime(fMaxTime);
  sinFunc->connect();

  if (engine) {
    engine->setFunction(sinFunc);
    m_ptr = engine;
  }
  else
    m_ptr = sinFunc;

  m_ptr->onChanged();
}

void CaFunction::SetCombinedSin(double Period1, double Amplitude1, double Period2,
  double Amplitude2, double MeanValue, const VARIANT FAR& MaxTime)
{
  // Get max time
  double fMaxTime = 0.0;
  if (MaxTime.vt != VT_EMPTY && MaxTime.vt != VT_ERROR) {
    // Convert to R8
    VARIANT varR8;
    VariantInit(&varR8);
    HRESULT hr = VariantChangeType(&varR8, &MaxTime, 0, VT_R8);
    if (FAILED(hr))
      AfxThrowOleException(E_INVALIDARG);
    fMaxTime = varR8.dblVal;
  }

  FmEngine* engine = getEngine(m_ptr);
  FmfComplSinus* complSinFunc = dynamic_cast<FmfComplSinus*>(engine->getFunction());

  if (!complSinFunc) {
    // Cleanup
    if (engine->getFunction() != NULL)
      engine->getFunction()->erase();
    // Create
    complSinFunc = new FmfComplSinus();
  }

  complSinFunc->setFunctionUse(FmMathFuncBase::FuncUse::GENERAL);
  complSinFunc->setFreqForWave1(1.0/Period1);
  complSinFunc->setFreqForWave2(1.0/Period2);
  complSinFunc->setAmplitudeWave1(Amplitude1);
  complSinFunc->setAmplitudeWave2(Amplitude2);
  complSinFunc->setAmplitudeDisplacement(MeanValue);
  complSinFunc->setMaxTime(fMaxTime);
  complSinFunc->connect();

  engine->setFunction(complSinFunc);
  m_ptr = engine;

  m_ptr->onChanged();
}

void CaFunction::SetPolyline(const VARIANT FAR& ArrayNx2, FunctionExtrapolationType Extrapolation)
{
  // Get safearray
  int nDims = 0, N = 0, M = 0;
  CaApplication::GetFromSafeArrayDims(ArrayNx2, &nDims, &N, &M);
  if (nDims != 2 || N < 1 || M != 2)
    AfxThrowOleException(E_INVALIDARG);
  double* pData = (double*)malloc(N * M * sizeof(double));
  if (!pData)
    AfxThrowOleException(E_INVALIDARG);
  CaApplication::GetFromSafeArray(ArrayNx2, N, M, pData);

  FmEngine* engine = dynamic_cast<FmEngine*>(m_ptr);
  FmModelMemberBase* func = engine ? engine->getFunction() : m_ptr;
  FmfLinVar* plFunc = dynamic_cast<FmfLinVar*>(func);
  if (!plFunc) {
    if (func) func->erase();
    func = plFunc = new FmfLinVar();
    plFunc->setFunctionUse(FmMathFuncBase::FuncUse::GENERAL, true);
    plFunc->connect();

    if (!engine) {
      engine = new FmEngine();
      engine->connect();
    }
    engine->setFunction(plFunc);
  }

  // Set data
  plFunc->setExtrapolationType(Extrapolation);
  for (int i = 0; i < N; i++)
    plFunc->addXYset(pData[0 + i*M], pData[1 + i*M]);
  free(pData);

  m_ptr = engine ? engine : func;

  m_ptr->onChanged();
}

void CaFunction::SetPolylineFromFile(LPCTSTR FileName, const VARIANT FAR& ChannelName,
  const VARIANT FAR& ScaleFactor, const VARIANT FAR& ZeroAdjust, const VARIANT FAR& VerticalShift)
{
  // Get channel name
  std::string strChannelName = "Not set";
  if (ChannelName.vt == VT_BSTR)
    strChannelName = CW2A(ChannelName.bstrVal);

  // Get scale factor
  double fScaleFactor = 1.0;
  if (ScaleFactor.vt != VT_EMPTY && ScaleFactor.vt != VT_ERROR) {
    // Convert to R8
    VARIANT varR8;
    VariantInit(&varR8);
    HRESULT hr = VariantChangeType(&varR8, &ScaleFactor, 0, VT_R8);
    if (FAILED(hr))
      AfxThrowOleException(E_INVALIDARG);
    fScaleFactor = varR8.dblVal;
  }

  // Get zero adjust
  bool bZeroAdjust = false;
  if (ZeroAdjust.vt != VT_EMPTY && ZeroAdjust.vt != VT_ERROR) {
    // Convert to bool
    VARIANT varBool;
    VariantInit(&varBool);
    HRESULT hr = VariantChangeType(&varBool, &ZeroAdjust, 0, VT_BOOL);
    if (FAILED(hr))
      AfxThrowOleException(E_INVALIDARG);
    bZeroAdjust = varBool.boolVal;
  }

  // Get vertical shift
  double fVerticalShift = 0.0;
  if (VerticalShift.vt != VT_EMPTY && VerticalShift.vt != VT_ERROR) {
    // Convert to R8
    VARIANT varR8;
    VariantInit(&varR8);
    HRESULT hr = VariantChangeType(&varR8, &VerticalShift, 0, VT_R8);
    if (FAILED(hr))
      AfxThrowOleException(E_INVALIDARG);
    fVerticalShift = varR8.dblVal;
  }

  FmEngine* engine = dynamic_cast<FmEngine*>(m_ptr);
  FmModelMemberBase* func = engine ? engine->getFunction() : m_ptr;
  FmfDeviceFunction* plFunc = dynamic_cast<FmfDeviceFunction*>(func);
  if (!plFunc) {
    std::string descr;
    if (func)
    {
      descr = func->getUserDescription();
      func->erase();
    }

    func = plFunc = new FmfDeviceFunction();
    plFunc->setFunctionUse((FmMathFuncBase::FuncUse)m_nUsage, true);
    plFunc->setUserDescription(descr);
    plFunc->connect();

    // If function type is neither spring nor damper-characteristic
    // (which does not have engines), create an engine if none exists
    if (!engine && ((FmMathFuncBase::FuncUse)m_nUsage < FmMathFuncBase::FuncUse::SPR_TRA_STIFF ||
                    (FmMathFuncBase::FuncUse)m_nUsage > FmMathFuncBase::FuncUse::DA_ROT_TORQUE)) {
      engine = new FmEngine();
      engine->connect();
    }

    if (engine)
      engine->setFunction(plFunc);
  }

  // Set data
  plFunc->setDevice(CaConvert(FileName),strChannelName);
  plFunc->scaleFactor.setValue(fScaleFactor);
  plFunc->verticalShift.setValue(fVerticalShift);
  plFunc->zeroAdjust.setValue(bZeroAdjust);

  m_ptr = engine ? engine : func;

  m_ptr->onChanged();
}

void CaFunction::SetMathExpression(LPCTSTR Expression)
{
  FmEngine* engine = getEngine(m_ptr);
  FmfMathExpr* mathFunc = dynamic_cast<FmfMathExpr*>(engine->getFunction());

  if (!mathFunc) {
    if (engine->getFunction() != NULL)
      engine->getFunction()->erase();
    mathFunc = new FmfMathExpr();
  }

  mathFunc->setFunctionUse(FmMathFuncBase::FuncUse::GENERAL);
  mathFunc->setExpressionString(CaConvert(Expression));
  mathFunc->connect();

  engine->setFunction(mathFunc);
  m_ptr = engine;

  m_ptr->onChanged();
}

void CaFunction::SetExternal()
{
  FmEngine* engine = getEngine(m_ptr);
  FmfExternalFunction* extFunc = dynamic_cast<FmfExternalFunction*>(engine->getFunction());
  if (extFunc) return;

  if (engine->getFunction() != NULL)
    engine->getFunction()->erase();

  extFunc = new FmfExternalFunction();
  extFunc->setFunctionUse(FmMathFuncBase::FuncUse::GENERAL);
  extFunc->connect();

  engine->setFunction(extFunc);
  m_ptr = engine;

  m_ptr->onChanged();
}

void CaFunction::SetJonswap(double Hs, double Tp, const VARIANT FAR& Gamma, const VARIANT FAR& N,
  const VARIANT FAR& RndSeed, const VARIANT FAR& TRangeMin, const VARIANT FAR& TRangeMax)
{
  // Create or reuse
  FmfWaveSpectrum* pFunc = dynamic_cast<FmfWaveSpectrum*>(m_ptr);
  if (pFunc == NULL) {
    if (m_ptr != NULL)
      m_ptr->erase();
    pFunc = new FmfWaveSpectrum();
    pFunc->setFunctionUse(FmMathFuncBase::WAVE_FUNCTION);
    pFunc->spectrum.setValue(FmfWaveSpectrum::JONSWAP);
    pFunc->connect();
  }

  // Set Hs and Tp
  pFunc->myHs.setValue(Hs);
  pFunc->myTp.setValue(Tp);

  // Set N
  VARIANT varI4;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &N, 0, VT_I4);
  if (varI4.vt == VT_I4 && N.vt != VT_EMPTY)
    pFunc->nComp.setValue(varI4.lVal);

  // Set RndSeed
  VariantInit(&varI4);
  VariantChangeType(&varI4, &RndSeed, 0, VT_I4);
  if (varI4.vt == VT_I4 && RndSeed.vt != VT_EMPTY)
    pFunc->myRandomSeed.setValue(varI4.lVal);

  // Set gamma
  VARIANT varR8;
  VariantInit(&varR8);
  VariantChangeType(&varR8, &Gamma, 0, VT_R8);
  if (varR8.vt == VT_R8 && Gamma.vt != VT_EMPTY) {
    // Check the specified value
    if (varR8.dblVal <= 0.0 || varR8.dblVal > 30.0)
      AfxThrowOleException(E_INVALIDARG);
    // Use the specified value
    pFunc->autoCalcPeakedness.setValue(false);
    pFunc->myPeakedness.setValue(varR8.dblVal);
  }
  else {
    // Auto-calculate using DNV recommendation
    pFunc->autoCalcPeakedness.setValue(true);
    // Note: Must auto-calculate the spectral peakedness before the T-range,
    // because the latter also depends on the former
    pFunc->deriveSpectralPeakedness();
  }

  // Set TRangeMin and TRangeMax
  VARIANT varR8min;
  VariantInit(&varR8);
  VariantInit(&varR8min);
  VariantChangeType(&varR8, &TRangeMax, 0, VT_R8);
  VariantChangeType(&varR8min, &TRangeMin, 0, VT_R8);
  if (TRangeMax.vt == VT_EMPTY) varR8.vt = VT_EMPTY;
  if (TRangeMin.vt == VT_EMPTY) varR8min.vt = VT_EMPTY;
  if (varR8.vt == VT_R8 && varR8min.vt == VT_R8) {
    // Use the specified value
    pFunc->autoCalcTrange.setValue(false);
    pFunc->myTrange.setValue(FmRange(varR8min.dblVal,varR8.dblVal));
  }
  else if (varR8.vt == VT_EMPTY && varR8min.vt == VT_EMPTY) {
    // Auto-calculate
    pFunc->autoCalcTrange.setValue(true);
    pFunc->deriveTrange();
  }
  else
    AfxThrowOleException(E_INVALIDARG);

  m_ptr = pFunc;
  m_nUsage = fuWaveFunction;

  m_ptr->onChanged();
}

void CaFunction::SetArgumentSingle(FunctionArgSingleDOFType DOF, FunctionArgVariableType Val,
                                   const VARIANT FAR& ArgObject1, const VARIANT FAR& ArgNumber)
{
  //TODO. the function now only accepts baseid's, make it accept objects

  //Check if current function is Engine. Only engines accept arguments
  FmEngine* pEngine = dynamic_cast<FmEngine*>(m_ptr);
  if (!pEngine) return;

  // Get ArgObject1 parameter as a number
  VARIANT varI4;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgObject1, 0, VT_I4);
  if (varI4.vt != VT_I4)
    AfxThrowOleException(E_INVALIDARG);
  long nArgObject1 = varI4.lVal;

  // Get ArgNumber
  long nArgNumber = 0;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgNumber, 0, VT_I4);
  if (varI4.vt == VT_I4 && ArgNumber.vt != VT_EMPTY)
    nArgNumber = varI4.lVal;

  FmModelMemberBase* pFmObject = FmDB::findObject(nArgObject1);
  FmIsMeasuredBase* pMeasured = dynamic_cast<FmIsMeasuredBase*>(pFmObject);
  if (!pMeasured) return;

  //Check if engine already has sensor that is not used by other functions
  FmSimpleSensor* pSensor = dynamic_cast<FmSimpleSensor*>(pEngine->getSensor(nArgNumber));
  if (pSensor) {
    std::vector<FmEngine*> referringEngines;
    pSensor->getEngines(referringEngines);
    if (referringEngines.size() > 1) pSensor = NULL;
  }

  //If no sensor was found in the engine object, create a new one
  if (!pSensor) {
    pSensor = new FmSimpleSensor();
    pSensor->setParentAssembly(pMeasured->getCommonAncestor(pMeasured));
    pSensor->setUserDescription("Sensor on " + pMeasured->getIdString());
    ListUI << "Creating " << pSensor->getUserDescription() << ".\n";
  }
  else
    //Just update description
    pSensor->setUserDescription("Sensor on " + pMeasured->getIdString());

  if (nArgNumber >= pEngine->getFunction()->getNoArgs() && dynamic_cast<FmfMathExpr*>(pEngine->getFunction()))
    pEngine->getFunction()->setNoArgs(nArgNumber + 1);

  pSensor->connect();
  pSensor->setMeasured(pMeasured);
  pEngine->setSensor(pSensor, nArgNumber);
  pEngine->onChanged();
  pSensor->onChanged();
  pSensor->draw();

  switch(DOF)
  {
  case FunctionArgSingleDOFType::faTX:
    pEngine->setDof(FmIsMeasuredBase::X_TRANS, nArgNumber);
    break;
  case FunctionArgSingleDOFType::faTY:
    pEngine->setDof(FmIsMeasuredBase::Y_TRANS, nArgNumber);
    break;
  case FunctionArgSingleDOFType::faTZ:
    pEngine->setDof(FmIsMeasuredBase::Z_TRANS, nArgNumber);
    break;
  case FunctionArgSingleDOFType::faRX:
    pEngine->setDof(FmIsMeasuredBase::X_ROT, nArgNumber);
    break;
  case FunctionArgSingleDOFType::faRY:
    pEngine->setDof(FmIsMeasuredBase::Y_ROT, nArgNumber);
    break;
  case FunctionArgSingleDOFType::faRZ:
    pEngine->setDof(FmIsMeasuredBase::Z_ROT, nArgNumber);
    break;
  }

  switch(Val)
  {
  case FunctionArgVariableType::faTriadPos:
    pEngine->setEntity(FmIsMeasuredBase::POS, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadLocalVel:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadGlobalVel:
    pEngine->setEntity(FmIsMeasuredBase::GLOBAL_VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadLocalAccel:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_ACC, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadGlobalAccel:
    pEngine->setEntity(FmIsMeasuredBase::GLOBAL_ACC, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadLocalForce:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadGlobalForce:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faDistance:
    pEngine->setEntity(FmIsMeasuredBase::DISTANCE, nArgNumber);
    break;
  case FunctionArgVariableType::faVelocity:
    pEngine->setEntity(FmIsMeasuredBase::VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faAcceleration:
    pEngine->setEntity(FmIsMeasuredBase::ACCEL, nArgNumber);
    break;
  case FunctionArgVariableType::faRelativePos:
    pEngine->setEntity(FmIsMeasuredBase::REL_POS, nArgNumber);
    break;
  case FunctionArgVariableType::faJointSpringAngle:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_ANG, nArgNumber);
    break;
  case FunctionArgVariableType::faJointSpringDefl:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_DEFL, nArgNumber);
    break;
  case FunctionArgVariableType::faJointSpringForce:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faJointDampAngle:
    pEngine->setEntity(FmIsMeasuredBase::JDAMP_ANG, nArgNumber);
    break;
  case FunctionArgVariableType::faJointDampVel:
    pEngine->setEntity(FmIsMeasuredBase::JDAMP_VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faJointDampForce:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faSpringDamperLength:
    pEngine->setEntity(FmIsMeasuredBase::LENGTH, nArgNumber);
    break;
  case FunctionArgVariableType::faSpringDefl:
    pEngine->setEntity(FmIsMeasuredBase::DEFL, nArgNumber);
    break;
  case FunctionArgVariableType::faSpringDamperForce:
    pEngine->setEntity(FmIsMeasuredBase::FORCE, nArgNumber);
    break;
  }
}


void CaFunction::SetArgumentRelative(FunctionArgRelativeDOFType DOF, FunctionArgVariableType Val,
                                     const VARIANT FAR& ArgObject1, const VARIANT FAR& ArgObject2,
                                     const VARIANT FAR& ArgNumber)
{
  //TODO. the function now only accepts baseid's, make it accept objects

  //Check if current function is Engine. Only engines accept arguments
  FmEngine* pEngine = dynamic_cast<FmEngine*>(m_ptr);
  if (!pEngine) return;

  // Get ArgObject1 parameter as a number
  VARIANT varI4;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgObject1, 0, VT_I4);
  if (varI4.vt != VT_I4)
    AfxThrowOleException(E_INVALIDARG);
  long nArgObject1 = varI4.lVal;

  // Get ArgNumber
  long nArgNumber = 0;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgNumber, 0, VT_I4);
  if (varI4.vt == VT_I4 && ArgNumber.vt != VT_EMPTY)
    nArgNumber = varI4.lVal;

  // Get ArgObject2 parameter as a number
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgObject2, 0, VT_I4);
  if (varI4.vt != VT_I4)
    AfxThrowOleException(E_INVALIDARG);
  long nArgObject2 = varI4.lVal;

  //Get class id for both argument-object
  FmModelMemberBase* pFmObject1 = FmDB::findObject(nArgObject1);
  FmModelMemberBase* pFmObject2 = FmDB::findObject(nArgObject2);

  FmTriad* pTriad1 = dynamic_cast<FmTriad*>(pFmObject1);
  FmTriad* pTriad2 = dynamic_cast<FmTriad*>(pFmObject2);

  //Relative sensor only accepts triads
  if (!pTriad1 || !pTriad2) return;

  //Relative sensor only works between different objects
  if (pTriad1 == pTriad2) {
    ListUI <<"ERROR: Relative sensors should be used on different objects.\n";
    ListUI <<"       Could not create relative sensor.\n";
    return;
  }

  //Create sensor between triad1 and triad2, and set it as argument on engine

  //Check if engine already has relative-sensor that is not used by other functions
  FmRelativeSensor* pSensor = dynamic_cast<FmRelativeSensor*>(pEngine->getSensor(nArgNumber));
  if (pSensor) {
    std::vector<FmEngine*> referringEngines;
    pSensor->getEngines(referringEngines);
    if (referringEngines.size() > 1) pSensor = NULL;
  }

  //If no relative sensor was found in the engine object, create a new one
  if (!pSensor) {
    pSensor = new FmRelativeSensor();
    pSensor->setParentAssembly(pTriad1->getCommonAncestor(pTriad2));
    pSensor->setUserDescription("Relative sensor between " + pTriad1->getIdString() + " and " + pTriad2->getIdString());
    ListUI << "Creating " << pSensor->getUserDescription() << ".\n";
  }
  else
    //Just update description
    pSensor->setUserDescription("Relative sensor between " + pTriad1->getIdString() + " and " + pTriad2->getIdString());

  if (nArgNumber >= pEngine->getFunction()->getNoArgs() && dynamic_cast<FmfMathExpr*>(pEngine->getFunction()))
    pEngine->getFunction()->setNoArgs(nArgNumber+1);

  pSensor->connect(pTriad1,pTriad2);
  pEngine->setSensor(pSensor, nArgNumber);
  pEngine->onChanged();
  pSensor->onChanged();
  pSensor->draw();

  switch(DOF)
  {
  case FunctionArgRelativeDOFType::faDX:
    pEngine->setDof(FmIsMeasuredBase::REL_X, nArgNumber);
    break;
  case FunctionArgRelativeDOFType::faDY:
    pEngine->setDof(FmIsMeasuredBase::REL_Y, nArgNumber);
    break;
  case FunctionArgRelativeDOFType::faDZ:
    pEngine->setDof(FmIsMeasuredBase::REL_Z, nArgNumber);
    break;
  case FunctionArgRelativeDOFType::faRelRX:
    pEngine->setDof(FmIsMeasuredBase::REL_RX, nArgNumber);
    break;
  case FunctionArgRelativeDOFType::faRelRY:
    pEngine->setDof(FmIsMeasuredBase::REL_RY, nArgNumber);
    break;
  case FunctionArgRelativeDOFType::faRelRZ:
    pEngine->setDof(FmIsMeasuredBase::REL_RZ, nArgNumber);
    break;
  case FunctionArgRelativeDOFType::faLength:
    pEngine->setDof(FmIsMeasuredBase::LENGTH, nArgNumber);
    break;
  }

  switch (Val)
  {
  case FunctionArgVariableType::faTriadPos:
    pEngine->setEntity(FmIsMeasuredBase::POS, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadLocalVel:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadGlobalVel:
    pEngine->setEntity(FmIsMeasuredBase::GLOBAL_VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadLocalAccel:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_ACC, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadGlobalAccel:
    pEngine->setEntity(FmIsMeasuredBase::GLOBAL_ACC, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadLocalForce:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faTriadGlobalForce:
    pEngine->setEntity(FmIsMeasuredBase::LOCAL_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faDistance:
    pEngine->setEntity(FmIsMeasuredBase::DISTANCE, nArgNumber);
    break;
  case FunctionArgVariableType::faVelocity:
    pEngine->setEntity(FmIsMeasuredBase::VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faAcceleration:
    pEngine->setEntity(FmIsMeasuredBase::ACCEL, nArgNumber);
    break;
  case FunctionArgVariableType::faRelativePos:
    pEngine->setEntity(FmIsMeasuredBase::REL_POS, nArgNumber);
    break;
  case FunctionArgVariableType::faJointSpringAngle:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_ANG, nArgNumber);
    break;
  case FunctionArgVariableType::faJointSpringDefl:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_DEFL, nArgNumber);
    break;
  case FunctionArgVariableType::faJointSpringForce:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faJointDampAngle:
    pEngine->setEntity(FmIsMeasuredBase::JDAMP_ANG, nArgNumber);
    break;
  case FunctionArgVariableType::faJointDampVel:
    pEngine->setEntity(FmIsMeasuredBase::JDAMP_VEL, nArgNumber);
    break;
  case FunctionArgVariableType::faJointDampForce:
    pEngine->setEntity(FmIsMeasuredBase::JSPR_FORCE, nArgNumber);
    break;
  case FunctionArgVariableType::faSpringDamperLength:
    pEngine->setEntity(FmIsMeasuredBase::LENGTH, nArgNumber);
    break;
  case FunctionArgVariableType::faSpringDefl:
    pEngine->setEntity(FmIsMeasuredBase::DEFL, nArgNumber);
    break;
  case FunctionArgVariableType::faSpringDamperForce:
    pEngine->setEntity(FmIsMeasuredBase::FORCE, nArgNumber);
    break;
  }
}


void CaFunction::SetArgumentScalar(const VARIANT FAR& ArgObject1, const VARIANT FAR& ArgNumber)
{
  //TODO. the function now only accepts baseid's, make it accept objects
  //Check if current function is Engine. Only engines accept arguments
  FmEngine* pEngine = dynamic_cast<FmEngine*>(m_ptr);
  if (!pEngine) return;

  // Get ArgObject1 parameter as a number
  VARIANT varI4;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgObject1, 0, VT_I4);
  if (varI4.vt != VT_I4)
    AfxThrowOleException(E_INVALIDARG);
  long nArgObject1 = varI4.lVal;

  // Get ArgNumber
  long nArgNumber = 0;
  VariantInit(&varI4);
  VariantChangeType(&varI4, &ArgNumber, 0, VT_I4);
  if (varI4.vt == VT_I4 && ArgNumber.vt != VT_EMPTY)
    nArgNumber = varI4.lVal;

  FmModelMemberBase* pFmObject1 = FmDB::findObject(nArgObject1);

  //Check if object is engine or control system output
  if (!(dynamic_cast<FmEngine*>(pFmObject1) || dynamic_cast<FmcOutput*>(pFmObject1)))
    return;

  //Check if engine already has sensor, if it's the only user, re-use it
  FmSimpleSensor* pSensor = dynamic_cast<FmSimpleSensor*>(pEngine->getSensor(nArgNumber));
  if (pSensor) {
    std::vector<FmEngine*> referringEngines;
    pSensor->getEngines(referringEngines);
    if (referringEngines.size() > 1) pSensor = NULL;
  }

  if (!pSensor) {
    pSensor = new FmSimpleSensor();
    pSensor->setParentAssembly(pFmObject1->getParentAssembly());
    pSensor->setUserDescription("Sensor on " + pFmObject1->getIdString());
    ListUI << "Creating " << pSensor->getUserDescription() << ".\n";
  }
  else
    //Just update description
    pSensor->setUserDescription("Sensor on " + pFmObject1->getIdString());

  FmIsMeasuredBase* pMeasured = dynamic_cast<FmIsMeasuredBase*>(pFmObject1);

  if (nArgNumber >= pEngine->getFunction()->getNoArgs() && dynamic_cast<FmfMathExpr*>(pEngine->getFunction()))
    pEngine->getFunction()->setNoArgs(nArgNumber+1);

  pSensor->connect();
  pSensor->setMeasured(pMeasured);
  pEngine->setSensor(pSensor, nArgNumber);
  pEngine->onChanged();
  pSensor->onChanged();
  pSensor->draw();
}


void CaFunction::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}



//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaFunction::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaFunction::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaFunction::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaFunction::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaFunction::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaFunction::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaFunction::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaFunction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->Invoke(dispidMember, riid, lcid,
          wFlags, pdispparams, pvarResult,
          pexcepinfo, puArgErr);
}


// Note: The following wrapper code will only be called by TLB
// interface calls, and not by IDispatch calls. So, you should
// only forward this call to the code in the methods part here
// (see further up in this file).

STDMETHODIMP CaFunction::XLocalClass::get_FunctionType(FunctionType* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_FunctionType();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_Parameters(BSTR* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_Parameters();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::put_Parameters(BSTR val)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->put_Parameters(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_ArgumentObject(VARIANT* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_ArgumentObject();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_ArgumentDOF(FunctionArgSingleDOFType* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_ArgumentDOF();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_ArgumentVariable(FunctionArgVariableType* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_ArgumentVariable();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_Usage(FunctionUsageType* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_Usage();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_Description();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->put_Description(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_BaseID();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *ppObj = pThis->get_Parent();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::get_ChannelID(long* pVal)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    *pVal = pThis->get_ChannelID();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetConstant(double Value)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetConstant(Value);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetLinear(double Slope)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetLinear(Slope);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetSin(double Period, double MeanValue, double Amplitude, VARIANT MaxTime)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetSin(Period, MeanValue, Amplitude, MaxTime);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetCombinedSin(double Period1, double Amplitude1, double Period2,
  double Amplitude2, double MeanValue, VARIANT MaxTime)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetCombinedSin(Period1, Amplitude1, Period2, Amplitude2, MeanValue, MaxTime);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetPolyline(VARIANT ArrayNx2, FunctionExtrapolationType Extrapolation)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetPolyline(ArrayNx2, Extrapolation);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetPolylineFromFile(BSTR FileName, VARIANT ChannelName, VARIANT ScaleFactor,
  VARIANT ZeroAdjust, VARIANT VerticalShift)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetPolylineFromFile(FileName, ChannelName, ScaleFactor, ZeroAdjust, VerticalShift);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetMathExpression(BSTR Expression)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetMathExpression(Expression);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetExternal()
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetExternal();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetJonswap(double Hs, double Tp, VARIANT Gamma, VARIANT N,
  VARIANT RndSeed, VARIANT TRangeMin, VARIANT TRangeMax)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetJonswap(Hs, Tp, Gamma, N, RndSeed, TRangeMin, TRangeMax);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetArgumentSingle(FunctionArgSingleDOFType DOF, FunctionArgVariableType Val,
  VARIANT ArgObject1, VARIANT ArgNumber)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetArgumentSingle(DOF, Val, ArgObject1, ArgNumber);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetArgumentRelative(FunctionArgRelativeDOFType DOF, FunctionArgVariableType Val,
  VARIANT ArgObject1, VARIANT ArgObject2, VARIANT ArgNumber)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetArgumentRelative(DOF, Val, ArgObject1, ArgObject2, ArgNumber);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::SetArgumentScalar(VARIANT ArgObject1, VARIANT ArgNumber)
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->SetArgumentScalar(ArgObject1, ArgNumber);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFunction::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaFunction, LocalClass);
  TRY
  {
    pThis->Delete();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}
