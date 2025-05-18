// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaUserDefinedElement.h"
#include "CaTriad.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmUserDefinedElement.H"
#include "vpmDB/FmTriad.H"

#include "FiUserElmPlugin/FiUserElmPlugin.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaUserDefinedElement, CCmdTarget)

BEGIN_MESSAGE_MAP(CaUserDefinedElement, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaUserDefinedElement, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaUserDefinedElement, "Description", dispidDescription, get_Description, put_Description, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaUserDefinedElement, "BaseID", dispidBaseID, get_BaseID, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaUserDefinedElement, "NumElementNodes", dispidNumElementNodes, get_NumElementNodes, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaUserDefinedElement, "Parent", dispidParent, get_Parent, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaUserDefinedElement, "GetTriad", dispidGetTriad, GetTriad, VT_UNKNOWN, VTS_I4)
  DISP_FUNCTION_ID(CaUserDefinedElement, "SetTriad", dispidSetTriad, SetTriad, VT_EMPTY, VTS_I4 VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaUserDefinedElement, "GetStructuralDamping", dispidGetStructuralDamping, GetStructuralDamping, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaUserDefinedElement, "SetStructuralDamping", dispidSetStructuralDamping, SetStructuralDamping, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaUserDefinedElement, "GetScaling", dispidGetScaling, GetScaling, VT_EMPTY, VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaUserDefinedElement, "SetScaling", dispidSetScaling, SetScaling, VT_EMPTY, VTS_R8 VTS_R8)
  DISP_FUNCTION_ID(CaUserDefinedElement, "Delete", dispidDelete, Delete, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaUserDefinedElement, CCmdTarget)
  INTERFACE_PART(CaUserDefinedElement, IID_IUserDefinedElement, LocalClass)
END_INTERFACE_MAP()

// {AE05B601-C080-4526-AD57-6BDD8E357483}
IMPLEMENT_OLECREATE(CaUserDefinedElement, "FEDEM.UserDefinedElement",
0xae05b601, 0xc080, 0x4526, 0xad, 0x57, 0x6b, 0xdd, 0x8e, 0x35, 0x74, 0x83);


CaUserDefinedElement::CaUserDefinedElement(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_ptr = NULL;
  m_numElementNodes = -1;
  signalConnector.Connect(this);
}

CaUserDefinedElement::~CaUserDefinedElement(void)
{
  ::AfxOleUnlockApp();
  m_ptr = NULL;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaUserDefinedElement::get_Description()
{
  CA_CHECK(m_ptr);

  return SysAllocString(CA2W(m_ptr->getUserDescription().c_str()));
}

void CaUserDefinedElement::put_Description(LPCTSTR val)
{
  CA_CHECK(m_ptr);

  m_ptr->setUserDescription(CaConvert(val));
  m_ptr->onChanged();
}

long CaUserDefinedElement::get_BaseID()
{
  CA_CHECK(m_ptr);

  return m_ptr->getBaseID();
}

long CaUserDefinedElement::get_NumElementNodes()
{
  CA_CHECK(m_ptr);

  return m_numElementNodes;
}

ISubAssembly* CaUserDefinedElement::get_Parent()
{
  CA_CHECK(m_ptr);

  FmModelMemberBase* pParent = (FmModelMemberBase*)m_ptr->getParentAssembly();
  if (pParent == NULL)
    return NULL;
  return (ISubAssembly*)CaApplication::CreateCOMObjectWrapper(pParent);
}

ITriad* CaUserDefinedElement::GetTriad(int pos)
{
  CA_CHECK(m_ptr);

  // Create triad wrapper
  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set its main property
  pCaTriad->m_pTriad = m_ptr->myTriads.getPtr(pos);

  // Return the interface
  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

void CaUserDefinedElement::SetTriad(int pos, ITriad* Triad)
{
  CA_CHECK(m_ptr);

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
  FmTriad* tm = pCaTriad->m_pTriad;
  if (tm == NULL)
    AfxThrowOleException(E_INVALIDARG);
  else if (!tm->hasElementBinding() && !tm->hasJointBinding())
  {
    // Adjust number of triad DOFs if not connected to other elements
    int niw = 0, nrw = 0;
    int ndof = FiUserElmPlugin::instance()->init(m_ptr->getBaseID(),
                                                 m_ptr->myType.getValue(),
                                                 0,0,niw,nrw);
    if (ndof > 0) tm->setNDOFs(ndof);
  }
  m_ptr->myTriads.setPtr(tm,pos);
  m_ptr->onChanged();
  m_ptr->draw();
}

void CaUserDefinedElement::GetStructuralDamping(double* MassProp, double* StiffProp)
{
  CA_CHECK(m_ptr);

  *MassProp = m_ptr->alpha1.getValue();
  *StiffProp = m_ptr->alpha2.getValue();
}

void CaUserDefinedElement::SetStructuralDamping(double MassProp, double StiffProp)
{
  CA_CHECK(m_ptr);

  if (m_ptr->alpha1.setValue(MassProp) |
      m_ptr->alpha2.setValue(StiffProp))
    m_ptr->onChanged();
}

void CaUserDefinedElement::GetScaling(double* StiffScale, double* MassScale)
{
  CA_CHECK(m_ptr);

  *StiffScale = m_ptr->stiffnessScale.getValue();
  *MassScale = m_ptr->massScale.getValue();
}

void CaUserDefinedElement::SetScaling(double StiffScale, double MassScale)
{
  CA_CHECK(m_ptr);

  if (m_ptr->stiffnessScale.setValue(StiffScale) |
      m_ptr->massScale.setValue(MassScale))
    m_ptr->onChanged();
}

void CaUserDefinedElement::Delete()
{
  CA_CHECK(m_ptr);

  m_ptr->erase();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaUserDefinedElement::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaUserDefinedElement::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaUserDefinedElement::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaUserDefinedElement::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaUserDefinedElement::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaUserDefinedElement::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid) 
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
          LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, 
          lcid, rgdispid);
}
STDMETHODIMP CaUserDefinedElement::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass)
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::get_Description(BSTR* pVal)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::put_Description(BSTR val)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::get_BaseID(long* pVal)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::get_NumElementNodes(long* pVal)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
  TRY
  {
    *pVal = pThis->get_NumElementNodes();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaUserDefinedElement::XLocalClass::get_Parent(ISubAssembly** ppObj)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::GetTriad(long pos, ITriad** Triad)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
  TRY
  {
    *Triad = pThis->GetTriad(pos);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaUserDefinedElement::XLocalClass::SetTriad(long pos, ITriad* Triad)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
  TRY
  {
    pThis->SetTriad(pos, Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaUserDefinedElement::XLocalClass::GetStructuralDamping(double* MassProp, double* StiffProp)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::SetStructuralDamping(double MassProp, double StiffProp)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::GetScaling(double* StiffScale, double* MassScale)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::SetScaling(double StiffScale, double MassScale)
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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

STDMETHODIMP CaUserDefinedElement::XLocalClass::Delete()
{
  METHOD_PROLOGUE(CaUserDefinedElement, LocalClass);
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
