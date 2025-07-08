// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaBeam.h"
#include "CaTriad.h"
#include "CaCrossSection.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmBeam.H"
#include "vpmDB/FmBeamProperty.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaBeam, CCmdTarget)

BEGIN_MESSAGE_MAP(CaBeam, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaBeam, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaBeam, "Mass", dispidMass, get_Mass, SetNotSupported, VT_R8)
  DISP_PROPERTY_EX_ID(CaBeam, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaBeam, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaBeam, "Visualize3D", dispidVisualize3D, get_Visualize3D, put_Visualize3D, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaBeam, "Visualize3DStartAngle", dispidVisualize3DStartAngle, get_Visualize3DStartAngle, put_Visualize3DStartAngle, VT_R8)
  DISP_PROPERTY_EX_ID(CaBeam, "Visualize3DStopAngle", dispidVisualize3DStopAngle, get_Visualize3DStopAngle, put_Visualize3DStopAngle, VT_R8)
  DISP_PROPERTY_EX_ID(CaBeam, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaBeam, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaBeam, "GetTriad1", dispidGetTriad1, GetTriad1, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaBeam, "SetTriad1", dispidSetTriad1, SetTriad1, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaBeam, "GetTriad2", dispidGetTriad2, GetTriad2, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaBeam, "SetTriad2", dispidSetTriad2, SetTriad2, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaBeam, "GetCrossSection", dispidGetCrossSection, GetCrossSection, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaBeam, "SetCrossSection", dispidSetCrossSection, SetCrossSection, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaBeam, "SetLocalZaxis", dispidSetLocalZaxis, SetLocalZaxis, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaBeam, "GetGlobalCS", dispidGetGlobalCS, GetGlobalCS, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaBeam, "GetLocalCS", dispidGetLocalCS, GetLocalCS, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaBeam, "GetStructuralDamping", dispidGetStructuralDamping, GetStructuralDamping, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaBeam, "SetStructuralDamping", dispidSetStructuralDamping, SetStructuralDamping, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaBeam, "GetScaling", dispidGetScaling, GetScaling, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaBeam, "SetScaling", dispidSetScaling, SetScaling, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaBeam, "GetLength", dispidGetScaling, GetScaling, VT_R8, VTS_NONE)
  DISP_FUNCTION_ID(CaBeam, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaBeam, CCmdTarget)
  INTERFACE_PART(CaBeam, IID_IBeam, LocalClass)
END_INTERFACE_MAP()

// {E26DB2AA-021D-48c2-BA67-593A4E1C2802}
IMPLEMENT_OLECREATE(CaBeam, "FEDEM.Beam",
0xe26db2aa, 0x21d, 0x48c2, 0xba, 0x67, 0x59, 0x3a, 0x4e, 0x1c, 0x28, 0x2);


CaBeam::CaBeam(void) : m_ptr(m_pBeam)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pBeam = NULL;
  signalConnector.Connect(this);
}

CaBeam::~CaBeam(void)
{
  ::AfxOleUnlockApp();
  m_pBeam = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaBeam::get_Mass()
{
  CA_CHECK(m_pBeam);

  return m_pBeam->getMass();
}

BSTR CaBeam::get_Description()
{
  CA_CHECK(m_pBeam);

  return SysAllocString(CA2W(m_pBeam->getUserDescription().c_str()));
}

void CaBeam::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pBeam);

  m_pBeam->setUserDescription(CaConvert(val));
  m_pBeam->onChanged();
}

long CaBeam::get_BaseID()
{
  CA_CHECK(m_pBeam);

  return m_pBeam->getBaseID();
}

BOOL CaBeam::get_Visualize3D()
{
  CA_CHECK(m_pBeam);

  return m_pBeam->myVisualize3D.getValue();
}

void CaBeam::put_Visualize3D(BOOL val)
{
  CA_CHECK(m_pBeam);

  m_pBeam->myVisualize3D.setValue(val);
  m_pBeam->draw();
}

double CaBeam::get_Visualize3DStartAngle()
{
  CA_CHECK(m_pBeam);

  return m_pBeam->myVisualize3DAngles.getValue().first;
}

void CaBeam::put_Visualize3DStartAngle(double val)
{
  CA_CHECK(m_pBeam);

  m_pBeam->myVisualize3DAngles.getValue().first = val;
  m_pBeam->draw();
}

double CaBeam::get_Visualize3DStopAngle()
{
  CA_CHECK(m_pBeam);

  return m_pBeam->myVisualize3DAngles.getValue().second;
}

void CaBeam::put_Visualize3DStopAngle(double val)
{
  CA_CHECK(m_pBeam);

  m_pBeam->myVisualize3DAngles.getValue().second = val;
  m_pBeam->draw();
}

ISubAssembly* CaBeam::get_Parent()
{
  CA_CHECK(m_pBeam);
  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pBeam->getParentAssembly();
  if (pParent == NULL)
      return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaBeam::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  CA_CHECK(m_pBeam);

  FaVec3 r = m_pBeam->getOrientation().getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

ITriad* CaBeam::GetTriad1()
{
  CA_CHECK(m_pBeam);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set its main property
  pCaTriad->m_pTriad = m_pBeam->getFirstTriad();
  if (pCaTriad->m_pTriad == NULL)
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaBeam::SetTriad1(ITriad* Triad)
{
  CA_CHECK(m_pBeam);

  // Get triad
  if (Triad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaTriad == NULL || pCaTriad->m_pTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  FmTriad* pOldTriad = m_pBeam->getFirstTriad();

  m_pBeam->setTriad(pCaTriad->m_pTriad, 0);
  m_pBeam->onChanged();
  m_pBeam->getSecondTriad()->draw();
  pOldTriad->draw();
  m_pBeam->draw();
}

ITriad* CaBeam::GetTriad2()
{
  CA_CHECK(m_pBeam);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set its main property
  pCaTriad->m_pTriad = m_pBeam->getSecondTriad();
  if (pCaTriad->m_pTriad == NULL)
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaBeam::SetTriad2(ITriad* Triad)
{
  CA_CHECK(m_pBeam);
  
  // Get triad
  if (Triad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaTriad == NULL || pCaTriad->m_pTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  FmTriad* pOldTriad = m_pBeam->getSecondTriad();

  m_pBeam->setTriad(pCaTriad->m_pTriad,1);
  m_pBeam->onChanged();
  m_pBeam->getSecondTriad()->draw();
  pOldTriad->draw();
  m_pBeam->draw();
}

ICrossSection* CaBeam::GetCrossSection()
{
  CA_CHECK(m_pBeam);

  // Create cross section wrapper
  CaCrossSection* pCaCrossSection = (CaCrossSection*)CaCrossSection::CreateObject();
  if (pCaCrossSection == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set its main property
  pCaCrossSection->m_pCrossSection = dynamic_cast<FmBeamProperty*>(m_pBeam->getProperty());
  if (pCaCrossSection->m_pCrossSection == NULL)
    return NULL;

  // Return the interface
  ICrossSection* pICrossSection = NULL;
  LPDISPATCH pDisp = pCaCrossSection->GetIDispatch(false);
  pDisp->QueryInterface(IID_ICrossSection, (void**)&pICrossSection);
  pCaCrossSection->InternalRelease();
  return pICrossSection;
}

void CaBeam::SetCrossSection(ICrossSection* CrossSection)
{
  CA_CHECK(m_pBeam);

  if (CrossSection == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  CrossSection->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaCrossSection* pCaCrossSection = dynamic_cast<CaCrossSection*>(CaCrossSection::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaCrossSection == NULL || pCaCrossSection->m_pCrossSection == NULL)
    AfxThrowOleException(E_INVALIDARG);

  m_pBeam->setProperty(pCaCrossSection->m_pCrossSection);
  m_pBeam->onChanged();
  m_pBeam->draw();
}

void CaBeam::SetLocalZaxis(double x, double y, double z)
{
  CA_CHECK(m_pBeam);

  m_pBeam->setOrientation(FaVec3(x,y,z));
  m_pBeam->draw();
}

void CaBeam::GetGlobalCS(VARIANT* Array4x3)
{
  CA_CHECK(m_pBeam);

  CaApplication::CreateSafeArray(Array4x3, m_pBeam->getGlobalCS());
}

void CaBeam::GetLocalCS(VARIANT* Array4x3)
{
  CA_CHECK(m_pBeam);

  CaApplication::CreateSafeArray(Array4x3, m_pBeam->getLocalCS());
}

void CaBeam::GetStructuralDamping(double* MassProp, double* StiffProp)
{
  CA_CHECK(m_pBeam);

  *MassProp = m_pBeam->alpha1.getValue();
  *StiffProp = m_pBeam->alpha2.getValue();
}

void CaBeam::SetStructuralDamping(double MassProp, double StiffProp)
{
  CA_CHECK(m_pBeam);

  m_pBeam->alpha1.setValue(MassProp);
  m_pBeam->alpha2.setValue(StiffProp);
}

void CaBeam::GetScaling(double* StiffScale, double* MassScale)
{
  CA_CHECK(m_pBeam);

  *StiffScale = m_pBeam->stiffnessScale.getValue();
  *MassScale = m_pBeam->massScale.getValue();
}

void CaBeam::SetScaling(double StiffScale, double MassScale)
{
  CA_CHECK(m_pBeam);

  m_pBeam->stiffnessScale = StiffScale;
  m_pBeam->massScale = MassScale;
}

double CaBeam::GetLength()
{
  CA_CHECK(m_pBeam);

  return m_pBeam->getLength();
}

void CaBeam::Delete()
{
  CA_CHECK(m_pBeam);

  m_pBeam->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaBeam::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
          return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaBeam::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
          return pThis->ExternalRelease();
}
STDMETHODIMP CaBeam::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
          return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaBeam::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaBeam::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaBeam::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaBeam::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaBeam, LocalClass)
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

STDMETHODIMP CaBeam::XLocalClass::get_Mass(double* pVal)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *pVal = pThis->get_Mass();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
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

STDMETHODIMP CaBeam::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
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

STDMETHODIMP CaBeam::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
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

STDMETHODIMP CaBeam::XLocalClass::get_Visualize3D(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *pVal = pThis->get_Visualize3D();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
    return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::put_Visualize3D(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->put_Visualize3D(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::get_Visualize3DStartAngle(double* pVal)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *pVal = pThis->get_Visualize3DStartAngle();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::put_Visualize3DStartAngle(double val)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->put_Visualize3DStartAngle(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::get_Visualize3DStopAngle(double* pVal)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *pVal = pThis->get_Visualize3DStopAngle();
  }
  CATCH_ALL(e)
    {
      return ResultFromScode(COleException::Process(e));
    }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::put_Visualize3DStopAngle(double val)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->put_Visualize3DStopAngle(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
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

STDMETHODIMP CaBeam::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->GetEulerRotationZYX(rx,ry,rz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetTriad1(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *ppRet = pThis->GetTriad1();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::SetTriad1(ITriad* Triad)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->SetTriad1(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetTriad2(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *ppRet = pThis->GetTriad2();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::SetTriad2(ITriad* Triad)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->SetTriad2(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetCrossSection(ICrossSection** ppRet)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *ppRet = pThis->GetCrossSection();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::SetCrossSection(ICrossSection* CrossSection)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->SetCrossSection(CrossSection);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::SetLocalZaxis(double x, double y, double z)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->SetLocalZaxis(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetGlobalCS(VARIANT* Array4x3)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->GetGlobalCS(Array4x3);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetLocalCS(VARIANT* Array4x3)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->GetLocalCS(Array4x3);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetStructuralDamping(double* MassProp, double* StiffProp)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->GetStructuralDamping(MassProp, StiffProp);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::SetStructuralDamping(double MassProp, double StiffProp)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->SetStructuralDamping(MassProp, StiffProp);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetScaling(double* StiffScale, double* MassScale)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->GetScaling(StiffScale, MassScale);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::SetScaling(double StiffScale, double MassScale)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    pThis->SetScaling(StiffScale, MassScale);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::GetLength(double* pRet)
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
  TRY
  {
    *pRet = pThis->GetLength();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaBeam::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaBeam, LocalClass);
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
