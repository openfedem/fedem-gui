// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaCurve.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmCurveSet.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaCurve, CCmdTarget)

BEGIN_MESSAGE_MAP(CaCurve, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaCurve, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaCurve, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaCurve, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaCurve, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaCurve, "Mode", dispidMode, get_Mode, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaCurve, "Legend", dispidLegend, get_Legend, put_Legend, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaCurve, "Color", dispidColor, get_Color, put_Color, VT_UI4)
  DISP_PROPERTY_EX_ID(CaCurve, "TimeRangeMin", dispidTimeRangeMin, get_TimeRangeMin, put_TimeRangeMin, VT_R8)
  DISP_PROPERTY_EX_ID(CaCurve, "TimeRangeMax", dispidTimeRangeMax, get_TimeRangeMax, put_TimeRangeMax, VT_R8)
  DISP_PROPERTY_EX_ID(CaCurve, "XAxisScale", dispidXAxisScale, get_XAxisScale, put_XAxisScale, VT_R8)
  DISP_PROPERTY_EX_ID(CaCurve, "YAxisScale", dispidYAxisScale, get_YAxisScale, put_YAxisScale, VT_R8)
  DISP_PROPERTY_EX_ID(CaCurve, "XAxisOffset", dispidXAxisOffset, get_XAxisOffset, put_XAxisOffset, VT_R8)
  DISP_PROPERTY_EX_ID(CaCurve, "YAxisOffset", dispidYAxisOffset, get_YAxisOffset, put_YAxisOffset, VT_R8)
  DISP_PROPERTY_EX_ID(CaCurve, "XAxisZeroAdjust", dispidXAxisZeroAdjust, get_XAxisZeroAdjust, put_XAxisZeroAdjust, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaCurve, "YAxisZeroAdjust", dispidYAxisZeroAdjust, get_YAxisZeroAdjust, put_YAxisZeroAdjust, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaCurve, "SymbolType", dispidSymbolType, get_SymbolType, put_SymbolType, VT_I4)
  DISP_FUNCTION_ID(CaCurve, "SetResult", dispidSetResult, SetResult, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CaCurve, "GetResult", dispidGetResult, GetResult, VT_EMPTY, VTS_I4 VTS_PBSTR VTS_PI4 VTS_PI4 VTS_PBSTR VTS_PBSTR)
  DISP_FUNCTION_ID(CaCurve, "SetResultOper", dispidSetResultOper, SetResultOper, VT_EMPTY, VTS_I4 VTS_BSTR)
  DISP_FUNCTION_ID(CaCurve, "GetResultOper", dispidGetResultOper, GetResultOper, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CaCurve, "SetCombinedCurve", dispidSetCombinedCurve, SetCombinedCurve, VT_EMPTY, VTS_BSTR VTS_UNKNOWN VTS_UNKNOWN VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaCurve, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaCurve, CCmdTarget)
  INTERFACE_PART(CaCurve, IID_ICurve, LocalClass)
END_INTERFACE_MAP()

// {FBA5FFE1-EE5F-4135-97E4-843C563E7D6B}
IMPLEMENT_OLECREATE(CaCurve, "FEDEM.Curve",
  0xfba5ffe1, 0xee5f, 0x4135, 0x97, 0xe4, 0x84, 0x3c, 0x56, 0x3e, 0x7d, 0x6b);


CaCurve::CaCurve(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  signalConnector.Connect(this);
}

CaCurve::~CaCurve(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaCurve::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaCurve::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

long CaCurve::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

IGraph* CaCurve::get_Parent()
{
  CA_CHECK(m_ptr);

  FmGraph* pParent = (FmGraph*)m_ptr->getOwnerGraph();
  if (pParent == NULL)
    return NULL;

  return (IGraph*)CaApplication::CreateCOMObjectWrapper(pParent);
}

long CaCurve::get_Mode()
{
  CA_CHECK(m_ptr);

  return m_ptr->usingInputMode();
}

BSTR CaCurve::get_Legend()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getLegend().c_str()));
}

void CaCurve::put_Legend(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setLegend(CaConvert(val));
  m_ptr->onChanged();
}

OLE_COLOR CaCurve::get_Color()
{
  CA_CHECK(m_ptr);

  const FmColor& v = m_ptr->getColor();
  return RGB(v[0] * 255.0f, v[1] * 255.0f, v[2] * 255.0f);
}

void CaCurve::put_Color(OLE_COLOR val)
{
  CA_CHECK(m_ptr);

  float r = (float)GetRValue(val) / 255.0f;
  float g = (float)GetGValue(val) / 255.0f;
  float b = (float)GetBValue(val) / 255.0f;
  m_ptr->setColor(r,g,b);
}

double CaCurve::get_TimeRangeMin()
{
  CA_CHECK(m_ptr);

  return m_ptr->getTimeRange().first;
}

void CaCurve::put_TimeRangeMin(double val)
{
  CA_CHECK(m_ptr);

  m_ptr->setTimeRange({val, m_ptr->getTimeRange().second});
}

double CaCurve::get_TimeRangeMax()
{
  CA_CHECK(m_ptr);

  return m_ptr->getTimeRange().second;
}

void CaCurve::put_TimeRangeMax(double val)
{
  CA_CHECK(m_ptr);

  m_ptr->setTimeRange({m_ptr->getTimeRange().first, val});
}

double CaCurve::get_XAxisScale()
{
  CA_CHECK(m_ptr);

  return m_ptr->getXScale();
}

void CaCurve::put_XAxisScale(double val)
{
  CA_CHECK(m_ptr);

  m_ptr->setScaleFactor(val, m_ptr->getYScale());
}

double CaCurve::get_YAxisScale()
{
  CA_CHECK(m_ptr);

  return m_ptr->getYScale();
}

void CaCurve::put_YAxisScale(double val)
{
  CA_CHECK(m_ptr);

  m_ptr->setScaleFactor(m_ptr->getXScale(), val);
}

double CaCurve::get_XAxisOffset()
{
  CA_CHECK(m_ptr);

  return m_ptr->getXOffset();
}

void CaCurve::put_XAxisOffset(double val)
{
  CA_CHECK(m_ptr);

  m_ptr->setOffset(val, m_ptr->getYOffset());
}

double CaCurve::get_YAxisOffset()
{
  CA_CHECK(m_ptr);

  return m_ptr->getYOffset();
}

void CaCurve::put_YAxisOffset(double val)
{
  CA_CHECK(m_ptr);

  m_ptr->setOffset(m_ptr->getXOffset(), val);
}

BOOL CaCurve::get_XAxisZeroAdjust()
{
  CA_CHECK(m_ptr);

  return m_ptr->getXZeroAdjust();
}

void CaCurve::put_XAxisZeroAdjust(BOOL val)
{
  CA_CHECK(m_ptr);

  m_ptr->setZeroAdjust(val, m_ptr->getYZeroAdjust());
}

BOOL CaCurve::get_YAxisZeroAdjust()
{
  CA_CHECK(m_ptr);

  return m_ptr->getYZeroAdjust();
}

void CaCurve::put_YAxisZeroAdjust(BOOL val)
{
  CA_CHECK(m_ptr);

  m_ptr->setZeroAdjust(m_ptr->getXZeroAdjust(), val);
}

long CaCurve::get_SymbolType()
{
  CA_CHECK(m_ptr);

  return m_ptr->getCurveSymbol();
}

void CaCurve::put_SymbolType(long val)
{
  CA_CHECK(m_ptr);

  m_ptr->setCurveSymbol(val);
}

void CaCurve::SetResult(int Axis, LPCTSTR OGType, long BaseID, long UserID,
                        LPCTSTR RefType, LPCTSTR DescPath)
{
  CA_CHECK(m_ptr);

  FFaResultDescription curveDesc(CaConvert(OGType).c_str(), BaseID, UserID);
  curveDesc.varRefType = CaConvert(RefType);

  std::string dcopy = CaConvert(DescPath);
  char* pstr = strtok(const_cast<char*>(dcopy.c_str()),";");
  for (; pstr; pstr = strtok(NULL,";"))
    curveDesc.varDescrPath.push_back(pstr);

  m_ptr->setResult(Axis,curveDesc);
}

void CaCurve::GetResult(int Axis, BSTR* OGType, long* BaseID, long* UserID,
                        BSTR* RefType, BSTR* DescPath)
{
  CA_CHECK(m_ptr);

  const FFaResultDescription& curveDesc = m_ptr->getResult(Axis);
  *OGType = SysAllocString(CA2W(curveDesc.OGType.c_str()));
  *BaseID = curveDesc.baseId;
  *UserID = curveDesc.userId;
  *RefType = SysAllocString(CA2W(curveDesc.varRefType.c_str()));
  std::string strDescPath;
  for (const std::string& path : curveDesc.varDescrPath)
    if (strDescPath.empty())
      strDescPath = path;
    else
      strDescPath += ";" + path;
  *DescPath = SysAllocString(CA2W(strDescPath.c_str()));
}

void CaCurve::SetResultOper(int Axis, LPCTSTR Oper)
{
  CA_CHECK(m_ptr);

  m_ptr->setResultOper(Axis, CaConvert(Oper));
}

BSTR CaCurve::GetResultOper(int Axis)
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getResultOper(Axis).c_str()));
}

void CaCurve::SetCombinedCurve(LPCTSTR Expression,
                               ICurve* A, ICurve* B,
                               const VARIANT FAR& C,
                               const VARIANT FAR& D,
                               const VARIANT FAR& E)
{
  CA_CHECK(m_ptr);

  // Set mode
  m_ptr->useInputMode(FmCurveSet::COMB_CURVES);

  // Set expression
  m_ptr->setExpression(CaConvert(Expression));

  // Lambda function for assigning a component curve
  auto&& setCurve = [this](IUnknown* crv, int icomp)
  {
    IDispatch* pDisp1 = NULL;
    crv->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(E_INVALIDARG);
    CaCurve* pCa = dynamic_cast<CaCurve*>(CaCurve::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCa == NULL || pCa->m_ptr == NULL)
      AfxThrowOleException(E_INVALIDARG);
    else
      m_ptr->setCurveComp(pCa->m_ptr, icomp);
  };

  // Set curve components
  if (A != NULL)
    setCurve(A,0);
  if (B != NULL)
    setCurve(B,1);
  if (C.vt == VT_UNKNOWN)
    setCurve(C.punkVal,2);
  if (D.vt == VT_UNKNOWN)
    setCurve(D.punkVal,3);
  if (E.vt == VT_UNKNOWN)
    setCurve(E.punkVal,4);
}

void CaCurve::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaCurve::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaCurve::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaCurve::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaCurve::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaCurve::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaCurve::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaCurve::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaCurve, LocalClass)
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

STDMETHODIMP CaCurve::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::get_Parent(IGraph** ppObj)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::get_Mode(long* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_Mode();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_Legend(BSTR* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_Legend();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_Legend(BSTR val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_Legend(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_Color(OLE_COLOR* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_Color();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_Color(OLE_COLOR val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_Color(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_TimeRangeMin(double* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::put_TimeRangeMin(double val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::get_TimeRangeMax(double* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::put_TimeRangeMax(double val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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

STDMETHODIMP CaCurve::XLocalClass::get_XAxisScale(double* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_XAxisScale();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_XAxisScale(double val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_XAxisScale(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_YAxisScale(double* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_YAxisScale();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_YAxisScale(double val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_YAxisScale(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_XAxisOffset(double* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_XAxisOffset();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_XAxisOffset(double val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_XAxisOffset(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_YAxisOffset(double* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_YAxisOffset();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_YAxisOffset(double val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_YAxisOffset(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_XAxisZeroAdjust(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_XAxisZeroAdjust();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_XAxisZeroAdjust(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_XAxisZeroAdjust(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_YAxisZeroAdjust(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_YAxisZeroAdjust();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_YAxisZeroAdjust(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_YAxisZeroAdjust(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::get_SymbolType(long* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->get_SymbolType();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::put_SymbolType(long val)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->put_SymbolType(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::SetResult(int Axis, BSTR OGType, long BaseID, long UserID,
  BSTR RefType, BSTR DescPath)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->SetResult(Axis, OGType, BaseID, UserID, RefType, DescPath);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::GetResult(int Axis, BSTR* OGType, long* BaseID, long* UserID,
  BSTR* RefType, BSTR* DescPath)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->GetResult(Axis, OGType, BaseID, UserID, RefType, DescPath);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::SetResultOper(int Axis, BSTR Oper)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->SetResultOper(Axis, Oper);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::GetResultOper(int Axis, BSTR* pVal)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    *pVal = pThis->GetResultOper(Axis);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::SetCombinedCurve(BSTR Expression, ICurve* A, ICurve* B,
      VARIANT C, VARIANT D, VARIANT E)
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
  TRY
  {
    pThis->SetCombinedCurve(Expression, A, B, C, D, E);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCurve::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaCurve, LocalClass);
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
