// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaCylindricJoint.h"
#include "CaTriad.h"
#include "CaDOF.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmDB.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaCylindricJoint, CCmdTarget)

BEGIN_MESSAGE_MAP(CaCylindricJoint, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaCylindricJoint, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "X", dispidX, get_X, put_X, VT_R8)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "Y", dispidY, get_Y, put_Y, VT_R8)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "Z", dispidZ, get_Z, put_Z, VT_R8)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_PARAM_ID(CaCylindricJoint, "DOF", dispidDOF, get_DOF, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "ScrewRatio", dispidScrewRatio, get_ScrewRatio, put_ScrewRatio, VT_R8)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "ScrewTransmission", dispidScrewTransmission, get_ScrewTransmission, put_ScrewTransmission, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaCylindricJoint, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaCylindricJoint, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCylindricJoint, "SetEulerRotationZYX", dispidSetEulerRotationZYX, SetEulerRotationZYX, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCylindricJoint, "GetRotationMatrix", dispidGetRotationMatrix, GetRotationMatrix, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaCylindricJoint, "SetRotationMatrix", dispidSetRotationMatrix, SetRotationMatrix, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaCylindricJoint, "SetPosition", dispidSetPosition, SetPosition, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCylindricJoint, "AddMasterTriad", dispidAddMasterTriad, AddMasterTriad, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaCylindricJoint, "SetMasterArcFromJoint", dispidSetMasterArcFromJoint, SetMasterArcFromJoint, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaCylindricJoint, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaCylindricJoint, CCmdTarget)
  INTERFACE_PART(CaCylindricJoint, IID_ICylindricJoint, LocalClass)
END_INTERFACE_MAP()

// {D80355AF-3FC6-41f6-B5E5-AD83414E75A6}
IMPLEMENT_OLECREATE(CaCylindricJoint, "FEDEM.CylindricJoint",
0xd80355af, 0x3fc6, 0x41f6, 0xb5, 0xe5, 0xad, 0x83, 0x41, 0x4e, 0x75, 0xa6);


CaCylindricJoint::CaCylindricJoint(void) : m_ptr(m_pCylJoint)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pCylJoint = NULL;
  signalConnector.Connect(this);
}

CaCylindricJoint::~CaCylindricJoint(void)
{
  ::AfxOleUnlockApp();
  m_pCylJoint = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaCylindricJoint::get_X()
{
  CA_CHECK(m_pCylJoint);

  return m_pCylJoint->getTranslation().x();
}

void CaCylindricJoint::put_X(double val)
{
  CA_CHECK(m_pCylJoint);

  FaVec3 p = m_pCylJoint->getTranslation();
  p.x(val);
  m_pCylJoint->setTranslation(p);

  m_pCylJoint->onChanged();
  m_pCylJoint->draw();
}

double CaCylindricJoint::get_Y()
{
  CA_CHECK(m_pCylJoint);

  return m_pCylJoint->getTranslation().y();
}

void CaCylindricJoint::put_Y(double val)
{
  CA_CHECK(m_pCylJoint);

  FaVec3 p = m_pCylJoint->getTranslation();
  p.y(val);
  m_pCylJoint->setTranslation(p);

  m_pCylJoint->onChanged();
  m_pCylJoint->draw();
}

double CaCylindricJoint::get_Z()
{
  CA_CHECK(m_pCylJoint);

  return m_pCylJoint->getTranslation().z();
}

void CaCylindricJoint::put_Z(double val)
{
  CA_CHECK(m_pCylJoint);

  FaVec3 p = m_pCylJoint->getTranslation();
  p.z(val);
  m_pCylJoint->setTranslation(p);

  m_pCylJoint->onChanged();
  m_pCylJoint->draw();
}

BSTR CaCylindricJoint::get_Description()
{
  CA_CHECK(m_pCylJoint);

  return SysAllocString(CA2W(m_pCylJoint->getUserDescription().c_str()));
}

void CaCylindricJoint::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pCylJoint);

  m_pCylJoint->setUserDescription(CaConvert(val));
  m_pCylJoint->onChanged();
}

IDOF* CaCylindricJoint::get_DOF(long nDOFIndex)
{
  CA_CHECK(m_pCylJoint);

  // Create DOF wrapper
  CaDOF* pCaDOF = (CaDOF*)CaDOF::CreateObject();
  if (pCaDOF == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set the main properties
  pCaDOF->m_pDOF = m_pCylJoint;
  pCaDOF->m_nDOFIndex = nDOFIndex;

  // Return the interface
  IDOF* pIDOF = NULL;
  LPDISPATCH pDisp = pCaDOF->GetIDispatch(false);
  pDisp->QueryInterface(IID_IDOF, (void**)&pIDOF);
  pCaDOF->InternalRelease();
  return pIDOF;
}

long CaCylindricJoint::get_BaseID()
{
  CA_CHECK(m_pCylJoint);

  return m_pCylJoint->getBaseID();
}

double CaCylindricJoint::get_ScrewRatio()
{
  CA_CHECK(m_pCylJoint);

  return m_pCylJoint->getScrewRatio();
}

void CaCylindricJoint::put_ScrewRatio(double val)
{
  CA_CHECK(m_pCylJoint);

  m_pCylJoint->setScrewRatio(val);

  m_pCylJoint->onChanged();
}

BOOL CaCylindricJoint::get_ScrewTransmission()
{
  CA_CHECK(m_pCylJoint);

  return m_pCylJoint->isScrewTransmission();
}

void CaCylindricJoint::put_ScrewTransmission(BOOL val)
{
  CA_CHECK(m_pCylJoint);

  if (val)
    m_pCylJoint->setAsScrewTransmission();
  else
    m_pCylJoint->unsetAsScrewTransmission();

  m_pCylJoint->onChanged();
}

ISubAssembly* CaCylindricJoint::get_Parent()
{
  CA_CHECK(m_pCylJoint);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pCylJoint->getParentAssembly();
  if (pParent == NULL)
    return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

void CaCylindricJoint::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  CA_CHECK(m_pCylJoint);

  FaVec3 r = m_pCylJoint->getOrientation().getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

void CaCylindricJoint::SetEulerRotationZYX(double rx, double ry, double rz)
{
  CA_CHECK(m_pCylJoint);

  FaMat33 m;
  m.eulerRotateZYX(FaVec3(rx,ry,rz));
  m_pCylJoint->setOrientation(m);

  m_pCylJoint->onChanged();
  m_pCylJoint->draw();
}

void CaCylindricJoint::GetRotationMatrix(VARIANT* Array3x3)
{
  CA_CHECK(m_pCylJoint);

  CaApplication::CreateSafeArray(Array3x3, m_pCylJoint->getOrientation());
}

void CaCylindricJoint::SetRotationMatrix(const VARIANT FAR& Array3x3)
{
  CA_CHECK(m_pCylJoint);

  FaMat33 m;
  CaApplication::GetFromSafeArray(Array3x3, m);
  m_pCylJoint->setOrientation(m);

  m_pCylJoint->onChanged();
  m_pCylJoint->draw();
}

void CaCylindricJoint::SetPosition(double x, double y, double z)
{
  CA_CHECK(m_pCylJoint);

  FaMat34 m = m_pCylJoint->getGlobalCS();
  m[3] = FaVec3(x,y,z);
  m_pCylJoint->setGlobalCS(m);

  m_pCylJoint->onChanged();
  m_pCylJoint->draw();
}

void CaCylindricJoint::AddMasterTriad(ITriad* MasterTriad)
{
  CA_CHECK(m_pCylJoint);

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

  Fm1DMaster* master = m_pCylJoint->getMaster();
  if (master == NULL)
    AfxThrowOleException(E_POINTER);

  // If one master triad present, set up cylindrical joint with the new triad
  if (master->size() == 1)
  {
    FmTriad* firstMaster = master->getFirstTriad();
    FmTriad* lastMaster = tm;

    FaVec3 zAxis(lastMaster->getTranslation() - firstMaster->getTranslation());
    double zLen = zAxis.length();
    if (zLen > FmDB::getPositionTolerance())
    {
      zAxis /= zLen;

      // Find an arbitrary X-axis direction which is normal to the Z-axis
      FaVec3 xAxis;
      if (hypot(zAxis.y(),zAxis.x()) > FmDB::getPositionTolerance())
        xAxis = FaVec3(-zAxis.y(),zAxis.x(),0.0); // {0,0,1} x zAxis;
      else
        xAxis = FaVec3(zAxis.z(),0.0,-zAxis.x()); // {0,1,0} x zAxis

      FaVec3 yAxis = zAxis ^ xAxis.normalize();
      FaMat33 orientation(xAxis,yAxis,zAxis);

      firstMaster->setOrientation(orientation);
      lastMaster->setOrientation(orientation);
      m_pCylJoint->getSlaveTriad()->setOrientation(orientation);

      if (master->addTriad(lastMaster))
      {
        firstMaster->draw();
        lastMaster->draw();
        m_pCylJoint->getSlaveTriad()->draw();
        m_pCylJoint->draw();
      }
    }
  }

  // If two or more master triads present, try to add the new one
  else if (master->addTriad(tm) && master->size() > 1)
  {
    tm->draw();
    m_pCylJoint->draw();
  }
}

void CaCylindricJoint::SetMasterArcFromJoint(ICylindricJoint* CylJoint)
{
  CA_CHECK(m_pCylJoint);

  // Get cyl joint
  if (CylJoint == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  CylJoint->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaCylindricJoint* pCaCylJoint = dynamic_cast<CaCylindricJoint*>(CaCylindricJoint::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaCylJoint == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmCylJoint* cj = pCaCylJoint->m_pCylJoint;
  if (cj == NULL)
    AfxThrowOleException(E_INVALIDARG);

  m_pCylJoint->setMaster(cj->getMaster());

  m_pCylJoint->onChanged();
}

void CaCylindricJoint::Delete()
{
  CA_CHECK(m_pCylJoint);

  m_pCylJoint->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaCylindricJoint::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaCylindricJoint::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaCylindricJoint::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaCylindricJoint::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaCylindricJoint::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaCylindricJoint::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaCylindricJoint::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass)
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_X(double* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::put_X(double val)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_Y(double* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::put_Y(double val)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_Z(double* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::put_Z(double val)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_DOF(long DOFIndex, IDOF** ppRet)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::get_ScrewRatio(double* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
  TRY
  {
    *pVal = pThis->get_ScrewRatio();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCylindricJoint::XLocalClass::put_ScrewRatio(double val)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
  TRY
  {
    pThis->put_ScrewRatio(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCylindricJoint::XLocalClass::get_ScrewTransmission(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
  TRY
  {
    *pVal = pThis->get_ScrewTransmission();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCylindricJoint::XLocalClass::put_ScrewTransmission(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
  TRY
  {
    pThis->put_ScrewTransmission(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCylindricJoint::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::SetEulerRotationZYX(double rx, double ry, double rz)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::GetRotationMatrix(VARIANT* Array3x3)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::SetRotationMatrix(VARIANT Array3x3)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::SetPosition(double x, double y, double z)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::AddMasterTriad(ITriad* MasterTriad)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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

STDMETHODIMP CaCylindricJoint::XLocalClass::SetMasterArcFromJoint(ICylindricJoint* CylJoint)
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
  TRY
  {
    pThis->SetMasterArcFromJoint(CylJoint);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCylindricJoint::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaCylindricJoint, LocalClass);
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
