// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaSubAssembly.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmAssemblyBase.H"
#include "vpmDB/FmDB.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaSubAssembly, CCmdTarget)

BEGIN_MESSAGE_MAP(CaSubAssembly, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaSubAssembly, CCmdTarget)
  DISP_PROPERTY_PARAM_ID(CaSubAssembly, "Item", dispidItem, get_Item, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "X", dispidX, get_X, put_X, VT_R8)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Y", dispidY, get_Y, put_Y, VT_R8)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Z", dispidZ, get_Z, put_Z, VT_R8)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "TotalMass", dispidTotalMass, get_TotalMass, SetNotSupported, VT_R8)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "TotalLength", dispidTotalLength, get_TotalLength, SetNotSupported, VT_R8)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Movable", dispidMovable, get_Movable, SetNotSupported, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "ModelFileName", dispidModelFileName, get_ModelFileName, put_ModelFileName, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Count", dispidCount, get_Count, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Tag", dispidTag, get_Tag, put_Tag, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaSubAssembly, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaSubAssembly, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_BOOL)
  DISP_FUNCTION_ID(CaSubAssembly, "SetEulerRotationZYX", dispidSetEulerRotationZYX, SetEulerRotationZYX, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_BOOL)
  DISP_FUNCTION_ID(CaSubAssembly, "GetRotationMatrix", dispidGetRotationMatrix, GetRotationMatrix, VT_EMPTY, VTS_PVARIANT VTS_BOOL)
  DISP_FUNCTION_ID(CaSubAssembly, "SetRotationMatrix", dispidSetRotationMatrix, SetRotationMatrix, VT_EMPTY, VTS_VARIANT VTS_BOOL)
  DISP_FUNCTION_ID(CaSubAssembly, "GetPosition", dispidGetPosition, GetPosition, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_BOOL)
  DISP_FUNCTION_ID(CaSubAssembly, "SetPosition", dispidSetPosition, SetPosition, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_BOOL)
  DISP_FUNCTION_ID(CaSubAssembly, "GetItem", dispidGetItem, GetItem, VT_UNKNOWN, VTS_I4)
  DISP_FUNCTION_ID(CaSubAssembly, "GetItemType", dispidGetItemType, GetItemType, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CaSubAssembly, "Remove", dispidRemove, Remove, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaSubAssembly, "RemoveAt", dispidRemoveAt, RemoveAt, VT_EMPTY, VTS_I4)
  DISP_FUNCTION_ID(CaSubAssembly, "Clear", dispidClear, Clear, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSubAssembly, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaSubAssembly, CCmdTarget)
  INTERFACE_PART(CaSubAssembly, IID_ISubAssembly, LocalClass)
END_INTERFACE_MAP()

// {ABFC3EDA-337F-48e4-9CD7-CA5969C54C11}
IMPLEMENT_OLECREATE(CaSubAssembly, "FEDEM.SubAssembly",
0xabfc3eda, 0x337f, 0x48e4, 0x9c, 0xd7, 0xca, 0x59, 0x69, 0xc5, 0x4c, 0x11);


CaSubAssembly::CaSubAssembly(void) : m_ptr(m_pSubAssembly)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pSubAssembly = NULL;
  signalConnector.Connect(this);
}

CaSubAssembly::~CaSubAssembly(void)
{
  ::AfxOleUnlockApp();
  m_pSubAssembly = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

IDispatch* CaSubAssembly::get_Item(long Index)
{
  CA_CHECK(m_pSubAssembly);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID(),m_pSubAssembly);
  for (FmModelMemberBase* item : items)
    if (m_pSubAssembly == static_cast<FmAssemblyBase*>(item->getParentAssembly()))
      if (Index == nCount++)
        return CaApplication::CreateCOMObjectWrapper(item);

  return NULL;
}

double CaSubAssembly::get_X()
{
  CA_CHECK(m_pSubAssembly);

  return m_pSubAssembly->getLocation().translation().x();
}

void CaSubAssembly::put_X(double val)
{
  CA_CHECK(m_pSubAssembly);

  FFa3DLocation l = m_pSubAssembly->getLocation();
  FaVec3 v = l.translation();
  v.x(val);
  l.setPos(FFa3DLocation::CART_X_Y_Z, v);
  m_pSubAssembly->setLocation(l);

  m_pSubAssembly->onChanged();
}

double CaSubAssembly::get_Y()
{
  CA_CHECK(m_pSubAssembly);

  return m_pSubAssembly->getLocation().translation().y();
}

void CaSubAssembly::put_Y(double val)
{
  CA_CHECK(m_pSubAssembly);

  FFa3DLocation l = m_pSubAssembly->getLocation();
  FaVec3 v = l.translation();
  v.y(val);
  l.setPos(FFa3DLocation::CART_X_Y_Z, v);
  m_pSubAssembly->setLocation(l);

  m_pSubAssembly->onChanged();
}

double CaSubAssembly::get_Z()
{
  CA_CHECK(m_pSubAssembly);

  return m_pSubAssembly->getLocation().translation().z();
}

void CaSubAssembly::put_Z(double val)
{
  CA_CHECK(m_pSubAssembly);

  FFa3DLocation l = m_pSubAssembly->getLocation();
  FaVec3 v = l.translation();
  v.z(val);
  l.setPos(FFa3DLocation::CART_X_Y_Z,v);
  m_pSubAssembly->setLocation(l);

  m_pSubAssembly->onChanged();
}

BSTR CaSubAssembly::get_Description()
{
  CA_CHECK(m_pSubAssembly);

  return SysAllocString(CA2W(m_pSubAssembly->getUserDescription().c_str()));
}

void CaSubAssembly::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pSubAssembly);

  m_pSubAssembly->setUserDescription(CaConvert(val));
  m_pSubAssembly->onChanged();
}

long CaSubAssembly::get_BaseID()
{
  CA_CHECK(m_pSubAssembly);

  return m_pSubAssembly->getBaseID();
}

double CaSubAssembly::get_TotalMass()
{
  CA_CHECK(m_pSubAssembly);
  return m_pSubAssembly->getTotalMass();
}

double CaSubAssembly::get_TotalLength()
{
  CA_CHECK(m_pSubAssembly);

  return m_pSubAssembly->getTotalLength();
}

BOOL CaSubAssembly::get_Movable()
{
  CA_CHECK(m_pSubAssembly);

  return m_pSubAssembly->isMovable();
}

BSTR CaSubAssembly::get_ModelFileName()
{
  CA_CHECK(m_pSubAssembly);

  return SysAllocString(CA2W(m_pSubAssembly->myModelFile.getValue().c_str()));
}

void CaSubAssembly::put_ModelFileName(LPCTSTR val)
{
  CA_CHECK(m_pSubAssembly);

  m_pSubAssembly->myModelFile.setValue(CaConvert(val));
}

long CaSubAssembly::get_Count()
{
  CA_CHECK(m_pSubAssembly);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID(),m_pSubAssembly);
  for (FmModelMemberBase* item : items)
    if (m_pSubAssembly == static_cast<FmAssemblyBase*>(item->getParentAssembly()))
      nCount++;

  return nCount;
}

BSTR CaSubAssembly::get_Tag()
{
  CA_CHECK(m_pSubAssembly);

  return SysAllocString(CA2W(m_pSubAssembly->getTag().c_str()));
}

void CaSubAssembly::put_Tag(LPCTSTR val)
{
  CA_CHECK(m_pSubAssembly);

  m_pSubAssembly->setTag(CaConvert(val));
  m_pSubAssembly->onChanged();
}

ISubAssembly* CaSubAssembly::get_Parent()
{
  CA_CHECK(m_pSubAssembly);

  FmBase* pParent = m_pSubAssembly->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper((FmModelMemberBase*)pParent);
}

void CaSubAssembly::GetEulerRotationZYX(double* rx, double* ry, double* rz, BOOL Global)
{
  CA_CHECK(m_pSubAssembly);

  FaVec3 r;
  if (Global)
    r = m_pSubAssembly->toGlobal(FaMat33()).getEulerZYX();
  else
    r = m_pSubAssembly->toLocal(FaMat33()).getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

void CaSubAssembly::SetEulerRotationZYX(double rx, double ry, double rz, BOOL Global)
{
  CA_CHECK(m_pSubAssembly);

  if (Global) {
    FaMat34 m = m_pSubAssembly->toGlobal(FaMat34());
    m.eulerRotateZYX(FaVec3(rx,ry,rz));
    m_pSubAssembly->setGlobalCS(m);
  } else {
    FaMat34 m = m_pSubAssembly->toLocal(FaMat34());
    m.eulerRotateZYX(FaVec3(rx,ry,rz));
    m_pSubAssembly->setLocalCS(m);
  }

  m_pSubAssembly->onChanged();
}

void CaSubAssembly::GetRotationMatrix(VARIANT* Array3x3, BOOL Global)
{
  CA_CHECK(m_pSubAssembly);

  FaMat33 m;
  if (Global)
    m = m_pSubAssembly->toGlobal(FaMat33());
  else
    m = m_pSubAssembly->toLocal(FaMat33());
  CaApplication::CreateSafeArray(Array3x3,m);
}

void CaSubAssembly::SetRotationMatrix(const VARIANT FAR& Array3x3, BOOL Global)
{
  CA_CHECK(m_pSubAssembly);

  FaMat33 m;
  CaApplication::GetFromSafeArray(Array3x3, m);
  if (Global)
    m_pSubAssembly->setGlobalCS(FaMat34(m,m_pSubAssembly->toGlobal(FaVec3())));
  else
    m_pSubAssembly->setLocalCS(FaMat34(m,m_pSubAssembly->toLocal(FaVec3())));

  m_pSubAssembly->onChanged();
}

void CaSubAssembly::GetPosition(double* x, double* y, double* z, BOOL Global)
{
  CA_CHECK(m_pSubAssembly);

  FaVec3 v;
  if (Global)
    v = m_pSubAssembly->toGlobal(FaVec3());
  else
    v = m_pSubAssembly->toLocal(FaVec3());
  *x = v.x();
  *y = v.y();
  *z = v.z();
}

void CaSubAssembly::SetPosition(double x, double y, double z, BOOL Global)
{
  CA_CHECK(m_pSubAssembly);

  FaVec3 v(x,y,z);
  if (Global)
    m_pSubAssembly->setGlobalCS(FaMat34(m_pSubAssembly->toGlobal(FaMat33()),v));
  else
    m_pSubAssembly->setLocalCS(FaMat34(m_pSubAssembly->toLocal(FaMat33()),v));

  m_pSubAssembly->onChanged();
}

IDispatch* CaSubAssembly::GetItem(long Index)
{
  CA_CHECK(m_pSubAssembly);

  return get_Item(Index);
}

BSTR CaSubAssembly::GetItemType(long Index)
{
  CA_CHECK(m_pSubAssembly);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items, FmModelMemberBase::getClassTypeID(), m_pSubAssembly);
  for (FmModelMemberBase* item : items)
    if (m_pSubAssembly == static_cast<FmAssemblyBase*>(item->getParentAssembly()))
      if (Index == nCount++)
        return SysAllocString(CA2W(item->getItemName()));

  return NULL;
}

void CaSubAssembly::Remove(IDispatch* Child)
{
  CA_CHECK(m_pSubAssembly);

  FmModelMemberBase* pChild = CaApplication::GetFromCOMObjectWrapper(Child);
  if (pChild == NULL)
    AfxThrowOleException(E_POINTER);

  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID(),m_pSubAssembly);
  for (FmModelMemberBase* item : items)
    if (m_pSubAssembly == static_cast<FmAssemblyBase*>(item->getParentAssembly()))
      if (pChild == item) {
        item->erase();
        return;
      }
}

void CaSubAssembly::RemoveAt(long Index)
{
  CA_CHECK(m_pSubAssembly);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID(),m_pSubAssembly);
  for (FmModelMemberBase* item : items)
    if (m_pSubAssembly == static_cast<FmAssemblyBase*>(item->getParentAssembly()))
      if (Index == nCount++) {
        item->erase();
        return;
      }
}

void CaSubAssembly::Clear()
{
  CA_CHECK(m_pSubAssembly);

  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID(),m_pSubAssembly);
  for (FmModelMemberBase* item : items)
    if (m_pSubAssembly == static_cast<FmAssemblyBase*>(item->getParentAssembly()))
      item->erase();
}

void CaSubAssembly::Delete()
{
  CA_CHECK(m_pSubAssembly);

  m_pSubAssembly->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaSubAssembly::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaSubAssembly::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaSubAssembly::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaSubAssembly::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaSubAssembly::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaSubAssembly::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaSubAssembly::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass)
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

STDMETHODIMP CaSubAssembly::XLocalClass::get_Item(long Index, IDispatch** ppObj)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *ppObj = pThis->get_Item(Index);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_X(double* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_X();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::put_X(double val)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->put_X(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Y(double* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_Y();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::put_Y(double val)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->put_Y(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Z(double* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_Z();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::put_Z(double val)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->put_Z(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
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

STDMETHODIMP CaSubAssembly::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
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

STDMETHODIMP CaSubAssembly::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
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

STDMETHODIMP CaSubAssembly::XLocalClass::get_TotalMass(double* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_TotalMass();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_TotalLength(double* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_TotalLength();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Movable(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_Movable();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_ModelFileName(BSTR* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_ModelFileName();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::put_ModelFileName(BSTR val)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->put_ModelFileName(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Count(long* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_Count();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Tag(BSTR* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->get_Tag();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::put_Tag(BSTR val)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->put_Tag(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
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

STDMETHODIMP CaSubAssembly::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz, VARIANT_BOOL Global)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->GetEulerRotationZYX(rx,ry,rz,Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::SetEulerRotationZYX(double rx, double ry, double rz, VARIANT_BOOL Global)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->SetEulerRotationZYX(rx,ry,rz,Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::GetRotationMatrix(VARIANT* Array3x3, VARIANT_BOOL Global)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->GetRotationMatrix(Array3x3,Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::SetRotationMatrix(VARIANT Array3x3, VARIANT_BOOL Global)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->SetRotationMatrix(Array3x3,Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::GetPosition(double* x, double* y, double* z, VARIANT_BOOL Global)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->GetPosition(x,y,z,Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::SetPosition(double x, double y, double z, VARIANT_BOOL Global)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->SetPosition(x,y,z,Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::GetItem(THIS_ long Index, IDispatch** ppObj)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *ppObj = pThis->GetItem(Index);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::GetItemType(THIS_ long Index, BSTR* pVal)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    *pVal = pThis->GetItemType(Index);
  }
    CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
    return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::Remove(IDispatch* Child)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->Remove(Child);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::RemoveAt(long Index)
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->RemoveAt(Index);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::Clear()
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
  TRY
  {
    pThis->Clear();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSubAssembly::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaSubAssembly, LocalClass);
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
