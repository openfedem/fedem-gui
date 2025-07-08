// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaJoint.h"
#include "CaFriction.h"
#include "CaTriad.h"
#include "CaDOF.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmFreeJoint.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up
IMPLEMENT_DYNCREATE(CaJoint, CCmdTarget)

BEGIN_MESSAGE_MAP(CaJoint, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaJoint, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaJoint, "X", dispidX, get_X, put_X, VT_R8)
  DISP_PROPERTY_EX_ID(CaJoint, "Y", dispidY, get_Y, put_Y, VT_R8)
  DISP_PROPERTY_EX_ID(CaJoint, "Z", dispidZ, get_Z, put_Z, VT_R8)
  DISP_PROPERTY_EX_ID(CaJoint, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_PARAM_ID(CaJoint, "DOF", dispidDOF, get_DOF, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaJoint, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaJoint, "MasterMovedAlong", dispidMasterMovedAlong, get_MasterMovedAlong, put_MasterMovedAlong, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaJoint, "SlaveMovedAlong", dispidSlaveMovedAlong, get_SlaveMovedAlong, put_SlaveMovedAlong, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaJoint, "TranslationalCoupling", dispidTranslationalCoupling, get_TranslationalCoupling, put_TranslationalCoupling, VT_I4)
  DISP_PROPERTY_EX_ID(CaJoint, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaJoint, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaJoint, "SetEulerRotationZYX", dispidSetEulerRotationZYX, SetEulerRotationZYX, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaJoint, "GetRotationMatrix", dispidGetRotationMatrix, GetRotationMatrix, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaJoint, "SetRotationMatrix", dispidSetRotationMatrix, SetRotationMatrix, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaJoint, "SetPosition", dispidSetPosition, SetPosition, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaJoint, "GetFriction", dispidGetFriction, GetFriction, VT_UNKNOWN, VTS_PI4)
  DISP_FUNCTION_ID(CaJoint, "SetFriction", dispidSetFriction, SetFriction, VT_EMPTY, VTS_I4 VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaJoint, "GetMasterTriad", dispidGetMasterTriad, GetMasterTriad, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaJoint, "SetMasterTriad", dispidSetMasterTriad, SetMasterTriad, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaJoint, "GetSlaveTriad", dispidGetSlaveTriad, GetSlaveTriad, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaJoint, "SetSlaveTriad", dispidSetSlaveTriad, SetSlaveTriad, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaJoint, "GetGlobalCS", dispidGetGlobalCS, GetGlobalCS, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaJoint, "SetGlobalCS", dispidSetGlobalCS, SetGlobalCS, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaJoint, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaJoint, CCmdTarget)
  INTERFACE_PART(CaJoint, IID_IJoint, LocalClass)
END_INTERFACE_MAP()

// {D79E8353-985F-42a7-98D1-5DB2BED4DA7B}
IMPLEMENT_OLECREATE(CaJoint, "FEDEM.Joint",
0xd79e8353, 0x985f, 0x42a7, 0x98, 0xd1, 0x5d, 0xb2, 0xbe, 0xd4, 0xda, 0x7b);


CaJoint::CaJoint(void) : m_ptr(m_pJoint)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pJoint = NULL;
  signalConnector.Connect(this);
}

CaJoint::~CaJoint(void)
{
  ::AfxOleUnlockApp();
  m_pJoint = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaJoint::get_X()
{
  CA_CHECK(m_pJoint);

  return m_pJoint->getTranslation().x();
}

void CaJoint::put_X(double val)
{
  CA_CHECK(m_pJoint);

  FaVec3 p = m_pJoint->getTranslation();
  p.x(val);
  m_pJoint->setTranslation(p);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

double CaJoint::get_Y()
{
  CA_CHECK(m_pJoint);

  return m_pJoint->getTranslation().y();
}

void CaJoint::put_Y(double val)
{
  CA_CHECK(m_pJoint);

  FaVec3 p = m_pJoint->getTranslation();
  p.y(val);
  m_pJoint->setTranslation(p);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

double CaJoint::get_Z()
{
  CA_CHECK(m_pJoint);

  return m_pJoint->getTranslation().z();
}

void CaJoint::put_Z(double val)
{
  CA_CHECK(m_pJoint);

  FaVec3 p = m_pJoint->getTranslation();
  p.z(val);
  m_pJoint->setTranslation(p);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

BSTR CaJoint::get_Description()
{
  CA_CHECK(m_pJoint);

  return SysAllocString(CA2W(m_pJoint->getUserDescription().c_str()));
}

void CaJoint::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pJoint);

  m_pJoint->setUserDescription(CaConvert(val));
  m_pJoint->onChanged();
}

IDOF* CaJoint::get_DOF(long nDOFIndex)
{
  CA_CHECK(m_pJoint);

  // Create DOF wrapper
  CaDOF* pCaDOF = (CaDOF*)CaDOF::CreateObject();
  if (pCaDOF == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set the main properties
  pCaDOF->m_pDOF = m_pJoint;
  pCaDOF->m_nDOFIndex = nDOFIndex;

  // Return the interface
  IDOF* pIDOF = NULL;
  LPDISPATCH pDisp = pCaDOF->GetIDispatch(false);
  pDisp->QueryInterface(IID_IDOF, (void**)&pIDOF);
  pCaDOF->InternalRelease();
  return pIDOF;
}

long CaJoint::get_BaseID()
{
  CA_CHECK(m_pJoint);

  return m_pJoint->getBaseID();
}

BOOL CaJoint::get_MasterMovedAlong()
{
  CA_CHECK(m_pJoint);

  return m_pJoint->isMasterMovedAlong();
}

void CaJoint::put_MasterMovedAlong(BOOL val)
{
  CA_CHECK(m_pJoint);

  m_pJoint->setMasterMovedAlong(val);
}

BOOL CaJoint::get_SlaveMovedAlong()
{
  CA_CHECK(m_pJoint);

  return m_pJoint->isSlaveMovedAlong();
}

void CaJoint::put_SlaveMovedAlong(BOOL val)
{
  CA_CHECK(m_pJoint);

  m_pJoint->setSlaveMovedAlong(val);
}

SpringCouplingType CaJoint::get_TranslationalCoupling()
{
  CA_CHECK(m_pJoint);

  return (SpringCouplingType)(long)m_pJoint->tranSpringCpl.getValue();
}

void CaJoint::put_TranslationalCoupling(SpringCouplingType val)
{
  CA_CHECK(m_pJoint);

  m_pJoint->tranSpringCpl.setValue((FmJointBase::SpringCplType)(long)val);
}

ISubAssembly* CaJoint::get_Parent()
{
  CA_CHECK(m_pJoint);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pJoint->getParentAssembly();
  if (pParent == NULL)
    return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaJoint::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  CA_CHECK(m_pJoint);

  FaVec3 r = m_pJoint->getOrientation().getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

void CaJoint::SetEulerRotationZYX(double rx, double ry, double rz)
{
  CA_CHECK(m_pJoint);

  FaMat33 m;
  m.eulerRotateZYX(FaVec3(rx,ry,rz));
  m_pJoint->setOrientation(m);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

void CaJoint::GetRotationMatrix(VARIANT* Array3x3)
{
  CA_CHECK(m_pJoint);

  CaApplication::CreateSafeArray(Array3x3, m_pJoint->getOrientation());
}

void CaJoint::SetRotationMatrix(const VARIANT FAR& Array3x3)
{
  CA_CHECK(m_pJoint);

  FaMat33 m;
  CaApplication::GetFromSafeArray(Array3x3, m);
  m_pJoint->setOrientation(m);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

void CaJoint::SetPosition(double x, double y, double z)
{
  CA_CHECK(m_pJoint);

  FaMat34 m = m_pJoint->getGlobalCS();
  m[3] = FaVec3(x,y,z);
  m_pJoint->setGlobalCS(m);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

IFriction* CaJoint::GetFriction(long* nDOFIndex)
{
  CA_CHECK(m_pJoint);

  if (nDOFIndex == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Get DOF index
  *nDOFIndex = m_pJoint->getFrictionDof() + 1;

  // Get
  FmFrictionBase* pFrict = m_pJoint->getFriction();
  if (pFrict == NULL)
    return NULL;

  // Create
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

void CaJoint::SetFriction(long nDOFIndex, IFriction* Friction)
{
  CA_CHECK(m_pJoint);

  if (nDOFIndex == -1)
    AfxThrowOleException(CO_E_OBJNOTCONNECTED);
  if ((nDOFIndex < 1) || (nDOFIndex > 6))
    AfxThrowOleException(ERROR_INVALID_INDEX);

   // Get friction
  FmFrictionBase* f = NULL;
  if (Friction != NULL) {
    IDispatch* pDisp1 = NULL;
    Friction->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(E_INVALIDARG);
    CaFriction* pCaFriction =
      dynamic_cast<CaFriction*>(CaFriction::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCaFriction == NULL)
      AfxThrowOleException(E_INVALIDARG);
    if (pCaFriction->m_pFriction == NULL)
      AfxThrowOleException(E_INVALIDARG);
    f = pCaFriction->m_pFriction;
  }

  // Set
  m_pJoint->setFriction(f);
  m_pJoint->setFrictionDof(nDOFIndex-1);

  m_pJoint->onChanged();
}

ITriad* CaJoint::GetMasterTriad()
{
  CA_CHECK(m_pJoint);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get its property
  pCaTriad->m_pTriad = m_pJoint->getItsMasterTriad();
  if (pCaTriad->m_pTriad == NULL)
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaJoint::SetMasterTriad(ITriad* Triad)
{
  CA_CHECK(m_pJoint);

  // Get triad
  if (Triad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  FmTriad* pOldTriad = m_pJoint->getItsMasterTriad();

  FmTriad* pTriad = pCaTriad->m_pTriad;
  if (dynamic_cast<FmTriad*>(pTriad) == NULL)
    AfxThrowOleException(E_INVALIDARG);
  m_pJoint->setAsMasterTriad(pTriad);

  m_pJoint->onChanged();
  m_pJoint->getSlaveTriad()->draw();
  if (pOldTriad)
    pOldTriad->draw();
  m_pJoint->draw();
}

ITriad* CaJoint::GetSlaveTriad()
{
  CA_CHECK(m_pJoint);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get its property
  pCaTriad->m_pTriad = m_pJoint->getSlaveTriad();
  if (pCaTriad->m_pTriad == NULL)
    return NULL;

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaJoint::SetSlaveTriad(ITriad* Triad)
{
  CA_CHECK(m_pJoint);

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

  FmTriad* pOldTriad = m_pJoint->getSlaveTriad();

  m_pJoint->setAsSlaveTriad(pCaTriad->m_pTriad);

  m_pJoint->onChanged();
  m_pJoint->getSlaveTriad()->draw();
  if (pOldTriad)
    pOldTriad->draw();
  m_pJoint->draw();
}

void CaJoint::GetGlobalCS(VARIANT* Array4x3)
{
  CA_CHECK(m_pJoint);

  CaApplication::CreateSafeArray(Array4x3, m_pJoint->getGlobalCS());
}

void CaJoint::SetGlobalCS(VARIANT Array4x3)
{
  CA_CHECK(m_pJoint);

  FaMat34 m;
  CaApplication::GetFromSafeArray(Array4x3, m);
  m_pJoint->setGlobalCS(m);

  m_pJoint->onChanged();
  m_pJoint->draw();
}

void CaJoint::Delete()
{
  CA_CHECK(m_pJoint);

  m_pJoint->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaJoint::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaJoint::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaJoint::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaJoint::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaJoint::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaJoint::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaJoint::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaJoint, LocalClass)
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

STDMETHODIMP CaJoint::XLocalClass::get_X(double* pVal)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::put_X(double val)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::get_Y(double* pVal)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::put_Y(double val)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::get_Z(double* pVal)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::put_Z(double val)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::get_DOF(long DOFIndex, IDOF** ppRet)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::get_MasterMovedAlong(VARIANT_BOOL* pVal)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		*pVal = pThis->get_MasterMovedAlong();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::put_MasterMovedAlong(VARIANT_BOOL val)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		pThis->put_MasterMovedAlong(val);
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::get_SlaveMovedAlong(VARIANT_BOOL* pVal)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		*pVal = pThis->get_SlaveMovedAlong();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::put_SlaveMovedAlong(VARIANT_BOOL val)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		pThis->put_SlaveMovedAlong(val);
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::get_TranslationalCoupling(SpringCouplingType* pVal)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		*pVal = pThis->get_TranslationalCoupling();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::put_TranslationalCoupling(SpringCouplingType val)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		pThis->put_TranslationalCoupling(val);
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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


STDMETHODIMP CaJoint::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::SetEulerRotationZYX(double rx, double ry, double rz)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::GetRotationMatrix(VARIANT* Array3x3)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::SetRotationMatrix(VARIANT Array3x3)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::SetPosition(double x, double y, double z)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::GetFriction(long* DOFIndex, IFriction** pRet)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::SetFriction(long DOFIndex, IFriction* Friction)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::GetMasterTriad(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
  TRY
  {
    *ppRet = pThis->GetMasterTriad();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::SetMasterTriad(ITriad* Triad)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
  TRY
  {
    pThis->SetMasterTriad(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::GetSlaveTriad(ITriad** ppRet)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
  TRY
  {
    *ppRet = pThis->GetSlaveTriad();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::SetSlaveTriad(ITriad* Triad)
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
  TRY
  {
    pThis->SetSlaveTriad(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::GetGlobalCS(VARIANT* Array4x3)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
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

STDMETHODIMP CaJoint::XLocalClass::SetGlobalCS(VARIANT Array4x3)
{
	METHOD_PROLOGUE(CaJoint, LocalClass);
	TRY
	{
		pThis->SetGlobalCS(Array4x3);
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaJoint::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaJoint, LocalClass);
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
