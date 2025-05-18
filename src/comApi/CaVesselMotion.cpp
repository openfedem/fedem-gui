// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaVesselMotion.h"
#include "CaApplication.h"
#include "CaFunction.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmVesselMotion.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaVesselMotion, CCmdTarget)

BEGIN_MESSAGE_MAP(CaVesselMotion, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaVesselMotion, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaVesselMotion, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaVesselMotion, "RAOFilePath", dispidRAOFilePath, get_RAOFilePath, put_RAOFilePath, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaVesselMotion, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaVesselMotion, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaVesselMotion, "WaveDir", dispidWaveDir, get_WaveDir, put_WaveDir, VT_I4)
  DISP_FUNCTION_ID(CaVesselMotion, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaVesselMotion, "GetWaveFunction", dispidGetWaveFunction, GetWaveFunction, VT_DISPATCH, VTS_NONE)
  DISP_FUNCTION_ID(CaVesselMotion, "SetWaveFunction", dispidSetWaveFunction, SetWaveFunction, VT_EMPTY, VTS_DISPATCH)
  DISP_FUNCTION_ID(CaVesselMotion, "GetMotionScaleFunction", dispidGetMotionScaleFunction, GetMotionScaleFunction, VT_DISPATCH, VTS_NONE)
  DISP_FUNCTION_ID(CaVesselMotion, "SetMotionScaleFunction", dispidSetMotionScaleFunction, SetMotionScaleFunction, VT_EMPTY, VTS_DISPATCH)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaVesselMotion, CCmdTarget)
  INTERFACE_PART(CaVesselMotion, IID_IVesselMotion, LocalClass)
END_INTERFACE_MAP()

// {D50B50EF-E74D-47CB-80EF-71D585ED9682}
IMPLEMENT_OLECREATE(CaVesselMotion, "FEDEM.VesselMotion",
0xd50b50ef, 0xe74d, 0x47cb, 0x80, 0xef, 0x71, 0xd5, 0x85, 0xed, 0x96, 0x82);


CaVesselMotion::CaVesselMotion(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  signalConnector.Connect(this);
}

CaVesselMotion::~CaVesselMotion(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaVesselMotion::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaVesselMotion::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

BSTR CaVesselMotion::get_RAOFilePath()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->raoFile.getValue().c_str()));
}

void CaVesselMotion::put_RAOFilePath(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->raoFile.setValue(CaConvert(val));
  m_ptr->onChanged();
}

IFunction* CaVesselMotion::GetWaveFunction()
{
  CA_CHECK(m_ptr);

  // Get FmFunction
  FmModelMemberBase* pFmFunction = m_ptr->waveFunction.getPointer();
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

void CaVesselMotion::SetWaveFunction(IFunction* Function)
{
  CA_CHECK(m_ptr);

  // Empty function
  if (Function == NULL) {
    m_ptr->waveFunction.setPointerToNull();
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

  // Check and set
  if (pCaFunction->m_ptr != NULL && pCaFunction->m_nUsage == fuWaveFunction)
    m_ptr->waveFunction = static_cast<FmMathFuncBase*>(pCaFunction->m_ptr);
}

long CaVesselMotion::get_WaveDir()
{
  CA_CHECK(m_ptr);

  return m_ptr->waveDir.getValue();
}

void CaVesselMotion::put_WaveDir(long val)
{
  CA_CHECK(m_ptr);

  m_ptr->waveDir.setValue(val);
}

IFunction* CaVesselMotion::GetMotionScaleFunction()
{
  CA_CHECK(m_ptr);

  // Get FmFunction
  FmModelMemberBase* pFmFunction = m_ptr->motionScale.getPointer();
  if (pFmFunction == NULL)
    return NULL;

  // Create COM wrapper
  CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaFunction->m_ptr = pFmFunction;
  pCaFunction->m_nUsage = fuGeneral;

  // Return interface
  IFunction* pIFunction = NULL;
  LPDISPATCH pDisp = pCaFunction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFunction, (void**)&pIFunction);
  pCaFunction->InternalRelease();
  return pIFunction;
}

void CaVesselMotion::SetMotionScaleFunction(IFunction* Function)
{
  CA_CHECK(m_ptr);

  // Empty function
  if (Function == NULL) {
    m_ptr->motionScale.setPointerToNull();
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

  // Check and set
  if (pCaFunction->m_ptr != NULL && pCaFunction->m_nUsage == fuGeneral)
    m_ptr->scaleMotions(static_cast<FmEngine*>(pCaFunction->m_ptr));
}

long CaVesselMotion::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

ISubAssembly* CaVesselMotion::get_Parent()
{
  CA_CHECK(m_ptr);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_ptr->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaVesselMotion::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaVesselMotion::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaVesselMotion::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaVesselMotion::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaVesselMotion::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaVesselMotion::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaVesselMotion::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaVesselMotion::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass)
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

STDMETHODIMP CaVesselMotion::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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

STDMETHODIMP CaVesselMotion::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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

STDMETHODIMP CaVesselMotion::XLocalClass::get_RAOFilePath(BSTR* pVal)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
  TRY
  {
    *pVal = pThis->get_RAOFilePath();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaVesselMotion::XLocalClass::put_RAOFilePath(BSTR val)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
  TRY
  {
    pThis->put_RAOFilePath(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaVesselMotion::XLocalClass::GetWaveFunction(IFunction** ppRet)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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

STDMETHODIMP CaVesselMotion::XLocalClass::SetWaveFunction(IFunction* Function)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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

STDMETHODIMP CaVesselMotion::XLocalClass::get_WaveDir(long* pVal)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
  TRY
  {
    *pVal = pThis->get_WaveDir();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaVesselMotion::XLocalClass::put_WaveDir(long val)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
  TRY
  {
    pThis->put_WaveDir(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaVesselMotion::XLocalClass::GetMotionScaleFunction(IFunction** ppRet)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
  TRY
  {
    *ppRet = pThis->GetMotionScaleFunction();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaVesselMotion::XLocalClass::SetMotionScaleFunction(IFunction* Function)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
  TRY
  {
    pThis->SetMotionScaleFunction(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}


STDMETHODIMP CaVesselMotion::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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

STDMETHODIMP CaVesselMotion::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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


STDMETHODIMP CaVesselMotion::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaVesselMotion, LocalClass);
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
