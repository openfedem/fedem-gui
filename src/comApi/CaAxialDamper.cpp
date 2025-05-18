// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaAxialDamper.h"
#include "CaTriad.h"
#include "CaFunction.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmAxialDamper.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaAxialDamper, CCmdTarget)

BEGIN_MESSAGE_MAP(CaAxialDamper, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaAxialDamper, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaAxialDamper, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaAxialDamper, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaAxialDamper, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaAxialDamper, "IsDefDamper", dispidIsDefDamper, get_IsDefDamper, put_IsDefDamper, VT_BOOL)
  DISP_FUNCTION_ID(CaAxialDamper, "GetTriad1", dispidGetTriad1, GetTriad1, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialDamper, "SetTriad1", dispidSetTriad1, SetTriad1, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialDamper, "GetTriad2", dispidGetTriad2, GetTriad2, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialDamper, "SetTriad2", dispidSetTriad2, SetTriad2, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialDamper, "GetDampingScale", dispidGetDampingScale, GetDampingScale, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialDamper, "SetDampingScale", dispidSetDampingScale, SetDampingScale, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialDamper, "GetDampingFunction", dispidGetDampingFunction, GetDampingFunction, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialDamper, "SetDampingFunction", dispidSetDampingFunction, SetDampingFunction, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialDamper, "GetDampingCoefficient", dispidGetDampingCoefficient, GetDampingCoefficient, VT_R8, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialDamper, "SetDampingCoefficient", dispidSetDampingCoefficient, SetDampingCoefficient, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaAxialDamper, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaAxialDamper, CCmdTarget)
  INTERFACE_PART(CaAxialDamper, IID_IAxialDamper, LocalClass)
END_INTERFACE_MAP()

// {ADEA99F0-8085-4d8a-8179-AE9B96277529}
IMPLEMENT_OLECREATE(CaAxialDamper, "FEDEM.AxialDamper",
0xadea99f0, 0x8085, 0x4d8a, 0x81, 0x79, 0xae, 0x9b, 0x96, 0x27, 0x75, 0x29);



CaAxialDamper::CaAxialDamper(void) : m_ptr(m_pAxialDamper)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pAxialDamper = NULL;
  signalConnector.Connect(this);
}

CaAxialDamper::~CaAxialDamper(void)
{
  ::AfxOleUnlockApp();
  m_pAxialDamper = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaAxialDamper::get_Description()
{
  CA_CHECK(m_pAxialDamper);

  return SysAllocString(CA2W(m_pAxialDamper->getUserDescription().c_str()));
}

void CaAxialDamper::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pAxialDamper);

  m_pAxialDamper->setUserDescription(CaConvert(val));
  m_pAxialDamper->onChanged();
}

long CaAxialDamper::get_BaseID()
{
  CA_CHECK(m_pAxialDamper);

  return m_pAxialDamper->getBaseID();
}

ISubAssembly* CaAxialDamper::get_Parent()
{
  CA_CHECK(m_pAxialDamper);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pAxialDamper->getParentAssembly();
  if (pParent == NULL)
    return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

BOOL CaAxialDamper::get_IsDefDamper()
{
  CA_CHECK(m_pAxialDamper);

  return m_pAxialDamper->isDefDamper.getValue();
}

void CaAxialDamper::put_IsDefDamper(BOOL val)
{
  CA_CHECK(m_pAxialDamper);

  m_pAxialDamper->isDefDamper.setValue(val);
}

ITriad* CaAxialDamper::GetTriad1()
{
  CA_CHECK(m_pAxialDamper);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get
  if (m_pAxialDamper->getFirstTriad())
    pCaTriad->m_pTriad = m_pAxialDamper->getFirstTriad();
  else
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaAxialDamper::SetTriad1(ITriad* Triad)
{
  CA_CHECK(m_pAxialDamper);

  // Get triad
  if (Triad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Set triad (cannot be null)
  m_pAxialDamper->setOwnerTriads(
    pCaTriad->m_pTriad,
    m_pAxialDamper->getSecondTriad());

  // Update
  m_pAxialDamper->draw();
}

ITriad* CaAxialDamper::GetTriad2()
{
  CA_CHECK(m_pAxialDamper);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get
  if (m_pAxialDamper->getSecondTriad())
    pCaTriad->m_pTriad = m_pAxialDamper->getSecondTriad();
  else
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaAxialDamper::SetTriad2(ITriad* Triad)
{
  CA_CHECK(m_pAxialDamper);

  // Get triad
  if (Triad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Set triad (cannot be null)
  m_pAxialDamper->setOwnerTriads(
    m_pAxialDamper->getFirstTriad(),
    pCaTriad->m_pTriad);

  // Update
  m_pAxialDamper->draw();
}

IFunction* CaAxialDamper::GetDampingScale()
{
  CA_CHECK(m_pAxialDamper);

  // Get
  FmEngine* pScaleFunc = m_pAxialDamper->getDampEngine();
  if (pScaleFunc == NULL)
    return NULL;

  // Create
  CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set
  pCaFunction->m_ptr = pScaleFunc;

  // Return
  IFunction* pIFunction = NULL;
  LPDISPATCH pDisp = pCaFunction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFunction, (void**)&pIFunction);
  pCaFunction->InternalRelease();
  return pIFunction;
}

void CaAxialDamper::SetDampingScale(IFunction* Function)
{
  CA_CHECK(m_pAxialDamper);

  // Get function
  FmEngine* pScaleFunc = NULL;
  if (Function != NULL) {
    IDispatch* pDisp1 = NULL;
    Function->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(E_INVALIDARG);
    CaFunction* pCaFunction =
      dynamic_cast<CaFunction*>(CaFunction::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCaFunction == NULL)
      AfxThrowOleException(E_INVALIDARG);
    pScaleFunc = dynamic_cast<FmEngine*>(pCaFunction->m_ptr);
    if (pScaleFunc == NULL)
      AfxThrowOleException(E_INVALIDARG);
  }

  // Set
  m_pAxialDamper->setDampEngine(pScaleFunc);
}

IFunction* CaAxialDamper::GetDampingFunction()
{
  CA_CHECK(m_pAxialDamper);

  // Get
  FmMathFuncBase* pFunc = m_pAxialDamper->getFunction();
  if (pFunc == NULL)
    return NULL;

  // Create
  CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set
  pCaFunction->m_ptr = pFunc;

  // Return
  IFunction* pIFunction = NULL;
  LPDISPATCH pDisp = pCaFunction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFunction, (void**)&pIFunction);
  pCaFunction->InternalRelease();
  return pIFunction;
}

void CaAxialDamper::SetDampingFunction(IFunction* Function)
{
  CA_CHECK(m_pAxialDamper);

  // Get function
  FmMathFuncBase* pFunc = NULL;
  if (Function != NULL) {
    IDispatch* pDisp1 = NULL;
    Function->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(E_INVALIDARG);
    CaFunction* pCaFunction =
      dynamic_cast<CaFunction*>(CaFunction::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCaFunction == NULL)
      AfxThrowOleException(E_INVALIDARG);
    pFunc = dynamic_cast<FmMathFuncBase*>(pCaFunction->m_ptr);
    if (pFunc == NULL)
      AfxThrowOleException(E_INVALIDARG);
  }

  // Set
  m_pAxialDamper->setFunction(pFunc);
}

double CaAxialDamper::GetDampingCoefficient()
{
  CA_CHECK(m_pAxialDamper);
  return m_pAxialDamper->getInitDamp();
}

void CaAxialDamper::SetDampingCoefficient(double Value)
{
  CA_CHECK(m_pAxialDamper);
  m_pAxialDamper->setInitDamp(Value);
}

void CaAxialDamper::Delete()
{
  CA_CHECK(m_pAxialDamper);

  m_pAxialDamper->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaAxialDamper::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaAxialDamper::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaAxialDamper::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaAxialDamper::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaAxialDamper::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaAxialDamper::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaAxialDamper::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass)
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

STDMETHODIMP CaAxialDamper::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
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

STDMETHODIMP CaAxialDamper::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
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

STDMETHODIMP CaAxialDamper::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
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

STDMETHODIMP CaAxialDamper::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
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

STDMETHODIMP CaAxialDamper::XLocalClass::get_IsDefDamper(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    *pVal = pThis->get_IsDefDamper();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::put_IsDefDamper(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    pThis->put_IsDefDamper(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::GetTriad1(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    *ppRet = pThis->GetTriad1();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::SetTriad1(ITriad* Triad)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    pThis->SetTriad1(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::GetTriad2(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    *ppRet = pThis->GetTriad2();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::SetTriad2(ITriad* Triad)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    pThis->SetTriad2(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::GetDampingScale(IFunction** ppObj)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    *ppObj = pThis->GetDampingScale();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::SetDampingScale(IFunction* Function)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    pThis->SetDampingScale(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::GetDampingFunction(IFunction** ppObj)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    *ppObj = pThis->GetDampingFunction();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::SetDampingFunction(IFunction* Function)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    pThis->SetDampingFunction(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::GetDampingCoefficient(double* pVal)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    *pVal = pThis->GetDampingCoefficient();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::SetDampingCoefficient(double Value)
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
  TRY
  {
    pThis->SetDampingCoefficient(Value);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialDamper::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaAxialDamper, LocalClass);
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
