// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaCrossSection.h"
#include "CaMaterial.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmBeamProperty.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaCrossSection, CCmdTarget)

BEGIN_MESSAGE_MAP(CaCrossSection, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaCrossSection, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaCrossSection, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaCrossSection, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaCrossSection, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaCrossSection, "GetCrossSectionType", dispidGetCrossSectionType, GetCrossSectionType, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CaCrossSection, "SetCrossSectionType", dispidSetCrossSectionType, SetCrossSectionType, VT_EMPTY, VTS_I4)
  DISP_FUNCTION_ID(CaCrossSection, "GetMaterial", dispidGetMaterial, GetMaterial, VT_UNKNOWN, VTS_NONE)
  DISP_FUNCTION_ID(CaCrossSection, "SetMaterial", dispidSetMaterial, SetMaterial, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaCrossSection, "GetShearCenterOffset", dispidGetShearCenterOffset, GetShearCenterOffset, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetShearCenterOffset", dispidSetShearCenterOffset, SetShearCenterOffset, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetPipeDiameters", dispidGetPipeDiameters, GetPipeDiameters, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetPipeDiameters", dispidSetPipeDiameters, SetPipeDiameters, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetPipeDependentProperties", dispidGetPipeDependentProperties, GetPipeDependentProperties, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetPipeDependentProperties", dispidSetPipeDependentProperties, SetPipeDependentProperties, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetPipeShearReductionFactors", dispidGetPipeShearReductionFactors, GetPipeShearReductionFactors, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetPipeShearReductionFactors", dispidSetPipeShearReductionFactors, SetPipeShearReductionFactors, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetGenericGeometricProperties", dispidGetGenericGeometricProperties, GetGenericGeometricProperties, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetGenericGeometricProperties", dispidSetGenericGeometricProperties, SetGenericGeometricProperties, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetGenericMomentOfInertia", dispidGetGenericMomentOfInertia, GetGenericMomentOfInertia, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetGenericMomentOfInertia", dispidSetGenericMomentOfInertia, SetGenericMomentOfInertia, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetGenericShearStiffness", dispidGetGenericShearStiffness, GetGenericShearStiffness, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetGenericShearStiffness", dispidSetGenericShearStiffness, SetGenericShearStiffness, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetHydroBuoyancyAndDragDiameters", dispidGetHydroBuoyancyAndDragDiameters, GetHydroBuoyancyAndDragDiameters, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetHydroBuoyancyAndDragDiameters", dispidSetHydroBuoyancyAndDragDiameters, SetHydroBuoyancyAndDragDiameters, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetHydrodynamicCoefficients", dispidGetHydrodynamicCoefficients, GetHydrodynamicCoefficients, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetHydrodynamicCoefficients", dispidSetHydrodynamicCoefficients, SetHydrodynamicCoefficients, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetHydrodynamicAxialCoefficients", dispidGetHydrodynamicAxialCoefficients, GetHydrodynamicAxialCoefficients, VT_EMPTY, VTS_PR8 VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaCrossSection, "SetHydrodynamicAxialCoefficients", dispidSetHydrodynamicAxialCoefficients, SetHydrodynamicAxialCoefficients, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetHydroSpinDragCoefficient", dispidGetHydroSpinDragCoefficient, GetHydroSpinDragCoefficient, VT_R8, VTS_NONE)
  DISP_FUNCTION_ID(CaCrossSection, "SetHydroSpinDragCoefficient", dispidSetHydroSpinDragCoefficient, SetHydroSpinDragCoefficient, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "GetHydroInternalFluidDiameter", dispidGetHydroInternalFluidDiameter, GetHydroInternalFluidDiameter, VT_R8, VTS_NONE)
  DISP_FUNCTION_ID(CaCrossSection, "SetHydroInternalFluidDiameter", dispidSetHydroInternalFluidDiameter, SetHydroInternalFluidDiameter, VT_EMPTY, VTS_R8)
  DISP_FUNCTION_ID(CaCrossSection, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaCrossSection, CCmdTarget)
  INTERFACE_PART(CaCrossSection, IID_ICrossSection, LocalClass)
END_INTERFACE_MAP()

// {BDA9F4A4-E60F-4dd1-BD4A-9F0F948558ED}
IMPLEMENT_OLECREATE(CaCrossSection, "FEDEM.CrossSection",
0xbda9f4a4, 0xe60f, 0x4dd1, 0xbd, 0x4a, 0x9f, 0xf, 0x94, 0x85, 0x58, 0xed);


CaCrossSection::CaCrossSection(void) : m_ptr(m_pCrossSection)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pCrossSection = NULL;
  signalConnector.Connect(this);
}

CaCrossSection::~CaCrossSection(void)
{
  ::AfxOleUnlockApp();
  m_pCrossSection = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaCrossSection::get_Description()
{
  CA_CHECK(m_pCrossSection);
  return SysAllocString(CA2W(m_pCrossSection->getUserDescription().c_str()));
}

void CaCrossSection::put_Description(LPCTSTR val)
{
  CA_CHECK(m_pCrossSection);

  m_pCrossSection->setUserDescription(CaConvert(val));
  m_pCrossSection->onChanged();
}

long CaCrossSection::get_BaseID()
{
  CA_CHECK(m_pCrossSection);
  return m_pCrossSection->getBaseID();
}

ISubAssembly* CaCrossSection::get_Parent()
{
  CA_CHECK(m_pCrossSection);
  FmModelMemberBase* pParent = (FmModelMemberBase*)m_pCrossSection->getParentAssembly();
  if (pParent == NULL)
      return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

CrossSectionType CaCrossSection::GetCrossSectionType()
{
  CA_CHECK(m_pCrossSection);
  return (CrossSectionType)(int)m_pCrossSection->crossSectionType.getValue();
}

void CaCrossSection::SetCrossSectionType(CrossSectionType CSType)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->crossSectionType.setValue(static_cast<FmBeamProperty::FmSection>(CSType));
  m_pCrossSection->onChanged();
}

IMaterial* CaCrossSection::GetMaterial()
{
  CA_CHECK(m_pCrossSection);

  // Create material wrapper
  CaMaterial* pCaMaterial = (CaMaterial*)CaMaterial::CreateObject();
  if (pCaMaterial == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set its main property
  pCaMaterial->m_ptr = m_pCrossSection->material.getPointer();

  // Return the interface
  IMaterial* pIMaterial = NULL;
  LPDISPATCH pDisp = pCaMaterial->GetIDispatch(false);
  pDisp->QueryInterface(IID_IMaterial, (void**)&pIMaterial);
  pCaMaterial->InternalRelease();
  return pIMaterial;
}

void CaCrossSection::SetMaterial(IMaterial* Material)
{
  CA_CHECK(m_pCrossSection);

  // Get material
  if (Material == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Material->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaMaterial* pCaMaterial =
    dynamic_cast<CaMaterial*>(CaMaterial::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaMaterial == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Set material
  m_pCrossSection->material.setRef(pCaMaterial->m_ptr);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetShearCenterOffset(double* sy, double* sz)
{
  CA_CHECK(m_pCrossSection);
  *sy = m_pCrossSection->ShrCentre.getValue().first;
  *sz = m_pCrossSection->ShrCentre.getValue().second;
}

void CaCrossSection::SetShearCenterOffset(double sy, double sz)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->ShrCentre = std::make_pair(sy,sz);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetPipeDiameters(double* OuterDiameter, double* InnerDiameter)
{
  CA_CHECK(m_pCrossSection);
  *OuterDiameter = m_pCrossSection->Do.getValue();
  *InnerDiameter = m_pCrossSection->Di.getValue();
}

void CaCrossSection::SetPipeDiameters(double OuterDiameter, double InnerDiameter)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Do.setValue(OuterDiameter);
  m_pCrossSection->Di.setValue(InnerDiameter);
  m_pCrossSection->onChanged();
  m_pCrossSection->updateDependentValues();
}

void CaCrossSection::GetPipeDependentProperties(double* A, double* Iy, double* Iz, double* Ip)
{
  CA_CHECK(m_pCrossSection);
  *A = m_pCrossSection->A.getValue();
  *Iy = m_pCrossSection->Iy.getValue();
  *Iz = m_pCrossSection->Iz.getValue();
  *Ip = m_pCrossSection->Ip.getValue();
}

void CaCrossSection::SetPipeDependentProperties(double A, double Iy, double Iz, double Ip)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->A.setValue(A);
  m_pCrossSection->Iy.setValue(Iy);
  m_pCrossSection->Iz.setValue(Iz);
  m_pCrossSection->Ip.setValue(Ip);
  m_pCrossSection->breakDependence.setValue(true);
  m_pCrossSection->onChanged();
  m_pCrossSection->updateDependentValues();
}

void CaCrossSection::GetPipeShearReductionFactors(double* ky, double* kz)
{
  CA_CHECK(m_pCrossSection);
  *ky = m_pCrossSection->ShrRed.getValue().first;
  *kz = m_pCrossSection->ShrRed.getValue().second;
}

void CaCrossSection::SetPipeShearReductionFactors(double ky, double kz)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->ShrRed = std::make_pair(ky,kz);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetGenericGeometricProperties(double* EA, double* EIyy, double* EIzz, double* GIt, double* mL, double* rhoIp)
{
  CA_CHECK(m_pCrossSection);
  *EA = m_pCrossSection->EA.getValue();
  *EIyy = m_pCrossSection->EI.getValue().first;
  *EIzz = m_pCrossSection->EI.getValue().second;
  *GIt = m_pCrossSection->GIt.getValue();
  *mL = m_pCrossSection->Mass.getValue();
  *rhoIp = m_pCrossSection->RoIp.getValue();
}

void CaCrossSection::SetGenericGeometricProperties(double EA, double EIyy, double EIzz, double GIt, double mL, double rhoIp)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->EA.setValue(EA);
  m_pCrossSection->EI = std::make_pair(EIyy,EIzz);
  m_pCrossSection->GIt.setValue(GIt);
  m_pCrossSection->Mass.setValue(mL);
  m_pCrossSection->RoIp.setValue(rhoIp);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetGenericMomentOfInertia(double* Ix, double* Iy, double* Iz)
{
  CA_CHECK(m_pCrossSection);
  *Ix = m_pCrossSection->Ip.getValue();
  *Iy = m_pCrossSection->Iy.getValue();
  *Iz = m_pCrossSection->Iz.getValue();
}

void CaCrossSection::SetGenericMomentOfInertia(double Ix, double Iy, double Iz)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Ip.setValue(Ix);
  m_pCrossSection->Iy.setValue(Iy);
  m_pCrossSection->Iz.setValue(Iz);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetGenericShearStiffness(double* GAsy, double* GAsz)
{
  CA_CHECK(m_pCrossSection);
  *GAsy = m_pCrossSection->GAs.getValue().first;
  *GAsz = m_pCrossSection->GAs.getValue().second;
}

void CaCrossSection::SetGenericShearStiffness(double GAsy, double GAsz)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->GAs = std::make_pair(GAsy,GAsz);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetHydroBuoyancyAndDragDiameters(double* Db, double* Dd)
{
  CA_CHECK(m_pCrossSection);
  *Db = m_pCrossSection->Db.getValue();
  *Dd = m_pCrossSection->Dd.getValue();
}

void CaCrossSection::SetHydroBuoyancyAndDragDiameters(double Db, double Dd)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Db.setValue(Db);
  m_pCrossSection->Dd.setValue(Dd);
  m_pCrossSection->hydroToggle.setValue(true);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetHydrodynamicCoefficients(double* Cd, double* Ca, double* Cm)
{
  CA_CHECK(m_pCrossSection);
  *Cd = m_pCrossSection->Cd.getValue();
  *Ca = m_pCrossSection->Ca.getValue();
  *Cm = m_pCrossSection->Cm.getValue();
}

void CaCrossSection::SetHydrodynamicCoefficients(double Cd, double Ca, double Cm)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Cd.setValue(Cd);
  m_pCrossSection->Ca.setValue(Ca);
  m_pCrossSection->Cm.setValue(Cm);
  m_pCrossSection->hydroToggle.setValue(true);
  m_pCrossSection->onChanged();
}

void CaCrossSection::GetHydrodynamicAxialCoefficients(double* Cd_axial, double* Ca_axial, double* Cm_axial)
{
  CA_CHECK(m_pCrossSection);
  *Cd_axial = m_pCrossSection->Cd_axial.getValue();
  *Ca_axial = m_pCrossSection->Ca_axial.getValue();
  *Cm_axial = m_pCrossSection->Cm_axial.getValue();
}

void CaCrossSection::SetHydrodynamicAxialCoefficients(double Cd_axial, double Ca_axial, double Cm_axial)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Cd_axial.setValue(Cd_axial);
  m_pCrossSection->Ca_axial.setValue(Ca_axial);
  m_pCrossSection->Cm_axial.setValue(Cm_axial);
  m_pCrossSection->hydroToggle.setValue(true);
  m_pCrossSection->onChanged();
}

double CaCrossSection::GetHydroSpinDragCoefficient()
{
  CA_CHECK(m_pCrossSection);
  return m_pCrossSection->Cd_spin.getValue();
}

void CaCrossSection::SetHydroSpinDragCoefficient(double Cd_spin)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Cd_spin.setValue(Cd_spin);
  m_pCrossSection->hydroToggle.setValue(true);
  m_pCrossSection->onChanged();
}

double CaCrossSection::GetHydroInternalFluidDiameter()
{
  CA_CHECK(m_pCrossSection);
  return m_pCrossSection->Di_hydro.getValue();
}

void CaCrossSection::SetHydroInternalFluidDiameter(double Di)
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->Di_hydro.setValue(Di);
  m_pCrossSection->hydroToggle.setValue(true);
  m_pCrossSection->onChanged();
}

void CaCrossSection::Delete()
{
  CA_CHECK(m_pCrossSection);
  m_pCrossSection->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaCrossSection::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaCrossSection::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaCrossSection::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaCrossSection::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaCrossSection::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaCrossSection::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaCrossSection::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass)
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

STDMETHODIMP CaCrossSection::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
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

STDMETHODIMP CaCrossSection::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
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

STDMETHODIMP CaCrossSection::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
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

STDMETHODIMP CaCrossSection::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
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

STDMETHODIMP CaCrossSection::XLocalClass::GetCrossSectionType(CrossSectionType* pRet)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    *pRet = pThis->GetCrossSectionType();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetCrossSectionType(CrossSectionType CSType)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetCrossSectionType(CSType);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetMaterial(IMaterial** ppRet)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    *ppRet = pThis->GetMaterial();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetMaterial(IMaterial* Material)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetMaterial(Material);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetShearCenterOffset(double* sy, double* sz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetShearCenterOffset(sy, sz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetShearCenterOffset(double sy, double sz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetShearCenterOffset(sy, sz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetPipeDiameters(double* OuterDiameter, double* InnerDiameter)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetPipeDiameters(OuterDiameter, InnerDiameter);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetPipeDiameters(double OuterDiameter, double InnerDiameter)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetPipeDiameters(OuterDiameter, InnerDiameter);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetPipeDependentProperties(double* A, double* Iy, double* Iz, double* Ip)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetPipeDependentProperties(A, Iy, Iz, Ip);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetPipeDependentProperties(double A, double Iy, double Iz, double Ip)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetPipeDependentProperties(A, Iy, Iz, Ip);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetPipeShearReductionFactors(double* ky, double* kz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetPipeShearReductionFactors(ky, kz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetPipeShearReductionFactors(double ky, double kz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetPipeShearReductionFactors(ky, kz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetGenericGeometricProperties(double* EA, double* EIyy, double* EIzz, double* GIt, double* mL, double* rhoIp)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetGenericGeometricProperties(EA, EIyy, EIzz, GIt, mL, rhoIp);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetGenericGeometricProperties(double EA, double EIyy, double EIzz, double GIt, double mL, double rhoIp)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetGenericGeometricProperties(EA, EIyy, EIzz, GIt, mL, rhoIp);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetGenericMomentOfInertia(double* Ix, double* Iy, double* Iz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetGenericMomentOfInertia(Ix, Iy, Iz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetGenericMomentOfInertia(double Ix, double Iy, double Iz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetGenericMomentOfInertia(Ix, Iy, Iz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetGenericShearStiffness(double* GAsy, double* GAsz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetGenericShearStiffness(GAsy, GAsz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetGenericShearStiffness(double GAsy, double GAsz)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetGenericShearStiffness(GAsy, GAsz);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetHydroBuoyancyAndDragDiameters(double* Db, double* Dd)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetHydroBuoyancyAndDragDiameters(Db, Dd);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetHydroBuoyancyAndDragDiameters(double Db, double Dd)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetHydroBuoyancyAndDragDiameters(Db, Dd);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetHydrodynamicCoefficients(double* Cd, double* Ca, double* Cm)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetHydrodynamicCoefficients(Cd, Ca, Cm);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetHydrodynamicCoefficients(double Cd, double Ca, double Cm)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetHydrodynamicCoefficients(Cd, Ca, Cm);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetHydrodynamicAxialCoefficients(double* Cd_axial, double* Ca_axial, double* Cm_axial)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->GetHydrodynamicAxialCoefficients(Cd_axial, Ca_axial, Cm_axial);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetHydrodynamicAxialCoefficients(double Cd_axial, double Ca_axial, double Cm_axial)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetHydrodynamicAxialCoefficients(Cd_axial, Ca_axial, Cm_axial);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetHydroSpinDragCoefficient(double* Cd_spin)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    *Cd_spin = pThis->GetHydroSpinDragCoefficient();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetHydroSpinDragCoefficient(double Cd_spin)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetHydroSpinDragCoefficient(Cd_spin);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::GetHydroInternalFluidDiameter(double* Di)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    *Di = pThis->GetHydroInternalFluidDiameter();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::SetHydroInternalFluidDiameter(double Di)
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
  TRY
  {
    pThis->SetHydroInternalFluidDiameter(Di);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaCrossSection::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaCrossSection, LocalClass);
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
