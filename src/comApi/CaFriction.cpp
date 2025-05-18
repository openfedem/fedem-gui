// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaFriction.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmRotFriction.H"
#include "vpmDB/FmPrismaticFriction.H"
#include "vpmDB/FmBearingFriction.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaFriction, CCmdTarget)

BEGIN_MESSAGE_MAP(CaFriction, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaFriction, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaFriction, "PrestressForce", dispidPrestressForce, get_PrestressForce, put_PrestressForce, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "CoulombCoefficient", dispidCoulombCoefficient, get_CoulombCoefficient, put_CoulombCoefficient, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "StribeckMagnitude", dispidStribeckMagnitude, get_StribeckMagnitude, put_StribeckMagnitude, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "CriticalStribeckSpeed", dispidCriticalStribeckSpeed, get_CriticalStribeckSpeed, put_CriticalStribeckSpeed, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "Radius", dispidRadius, get_Radius, put_Radius, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "BendingCapacityDistance", dispidBendingCapacityDistance, get_BendingCapacityDistance, put_BendingCapacityDistance, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "BearingConstant", dispidBearingConstant, get_BearingConstant, put_BearingConstant, VT_R8)
  DISP_PROPERTY_EX_ID(CaFriction, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaFriction, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaFriction, CCmdTarget)
  INTERFACE_PART(CaFriction, IID_IFriction, LocalClass)
END_INTERFACE_MAP()

// {FE15FDAE-9D67-47a9-B72E-E7106EE3D19B}
IMPLEMENT_OLECREATE(CaFriction, "FEDEM.Friction", 
0xfe15fdae, 0x9d67, 0x47a9, 0xb7, 0x2e, 0xe7, 0x10, 0x6e, 0xe3, 0xd1, 0x9b);


CaFriction::CaFriction(void) : m_ptr(m_pFriction)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pFriction = NULL;
  signalConnector.Connect(this);
}

CaFriction::~CaFriction(void)
{
  ::AfxOleUnlockApp();
  m_pFriction = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaFriction::get_PrestressForce()
{
  CA_CHECK(m_pFriction);
  
  return m_pFriction->getPrestressLoad();
}

void CaFriction::put_PrestressForce(double val)
{
  CA_CHECK(m_pFriction);
  
  m_pFriction->setPrestressLoad(val);
}

double CaFriction::get_CoulombCoefficient()
{
  CA_CHECK(m_pFriction);
  
  return m_pFriction->getCoulombCoeff();
}

void CaFriction::put_CoulombCoefficient(double val)
{
  CA_CHECK(m_pFriction);
  
  m_pFriction->setCoulombCoeff(val);
}

double CaFriction::get_StribeckMagnitude()
{
  CA_CHECK(m_pFriction);
  
  return m_pFriction->getStribeckMagn();
}

void CaFriction::put_StribeckMagnitude(double val)
{
  CA_CHECK(m_pFriction);
  
  m_pFriction->setStribeckMagn(val);
}

double CaFriction::get_CriticalStribeckSpeed()
{
  CA_CHECK(m_pFriction);
  
  return m_pFriction->getStribeckSpeed();
}

void CaFriction::put_CriticalStribeckSpeed(double val)
{
  CA_CHECK(m_pFriction);
  
  m_pFriction->setStribeckSpeed(val);
}

double CaFriction::get_Radius()
{
  CA_CHECK(m_pFriction);
  
  FmRotFriction* pRotFric = dynamic_cast<FmRotFriction*>(m_pFriction);
  if(pRotFric){
    return pRotFric->getRadius();
  }
  FmPrismaticFriction* pPrismFric = dynamic_cast<FmPrismaticFriction*>(m_pFriction);
  if(pPrismFric){
    return pPrismFric->getRadius();
  }
  return 0;
}

void CaFriction::put_Radius(double val)
{
  CA_CHECK(m_pFriction);
  
  FmRotFriction* pRotFric = dynamic_cast<FmRotFriction*>(m_pFriction);
  if(pRotFric){
    pRotFric->setRadius(val);
  }
  FmPrismaticFriction* pPrismFric = dynamic_cast<FmPrismaticFriction*>(m_pFriction);
  if(pPrismFric){
    pPrismFric->setRadius(val);
  }
}

double CaFriction::get_BendingCapacityDistance()
{
  CA_CHECK(m_pFriction);

  FmBearingFriction* pBearFric = dynamic_cast<FmBearingFriction*>(m_pFriction);
  if(pBearFric){
    return pBearFric->getCapacity();
  }
  return 0;
}

void CaFriction::put_BendingCapacityDistance(double val)
{
  CA_CHECK(m_pFriction);

  FmBearingFriction* pBearFric = dynamic_cast<FmBearingFriction*>(m_pFriction);
  if(pBearFric){
    pBearFric->setCapacity(val);
  }
}

double CaFriction::get_BearingConstant()
{
  CA_CHECK(m_pFriction);

  FmBearingFriction* pBearFric = dynamic_cast<FmBearingFriction*>(m_pFriction);
  if(pBearFric){
    return pBearFric->getConstant();
  }
  FmPrismaticFriction* pPrismFric = dynamic_cast<FmPrismaticFriction*>(m_pFriction);
  if(pPrismFric){
    return pPrismFric->getConstant();
  }
  return 0;
}

void CaFriction::put_BearingConstant(double val)
{
  CA_CHECK(m_pFriction);

 FmBearingFriction* pBearFric = dynamic_cast<FmBearingFriction*>(m_pFriction);
  if(pBearFric){
    pBearFric->setConstant(val);
  }
  FmPrismaticFriction* pPrismFric = dynamic_cast<FmPrismaticFriction*>(m_pFriction);
  if(pPrismFric){
    pPrismFric->setConstant(val);
  }
}

BSTR CaFriction::get_Description()
{
  CA_CHECK(m_pFriction);
  return SysAllocString(CA2W(m_pFriction->getUserDescription().c_str()));
}

void CaFriction::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pFriction);

  m_pFriction->setUserDescription(CaConvert(val));
  m_pFriction->onChanged();
}

ISubAssembly* CaFriction::get_Parent()
{
  CA_CHECK(m_pFriction);
  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pFriction->getParentAssembly();
  if (pParent == NULL)
      return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaFriction::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaFriction::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaFriction::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaFriction::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaFriction::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaFriction::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid) 
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, 
          lcid, rgdispid);
}
STDMETHODIMP CaFriction::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaFriction, LocalClass)
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

STDMETHODIMP CaFriction::XLocalClass::get_PrestressForce(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_PrestressForce();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_PrestressForce(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_PrestressForce(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_CoulombCoefficient(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_CoulombCoefficient();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_CoulombCoefficient(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_CoulombCoefficient(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_StribeckMagnitude(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_StribeckMagnitude();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_StribeckMagnitude(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_StribeckMagnitude(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_CriticalStribeckSpeed(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_CriticalStribeckSpeed();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_CriticalStribeckSpeed(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_CriticalStribeckSpeed(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_Radius(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_Radius();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_Radius(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_Radius(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_BendingCapacityDistance(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_BendingCapacityDistance();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_BendingCapacityDistance(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_BendingCapacityDistance(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_BearingConstant(double* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    *pVal = pThis->get_BearingConstant();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::put_BearingConstant(double val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
  TRY
  {
    pThis->put_BearingConstant(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaFriction::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
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

STDMETHODIMP CaFriction::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
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

STDMETHODIMP CaFriction::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaFriction, LocalClass);
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
