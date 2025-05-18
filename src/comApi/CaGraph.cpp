// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaGraph.h"
#include "CaCurve.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmGraph.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaGraph, CCmdTarget)

BEGIN_MESSAGE_MAP(CaGraph, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaGraph, CCmdTarget)
  DISP_PROPERTY_PARAM_ID(CaGraph, "Item", dispidItem, get_Item, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaGraph, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGraph, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaGraph, "Title", dispidTitle, get_Title, put_Title, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGraph, "SubTitle", dispidSubTitle, get_SubTitle, put_SubTitle, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGraph, "XAxisLabel", dispidXAxisLabel, get_XAxisLabel, put_XAxisLabel, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGraph, "YAxisLabel", dispidYAxisLabel, get_YAxisLabel, put_YAxisLabel, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaGraph, "GridType", dispidGridType, get_GridType, put_GridType, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "ShowLegend", dispidShowLegend, get_ShowLegend, put_ShowLegend, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaGraph, "AutoScaleFlag", dispidAutoScaleFlag, get_AutoScaleFlag, put_AutoScaleFlag, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaGraph, "UseTimeRange", dispidUseTimeRange, get_UseTimeRange, put_UseTimeRange, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaGraph, "TimeRangeMin", dispidTimeRangeMin, get_TimeRangeMin, put_TimeRangeMin, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "TimeRangeMax", dispidTimeRangeMax, get_TimeRangeMax, put_TimeRangeMax, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "XAxisScaleMin", dispidXAxisScaleMin, get_XAxisScaleMin, put_XAxisScaleMin, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "XAxisScaleMax", dispidXAxisScaleMax, get_XAxisScaleMax, put_XAxisScaleMax, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "YAxisScaleMin", dispidYAxisScaleMin, get_YAxisScaleMin, put_YAxisScaleMin, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "YAxisScaleMax", dispidYAxisScaleMax, get_YAxisScaleMax, put_YAxisScaleMax, VT_R8)
  DISP_PROPERTY_EX_ID(CaGraph, "IsFunctionPreview", dispidIsFunctionPreview, get_IsFunctionPreview, SetNotSupported, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaGraph, "IsBeamDiagram", dispidIsBeamDiagram, get_IsBeamDiagram, SetNotSupported, VT_BOOL)
  DISP_FUNCTION_ID(CaGraph, "CreateCurve", dispidCreateCurve, CreateCurve, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaGraph, "GetCount", dispidGetCount, GetCount, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CaGraph, "GetItem", dispidGetItem, GetItem, VT_UNKNOWN, VTS_I4)
  DISP_FUNCTION_ID(CaGraph, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaGraph, CCmdTarget)
  INTERFACE_PART(CaGraph, IID_IGraph, LocalClass)
END_INTERFACE_MAP()

// {DD4B8DBF-2F71-4FFB-8E33-5AB0D6B89371}
IMPLEMENT_OLECREATE(CaGraph, "FEDEM.Graph",
0xdd4b8dbf, 0x2f71, 0x4ffb, 0x8e, 0x33, 0x5a, 0xb0, 0xd6, 0xb8, 0x93, 0x71);



CaGraph::CaGraph(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  signalConnector.Connect(this);
}

CaGraph::~CaGraph(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

ICurve* CaGraph::get_Item(long Index)
{
  CA_CHECK(m_ptr);

  return GetItem(Index);
}

BSTR CaGraph::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaGraph::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

long CaGraph::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

BSTR CaGraph::get_Title()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getTitle().c_str()));
}

void CaGraph::put_Title(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setTitle(CaConvert(val));
  m_ptr->onChanged();
}

BSTR CaGraph::get_SubTitle()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getSubTitle().c_str()));
}

void CaGraph::put_SubTitle(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setSubTitle(CaConvert(val));
  m_ptr->onChanged();
}

BSTR CaGraph::get_XAxisLabel()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getXaxisLabel().c_str()));
}

void CaGraph::put_XAxisLabel(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setXaxisLabel(CaConvert(val));
  m_ptr->onChanged();
}

BSTR CaGraph::get_YAxisLabel()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getYaxisLabel().c_str()));
}

void CaGraph::put_YAxisLabel(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setYaxisLabel(CaConvert(val));
  m_ptr->onChanged();
}

long CaGraph::get_GridType()
{
  CA_CHECK(m_ptr);

  return m_ptr->getGridType();
}

void CaGraph::put_GridType(long val)
{
  CA_CHECK(m_ptr);

  m_ptr->setGridType(val);
}

BOOL CaGraph::get_ShowLegend()
{
  CA_CHECK(m_ptr);

  return m_ptr->getShowLegend();
}

void CaGraph::put_ShowLegend(BOOL val)
{
  CA_CHECK(m_ptr);

  m_ptr->setShowLegend(val);
}

BOOL CaGraph::get_AutoScaleFlag()
{
  CA_CHECK(m_ptr);

  return m_ptr->getAutoScaleFlag();
}

void CaGraph::put_AutoScaleFlag(BOOL val)
{
  CA_CHECK(m_ptr);

  m_ptr->setAutoScaleFlag(val);
}

BOOL CaGraph::get_UseTimeRange()
{
  CA_CHECK(m_ptr);

  return m_ptr->getUseTimeRange();
}

void CaGraph::put_UseTimeRange(BOOL val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUseTimeRange(val);
}

double CaGraph::get_TimeRangeMin()
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getTimeRange(min, max);
  return min;
}

void CaGraph::put_TimeRangeMin(double val)
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getTimeRange(min, max);
  m_ptr->setTimeRange(val, max);
}

double CaGraph::get_TimeRangeMax()
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getTimeRange(min, max);
  return max;
}

void CaGraph::put_TimeRangeMax(double val)
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getTimeRange(min, max);
  m_ptr->setTimeRange(min, val);
}

double CaGraph::get_XAxisScaleMin()
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getXaxisScale(min, max);
  return min;
}

void CaGraph::put_XAxisScaleMin(double val)
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getXaxisScale(min, max);
  m_ptr->setXaxisScale(val, max);
}

double CaGraph::get_XAxisScaleMax()
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getXaxisScale(min, max);
  return max;
}

void CaGraph::put_XAxisScaleMax(double val)
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getXaxisScale(min, max);
  m_ptr->setXaxisScale(min, val);
}

double CaGraph::get_YAxisScaleMin()
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getYaxisScale(min, max);
  return min;
}

void CaGraph::put_YAxisScaleMin(double val)
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getYaxisScale(min, max);
  m_ptr->setYaxisScale(val, max);
}

double CaGraph::get_YAxisScaleMax()
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getYaxisScale(min, max);
  return max;
}

void CaGraph::put_YAxisScaleMax(double val)
{
  CA_CHECK(m_ptr);

  double min = 0.0, max = 0.0;
  m_ptr->getYaxisScale(min, max);
  m_ptr->setYaxisScale(min, val);
}

BOOL CaGraph::get_IsFunctionPreview()
{
  CA_CHECK(m_ptr);

  return m_ptr->isFuncPreview();
}

BOOL CaGraph::get_IsBeamDiagram()
{
  CA_CHECK(m_ptr);

  return m_ptr->isBeamDiagram();
}

ICurve* CaGraph::CreateCurve()
{
  CA_CHECK(m_ptr);

  // Create
  FmCurveSet* curve = new FmCurveSet();
  m_ptr->addCurveSet(curve);
  curve->setColor(m_ptr->getCurveDefaultColor());
  curve->onChanged();

  // Create wrapper
  CaCurve* pCaCurve = (CaCurve*)CaCurve::CreateObject();
  if (pCaCurve == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaCurve->m_ptr = curve;

  // Return
  ICurve* pICurve = NULL;
  LPDISPATCH pDisp = pCaCurve->GetIDispatch(false);
  pDisp->QueryInterface(IID_ICurve, (void**)&pICurve);
  pCaCurve->InternalRelease();
  return pICurve;
}

ICurve* CaGraph::GetItem(long Index)
{
  CA_CHECK(m_ptr);

  // Get curves
  std::vector<FmCurveSet*> curves;
  m_ptr->getCurveSets(curves);

  // Check
  if ((Index < 0) || (Index > curves.size()))
    AfxThrowOleException(E_INVALIDARG);

  // Create wrapper
  CaCurve* pCaCurve = (CaCurve*)CaCurve::CreateObject();
  if (pCaCurve == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaCurve->m_ptr = curves.at(Index);

  // Return
  ICurve* pICurve = NULL;
  LPDISPATCH pDisp = pCaCurve->GetIDispatch(false);
  pDisp->QueryInterface(IID_ICurve, (void**)&pICurve);
  pCaCurve->InternalRelease();
  return pICurve;
}

long CaGraph::GetCount()
{
  CA_CHECK(m_ptr);

  return m_ptr->getCurveSetCount();
}

void CaGraph::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaGraph::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaGraph::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaGraph::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaGraph::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaGraph::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaGraph::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaGraph::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaGraph, LocalClass)
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

STDMETHODIMP CaGraph::XLocalClass::get_Item(long Index, ICurve** ppObj)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
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

STDMETHODIMP CaGraph::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
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

STDMETHODIMP CaGraph::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
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

STDMETHODIMP CaGraph::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
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

STDMETHODIMP CaGraph::XLocalClass::get_Title(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_Title();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_Title(BSTR val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_Title(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_SubTitle(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_SubTitle();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_SubTitle(BSTR val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_SubTitle(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_XAxisLabel(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_XAxisLabel();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_XAxisLabel(BSTR val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_XAxisLabel(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_YAxisLabel(BSTR* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_YAxisLabel();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_YAxisLabel(BSTR val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_YAxisLabel(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_GridType(long* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_GridType();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_GridType(long val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_GridType(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_ShowLegend(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_ShowLegend();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_ShowLegend(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_ShowLegend(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_AutoScaleFlag(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_AutoScaleFlag();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_AutoScaleFlag(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_AutoScaleFlag(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_UseTimeRange(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_UseTimeRange();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_UseTimeRange(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_UseTimeRange(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_TimeRangeMin(double* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_TimeRangeMin();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_TimeRangeMin(double val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_TimeRangeMin(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_TimeRangeMax(double* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_TimeRangeMax();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_TimeRangeMax(double val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_TimeRangeMax(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_XAxisScaleMin(double* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_XAxisScaleMin();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_XAxisScaleMin(double val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_XAxisScaleMin(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_XAxisScaleMax(double* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_XAxisScaleMax();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_XAxisScaleMax(double val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_XAxisScaleMax(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_YAxisScaleMin(double* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_YAxisScaleMin();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_YAxisScaleMin(double val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_YAxisScaleMin(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_YAxisScaleMax(double* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_YAxisScaleMax();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::put_YAxisScaleMax(double val)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    pThis->put_YAxisScaleMax(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_IsFunctionPreview(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_IsFunctionPreview();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::get_IsBeamDiagram(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->get_IsBeamDiagram();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::CreateCurve(ICurve** ppObj)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *ppObj = pThis->CreateCurve();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::GetItem(long Index, ICurve** ppObj)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
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

STDMETHODIMP CaGraph::XLocalClass::GetCount(long* pVal)
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
  TRY
  {
    *pVal = pThis->GetCount();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaGraph::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaGraph, LocalClass);
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
