// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaGenericObject.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmGenericDBObject.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaGenericObject, CCmdTarget)

BEGIN_MESSAGE_MAP(CaGenericObject, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaGenericObject, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaGenericObject, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGenericObject, "ObjectType", dispidObjectType, get_ObjectType, put_ObjectType, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGenericObject, "ObjectDefinition", dispidObjectDefinition, get_ObjectDefinition, put_ObjectDefinition, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGenericObject, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaGenericObject, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaGenericObject, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaGenericObject, CCmdTarget)
  INTERFACE_PART(CaGenericObject, IID_IGenericObject, LocalClass)
END_INTERFACE_MAP()

// {FE0A9B4B-64E6-4276-8E3E-71CB4090F925}
IMPLEMENT_OLECREATE(CaGenericObject, "FEDEM.GenericObject", 
0xfe0a9b4b, 0x64e6, 0x4276, 0x8e, 0x3e, 0x71, 0xcb, 0x40, 0x90, 0xf9, 0x25);


CaGenericObject::CaGenericObject(void) : m_ptr(m_pGenericObject)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pGenericObject = NULL;
  signalConnector.Connect(this);
}

CaGenericObject::~CaGenericObject(void)
{
  ::AfxOleUnlockApp();
  m_pGenericObject = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaGenericObject::get_Description()
{
  CA_CHECK(m_pGenericObject);
  return SysAllocString(CA2W(m_pGenericObject->getUserDescription().c_str()));
}

void CaGenericObject::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pGenericObject);

  m_pGenericObject->setUserDescription(CaConvert(val));
  m_pGenericObject->onChanged();
}

BSTR CaGenericObject::get_ObjectType()
{
  CA_CHECK(m_pGenericObject);
  return SysAllocString(CA2W(m_pGenericObject->objectType.getValue().c_str()));
}

void CaGenericObject::put_ObjectType(LPCTSTR val)
{
  CA_CHECK(m_pGenericObject);

  m_pGenericObject->objectType.setValue(CaConvert(val));
  m_pGenericObject->onChanged();
}

BSTR CaGenericObject::get_ObjectDefinition()
{
  CA_CHECK(m_pGenericObject);
  return SysAllocString(CA2W(m_pGenericObject->objectDefinition.getValue().c_str()));
}

void CaGenericObject::put_ObjectDefinition(LPCTSTR val)
{
  CA_CHECK(m_pGenericObject);

  m_pGenericObject->objectDefinition.setValue(CaConvert(val));
  m_pGenericObject->onChanged();
}

long CaGenericObject::get_BaseID()
{
  CA_CHECK(m_pGenericObject);
  return m_pGenericObject->getBaseID();
}

ISubAssembly* CaGenericObject::get_Parent()
{
  CA_CHECK(m_pGenericObject);
  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pGenericObject->getParentAssembly();
  if (pParent == NULL)
      return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaGenericObject::Delete()
{
  CA_CHECK(m_pGenericObject);
  m_pGenericObject->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaGenericObject::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaGenericObject::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaGenericObject::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaGenericObject::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaGenericObject::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaGenericObject::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid) 
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, 
          lcid, rgdispid);
}
STDMETHODIMP CaGenericObject::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass)
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

STDMETHODIMP CaGenericObject::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
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

STDMETHODIMP CaGenericObject::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
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

STDMETHODIMP CaGenericObject::XLocalClass::get_ObjectType(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
  TRY
  {
    *pVal = pThis->get_ObjectType();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGenericObject::XLocalClass::put_ObjectType(BSTR val)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
  TRY
  {
    pThis->put_ObjectType(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGenericObject::XLocalClass::get_ObjectDefinition(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
  TRY
  {
    *pVal = pThis->get_ObjectDefinition();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGenericObject::XLocalClass::put_ObjectDefinition(BSTR val)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
  TRY
  {
    pThis->put_ObjectDefinition(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGenericObject::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
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

STDMETHODIMP CaGenericObject::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
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

STDMETHODIMP CaGenericObject::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaGenericObject, LocalClass);
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
