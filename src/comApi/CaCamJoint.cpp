// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaCamJoint.h"
#include "CaFriction.h"
#include "CaTriad.h"
#include "CaDOF.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmCamFriction.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up
IMPLEMENT_DYNCREATE(CaCamJoint, CCmdTarget)

BEGIN_MESSAGE_MAP(CaCamJoint, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaCamJoint, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaCamJoint, "X", dispidX, get_X, put_X, VT_R8)
  DISP_PROPERTY_EX_ID(CaCamJoint, "Y", dispidY, get_Y, put_Y, VT_R8)
  DISP_PROPERTY_EX_ID(CaCamJoint, "Z", dispidZ, get_Z, put_Z, VT_R8)
  DISP_PROPERTY_EX_ID(CaCamJoint, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_PARAM_ID(CaCamJoint, "DOF", dispidDOF, get_DOF, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaCamJoint, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaCamJoint, "Thickness", dispidThickness, get_Thickness, put_Thickness, VT_R8)
  DISP_PROPERTY_EX_ID(CaCamJoint, "Width", dispidWidth, get_Width, put_Width, VT_R8)
  DISP_PROPERTY_EX_ID(CaCamJoint, "UseRadialStiffness", dispidUseRadialStiffness, get_UseRadialStiffness, put_UseRadialStiffness, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaCamJoint, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaCamJoint, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCamJoint, "SetEulerRotationZYX", dispidSetEulerRotationZYX, SetEulerRotationZYX, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCamJoint, "GetRotationMatrix", dispidGetRotationMatrix, GetRotationMatrix, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaCamJoint, "SetRotationMatrix", dispidSetRotationMatrix, SetRotationMatrix, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaCamJoint, "SetPosition", dispidSetPosition, SetPosition, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCamJoint, "GetFriction", dispidGetFriction, GetFriction, VT_UNKNOWN, VTS_PI4)
  DISP_FUNCTION_ID(CaCamJoint, "SetFriction", dispidSetFriction, SetFriction, VT_EMPTY, VTS_I4 VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaCamJoint, "AddMasterTriad", dispidAddMasterTriad, AddMasterTriad, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaCamJoint, "SetMasterArcFromJoint", dispidSetMasterArcFromJoint, SetMasterArcFromJoint, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaCamJoint, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaCamJoint, CCmdTarget)
  INTERFACE_PART(CaCamJoint, IID_ICamJoint, LocalClass)
END_INTERFACE_MAP()

// {B42A03CA-D9C7-4652-AD8C-B8132C9FFA1B}
IMPLEMENT_OLECREATE(CaCamJoint, "FEDEM.CamJoint",
0xb42a03ca, 0xd9c7, 0x4652, 0xad, 0x8c, 0xb8, 0x13, 0x2c, 0x9f, 0xfa, 0x1b);



CaCamJoint::CaCamJoint(void) : m_ptr(m_pCamJoint)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pCamJoint = NULL;
  signalConnector.Connect(this);
}

CaCamJoint::~CaCamJoint(void)
{
  ::AfxOleUnlockApp();
  m_pCamJoint = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaCamJoint::get_X()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->getTranslation().x();
}

void CaCamJoint::put_X(double val)
{
  CA_CHECK(m_pCamJoint);

  FaVec3 p = m_pCamJoint->getTranslation();
  p.x(val);
  m_pCamJoint->setTranslation(p);

  m_pCamJoint->onChanged();
  m_pCamJoint->draw();
}

double CaCamJoint::get_Y()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->getTranslation().y();
}

void CaCamJoint::put_Y(double val)
{
  CA_CHECK(m_pCamJoint);

  FaVec3 p = m_pCamJoint->getTranslation();
  p.y(val);
  m_pCamJoint->setTranslation(p);

  m_pCamJoint->onChanged();
  m_pCamJoint->draw();
}

double CaCamJoint::get_Z()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->getTranslation().z();
}

void CaCamJoint::put_Z(double val)
{
  CA_CHECK(m_pCamJoint);

  FaVec3 p = m_pCamJoint->getTranslation();
  p.z(val);
  m_pCamJoint->setTranslation(p);

  m_pCamJoint->onChanged();
  m_pCamJoint->draw();
}

BSTR CaCamJoint::get_Description()
{
  CA_CHECK(m_pCamJoint);

  return SysAllocString(CA2W(m_pCamJoint->getUserDescription().c_str()));
}

void CaCamJoint::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pCamJoint);

  m_pCamJoint->setUserDescription(CaConvert(val));
  m_pCamJoint->onChanged();
}

IDOF* CaCamJoint::get_DOF(long nDOFIndex)
{
  CA_CHECK(m_pCamJoint);

  // Create DOF wrapper
  CaDOF* pCaDOF = (CaDOF*)CaDOF::CreateObject();
  if (pCaDOF == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set the main properties
  pCaDOF->m_pDOF = m_pCamJoint;
  pCaDOF->m_nDOFIndex = nDOFIndex;

  // Return the interface
  IDOF* pIDOF = NULL;
  LPDISPATCH pDisp = pCaDOF->GetIDispatch(false);
  pDisp->QueryInterface(IID_IDOF, (void**)&pIDOF);
  pCaDOF->InternalRelease();
  return pIDOF;
}

long CaCamJoint::get_BaseID()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->getBaseID();
}

double CaCamJoint::get_Thickness()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->getThickness();
}

void CaCamJoint::put_Thickness(double val)
{
  CA_CHECK(m_pCamJoint);

  m_pCamJoint->setThickness(val);

  m_pCamJoint->onChanged();
}

double CaCamJoint::get_Width()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->getWidth();
}

void CaCamJoint::put_Width(double val)
{
  CA_CHECK(m_pCamJoint);

  m_pCamJoint->setWidth(val);

  m_pCamJoint->onChanged();
}

BOOL CaCamJoint::get_UseRadialStiffness()
{
  CA_CHECK(m_pCamJoint);

  return m_pCamJoint->isUsingRadialContact();
}

void CaCamJoint::put_UseRadialStiffness(BOOL val)
{
  CA_CHECK(m_pCamJoint);

  m_pCamJoint->setUsingRadialContact(val);

  m_pCamJoint->onChanged();
}

ISubAssembly* CaCamJoint::get_Parent()
{
  CA_CHECK(m_pCamJoint);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pCamJoint->getParentAssembly();
  if (pParent == NULL)
    return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaCamJoint::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  CA_CHECK(m_pCamJoint);

  FaVec3 r = m_pCamJoint->getOrientation().getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

void CaCamJoint::SetEulerRotationZYX(double rx, double ry, double rz)
{
  CA_CHECK(m_pCamJoint);

  FaMat33 m;
  m.eulerRotateZYX(FaVec3(rx,ry,rz));
  m_pCamJoint->setOrientation(m);

  m_pCamJoint->onChanged();
  m_pCamJoint->draw();
}

void CaCamJoint::GetRotationMatrix(VARIANT* Array3x3)
{
  CA_CHECK(m_pCamJoint);

  CaApplication::CreateSafeArray(Array3x3, m_pCamJoint->getOrientation());
}

void CaCamJoint::SetRotationMatrix(const VARIANT FAR& Array3x3)
{
  CA_CHECK(m_pCamJoint);

  FaMat33 m;
  CaApplication::GetFromSafeArray(Array3x3, m);
  m_pCamJoint->setOrientation(m);

  m_pCamJoint->onChanged();
  m_pCamJoint->draw();
}

void CaCamJoint::SetPosition(double x, double y, double z)
{
  CA_CHECK(m_pCamJoint);

  FaMat34 m = m_pCamJoint->getGlobalCS();
  m[3] = FaVec3(x,y,z);
  m_pCamJoint->setGlobalCS(m);

  m_pCamJoint->onChanged();
  m_pCamJoint->draw();
}

IFriction* CaCamJoint::GetFriction(long* nDOFIndex)
{
  CA_CHECK(m_pCamJoint);

  if (nDOFIndex == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Get DOF index
  *nDOFIndex = m_pCamJoint->getFrictionDof() + 1;

  // Get
  FmFrictionBase* pFrict = m_pCamJoint->getFriction();
  if (pFrict == NULL)
    return NULL;

  CaFriction* pCaFriction = (CaFriction*)CaFriction::CreateObject();
  if (pCaFriction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set
  pCaFriction->m_pFriction = pFrict;

  // Return
  IFriction* pIFriction = NULL;
  LPDISPATCH pDisp = pCaFriction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFriction, (void**)&pIFriction);
  pCaFriction->InternalRelease();
  return pIFriction;
}

void CaCamJoint::SetFriction(long nDOFIndex, IFriction* Friction)
{
  CA_CHECK(m_pCamJoint);

  if (nDOFIndex == -1)
    AfxThrowOleException(CO_E_OBJNOTCONNECTED);
  if ((nDOFIndex < 1) || (nDOFIndex > 6))
    AfxThrowOleException(ERROR_INVALID_INDEX);

  // Get friction
  if (Friction == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Friction->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaFriction* pCaFriction = dynamic_cast<CaFriction*>(CaFriction::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaFriction == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmCamFriction* fric = dynamic_cast<FmCamFriction*>(pCaFriction->m_pFriction);
  if (fric == NULL)
    AfxThrowOleException(E_INVALIDARG);

  m_pCamJoint->setFriction(fric);
  m_pCamJoint->setFrictionDof(nDOFIndex-1);

  m_pCamJoint->onChanged();
}

void CaCamJoint::AddMasterTriad(ITriad* MasterTriad)
{
  CA_CHECK(m_pCamJoint);

  // Get master triad
  if (MasterTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  MasterTriad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaMasterTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaMasterTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* tm = pCaMasterTriad->m_pTriad;
  if (tm == NULL)
    AfxThrowOleException(E_INVALIDARG);

  m_pCamJoint->addAsMasterTriad(tm);
  m_pCamJoint->setDefaultRotationOnMasters();

  m_pCamJoint->onChanged();
}

void CaCamJoint::SetMasterArcFromJoint(ICamJoint* CamJoint)
{
  CA_CHECK(m_pCamJoint);

  // Get cam joint
  if (CamJoint == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  CamJoint->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaCamJoint* pCaCamJoint = dynamic_cast<CaCamJoint*>(CaCamJoint::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaCamJoint == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmCamJoint* cj = pCaCamJoint->m_pCamJoint;
  if (cj == NULL)
    AfxThrowOleException(E_INVALIDARG);

  m_pCamJoint->setMaster(cj->getMaster());

  m_pCamJoint->onChanged();
}

void CaCamJoint::Delete()
{
  CA_CHECK(m_pCamJoint);

  m_pCamJoint->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaCamJoint::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaCamJoint::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaCamJoint::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaCamJoint::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaCamJoint::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaCamJoint::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaCamJoint::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass)
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

STDMETHODIMP CaCamJoint::XLocalClass::get_X(double* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::put_X(double val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::get_Y(double* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::put_Y(double val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::get_Z(double* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::put_Z(double val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::get_DOF(long DOFIndex, IDOF** ppRet)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    *ppRet = pThis->get_DOF(DOFIndex);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::get_Thickness(double* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    *pVal = pThis->get_Thickness();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::put_Thickness(double val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->put_Thickness(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::get_Width(double* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    *pVal = pThis->get_Width();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::put_Width(double val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->put_Width(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::get_UseRadialStiffness(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    *pVal = pThis->get_UseRadialStiffness();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::put_UseRadialStiffness(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->put_UseRadialStiffness(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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

STDMETHODIMP CaCamJoint::XLocalClass::SetEulerRotationZYX(double rx, double ry, double rz)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->SetEulerRotationZYX(rx,ry,rz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::GetRotationMatrix(VARIANT* Array3x3)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->GetRotationMatrix(Array3x3);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::SetRotationMatrix(VARIANT Array3x3)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->SetRotationMatrix(Array3x3);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::SetPosition(double x, double y, double z)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->SetPosition(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::GetFriction(long* DOFIndex, IFriction** pRet)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    *pRet = pThis->GetFriction(DOFIndex);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::SetFriction(long DOFIndex, IFriction* Friction)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->SetFriction(DOFIndex,Friction);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::AddMasterTriad(ITriad* MasterTriad)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->AddMasterTriad(MasterTriad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::SetMasterArcFromJoint(ICamJoint* CamJoint)
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
  TRY
  {
    pThis->SetMasterArcFromJoint(CamJoint);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCamJoint::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaCamJoint, LocalClass);
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
