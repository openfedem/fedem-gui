// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaSimulationEvent.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmfWaveSinus.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaSimulationEvent, CCmdTarget)

BEGIN_MESSAGE_MAP(CaSimulationEvent, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaSimulationEvent, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaSimulationEvent, "ID", dispidID, get_ID, put_ID, VT_I4)
  DISP_PROPERTY_EX_ID(CaSimulationEvent, "Probability", dispidProbability, get_Probability, put_Probability, VT_R8)
  DISP_PROPERTY_EX_ID(CaSimulationEvent, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_FUNCTION_ID(CaSimulationEvent, "GetFRSFileNames", dispidGetFRSFileNames, GetFRSFileNames, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CaSimulationEvent, "Activate", dispidActivate, Activate, VT_EMPTY, VTS_BOOL VTS_BOOL)
  DISP_FUNCTION_ID(CaSimulationEvent, "AddPropertyValue", dispidAddPropertyValue, AddPropertyValue, VT_EMPTY, VTS_DISPATCH VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CaSimulationEvent, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaSimulationEvent, CCmdTarget)
  INTERFACE_PART(CaSimulationEvent, IID_ISimulationEvent, LocalClass)
END_INTERFACE_MAP()

// {E70CCA6B-F639-4e24-A451-47479DFA42C1}
IMPLEMENT_OLECREATE(CaSimulationEvent, "FEDEM.SimulationEvent",
0xe70cca6b, 0xf639, 0x4e24, 0xa4, 0x51, 0x47, 0x47, 0x9d, 0xfa, 0x42, 0xc1);


CaSimulationEvent::CaSimulationEvent(void) : m_ptr(m_pSimulationEvent)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pSimulationEvent = NULL;
  signalConnector.Connect(this);
}

CaSimulationEvent::~CaSimulationEvent(void)
{
  ::AfxOleUnlockApp();
  m_pSimulationEvent = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

long CaSimulationEvent::get_ID()
{
  CA_CHECK(m_pSimulationEvent);
  return m_pSimulationEvent->getID();
}

void CaSimulationEvent::put_ID(long val)
{
  CA_CHECK(m_pSimulationEvent);
  m_pSimulationEvent->setID(val);
  m_pSimulationEvent->onChanged();
}

double CaSimulationEvent::get_Probability()
{
  CA_CHECK(m_pSimulationEvent);
  return m_pSimulationEvent->getProbability();
}

void CaSimulationEvent::put_Probability(double val)
{
  CA_CHECK(m_pSimulationEvent);
  m_pSimulationEvent->setProbability(val);
  m_pSimulationEvent->onChanged();
}

BSTR CaSimulationEvent::get_Description()
{
  CA_CHECK(m_pSimulationEvent);
  return SysAllocString(CA2W(m_pSimulationEvent->getUserDescription().c_str()));
}

void CaSimulationEvent::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pSimulationEvent);

  m_pSimulationEvent->setUserDescription(CaConvert(val));
  m_pSimulationEvent->onChanged();
}

BSTR CaSimulationEvent::GetFRSFileNames()
{
  CA_CHECK(m_pSimulationEvent);
  FmResultStatusData* rsd = m_pSimulationEvent->getResultStatusData();
  return SysAllocString(CA2W(rsd->getFileNames("frs").c_str()));
}

void CaSimulationEvent::Activate(BOOL Activate, BOOL Notify)
{
  CA_CHECK(m_pSimulationEvent);
  m_pSimulationEvent->activate(Activate, Notify);
}

void CaSimulationEvent::AddPropertyValue(IDispatch* ModelObject, LPCTSTR PropertyName, LPCTSTR Value)
{
  CA_CHECK(m_pSimulationEvent);

  // Get arguments
  FmSimulationModelBase* modelObj =
    dynamic_cast<FmSimulationModelBase*>(CaApplication::GetFromCOMObjectWrapper(ModelObject));
  std::string fieldName = CaConvert(PropertyName);
  std::string fieldValue = CaConvert(Value);

  // Field name mapping for Jonswap wave spectrum functions
  FmfWaveSpectrum* pWaveFunc = dynamic_cast<FmfWaveSpectrum*>(modelObj);
  if ((pWaveFunc != NULL) && (pWaveFunc->spectrum.getValue() == FmfWaveSpectrum::FmSpectrum::JONSWAP)) {
    if (_wcsicmp(PropertyName, L"Hs") == 0)
      fieldName = "SIGNIFICANT_WAVE_HEIGHT";
    else if (_wcsicmp(PropertyName, L"Tp") == 0)
      fieldName = "PEAK_PERIOD";
    else if (_wcsicmp(PropertyName, L"Gamma") == 0)
      fieldName = "SPECTRAL_PEAKEDNESS";
    else if (_wcsicmp(PropertyName, L"N") == 0)
      fieldName = "WAVE_COMPONENTS";
    else if (_wcsicmp(PropertyName, L"RndSeed") == 0)
      fieldName = "RANDOM_SEED";
    else if (_wcsicmp(PropertyName, L"TRange") == 0)
      fieldName = "PERIOD_RANGE";
    else if (_wcsicmp(PropertyName, L"TRangeMin") == 0)
      AfxThrowOleException(E_INVALIDARG); // Safeguard
    else if (_wcsicmp(PropertyName, L"TRangeMax") == 0)
      AfxThrowOleException(E_INVALIDARG); // Safeguard
    else if (_wcsicmp(PropertyName, L"AutoCalcGamma") == 0)
      fieldName = "AUTO_CALC_SPECTRAL_PEAKEDNESS";
    else if (_wcsicmp(PropertyName, L"AutoCalcTRange") == 0)
      fieldName = "AUTO_CALC_PERIOD_RANGE";
  }

  if (!m_pSimulationEvent->addFieldValue(modelObj, fieldName, fieldValue))
    AfxThrowOleException(CO_E_ERRORINAPP);

  m_pSimulationEvent->initAfterResolve();
}

void CaSimulationEvent::Delete()
{
  CA_CHECK(m_pSimulationEvent);
  m_pSimulationEvent->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaSimulationEvent::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaSimulationEvent::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaSimulationEvent::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaSimulationEvent::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaSimulationEvent::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaSimulationEvent::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaSimulationEvent::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass)
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

STDMETHODIMP CaSimulationEvent::XLocalClass::get_ID(long* pVal)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    *pVal = pThis->get_ID();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::put_ID(long val)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    pThis->put_ID(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::get_Probability(double* pVal)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    *pVal = pThis->get_Probability();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::put_Probability(double val)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    pThis->put_Probability(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
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

STDMETHODIMP CaSimulationEvent::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
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

STDMETHODIMP CaSimulationEvent::XLocalClass::GetFRSFileNames(BSTR* pVal)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    *pVal = pThis->GetFRSFileNames();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::Activate(VARIANT_BOOL Activate, VARIANT_BOOL Notify)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    pThis->Activate(Activate, Notify);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::AddPropertyValue(IDispatch* ModelObject, BSTR PropertyName, BSTR Value)
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
  TRY
  {
    pThis->AddPropertyValue(ModelObject, PropertyName, Value);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaSimulationEvent::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaSimulationEvent, LocalClass);
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
