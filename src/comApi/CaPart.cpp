// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaPart.h"
#include "CaTriad.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmPart.H"
#include "vpmDB/FmTriad.H"
#include "vpmDisplay/FdPickedPoints.H"

//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaPart, CCmdTarget)

BEGIN_MESSAGE_MAP(CaPart, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaPart, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaPart, "X", dispidX, get_X, put_X, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Y", dispidY, get_Y, put_Y, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Z", dispidZ, get_Z, put_Z, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Mass", dispidMass, get_Mass, put_Mass, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Ix", dispidIx, get_Ix, put_Ix, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Iy", dispidIy, get_Iy, put_Iy, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Iz", dispidIz, get_Iz, put_Iz, VT_R8)
  DISP_PROPERTY_EX_ID(CaPart, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaPart, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaPart, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaPart, "IsFEPart", dispidIsFEPart, get_IsFEPart, SetNotSupported, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaPart, "FEComponentModes", dispidFEComponentModes, get_FEComponentModes, put_FEComponentModes, VT_I4)
  DISP_FUNCTION_ID(CaPart, "GetEulerRotationZYX", dispidGetEulerRotationZYX, GetEulerRotationZYX, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "SetEulerRotationZYX", dispidSetEulerRotationZYX, SetEulerRotationZYX, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "GetRotationMatrix", dispidGetRotationMatrix, GetRotationMatrix, VT_EMPTY, VTS_PVARIANT)
  DISP_FUNCTION_ID(CaPart, "SetRotationMatrix", dispidSetRotationMatrix, SetRotationMatrix, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaPart, "SetPosition", dispidSetPosition, SetPosition, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "SetMass", dispidSetMass, SetMass, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "GetCOG", dispidGetCOG, GetCOG, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "SetCOG", dispidSetCOG, SetCOG, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "GetPrincipalAxisOfInertia", dispidGetPrincipalAxisOfInertia, GetPrincipalAxisOfInertia, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "SetPrincipalAxisOfInertia", dispidSetPrincipalAxisOfInertia, SetPrincipalAxisOfInertia, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "GetStructuralDamping", dispidGetStructuralDamping, GetStructuralDamping, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "SetStructuralDamping", dispidSetStructuralDamping, SetStructuralDamping, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "GetScaling", dispidGetScaling, GetScaling, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "SetScaling", dispidSetScaling, SetScaling, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "AddTriad", dispidAddTriad, AddTriad, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaPart, "CalculateCOG", dispidCalculateCOG, CalculateCOG, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaPart, "GetBoundingBox", dispidGetBoundingBox, GetBoundingBox, VT_BOOL, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "FEGetNodeCount", dispidFEGetNodeCount, FEGetNodeCount, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CaPart, "FEGetNodePosition", dispidFEGetNodePosition, FEGetNodePosition, VT_I4, VTS_I4 VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "FEGetNodeStatus", dispidFEGetNodeStatus, FEGetNodePosition, VT_I4, VTS_I4)
  DISP_FUNCTION_ID(CaPart, "FEGetNodeAtPoint", dispidFEGetNodeAtPoint, FEGetNodeAtPoint, VT_I4, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaPart, "FEAttach", dispidFEAttach, FEAttach, VT_BOOL, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaPart, "FECreateSpider", dispidFECreateSpider, FECreateSpider, VT_EMPTY, VTS_I4 VTS_VARIANT VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaPart, "SetTranslationalStiffness", dispidSetTranslationalStiffness, SetTranslationalStiffness, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaPart, "SetRotationalStiffness", dispidSetRotationalStiffness, SetRotationalStiffness, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaPart, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaPart, "SetPrincipalAxisOfInertiaExExy", dispidSetPrincipalAxisOfInertiaExExy, SetPrincipalAxisOfInertiaExExy, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaPart, "SetVisualizationFile", dispidSetVisualizationFile, SetVisualizationFile, VT_EMPTY, VTS_BSTR)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaPart, CCmdTarget)
  INTERFACE_PART(CaPart, IID_IPart, LocalClass)
END_INTERFACE_MAP()

// {B926A4A1-8F7D-4419-8609-71F4E901848D}
IMPLEMENT_OLECREATE(CaPart, "FEDEM.Part",
0xb926a4a1, 0x8f7d, 0x4419, 0x86, 0x9, 0x71, 0xf4, 0xe9, 0x1, 0x84, 0x8d);



CaPart::CaPart(void) : m_ptr(m_pGenPart)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pGenPart = NULL;
  signalConnector.Connect(this);
}

CaPart::~CaPart(void)
{
  ::AfxOleUnlockApp();
  m_pGenPart = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaPart::get_X()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->getTranslation().x();
}

void CaPart::put_X(double val)
{
  CA_CHECK(m_pGenPart);

  FaVec3 p = m_pGenPart->getTranslation();
  p.x(val);
  m_pGenPart->setTranslation(p);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

double CaPart::get_Y()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->getTranslation().y();
}

void CaPart::put_Y(double val)
{
  CA_CHECK(m_pGenPart);

  FaVec3 p = m_pGenPart->getTranslation();
  p.y(val);
  m_pGenPart->setTranslation(p);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

double CaPart::get_Z()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->getTranslation().z();
}

void CaPart::put_Z(double val)
{
  CA_CHECK(m_pGenPart);

  FaVec3 p = m_pGenPart->getTranslation();
  p.z(val);
  m_pGenPart->setTranslation(p);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

double CaPart::get_Mass()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->mass.getValue();
}

void CaPart::put_Mass(double val)
{
  CA_CHECK(m_pGenPart);

  if (m_pGenPart->mass.setValue(val))
    m_pGenPart->onChanged();
}

double CaPart::get_Ix()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->inertia.getValue()[0];
}

void CaPart::put_Ix(double val)
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->inertia.getValue()[0] = val;

  m_pGenPart->onChanged();
}

double CaPart::get_Iy()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->inertia.getValue()[1];
}

void CaPart::put_Iy(double val)
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->inertia.getValue()[1] = val;

  m_pGenPart->onChanged();
}

double CaPart::get_Iz()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->inertia.getValue()[2];
}

void CaPart::put_Iz(double val)
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->inertia.getValue()[2] = val;

  m_pGenPart->onChanged();
}

BSTR CaPart::get_Description()
{
  CA_CHECK(m_pGenPart);

  return SysAllocString(CA2W(m_pGenPart->getUserDescription().c_str()));
}

void CaPart::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->setUserDescription(CaConvert(val));
  m_pGenPart->onChanged();
}

long CaPart::get_BaseID()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->getBaseID();
}

ISubAssembly* CaPart::get_Parent()
{
  CA_CHECK(m_pGenPart);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pGenPart->getParentAssembly();
  if (pParent == NULL)
    return NULL;

  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

BOOL CaPart::get_IsFEPart()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->isFEPart();
}

long CaPart::get_FEComponentModes()
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->nGenModes.getValue();
}

void CaPart::put_FEComponentModes(long val)
{
  CA_CHECK(m_pGenPart);

  if (m_pGenPart->nGenModes.setValue(val))
    m_pGenPart->onChanged();
}

void CaPart::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  CA_CHECK(m_pGenPart);

  FaVec3 r = m_pGenPart->getOrientation().getEulerZYX();
  *rx = r.x();
  *ry = r.y();
  *rz = r.z();
}

void CaPart::SetEulerRotationZYX(double rx, double ry, double rz)
{
  CA_CHECK(m_pGenPart);

  FaMat33 m;
  m.eulerRotateZYX(FaVec3(rx,ry,rz));
  m_pGenPart->setOrientation(m);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

void CaPart::GetRotationMatrix(VARIANT* Array3x3)
{
  CA_CHECK(m_pGenPart);

  FaMat33 m = m_pGenPart->getOrientation();
  CaApplication::CreateSafeArray(Array3x3, m);
}

void CaPart::SetRotationMatrix(const VARIANT FAR& Array3x3)
{
  CA_CHECK(m_pGenPart);

  FaMat33 m;
  CaApplication::GetFromSafeArray(Array3x3, m);
  m_pGenPart->setOrientation(m);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

void CaPart::SetPosition(double x, double y, double z)
{
  CA_CHECK(m_pGenPart);

  FaMat34 m = m_pGenPart->getGlobalCS();
  m[3] = FaVec3(x,y,z);
  m_pGenPart->setGlobalCS(m);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

void CaPart::SetMass(double Mass, double Ix, double Iy, double Iz)
{
  CA_CHECK(m_pGenPart);

  if ((m_pGenPart->mass.setValue(Mass) |
       m_pGenPart->inertia.setValue(FFaTensor3(Ix,Iy,Iz))) > 0)
    m_pGenPart->onChanged();
}

void CaPart::GetCOG(double* x, double* y, double* z)
{
  CA_CHECK(m_pGenPart);

  FaVec3 v = m_pGenPart->getPositionCG().translation();
  *x = v.x();
  *y = v.y();
  *z = v.z();
}

void CaPart::SetCOG(double x, double y, double z)
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->setPositionCG(FaVec3(x,y,z));

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

void CaPart::GetPrincipalAxisOfInertia(double* rx, double* ry, double* rz)
{
  // TODO!

  // Currently not supported!
  AfxThrowOleException(E_NOTIMPL);
}

void CaPart::SetPrincipalAxisOfInertia(double rx, double ry, double rz)
{
  // TODO!

  // Currently not supported!
  AfxThrowOleException(E_NOTIMPL);
}

void CaPart::SetPrincipalAxisOfInertiaExExy(double XAxisX, double XAxisY, double XAxisZ,
    double XYPlaneX, double XYPlaneY, double XYPlaneZ)
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->setOrientationCG(FaVec3(XAxisX,XAxisY,XAxisZ),
                               FaVec3(XYPlaneX,XYPlaneY,XYPlaneZ));

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

void CaPart::GetStructuralDamping(double* MassProp, double* StiffProp)
{
  CA_CHECK(m_pGenPart);

  *MassProp = m_pGenPart->alpha1.getValue();
  *StiffProp = m_pGenPart->alpha2.getValue();
}

void CaPart::SetStructuralDamping(double MassProp, double StiffProp)
{
  CA_CHECK(m_pGenPart);

  if ((m_pGenPart->alpha1.setValue(MassProp) |
       m_pGenPart->alpha2.setValue(StiffProp)) > 0)
    m_pGenPart->onChanged();
}

void CaPart::GetScaling(double* StiffScale, double* MassScale)
{
  CA_CHECK(m_pGenPart);

  *StiffScale = m_pGenPart->stiffnessScale.getValue();
  *MassScale = m_pGenPart->massScale.getValue();
}

void CaPart::SetScaling(double StiffScale, double MassScale)
{
  CA_CHECK(m_pGenPart);

  if ((m_pGenPart->stiffnessScale.setValue(StiffScale) |
       m_pGenPart->massScale.setValue(MassScale)) > 0)
    m_pGenPart->onChanged();
}

void CaPart::AddTriad(ITriad* Triad)
{
  CA_CHECK(m_pGenPart);

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

  if (pCaTriad->m_pTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Connect the triad
  pCaTriad->m_pTriad->connect(m_pGenPart);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

void CaPart::CalculateCOG()
{
  CA_CHECK(m_pGenPart);

  std::vector<FmTriad*> triads;
  m_pGenPart->getTriads(triads);

  FaVec3 cg;
  for (FmTriad* triad : triads)
    cg += triad->getGlobalTranslation();
  cg /= triads.size();

  m_pGenPart->setPositionCG(cg);
  m_pGenPart->setCGPosRef(m_pGenPart);
  m_pGenPart->setCGRotRef(m_pGenPart);

  m_pGenPart->onChanged();
  m_pGenPart->draw();
}

BOOL CaPart::GetBoundingBox(double* MaxX, double* MaxY, double* MaxZ,
    double* MinX, double* MinY, double* MinZ)
{
  CA_CHECK(m_pGenPart);

  FaVec3 vMax;
  FaVec3 vMin;
  if (!m_pGenPart->getBBox(vMax,vMin))
    return false;

  *MaxX = vMax.x();
  *MaxY = vMax.y();
  *MaxZ = vMax.z();
  *MinX = vMin.x();
  *MinY = vMin.y();
  *MinZ = vMin.z();
  return true;
}

long CaPart::FEGetNodeCount()
{
  CA_CHECK(m_pGenPart);

  int nnod = 0;
  m_pGenPart->getMeshParams(&nnod);
  return nnod;
}

long CaPart::FEGetNodePosition(long Index, double* X, double* Y, double* Z)
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->getNodePos(Index,X,Y,Z);
}

long CaPart::FEGetNodeAtPoint(double X, double Y, double Z, double Tolerance)
{
  CA_CHECK(m_pGenPart);

  return m_pGenPart->getNodeIDAtPoint(FaVec3(X,Y,Z),Tolerance);
}

BOOL CaPart::FEAttach(IDispatch* Object)
{
  CA_CHECK(m_pGenPart);

  // Get object
  FmModelMemberBase* pObject = CaApplication::GetFromCOMObjectWrapper(Object);
  if (pObject == NULL)
    AfxThrowOleException(E_POINTER);

  // Attach
  return m_pGenPart->attach(pObject, true);
}

void CaPart::FECreateSpider(long SpiderType, VARIANT NodeIndexList, ITriad* Triad)
{
  CA_CHECK(m_pGenPart);

  // Get array size
  int nDims = 0, nN = 0, nM = 0;
  CaApplication::GetFromSafeArrayDims(NodeIndexList, &nDims, &nN, &nM);
  if (nDims != 1)
    AfxThrowOleException(E_INVALIDARG);

  // Get array data
  long* pData = (long*)malloc(nN * sizeof(long));
  if (pData == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  CaApplication::GetFromSafeArray(NodeIndexList, nN, 0, pData);
  std::vector<int> nodes(nN);
  for (int i = 0; i < nN; i++)
    nodes[i] = pData[i];

  free(pData);

  // Get triad (optional)
  FmTriad* pTriad = NULL;
  if (Triad != NULL) {
    IDispatch* pDisp1 = NULL;
    Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(E_INVALIDARG);

    CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCaTriad == NULL)
      AfxThrowOleException(E_INVALIDARG);

    pTriad = pCaTriad->m_pTriad;
  }

  // Create the spider element
  m_pGenPart->createConnector(nodes,FdPickedPoints::getFirstPickedPoint(),pTriad,SpiderType);
}


void CaPart::SetTranslationalStiffness(double TransStiffness)
{
  CA_CHECK(m_pGenPart);

  if ((m_pGenPart->kt.setValue(TransStiffness) |
       m_pGenPart->myGenericPartStiffType.setValue(FmPart::NODE_STIFFNESS)) > 0)
    m_pGenPart->onChanged();
}

void CaPart::SetRotationalStiffness(double RotStiffness)
{
  CA_CHECK(m_pGenPart);

  if ((m_pGenPart->kr.setValue(RotStiffness) |
       m_pGenPart->myGenericPartStiffType.setValue(FmPart::NODE_STIFFNESS)) > 0)
    m_pGenPart->onChanged();
}

void CaPart::SetVisualizationFile(LPCTSTR FileName)
{
  CA_CHECK(m_pGenPart);

  if (m_pGenPart->setVisualizationFile(CaConvert(FileName)))
    m_pGenPart->onChanged();
}

void CaPart::Delete()
{
  CA_CHECK(m_pGenPart);

  m_pGenPart->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaPart::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaPart, LocalClass)
  return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CaPart::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaPart, LocalClass)
  return pThis->ExternalRelease();
}

STDMETHODIMP CaPart::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaPart, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CaPart::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaPart, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}

STDMETHODIMP CaPart::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaPart, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}

STDMETHODIMP CaPart::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaPart, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
          lcid, rgdispid);
}

STDMETHODIMP CaPart::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaPart, LocalClass)
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

STDMETHODIMP CaPart::XLocalClass::get_X(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_X(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Y(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Y(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Z(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Z(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Mass(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Mass(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Ix(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Ix(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Iy(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Iy(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Iz(double* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Iz(double val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::get_IsFEPart(VARIANT_BOOL* pRet)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pRet = pThis->get_IsFEPart();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::get_FEComponentModes(long* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pVal = pThis->get_FEComponentModes();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::put_FEComponentModes(long val)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->put_FEComponentModes(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::GetEulerRotationZYX(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::SetEulerRotationZYX(double rx, double ry, double rz)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::GetRotationMatrix(VARIANT* Array3x3)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::SetRotationMatrix(VARIANT Array3x3)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::SetPosition(double x, double y, double z)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::SetMass(double Mass, double Ix, double Iy, double Iz)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::GetCOG(double* x, double* y, double* z)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->GetCOG(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::SetCOG(double x, double y, double z)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->SetCOG(x,y,z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::GetPrincipalAxisOfInertia(double* rx, double* ry, double* rz)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->GetPrincipalAxisOfInertia(rx,ry,rz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::SetPrincipalAxisOfInertia(double rx, double ry, double rz)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->SetPrincipalAxisOfInertia(rx,ry,rz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::SetPrincipalAxisOfInertiaExExy(double XAxisX, double XAxisY, double XAxisZ,
    double XYPlaneX, double XYPlaneY, double XYPlaneZ)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->SetPrincipalAxisOfInertiaExExy(XAxisX, XAxisY, XAxisZ, XYPlaneX, XYPlaneY, XYPlaneZ);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::GetStructuralDamping(double* MassProp, double* StiffProp)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::SetStructuralDamping(double MassProp, double StiffProp)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::GetScaling(double* StiffScale, double* MassScale)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::SetScaling(double StiffScale, double MassScale)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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

STDMETHODIMP CaPart::XLocalClass::AddTriad(ITriad* Triad)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->AddTriad(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::CalculateCOG()
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->CalculateCOG();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::GetBoundingBox(
    double* MaxX, double* MaxY, double* MaxZ,
    double* MinX, double* MinY, double* MinZ, VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pVal = pThis->GetBoundingBox(MaxX, MaxY, MaxZ, MinX, MinY, MinZ);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::FEGetNodeCount(long* pnVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pnVal = pThis->FEGetNodeCount();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::FEGetNodePosition(long Index, double* X, double* Y, double* Z)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->FEGetNodePosition(Index, X, Y, Z);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::FEGetNodeStatus(long Index, long* pnVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pnVal = pThis->FEGetNodePosition(Index);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::FEGetNodeAtPoint(double X, double Y, double Z, double Tolerance, long* pnVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pnVal = pThis->FEGetNodeAtPoint(X, Y, Z, Tolerance);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::FEAttach(IDispatch* Object, VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    *pVal = pThis->FEAttach(Object);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::FECreateSpider(long SpiderType, VARIANT NodeIndexList, ITriad* Triad)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->FECreateSpider(SpiderType, NodeIndexList, Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::SetTranslationalStiffness(double TransStiffness)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->SetTranslationalStiffness(TransStiffness);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::SetRotationalStiffness(double RotStiffness)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->SetRotationalStiffness(RotStiffness);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::SetVisualizationFile(BSTR FileName)
{
  METHOD_PROLOGUE(CaPart, LocalClass);
  TRY
  {
    pThis->SetVisualizationFile(FileName);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaPart::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaPart, LocalClass);
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
