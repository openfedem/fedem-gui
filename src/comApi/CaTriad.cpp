// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaTriad.h"
#include "CaBeam.h"
#include "CaJoint.h"
#include "CaCamJoint.h"
#include "CaDOF.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmTriad.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmCamJoint.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaTriad, CCmdTarget)

BEGIN_MESSAGE_MAP(CaTriad, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaTriad, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaTriad, "X", dispidX, get_X, put_X, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Y", dispidY, get_Y, put_Y, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Z", dispidZ, get_Z, put_Z, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Mass", dispidMass, get_Mass, put_Mass, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Ix", dispidIx, get_Ix, put_Ix, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Iy", dispidIy, get_Iy, put_Iy, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Iz", dispidIz, get_Iz, put_Iz, VT_R8)
  DISP_PROPERTY_EX_ID(CaTriad, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_PARAM_ID(CaTriad, "DOF", dispidDOF, get_DOF, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaTriad, "ReferenceCS", dispidReferenceCS, get_ReferenceCS, put_ReferenceCS, VT_I4)
  DISP_PROPERTY_EX_ID(CaTriad, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaTriad, "Tag", dispidTag, get_Tag, put_Tag, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaTriad, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaTriad, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaTriad, "SetEulerRotationZYX", dispidSetEulerRotationZYX, SetEulerRotationZYX, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaTriad, "GetRotationMatrix", dispidGetRotationMatrix, GetRotationMatrix, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaTriad, "SetRotationMatrix", dispidSetRotationMatrix, SetRotationMatrix, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaTriad, "SetMass", dispidSetMass, SetMass, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaTriad, "SetPosition", dispidSetPosition, SetPosition, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaTriad, "GetAttachedObjects", dispidGetAttachedObjects, GetAttachedObjects, VT_VARIANT, VTS_I4)
  DISP_FUNCTION_ID(CaTriad, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaTriad, CCmdTarget)
  INTERFACE_PART(CaTriad, IID_ITriad, LocalClass)
END_INTERFACE_MAP()

// {BF7F6B25-75C0-4459-A9E3-CA77A7E00979}
IMPLEMENT_OLECREATE(CaTriad, "FEDEM.Triad",
0xbf7f6b25, 0x75c0, 0x4459, 0xa9, 0xe3, 0xca, 0x77, 0xa7, 0xe0, 0x9, 0x79);


CaTriad::CaTriad(void) : m_ptr(m_pTriad)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pTriad = NULL;
  signalConnector.Connect(this);
}

CaTriad::~CaTriad(void)
{
  ::AfxOleUnlockApp();
  m_pTriad = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaTriad::get_X()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getGlobalTranslation().x();
}

void CaTriad::put_X(double val)
{
  CA_CHECK(m_pTriad);

  FaMat34 m = m_pTriad->getGlobalCS();
  m[3].x(val);
  m_pTriad->setGlobalCS(m);

  m_pTriad->onChanged();
  m_pTriad->draw();
}

double CaTriad::get_Y()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getGlobalTranslation().y();
}

void CaTriad::put_Y(double val)
{
  CA_CHECK(m_pTriad);

  FaMat34 m = m_pTriad->getGlobalCS();
  m[3].y(val);
  m_pTriad->setGlobalCS(m);

  m_pTriad->onChanged();
  m_pTriad->draw();
}

double CaTriad::get_Z()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getGlobalTranslation().z();
}

void CaTriad::put_Z(double val)
{
  CA_CHECK(m_pTriad);

  FaMat34 m = m_pTriad->getGlobalCS();
  m[3].z(val);
  m_pTriad->setGlobalCS(m);

  m_pTriad->onChanged();
  m_pTriad->draw();
}

double CaTriad::get_Mass()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getAddMass();
}

void CaTriad::put_Mass(double val)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setAddedMass(val);
  m_pTriad->onChanged();
}

double CaTriad::get_Ix()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getAddMass(3);
}

void CaTriad::put_Ix(double val)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setAddMass(3, val);

  m_pTriad->onChanged();
}

double CaTriad::get_Iy()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getAddMass(4);
}

void CaTriad::put_Iy(double val)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setAddMass(4, val);

  m_pTriad->onChanged();
}

double CaTriad::get_Iz()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getAddMass(5);
}

void CaTriad::put_Iz(double val)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setAddMass(5, val);

  m_pTriad->onChanged();
}

BSTR CaTriad::get_Description()
{
  CA_CHECK(m_pTriad);

  return SysAllocString(CA2W(m_pTriad->getUserDescription().c_str()));
}

void CaTriad::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setUserDescription(CaConvert(val));
  m_pTriad->onChanged();
}

IDOF* CaTriad::get_DOF(long nDOFIndex)
{
  CA_CHECK(m_pTriad);

  // Create DOF wrapper
  CaDOF* pCaDOF = (CaDOF*)CaDOF::CreateObject();
  if (pCaDOF == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set the main properties
  pCaDOF->m_pDOF = m_pTriad;
  pCaDOF->m_nDOFIndex = nDOFIndex;

  // Return the interface
  IDOF* pIDOF = NULL;
  LPDISPATCH pDisp = pCaDOF->GetIDispatch(false);
  pDisp->QueryInterface(IID_IDOF, (void**)&pIDOF);
  pCaDOF->InternalRelease();
  return pIDOF;
}

ReferenceCSType CaTriad::get_ReferenceCS()
{
  CA_CHECK(m_pTriad);

  switch (m_pTriad->itsLocalDir.getValue()) {
  case FmTriad::GLOBAL:
    return rcsGlobal;
  case FmTriad::LOCAL_WITHROTATED:
    return rcsLocalWithRotated;
  default:
    return (ReferenceCSType)-1;
  }
}

void CaTriad::put_ReferenceCS(ReferenceCSType val)
{
  CA_CHECK(m_pTriad);

  if (val == rcsGlobal)
    m_pTriad->itsLocalDir.setValue(FmTriad::GLOBAL);
  else if (val == rcsLocalInitial)
    m_pTriad->itsLocalDir.setValue(FmTriad::LOCAL_INITIAL);
  else if (val == rcsLocalWithRotated)
    m_pTriad->itsLocalDir.setValue(FmTriad::LOCAL_WITHROTATED);
}

long CaTriad::get_BaseID()
{
  CA_CHECK(m_pTriad);

  return m_pTriad->getBaseID();
}

BSTR CaTriad::get_Tag()
{
  CA_CHECK(m_pTriad);

  return SysAllocString(CA2W(m_pTriad->getTag().c_str()));
}

void CaTriad::put_Tag(LPCTSTR val)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setTag(CaConvert(val));
  m_pTriad->onChanged();
}

ISubAssembly* CaTriad::get_Parent()
{
  CA_CHECK(m_pTriad);

  FmBase* pParent = m_pTriad->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper((FmModelMemberBase*)pParent);
}

void CaTriad::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  CA_CHECK(m_pTriad);

  FaVec3 r = m_pTriad->getOrientation().getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

void CaTriad::SetEulerRotationZYX(double rx, double ry, double rz)
{
  CA_CHECK(m_pTriad);

  FaMat33 m;
  m.eulerRotateZYX(FaVec3(rx,ry,rz));

  m_pTriad->setOrientation(m);

  m_pTriad->onChanged();
  m_pTriad->draw();
}

void CaTriad::GetRotationMatrix(VARIANT* Array3x3)
{
  CA_CHECK(m_pTriad);

  CaApplication::CreateSafeArray(Array3x3, m_pTriad->getOrientation());
}

void CaTriad::SetRotationMatrix(const VARIANT FAR& Array3x3)
{
  CA_CHECK(m_pTriad);

  FaMat33 m;
  CaApplication::GetFromSafeArray(Array3x3, m);
  m_pTriad->setOrientation(m);

  m_pTriad->onChanged();
  m_pTriad->draw();
}

void CaTriad::SetMass(double Mass, double Ix, double Iy, double Iz)
{
  CA_CHECK(m_pTriad);

  m_pTriad->setAddedMass(Mass);
  m_pTriad->setAddMass(3, Ix);
  m_pTriad->setAddMass(4, Iy);
  m_pTriad->setAddMass(5, Iz);

  m_pTriad->onChanged();
}

void CaTriad::SetPosition(double x, double y, double z)
{
  CA_CHECK(m_pTriad);

  FaMat34 m = m_pTriad->getGlobalCS();
  m[3] = FaVec3(x,y,z);
  m_pTriad->setGlobalCS(m);

  m_pTriad->onChanged();
  m_pTriad->draw();
}

VARIANT CaTriad::GetAttachedObjects(ObjectType ObjectTypeFlags)
{
  CA_CHECK(m_pTriad);

  VARIANT vRet;
  vRet.vt = VT_EMPTY;

  // Get attached objects
  std::vector<FmIsPositionedBase*> attachedObjects;
  if (ObjectTypeFlags & otBeam) {
    std::vector<FmBeam*> beams;
    m_pTriad->getBeamBinding(beams);
    for (FmBeam* beam : beams)
      attachedObjects.push_back(beam);
  }
  if (ObjectTypeFlags & otJoint) {
    std::vector<FmJointBase*> joints;
    m_pTriad->getJointBinding(joints);
    for (FmJointBase* joint : joints)
      if (joint->isOfType(FmFreeJoint::getClassTypeID()))
        attachedObjects.push_back(joint);
  }
  if (ObjectTypeFlags & otCamJoint) {
    std::vector<FmJointBase*> joints;
    m_pTriad->getJointBinding(joints);
    for (FmJointBase* joint : joints)
      if (joint->isOfType(FmCamJoint::getClassTypeID()))
        attachedObjects.push_back(joint);
   }

  // Create safearray
  SAFEARRAYBOUND bounds;
  bounds.lLbound = 0;
  bounds.cElements = attachedObjects.size();
  SAFEARRAY* pSA = SafeArrayCreate(VT_VARIANT, 1, &bounds);
  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set data
  LONG ix = 0;
  VARIANT varUnk;
  VariantInit(&varUnk);
  varUnk.vt = VT_UNKNOWN;
  for (FmIsPositionedBase* pObj : attachedObjects)
    if (pObj->isOfType(FmBeam::getClassTypeID())) {
      CaBeam* pCaBeam = (CaBeam*)CaBeam::CreateObject();
      if (pCaBeam == NULL)
        return vRet;
      pCaBeam->m_pBeam = static_cast<FmBeam*>(pObj);
      IBeam* pIBeam = NULL;
      LPDISPATCH pDisp = pCaBeam->GetIDispatch(false);
      pDisp->QueryInterface(IID_IBeam, (void**)&pIBeam);
      pCaBeam->InternalRelease();
      // Set element data
      varUnk.punkVal = pIBeam;
      SafeArrayPutElement(pSA, &ix, &varUnk);
      ix++;
    }
    else if (pObj->isOfType(FmFreeJoint::getClassTypeID())) {
      CaJoint* pCaJoint = (CaJoint*)CaJoint::CreateObject();
      if (pCaJoint == NULL)
        return vRet;
      pCaJoint->m_pJoint = static_cast<FmFreeJoint*>(pObj);
      IJoint* pIJoint = NULL;
      LPDISPATCH pDisp = pCaJoint->GetIDispatch(false);
      pDisp->QueryInterface(IID_IJoint, (void**)&pIJoint);
      pCaJoint->InternalRelease();
      // Set element data
      varUnk.punkVal = pIJoint;
      SafeArrayPutElement(pSA, &ix, &varUnk);
      ix++;
    }
    else if (pObj->isOfType(FmCamJoint::getClassTypeID())) {
      CaCamJoint* pCaCamJoint = (CaCamJoint*)CaCamJoint::CreateObject();
      if (pCaCamJoint == NULL)
        return vRet;
      pCaCamJoint->m_pCamJoint = static_cast<FmCamJoint*>(pObj);
      ICamJoint* pICamJoint = NULL;
      LPDISPATCH pDisp = pCaCamJoint->GetIDispatch(false);
      pDisp->QueryInterface(IID_ICamJoint, (void**)&pICamJoint);
      pCaCamJoint->InternalRelease();
      // Set element data
      varUnk.punkVal = pICamJoint;
      SafeArrayPutElement(pSA, &ix, &varUnk);
      ix++;
    }

  // Set variant
  vRet.vt = VT_ARRAY | VT_VARIANT;
  vRet.parray = pSA;
  return vRet;
}

void CaTriad::Delete()
{
  CA_CHECK(m_pTriad);

  m_pTriad->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaTriad::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaTriad::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaTriad::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaTriad::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaTriad::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaTriad::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}
STDMETHODIMP CaTriad::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaTriad, LocalClass)
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

STDMETHODIMP CaTriad::XLocalClass::get_X(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::put_X(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::get_Y(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::put_Y(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::get_Z(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::put_Z(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::get_Mass(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::put_Mass(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->put_Mass(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::get_Ix(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    *pVal = pThis->get_Ix();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::put_Ix(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->put_Ix(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::get_Iy(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    *pVal = pThis->get_Iy();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::put_Iy(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->put_Iy(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::get_Iz(double* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    *pVal = pThis->get_Iz();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::put_Iz(double val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->put_Iz(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::get_DOF(long DOFIndex, IDOF** ppRet)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::get_ReferenceCS(ReferenceCSType* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    *pVal = pThis->get_ReferenceCS();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::put_ReferenceCS(ReferenceCSType val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->put_ReferenceCS(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::get_Tag(BSTR* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    *pVal = pThis->get_Tag();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::put_Tag(BSTR val)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->put_Tag(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::SetEulerRotationZYX(double rx, double ry, double rz)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::GetRotationMatrix(VARIANT* Array3x3)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::SetRotationMatrix(VARIANT Array3x3)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::SetMass(double Mass, double Ix, double Iy, double Iz)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    pThis->SetMass(Mass,Ix,Iy,Iz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::SetPosition(double x, double y, double z)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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

STDMETHODIMP CaTriad::XLocalClass::GetAttachedObjects(ObjectType ObjectTypeFlags, VARIANT* pVal)
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
  TRY
  {
    *pVal = pThis->GetAttachedObjects(ObjectTypeFlags);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaTriad::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaTriad, LocalClass);
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
