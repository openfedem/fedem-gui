// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaMaterial.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmMaterialProperty.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaMaterial, CCmdTarget)

BEGIN_MESSAGE_MAP(CaMaterial, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaMaterial, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaMaterial, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaMaterial, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaMaterial, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaMaterial, "GetMaterialProperties", dispidGetMaterialProperties, GetMaterialProperties, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaMaterial, "SetMaterialProperties", dispidSetMaterialProperties, SetMaterialProperties, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaMaterial, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaMaterial, CCmdTarget)
  INTERFACE_PART(CaMaterial, IID_IMaterial, LocalClass)
END_INTERFACE_MAP()

// {BEED35E0-B1E8-435f-93AA-4125892B865D}
IMPLEMENT_OLECREATE(CaMaterial, "FEDEM.Material",
0xbeed35e0, 0xb1e8, 0x435f, 0x93, 0xaa, 0x41, 0x25, 0x89, 0x2b, 0x86, 0x5d);


CaMaterial::CaMaterial(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  signalConnector.Connect(this);
}

CaMaterial::~CaMaterial(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaMaterial::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaMaterial::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

long CaMaterial::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

ISubAssembly* CaMaterial::get_Parent()
{
  CA_CHECK(m_ptr);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_ptr->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaMaterial::GetMaterialProperties(double* Rho, double* E, double* nu, double* G)
{
  CA_CHECK(m_ptr);

  *Rho = m_ptr->Rho.getValue();
  *E = m_ptr->E.getValue();
  *G = m_ptr->G.getValue();
  *nu = m_ptr->nu.getValue();
}

void CaMaterial::SetMaterialProperties(double Rho, double E, double nu)
{
  CA_CHECK(m_ptr);

  if (m_ptr->updateProperties(Rho,E,m_ptr->G.getValue(),nu))
    m_ptr->onChanged();
  else
    AfxThrowOleException(E_INVALIDARG);
}

void CaMaterial::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaMaterial::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaMaterial::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaMaterial::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaMaterial::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaMaterial::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaMaterial::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaMaterial::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass)
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

STDMETHODIMP CaMaterial::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
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

STDMETHODIMP CaMaterial::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
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

STDMETHODIMP CaMaterial::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
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

STDMETHODIMP CaMaterial::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
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

STDMETHODIMP CaMaterial::XLocalClass::GetMaterialProperties(double* Rho, double* E, double* nu, double* G)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
  TRY
  {
    pThis->GetMaterialProperties(Rho, E, nu, G);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaMaterial::XLocalClass::SetMaterialProperties(double Rho, double E, double nu)
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
  TRY
  {
    pThis->SetMaterialProperties(Rho, E, nu);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaMaterial::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaMaterial, LocalClass);
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
