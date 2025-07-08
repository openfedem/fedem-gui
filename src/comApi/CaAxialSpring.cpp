// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaAxialSpring.h"
#include "CaTriad.h"
#include "CaFunction.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmAxialSpring.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaAxialSpring, CCmdTarget)

BEGIN_MESSAGE_MAP(CaAxialSpring, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaAxialSpring, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaAxialSpring, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaAxialSpring, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaAxialSpring, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialSpring, "GetTriad1", dispidGetTriad1, GetTriad1, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialSpring, "SetTriad1", dispidSetTriad1, SetTriad1, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialSpring, "GetTriad2", dispidGetTriad2, GetTriad2, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialSpring, "SetTriad2", dispidSetTriad2, SetTriad2, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialSpring, "GetStiffnessScale", dispidGetStiffnessScale, GetStiffnessScale, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialSpring, "SetStiffnessScale", dispidSetStiffnessScale, SetStiffnessScale, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialSpring, "GetStiffnessFunction", dispidGetStiffnessFunction, GetStiffnessFunction, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialSpring, "SetStiffnessFunction", dispidSetStiffnessFunction, SetStiffnessFunction, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialSpring, "GetStiffnessCoefficient", dispidGetStiffnessCoefficient, GetStiffnessCoefficient, VT_R8, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialSpring, "SetStiffnessCoefficient", dispidSetStiffnessCoefficient, SetStiffnessCoefficient, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaAxialSpring, "GetStressFreeLengthOrDeflection", dispidGetStressFreeLengthOrDeflection, GetStressFreeLengthOrDeflection, VT_R8, VTS_PBOOL)
  DISP_FUNCTION_ID(CaAxialSpring, "SetStressFreeLengthOrDeflection", dispidSetStressFreeLengthOrDeflection, SetStressFreeLengthOrDeflection, VT_EMPTY, VTS_BOOL VTS_R8)
  DISP_FUNCTION_ID(CaAxialSpring, "GetStressFreeLengthChange", dispidGetStressFreeLengthChange, GetStressFreeLengthChange, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaAxialSpring, "SetStressFreeLengthChange", dispidSetStressFreeLengthChange, SetStressFreeLengthChange, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaAxialSpring, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaAxialSpring, CCmdTarget)
  INTERFACE_PART(CaAxialSpring, IID_IAxialSpring, LocalClass)
END_INTERFACE_MAP()

// {EF6D3EA9-2F40-4836-9083-C32E4091812E}
IMPLEMENT_OLECREATE(CaAxialSpring, "FEDEM.AxialSpring",
0xef6d3ea9, 0x2f40, 0x4836, 0x90, 0x83, 0xc3, 0x2e, 0x40, 0x91, 0x81, 0x2e);


CaAxialSpring::CaAxialSpring(void) : m_ptr(m_pAxialSpring)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pAxialSpring = NULL;
  signalConnector.Connect(this);
}

CaAxialSpring::~CaAxialSpring(void)
{
  ::AfxOleUnlockApp();
  m_pAxialSpring = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaAxialSpring::get_Description()
{
  CA_CHECK(m_pAxialSpring);

  return SysAllocString(CA2W(m_pAxialSpring->getUserDescription().c_str()));
}

void CaAxialSpring::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pAxialSpring);

  m_pAxialSpring->setUserDescription(CaConvert(val));
  m_pAxialSpring->onChanged();
}

long CaAxialSpring::get_BaseID()
{
  CA_CHECK(m_pAxialSpring);

  return m_pAxialSpring->getBaseID();
}

ISubAssembly* CaAxialSpring::get_Parent()
{
  CA_CHECK(m_pAxialSpring);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pAxialSpring->getParentAssembly();
  if (pParent == NULL)
    return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

ITriad* CaAxialSpring::GetTriad1()
{
  CA_CHECK(m_pAxialSpring);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get
  if (m_pAxialSpring->getFirstTriad())
    pCaTriad->m_pTriad = m_pAxialSpring->getFirstTriad();
  else
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaAxialSpring::SetTriad1(ITriad* Triad)
{
  CA_CHECK(m_pAxialSpring);

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
  m_pAxialSpring->setOwnerTriads(
    pCaTriad->m_pTriad,
    m_pAxialSpring->getSecondTriad());

  // Update
  m_pAxialSpring->draw();
}

ITriad* CaAxialSpring::GetTriad2()
{
  CA_CHECK(m_pAxialSpring);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get
  if (m_pAxialSpring->getSecondTriad())
    pCaTriad->m_pTriad = m_pAxialSpring->getSecondTriad();
  else
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaAxialSpring::SetTriad2(ITriad* Triad)
{
  CA_CHECK(m_pAxialSpring);

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
  m_pAxialSpring->setOwnerTriads(
    m_pAxialSpring->getFirstTriad(),
    pCaTriad->m_pTriad);

  // Update
  m_pAxialSpring->draw();
}

IFunction* CaAxialSpring::GetStiffnessScale()
{
  CA_CHECK(m_pAxialSpring);

  // Get
  FmEngine* pScaleFunc = m_pAxialSpring->getScaleEngine();
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

void CaAxialSpring::SetStiffnessScale(IFunction* Function)
{
  CA_CHECK(m_pAxialSpring);

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
  m_pAxialSpring->setScaleEngine(pScaleFunc);
}

IFunction* CaAxialSpring::GetStiffnessFunction()
{
  CA_CHECK(m_pAxialSpring);

  // Get
  FmMathFuncBase* pFunc = m_pAxialSpring->getStiffFunction();
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

void CaAxialSpring::SetStiffnessFunction(IFunction* Function)
{
  CA_CHECK(m_pAxialSpring);

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
  m_pAxialSpring->setSpringCharOrStiffFunction(pFunc);
}

double CaAxialSpring::GetStiffnessCoefficient()
{
  CA_CHECK(m_pAxialSpring);
  return m_pAxialSpring->getInitStiff();
}

void CaAxialSpring::SetStiffnessCoefficient(double Value)
{
  CA_CHECK(m_pAxialSpring);
  m_pAxialSpring->setInitStiff(Value);
}

double CaAxialSpring::GetStressFreeLengthOrDeflection(BOOL* pIsDeflection)
{
  CA_CHECK(m_pAxialSpring);
  double val = 0;
  *pIsDeflection = m_pAxialSpring->getInitLengthOrDefl(val);
  return val;
}

void CaAxialSpring::SetStressFreeLengthOrDeflection(BOOL IsDeflection, double Value)
{
  CA_CHECK(m_pAxialSpring);
  m_pAxialSpring->setInitLengthOrDefl(Value, IsDeflection);
}

IFunction* CaAxialSpring::GetStressFreeLengthChange()
{
  CA_CHECK(m_pAxialSpring);

  // Get
  FmEngine* pFunc = m_pAxialSpring->getLengthEngine();
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

void CaAxialSpring::SetStressFreeLengthChange(IFunction* Function)
{
  CA_CHECK(m_pAxialSpring);

  // Get function
  FmEngine* pFunc = NULL;
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
    pFunc = dynamic_cast<FmEngine*>(pCaFunction->m_ptr);
    if (pFunc == NULL)
      AfxThrowOleException(E_INVALIDARG);
  }

  // Set
  m_pAxialSpring->setLengthEngine(pFunc);
}

void CaAxialSpring::Delete()
{
  CA_CHECK(m_pAxialSpring);

  m_pAxialSpring->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaAxialSpring::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaAxialSpring::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaAxialSpring::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaAxialSpring::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaAxialSpring::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaAxialSpring::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaAxialSpring::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass)
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

STDMETHODIMP CaAxialSpring::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::GetTriad1(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::SetTriad1(ITriad* Triad)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::GetTriad2(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::SetTriad2(ITriad* Triad)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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

STDMETHODIMP CaAxialSpring::XLocalClass::GetStiffnessScale(IFunction** ppObj)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    *ppObj = pThis->GetStiffnessScale();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::SetStiffnessScale(IFunction* Function)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    pThis->SetStiffnessScale(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::GetStiffnessFunction(IFunction** ppObj)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    *ppObj = pThis->GetStiffnessFunction();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::SetStiffnessFunction(IFunction* Function)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    pThis->SetStiffnessFunction(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::GetStiffnessCoefficient(double* pVal)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    *pVal = pThis->GetStiffnessCoefficient();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::SetStiffnessCoefficient(double Val)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    pThis->SetStiffnessCoefficient(Val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::GetStressFreeLengthOrDeflection(VARIANT_BOOL* IsDeflection, double* pVal)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    BOOL v = false;
    *pVal = pThis->GetStressFreeLengthOrDeflection(&v);
    *IsDeflection = v;
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::SetStressFreeLengthOrDeflection(VARIANT_BOOL IsDeflection, double Value)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    pThis->SetStressFreeLengthOrDeflection(IsDeflection, Value);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::GetStressFreeLengthChange(IFunction** ppObj)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    *ppObj = pThis->GetStressFreeLengthChange();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::SetStressFreeLengthChange(IFunction* Function)
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
  TRY
  {
    pThis->SetStressFreeLengthChange(Function);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaAxialSpring::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaAxialSpring, LocalClass);
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
