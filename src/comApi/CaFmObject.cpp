// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaFmObject.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmModelMemberBase.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaFmObject, CCmdTarget)

BEGIN_MESSAGE_MAP(CaFmObject, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaFmObject, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaFmObject, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaFmObject, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaFmObject, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaFmObject, "GetValue", dispidGetValue, GetValue, VT_VARIANT, VTS_BSTR)
  DISP_FUNCTION_ID(CaFmObject, "SetValue", dispidSetValue, SetValue, VT_EMPTY, VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CaFmObject, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaFmObject, CCmdTarget)
  INTERFACE_PART(CaFmObject, IID_IFmObject, LocalClass)
END_INTERFACE_MAP()

// {EA7B3CEF-26E7-4FC0-8E0C-8583176FD67F}
IMPLEMENT_OLECREATE(CaFmObject, "FEDEM.FmObject",
0xea7b3cef, 0x26e7, 0x4fc0, 0x8e, 0xc, 0x85, 0x83, 0x17, 0x6f, 0xd6, 0x7f);


CaFmObject::CaFmObject(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  signalConnector.Connect(this);
}

CaFmObject::~CaFmObject(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaFmObject::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaFmObject::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

long CaFmObject::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

ISubAssembly* CaFmObject::get_Parent()
{
  CA_CHECK(m_ptr);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_ptr->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

BSTR CaFmObject::GetValue(LPCTSTR FieldName)
{
  CA_CHECK(m_ptr);

  FFaFieldBase* pField = m_ptr->getField(CaConvert(FieldName));
  if (pField == NULL)
    return SysAllocString(CA2W(""));

  std::ostringstream s;
  s << *pField;
  return SysAllocString(CA2W(s.str().c_str()));
}

void CaFmObject::SetValue(LPCTSTR FieldName, LPCTSTR Value)
{
  CA_CHECK(m_ptr);

  FFaFieldBase* pField = m_ptr->getField(CaConvert(FieldName));
  if (pField == NULL)
    return;

  std::istringstream s(CaConvert(Value));
  s >> *pField;
}

void CaFmObject::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaFmObject::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaFmObject::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaFmObject::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaFmObject::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaFmObject::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaFmObject::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaFmObject::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass)
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

STDMETHODIMP CaFmObject::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
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

STDMETHODIMP CaFmObject::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
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

STDMETHODIMP CaFmObject::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
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

STDMETHODIMP CaFmObject::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
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

STDMETHODIMP CaFmObject::XLocalClass::GetValue(BSTR FieldName, BSTR* Value)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
  TRY
  {
    *Value = pThis->GetValue(FieldName);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFmObject::XLocalClass::SetValue(BSTR FieldName, BSTR Value)
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
  TRY
  {
    pThis->SetValue(FieldName, Value);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFmObject::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaFmObject, LocalClass);
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
