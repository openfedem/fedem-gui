// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaSeaEnvironment.h"
#include "CaFunction.h"

#include "vpmDB/FmSeaState.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmDB.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaSeaEnvironment, CCmdTarget)

BEGIN_MESSAGE_MAP(CaSeaEnvironment, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaSeaEnvironment, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "WaterDensity", dispidWaterDensity, get_WaterDensity, put_WaterDensity, VT_R8)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "MeanSeaLevel", dispidMeanSeaLevel, get_MeanSeaLevel, put_MeanSeaLevel, VT_R8)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "WaterDepth", dispidWaterDepth, get_WaterDepth, put_WaterDepth, VT_R8)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "MarineGrowthDensity", dispidMarineGrowthDensity, get_MarineGrowthDensity, put_MarineGrowthDensity, VT_R8)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "MarineGrowthThickness", dispidMarineGrowthThickness, get_MarineGrowthThickness, put_MarineGrowthThickness, VT_R8)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "MarineGrowthUpperLimit", dispidMarineGrowthUpperLimit, get_MarineGrowthUpperLimit, put_MarineGrowthUpperLimit, VT_R8)
  DISP_PROPERTY_EX_ID(CaSeaEnvironment, "MarineGrowthLowerLimit", dispidMarineGrowthLowerLimit, get_MarineGrowthLowerLimit, put_MarineGrowthLowerLimit, VT_R8)
  DISP_FUNCTION_ID(CaSeaEnvironment, "GetGravitation", dispidGetGravitation, GetGravitation, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaSeaEnvironment, "SetGravitation", dispidSetGravitation, SetGravitation, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaSeaEnvironment, "GetWaveDirection", dispidGetWaveDirection, GetWaveDirection, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaSeaEnvironment, "SetWaveDirection", dispidSetWaveDirection, SetWaveDirection, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaSeaEnvironment, "GetWaveFunction", dispidGetWaveFunction, GetWaveFunction, VT_DISPATCH, VTS_NONE)
  DISP_FUNCTION_ID(CaSeaEnvironment, "SetWaveFunction", dispidSetWaveFunction, SetWaveFunction, VT_EMPTY, VTS_DISPATCH)
  DISP_FUNCTION_ID(CaSeaEnvironment, "GetCurrentFunction", dispidGetCurrentFunction, GetCurrentFunction, VT_DISPATCH, VTS_NONE)
  DISP_FUNCTION_ID(CaSeaEnvironment, "SetCurrentFunction", dispidSetCurrentFunction, SetCurrentFunction, VT_EMPTY, VTS_DISPATCH)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaSeaEnvironment, CCmdTarget)
  INTERFACE_PART(CaSeaEnvironment, IID_ISeaEnvironment, LocalClass)
END_INTERFACE_MAP()

// {D3678CDB-4998-4745-8F2B-C2A9FD8E8DB5}
IMPLEMENT_OLECREATE(CaSeaEnvironment, "FEDEM.SeaEnvironment",
0xd3678cdb, 0x4998, 0x4745, 0x8f, 0x2b, 0xc2, 0xa9, 0xfd, 0x8e, 0x8d, 0xb5);


CaSeaEnvironment::CaSeaEnvironment(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
}

CaSeaEnvironment::~CaSeaEnvironment(void)
{
  ::AfxOleUnlockApp();
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaSeaEnvironment::get_WaterDensity()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->waterDensity.getValue() : 0.0;
}

void CaSeaEnvironment::put_WaterDensity(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->waterDensity.setValue(Val);
}

double CaSeaEnvironment::get_MeanSeaLevel()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->meanSeaLevel.getValue() : 0.0;
}

void CaSeaEnvironment::put_MeanSeaLevel(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->setMeanSeaLevel(Val);
}

double CaSeaEnvironment::get_WaterDepth()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->seaDepth.getValue() : 0.0;
}

void CaSeaEnvironment::put_WaterDepth(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->setSeaDepth(Val);
}

double CaSeaEnvironment::get_MarineGrowthDensity()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->growthDensity.getValue() : 0.0;
}

void CaSeaEnvironment::put_MarineGrowthDensity(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->growthDensity.setValue(Val);
}

double CaSeaEnvironment::get_MarineGrowthThickness()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->growthThickness.getValue() : 0.0;
}

void CaSeaEnvironment::put_MarineGrowthThickness(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->growthThickness.setValue(Val);
}

double CaSeaEnvironment::get_MarineGrowthUpperLimit()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->growthLimit.getValue().first : 0.0;
}

void CaSeaEnvironment::put_MarineGrowthUpperLimit(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->growthLimit = { Val, sea->growthLimit.getValue().second };
}

double CaSeaEnvironment::get_MarineGrowthLowerLimit()
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  return sea ? sea->growthLimit.getValue().second : 0.0;
}

void CaSeaEnvironment::put_MarineGrowthLowerLimit(double Val)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->growthLimit = { sea->growthLimit.getValue().first, Val };
}

void CaSeaEnvironment::GetGravitation(double* x, double* y, double* z)
{
  FaVec3 v = FmDB::getMechanismObject()->gravity.getValue();
  *x = v.x();
  *y = v.y();
  *z = v.z();
}

void CaSeaEnvironment::SetGravitation(double x, double y, double z)
{
  FaVec3 v(x,y,z);
  FmDB::getMechanismObject()->setGravity(v);
}

void CaSeaEnvironment::GetWaveDirection(double* x, double* y, double* z)
{
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  if (!sea) return;

  const FaVec3& v = sea->waveDir.getValue();
  *x = v.x();
  *y = v.y();
  *z = v.z();
}

void CaSeaEnvironment::SetWaveDirection(double x, double y, double z)
{
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea) sea->setWaveDir(FaVec3(x,y,z));
}

IFunction* CaSeaEnvironment::GetWaveFunction()
{
  // Get FmFunction
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  if (sea == NULL)
    return NULL;
  FmModelMemberBase* pFmFunction = sea->waveFunction.getPointer();
  if (pFmFunction == NULL)
    return NULL;

  // Create COM wrapper
  CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaFunction->m_ptr = pFmFunction;
  pCaFunction->m_nUsage = fuWaveFunction;

  // Return interface
  IFunction* pIFunction = NULL;
  LPDISPATCH pDisp = pCaFunction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFunction, (void**)&pIFunction);
  pCaFunction->InternalRelease();
  return pIFunction;
}

void CaSeaEnvironment::SetWaveFunction(IFunction* Function)
{
  // Get sea state
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea == NULL)
    return;

  // Empty function
  if (Function == NULL) {
    sea->waveFunction.setPointerToNull();
    return;
  }

  // Get function
  IDispatch* pDisp1 = NULL;
  Function->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaFunction* pCaFunction = dynamic_cast<CaFunction*>(CaFunction::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Check
  if ((pCaFunction->m_ptr == NULL) || (pCaFunction->m_nUsage != fuWaveFunction))
    return;

  // Set
  sea->waveFunction.setRef(pCaFunction->m_ptr);
}

IFunction* CaSeaEnvironment::GetCurrentFunction()
{
  // Get FmFunction
  FmSeaState* sea = FmDB::getSeaStateObject(false);
  if (sea == NULL)
    return NULL;
  FmModelMemberBase* pFmFunction = sea->currFunction.getPointer();
  if (pFmFunction == NULL)
    return NULL;

  // Create COM wrapper
  CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaFunction->m_ptr = pFmFunction;
  pCaFunction->m_nUsage = fuCurrentFunction;

  // Return interface
  IFunction* pIFunction = NULL;
  LPDISPATCH pDisp = pCaFunction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFunction, (void**)&pIFunction);
  pCaFunction->InternalRelease();
  return pIFunction;
}

void CaSeaEnvironment::SetCurrentFunction(IFunction* Function)
{
  // Get sea state
  FmSeaState* sea = FmDB::getSeaStateObject();
  if (sea == NULL)
    return;

  // Empty function
  if (Function == NULL) {
    sea->waveFunction.setPointerToNull();
    return;
  }

  // Get function
  IDispatch* pDisp1 = NULL;
  Function->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaFunction* pCaFunction = dynamic_cast<CaFunction*>(CaFunction::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Check
  if ((pCaFunction->m_ptr == NULL) || (pCaFunction->m_nUsage != fuCurrentFunction))
    return;

  // Set
  sea->currFunction.setRef(pCaFunction->m_ptr);
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaSeaEnvironment::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaSeaEnvironment::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaSeaEnvironment::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaSeaEnvironment::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaSeaEnvironment::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaSeaEnvironment::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaSeaEnvironment::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass)
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

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_WaterDensity(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_WaterDensity();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_WaterDensity(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_WaterDensity(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_MeanSeaLevel(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_MeanSeaLevel();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_MeanSeaLevel(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_MeanSeaLevel(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_WaterDepth(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_WaterDepth();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_WaterDepth(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_WaterDepth(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_MarineGrowthDensity(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_MarineGrowthDensity();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_MarineGrowthDensity(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_MarineGrowthDensity(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_MarineGrowthThickness(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_MarineGrowthThickness();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_MarineGrowthThickness(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_MarineGrowthThickness(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_MarineGrowthUpperLimit(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_MarineGrowthUpperLimit();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_MarineGrowthUpperLimit(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_MarineGrowthUpperLimit(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::get_MarineGrowthLowerLimit(double* pVal)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *pVal = pThis->get_MarineGrowthLowerLimit();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::put_MarineGrowthLowerLimit(double val)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->put_MarineGrowthLowerLimit(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::GetGravitation(double* x, double* y, double* z)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->GetGravitation(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::SetGravitation(double x, double y, double z)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->SetGravitation(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::GetWaveDirection(double* x, double* y, double* z)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->GetWaveDirection(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::SetWaveDirection(double x, double y, double z)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->SetWaveDirection(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::GetWaveFunction(IFunction** ppRet)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *ppRet = pThis->GetWaveFunction();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::SetWaveFunction(IFunction* Function)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->SetWaveFunction(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::GetCurrentFunction(IFunction** ppRet)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    *ppRet = pThis->GetCurrentFunction();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSeaEnvironment::XLocalClass::SetCurrentFunction(IFunction* Function)
{
  METHOD_PROLOGUE(CaSeaEnvironment, LocalClass);
  TRY
  {
    pThis->SetCurrentFunction(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}
