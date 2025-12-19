// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaSelectionManager.h"
#include "CaApplication.h"

#include "vpmDB/FmModelMemberBase.H"
#include "vpmApp/FapEventManager.H"
#include "vpmDisplay/FdPickedPoints.H"
#include "vpmDisplay/FdDB.H"
#include "vpmUI/FuiModes.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaSelectionManager, CCmdTarget)

BEGIN_MESSAGE_MAP(CaSelectionManager, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaSelectionManager, CCmdTarget)
  DISP_PROPERTY_PARAM_ID(CaSelectionManager, "Item", dispidItem, get_Item, put_Item, VT_DISPATCH, VTS_I4)
  DISP_PROPERTY_EX_ID(CaSelectionManager, "LastItem", dispidLastItem, get_LastItem, put_LastItem, VT_DISPATCH)
  DISP_PROPERTY_EX_ID(CaSelectionManager, "Count", dispidCount, get_Count, SetNotSupported, VT_I4)
  DISP_FUNCTION_ID(CaSelectionManager, "Add", dispidAdd, Add, VT_EMPTY, VTS_DISPATCH)
  DISP_FUNCTION_ID(CaSelectionManager, "Insert", dispidInsert, Insert, VT_EMPTY, VTS_I4 VTS_DISPATCH)
  DISP_FUNCTION_ID(CaSelectionManager, "Remove", dispidRemove, Remove, VT_EMPTY, VTS_DISPATCH)
  DISP_FUNCTION_ID(CaSelectionManager, "RemoveAt", dispidRemoveAt, RemoveAt, VT_EMPTY, VTS_I4)
  DISP_FUNCTION_ID(CaSelectionManager, "Clear", dispidClear, Clear, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSelectionManager, "FEGetPickedPoint", dispidFEGetPickedPoint, FEGetPickedPoint, VT_BOOL, VTS_PR8 VTS_PR8 VTS_PR8 VTS_BOOL)
  DISP_FUNCTION_ID(CaSelectionManager, "FESetPickPointMode", dispidFESetPickPointMode, FESetPickPointMode, VT_EMPTY, VTS_BOOL VTS_VARIANT)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaSelectionManager, CCmdTarget)
  INTERFACE_PART(CaSelectionManager, IID_ISelectionManager, LocalClass)
END_INTERFACE_MAP()

// {C0909AB9-EF0E-4ab8-A865-A8FA450B71D3}
IMPLEMENT_OLECREATE(CaSelectionManager, "FEDEM.SelectionManager",
0xc0909ab9, 0xef0e, 0x4ab8, 0xa8, 0x65, 0xa8, 0xfa, 0x45, 0xb, 0x71, 0xd3);


CaSelectionManager::CaSelectionManager(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
}

CaSelectionManager::~CaSelectionManager(void)
{
  ::AfxOleUnlockApp();
}


//////////////////////////////////////////////////////////////////////
// Methods

IDispatch* CaSelectionManager::get_Item(long Index)
{
  FmModelMemberBase* item = dynamic_cast<FmModelMemberBase*>(FapEventManager::getPermSelectedObject(Index));
  if (item == NULL)
    return NULL;
  // Create and return COM object wrapper
  return CaApplication::CreateCOMObjectWrapper(item);
}

void CaSelectionManager::put_Item(long Index, IDispatch* pObj)
{
  FmModelMemberBase* item = CaApplication::GetFromCOMObjectWrapper(pObj);
  if (item == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FapEventManager::permSelect(item, Index);
}

IDispatch* CaSelectionManager::get_LastItem()
{
  FmModelMemberBase* item = dynamic_cast<FmModelMemberBase*>(FapEventManager::getLastPermSelectedObject());
  if (item == NULL)
    return NULL;
  // Create and return COM object wrapper
  return CaApplication::CreateCOMObjectWrapper(item);
}

void CaSelectionManager::put_LastItem(IDispatch* pObj)
{
  FmModelMemberBase* item = CaApplication::GetFromCOMObjectWrapper(pObj);
  if (item == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FapEventManager::permUnselect(item);
  FapEventManager::permSelect(item);
}

long CaSelectionManager::get_Count()
{
  return FapEventManager::getNumPermSelected();
}

void CaSelectionManager::Add(IDispatch* Item)
{
  FmModelMemberBase* item = CaApplication::GetFromCOMObjectWrapper(Item);
  if (item == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FapEventManager::permSelect(item);
}

void CaSelectionManager::Insert(long Index, IDispatch* Item)
{
  FmModelMemberBase* item = CaApplication::GetFromCOMObjectWrapper(Item);
  if (item == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FapEventManager::permSelectInsert(item, Index);
}

void CaSelectionManager::Remove(IDispatch* Item)
{
  FmModelMemberBase* item = CaApplication::GetFromCOMObjectWrapper(Item);
  if (item == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FapEventManager::permUnselect(item);
}

void CaSelectionManager::RemoveAt(long Index)
{
  FapEventManager::permUnselect(Index);
}

void CaSelectionManager::Clear()
{
  FapEventManager::permUnselectAll();
}

BOOL CaSelectionManager::FEGetPickedPoint(double* x, double* y, double* z, BOOL Global)
{
    // Get picked point list
    std::vector<FaVec3> globalPoints;
    FdPickedPoints::getAllPickedPointsGlobal(globalPoints);
    if (globalPoints.size() == 0)
        return FALSE;

    // Get picked point position
    FaVec3 v = FdDB::getPPoint(0, (bool)Global);
    *x = v.x();
    *y = v.y();
    *z = v.z();
    return TRUE;
}

void CaSelectionManager::FESetPickPointMode(BOOL On, const VARIANT FAR& Desc)
{
    // Set tip
    FuiModes::tipComPicking = "";
    if (Desc.vt == VT_BSTR) {
        std::string strDesc(CW2A(Desc.bstrVal));
        FuiModes::tipComPicking = strDesc;
    }

    // Set mode
    if (On)
        FuiModes::setMode(FuiModes::COMPICKPOINT_MODE);
    else
        FuiModes::cancel();
}

BOOL CaSelectionManager::FEGetPickPointMode(long* State)
{
    if (State != NULL)
        *State = FuiModes::getState();
    return FuiModes::getMode();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaSelectionManager::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaSelectionManager::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaSelectionManager::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaSelectionManager::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaSelectionManager::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaSelectionManager::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaSelectionManager::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass)
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

STDMETHODIMP CaSelectionManager::XLocalClass::get_Item(long Index, IDispatch** ppObj)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
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

STDMETHODIMP CaSelectionManager::XLocalClass::put_Item(long Index, IDispatch* pObj)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    pThis->put_Item(Index, pObj);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::get_LastItem(IDispatch** ppObj)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    *ppObj = pThis->get_LastItem();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::put_LastItem(IDispatch* pObj)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    pThis->put_LastItem(pObj);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::get_Count(long* pnVal)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    *pnVal = pThis->get_Count();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::Add(IDispatch* Item)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    pThis->Add(Item);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::Insert(long Index, IDispatch* Item)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    pThis->Insert(Index, Item);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::Remove(IDispatch* Item)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    pThis->Remove(Item);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::RemoveAt(long Index)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
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

STDMETHODIMP CaSelectionManager::XLocalClass::Clear()
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
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

STDMETHODIMP CaSelectionManager::XLocalClass::FEGetPickedPoint(double* x, double* y, double* z, VARIANT_BOOL Global, VARIANT_BOOL* pRet)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    *pRet = pThis->FEGetPickedPoint(x, y, z, Global);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::FESetPickPointMode(VARIANT_BOOL On, VARIANT Desc)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    pThis->FESetPickPointMode(On, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSelectionManager::XLocalClass::FEGetPickPointMode(long* State, VARIANT_BOOL* pRet)
{
  METHOD_PROLOGUE(CaSelectionManager, LocalClass);
  TRY
  {
    *pRet = pThis->FEGetPickPointMode(State);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}
