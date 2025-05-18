// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaStrainRosette.h"
#include "CaPart.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmStrainRosette.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaStrainRosette, CCmdTarget)

BEGIN_MESSAGE_MAP(CaStrainRosette, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaStrainRosette, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "ElasticModulus", dispidElasticModulus, get_ElasticModulus, put_ElasticModulus, VT_R8)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "PoissonRatio", dispidPoissonRatio, get_PoissonRatio, put_PoissonRatio, VT_R8)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "UseFEThickness", dispidUseFEThickness, get_UseFEThickness, put_UseFEThickness, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "UseFEMaterial", dispidUseFEMaterial, get_UseFEMaterial, put_UseFEMaterial, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "StartStrainsToZero", dispidStartStrainsToZero, get_StartStrainsToZero, put_StartStrainsToZero, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "StrainRosetteType", dispidStrainRosetteType, get_StrainRosetteType, put_StrainRosetteType, VT_I4)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "AngleOffset", dispidAngleOffset, get_AngleOffset, put_AngleOffset, VT_R8)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "LayerHeight", dispidLayerHeight, get_LayerHeight, put_LayerHeight, VT_R8)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Node1", dispidNode1, get_Node1, put_Node1, VT_I4)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Node2", dispidNode2, get_Node2, put_Node2, VT_I4)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Node3", dispidNode3, get_Node3, put_Node3, VT_I4)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Node4", dispidNode4, get_Node4, put_Node4, VT_I4)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Part", dispidPart, get_Part, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaStrainRosette, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaStrainRosette, "GetReferenceDirection", dispidGetReferenceDirection, GetReferenceDirection, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaStrainRosette, "SetReferenceDirection", dispidSetReferenceDirection, SetReferenceDirection, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaStrainRosette, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaStrainRosette, CCmdTarget)
  INTERFACE_PART(CaStrainRosette, IID_IStrainRosette, LocalClass)
END_INTERFACE_MAP()

// {EDB34935-C286-4942-A504-4349297F0CB5}
IMPLEMENT_OLECREATE(CaStrainRosette, "FEDEM.StrainRosette",
0xedb34935, 0xc286, 0x4942, 0xa5, 0x4, 0x43, 0x49, 0x29, 0x7f, 0xc, 0xb5);


CaStrainRosette::CaStrainRosette(void) : m_ptr(m_pStrainRosette)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pStrainRosette = NULL;
  signalConnector.Connect(this);
}

CaStrainRosette::~CaStrainRosette(void)
{
  ::AfxOleUnlockApp();
  m_pStrainRosette = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaStrainRosette::get_Description()
{
  CA_CHECK(m_pStrainRosette);

  return SysAllocString(CA2W(m_pStrainRosette->getUserDescription().c_str()));
}

void CaStrainRosette::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->setUserDescription(CaConvert(val));
  m_pStrainRosette->onChanged();
}

long CaStrainRosette::get_BaseID()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->getBaseID();
}

double CaStrainRosette::get_ElasticModulus()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->getEMod();
}

void CaStrainRosette::put_ElasticModulus(double val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->EMod.setValue(val);
}

double CaStrainRosette::get_PoissonRatio()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->getNu();
}

void CaStrainRosette::put_PoissonRatio(double val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->nu.setValue(val);
}

BOOL CaStrainRosette::get_UseFEThickness()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->useFEThickness.getValue();
}

void CaStrainRosette::put_UseFEThickness(BOOL val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->useFEThickness.setValue(val);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

BOOL CaStrainRosette::get_UseFEMaterial()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->useFEMaterial.getValue();
}

void CaStrainRosette::put_UseFEMaterial(BOOL val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->useFEMaterial.setValue(val);
}

StrainRosetteType CaStrainRosette::get_StrainRosetteType()
{
  CA_CHECK(m_pStrainRosette);

  switch (m_pStrainRosette->rosetteType.getValue())
    {
    case FmStrainRosette::RosetteType::SINGLE_GAGE:
      return StrainRosetteType::srSingleGage;
    case FmStrainRosette::RosetteType::DOUBLE_GAGE_90:
      return StrainRosetteType::srDoubleGage90;
    case FmStrainRosette::RosetteType::TRIPLE_GAGE_45:
      return StrainRosetteType::srTripleGage45;
    case FmStrainRosette::RosetteType::TRIPLE_GAGE_60:
      return StrainRosetteType::srTripleGage60;
    default:
      return StrainRosetteType::srSingleGage;
    }
}

void CaStrainRosette::put_StrainRosetteType(StrainRosetteType val)
{
  CA_CHECK(m_pStrainRosette);

  switch (val)
    {
    case StrainRosetteType::srSingleGage:
      m_pStrainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::SINGLE_GAGE);
      break;
    case StrainRosetteType::srDoubleGage90:
      m_pStrainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::DOUBLE_GAGE_90);
      break;
    case StrainRosetteType::srTripleGage45:
      m_pStrainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::TRIPLE_GAGE_45);
      break;
    case StrainRosetteType::srTripleGage60:
      m_pStrainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::TRIPLE_GAGE_60);
      break;
    default:
      m_pStrainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::SINGLE_GAGE);
      break;
    }

  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

double CaStrainRosette::get_AngleOffset()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->angle.getValue();
}

void CaStrainRosette::put_AngleOffset(double val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->angle.setValue(val);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

double CaStrainRosette::get_LayerHeight()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->zPos.getValue();
}

void CaStrainRosette::put_LayerHeight(double val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->zPos.setValue(val);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

long CaStrainRosette::get_Node1()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->node1.getValue();
}

void CaStrainRosette::put_Node1(long val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->setNode(val,1);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

long CaStrainRosette::get_Node2()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->node2.getValue();
}

void CaStrainRosette::put_Node2(long val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->setNode(val,2);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

long CaStrainRosette::get_Node3()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->node3.getValue();
}

void CaStrainRosette::put_Node3(long val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->setNode(val,3);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

long CaStrainRosette::get_Node4()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->node4.getValue();
}

void CaStrainRosette::put_Node4(long val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->setNode(val,4);
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}

IPart* CaStrainRosette::get_Part()
{
  CA_CHECK(m_pStrainRosette);

  // Create triad wrapper
  CaPart* pCaPart = (CaPart*)CaPart::CreateObject();
  if (pCaPart == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get its property
  pCaPart->m_pGenPart = m_pStrainRosette->rosetteLink.getPointer();
  if (pCaPart->m_pGenPart == NULL)
    return NULL;

  // Return the interface
  IPart* pIPart = NULL;
  LPDISPATCH pDisp = pCaPart->GetIDispatch(false);
  pDisp->QueryInterface(IID_IPart, (void**)&pIPart);
  pCaPart->InternalRelease();
  return pIPart;
}

ISubAssembly* CaStrainRosette::get_Parent()
{
  CA_CHECK(m_pStrainRosette);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pStrainRosette->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaStrainRosette::GetReferenceDirection(double* X, double* Y, double* Z)
{
  CA_CHECK(m_pStrainRosette);

  FaVec3 vec = m_pStrainRosette->angleOriginVector.getValue();
  *X = vec.x();
  *Y = vec.y();
  *Z = vec.z();
}

void CaStrainRosette::SetReferenceDirection(double X, double Y, double Z)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->angleOriginVector.setValue(FaVec3(X,Y,Z));
  m_pStrainRosette->onChanged();
  m_pStrainRosette->draw();
}


BOOL CaStrainRosette::get_StartStrainsToZero()
{
  CA_CHECK(m_pStrainRosette);

  return m_pStrainRosette->removeStartStrains.getValue();
}

void CaStrainRosette::put_StartStrainsToZero(BOOL val)
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->removeStartStrains.setValue(val);
}


void CaStrainRosette::Delete()
{
  CA_CHECK(m_pStrainRosette);

  m_pStrainRosette->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaStrainRosette::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaStrainRosette::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaStrainRosette::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaStrainRosette::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaStrainRosette::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaStrainRosette::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}

STDMETHODIMP CaStrainRosette::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass)
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

STDMETHODIMP CaStrainRosette::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
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

STDMETHODIMP CaStrainRosette::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
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

STDMETHODIMP CaStrainRosette::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
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

STDMETHODIMP CaStrainRosette::XLocalClass::get_ElasticModulus(double* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_ElasticModulus();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_ElasticModulus(double val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_ElasticModulus(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_PoissonRatio(double* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_PoissonRatio();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_PoissonRatio(double val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_PoissonRatio(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_UseFEThickness(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_UseFEThickness();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_UseFEThickness(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_UseFEThickness(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_UseFEMaterial(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_UseFEMaterial();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_UseFEMaterial(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_UseFEMaterial(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_StrainRosetteType(StrainRosetteType* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_StrainRosetteType();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_StrainRosetteType(StrainRosetteType val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_StrainRosetteType(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_AngleOffset(double* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_AngleOffset();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_AngleOffset(double val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_AngleOffset(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_LayerHeight(double* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_LayerHeight();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_LayerHeight(double val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_LayerHeight(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_Node1(long* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_Node1();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_Node1(long val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_Node1(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_Node2(long* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_Node2();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_Node2(long val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_Node2(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_Node3(long* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_Node3();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_Node3(long val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_Node3(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_Node4(long* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_Node4();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_Node4(long val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_Node4(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_Part(IPart** pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_Part();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
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

STDMETHODIMP CaStrainRosette::XLocalClass::GetReferenceDirection(double* X, double* Y, double* Z)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->GetReferenceDirection(X, Y, Z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::SetReferenceDirection(double X, double Y, double Z)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->SetReferenceDirection(X, Y, Z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::get_StartStrainsToZero(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    *pVal = pThis->get_StartStrainsToZero();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaStrainRosette::XLocalClass::put_StartStrainsToZero(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
  TRY
  {
    pThis->put_StartStrainsToZero(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}


STDMETHODIMP CaStrainRosette::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaStrainRosette, LocalClass);
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
