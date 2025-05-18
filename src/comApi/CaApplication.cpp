// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaApplication.h"
#include "CaSelectionManager.h"
#include "CaSolverManager.h"
#include "CaSimulationEventManager.h"
#ifdef FT_HAS_GRAPHVIEW
#include "CaResultExtractor.h"
#endif
#include "CaSeaEnvironment.h"
#include "CaSubAssembly.h"
#include "CaUserDefinedElement.h"
#include "CaGenericObject.h"
#include "CaAxialDamper.h"
#include "CaAxialSpring.h"
#include "CaCamJoint.h"
#include "CaCylindricJoint.h"
#include "CaJoint.h"
#include "CaPart.h"
#include "CaBeam.h"
#include "CaStrainRosette.h"
#include "CaTriad.h"
#include "CaDOF.h"
#include "CaCrossSection.h"
#include "CaMaterial.h"
#include "CaFriction.h"
#include "CaFunction.h"
#include "CaGraph.h"
#include "CaCurve.h"
#include "CaFmObject.h"
#include "CaVesselMotion.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmStructAssembly.H"
#include "vpmDB/FmRiser.H"
#include "vpmDB/FmJacket.H"
#include "vpmDB/FmSoilPile.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmAxialDamper.H"
#include "vpmDB/FmAxialSpring.H"
#include "vpmDB/FmStraightMaster.H"
#include "vpmDB/FmCamJoint.H"
#include "vpmDB/FmCylJoint.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmPart.H"
#include "vpmDB/FmUserDefinedElement.H"
#include "vpmDB/FmGenericDBObject.H"
#include "vpmDB/FmBeam.H"
#include "vpmDB/FmBeamProperty.H"
#include "vpmDB/FmMaterialProperty.H"
#include "vpmDB/FmStrainRosette.H"
#include "vpmDB/FmTransFriction.H"
#include "vpmDB/FmRotFriction.H"
#include "vpmDB/FmBearingFriction.H"
#include "vpmDB/FmPrismaticFriction.H"
#include "vpmDB/FmCamFriction.H"
#include "vpmDB/FmfLinVar.H"
#include "vpmDB/FmCurveSet.H"
#include "vpmDB/FmVesselMotion.H"
#include "vpmDB/FmFileSys.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmCreate.H"
#include "vpmPM/FpPM.H"

#include "FiUserElmPlugin/FiUserElmPlugin.H"

#ifdef ERROR
#undef ERROR
#endif

#include "vpmUI/Fui.H"
#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmApp/FapLicenseManager.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaOS/FFaFilePath.H"

#define CA_CHECK_LICENSE(checkout) \
  if (!FapLicenseManager::checkLicense("FA-COM",checkout)) \
    AfxThrowOleException(CLASS_E_NOTLICENSED)


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaApplication, CCmdTarget)

BEGIN_MESSAGE_MAP(CaApplication, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaApplication, CCmdTarget)
  DISP_PROPERTY_PARAM_ID(CaApplication, "Item", dispidItem, get_Item, SetNotSupported, VT_UNKNOWN, VTS_I4)
  DISP_PROPERTY_EX_ID(CaApplication, "ExePath", dispidExePath, get_ExePath, SetNotSupported, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaApplication, "ModelFileName", dispidModelFileName, get_ModelFileName, SetNotSupported, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaApplication, "Count", dispidCount, get_Count, SetNotSupported, VT_I4)
  DISP_PROPERTY_EX_ID(CaApplication, "SimulationEvents", dispidSimulationEvents, get_SimulationEvents, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaApplication, "Selection", dispidSelection, get_Selection, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaApplication, "Solvers", dispidSolvers, get_Solvers, SetNotSupported, VT_UNKNOWN)
  DISP_PROPERTY_EX_ID(CaApplication, "SeaEnvironment", dispidSeaEnvironment, get_SeaEnvironment, SetNotSupported, VT_UNKNOWN)
  DISP_FUNCTION_ID(CaApplication, "Open", dispidOpen, Open, VT_BOOL, VTS_BSTR VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "Save", dispidSave, Save, VT_BOOL, VTS_NONE)
  DISP_FUNCTION_ID(CaApplication, "SaveAs", dispidSaveAs, SaveAs, VT_BOOL, VTS_BSTR VTS_BOOL VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "Close", dispidClose, Close, VT_BOOL, VTS_NONE)
  DISP_FUNCTION_ID(CaApplication, "Exit", dispidExit, Exit, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaApplication, "Show", dispidShow, Show, VT_EMPTY, VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "CreateTriad", dispidCreateTriad, CreateTriad, VT_UNKNOWN, VTS_R8 VTS_R8 VTS_R8 VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateJoint", dispidCreateJoint, CreateJoint, VT_UNKNOWN, VTS_UNKNOWN VTS_UNKNOWN VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateCamJoint", dispidCreateCamJoint, CreateCamJoint, VT_UNKNOWN, VTS_UNKNOWN VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateCylindricJoint", dispidCreateCylindricJoint, CreateCylindricJoint, VT_UNKNOWN, VTS_UNKNOWN VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreatePart", dispidCreatePart, CreatePart, VT_UNKNOWN, VTS_R8 VTS_R8 VTS_R8 VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateBeam", dispidCreateBeam, CreateBeam, VT_UNKNOWN, VTS_UNKNOWN VTS_UNKNOWN VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateFunction", dispidCreateFunction, CreateFunction, VT_UNKNOWN, VTS_I4 VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateStrainRosette", dispidCreateStrainRosette, CreateStrainRosette, VT_UNKNOWN, VTS_I4 VTS_UNKNOWN VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateCrossSection", dispidCreateCrossSection, CreateCrossSection, VT_UNKNOWN, VTS_I4 VTS_UNKNOWN VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateMaterial", dispidCreateMaterial, CreateMaterial, VT_UNKNOWN, VTS_R8 VTS_R8 VTS_R8 VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateFriction", dispidCreateFriction, CreateFriction, VT_UNKNOWN, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateAxialSpring", dispidCreateAxialSpring, CreateAxialSpring, VT_UNKNOWN, VTS_UNKNOWN VTS_UNKNOWN VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateAxialDamper", dispidCreateAxialDamper, CreateAxialDamper, VT_UNKNOWN, VTS_UNKNOWN VTS_UNKNOWN VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateResultExtractor", dispidCreateResultExtractor, CreateResultExtractor, VT_UNKNOWN, VTS_BSTR)
  DISP_FUNCTION_ID(CaApplication, "CreateSubAssembly", dispidCreateSubAssembly, CreateSubAssembly, VT_DISPATCH, VTS_I4 VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "SetCurrentSubAssembly", dispidSetCurrentSubAssembly, SetCurrentSubAssembly, VT_EMPTY, VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateUserDefinedElement", dispidCreateUserDefinedElement, CreateUserDefinedElement, VT_DISPATCH, VTS_I4 VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateGenericObject", dispidCreateGenericObject, CreateGenericObject, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateVesselMotion", dispidCreateVesselMotion, CreateVesselMotion, VT_UNKNOWN, VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "GetCount", dispidGetCount, GetCount, VT_I4, VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "GetItem", dispidGetItem, GetItem, VT_UNKNOWN, VTS_I4 VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "FindByBaseID", dispidFindByBaseID, FindByBaseID, VT_DISPATCH, VTS_I4)
  DISP_FUNCTION_ID(CaApplication, "FindByDescription", dispidFindByDescription, FindByDescription, VT_DISPATCH, VTS_BSTR)
  DISP_FUNCTION_ID(CaApplication, "Remove", dispidRemove, Remove, VT_EMPTY, VTS_VARIANT VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "AttachToGround", dispidAttachToGround, AttachToGround, VT_EMPTY, VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaApplication, "LoadFEPart", dispidLoadFEPart, LoadFEPart, VT_DISPATCH, VTS_BSTR VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "GetItemType", dispidGetItemType, GetItemType, VT_BSTR, VTS_I4 VTS_BOOL)
  DISP_FUNCTION_ID(CaApplication, "GetFmObject", dispidGetFmObject, GetFmObject, VT_UNKNOWN, VTS_VARIANT)
  DISP_FUNCTION_ID(CaApplication, "CreateMooringLine", dispidCreateMooringLine, CreateMooringLine, VT_BOOL, VTS_UNKNOWN VTS_UNKNOWN VTS_R8 VTS_R8 VTS_I4)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaApplication, CCmdTarget)
  INTERFACE_PART(CaApplication, IID_IApplication, LocalClass)
END_INTERFACE_MAP()

// {BEB5424C-69A4-4130-98D4-53D45F15364F}
IMPLEMENT_OLECREATE(CaApplication, "FEDEM.Application",
0xbeb5424c, 0x69a4, 0x4130, 0x98, 0xd4, 0x53, 0xd4, 0x5f, 0x15, 0x36, 0x4f);


CaApplication::CaApplication(void) : m_ptr(m_pCurrentSubAssembly)
{
  EnableAutomation();
  ::AfxOleLockApp();
  m_pCurrentSubAssembly = NULL;
  signalConnector.Connect(this);
}

CaApplication::~CaApplication(void)
{
  ::AfxOleUnlockApp();
}


//////////////////////////////////////////////////////////////////////
// Methods

IDispatch* CaApplication::get_Item(long Index)
{
  return GetItem(Index, false);
}

BSTR CaApplication::get_ExePath()
{
  wchar_t szPath[MAX_PATH];
  GetModuleFileName(NULL, szPath, MAX_PATH);
  return SysAllocString(szPath);
}

BSTR CaApplication::get_ModelFileName()
{
  return SysAllocString(CA2W(FmDB::getMechanismObject()->getModelFileName().c_str()));
}

long CaApplication::get_Count()
{
  return GetCount(FALSE);
}

ISimulationEventManager* CaApplication::get_SimulationEvents()
{
  CA_CHECK_LICENSE(false);

  CaSimulationEventManager* pCaSimulationEventManager = (CaSimulationEventManager*)CaSimulationEventManager::CreateObject();
  if (pCaSimulationEventManager == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  ISimulationEventManager* pISimulationEventManager = NULL;
  LPDISPATCH pDisp = pCaSimulationEventManager->GetIDispatch(false);
  pDisp->QueryInterface(IID_ISimulationEventManager, (void**)&pISimulationEventManager);
  pCaSimulationEventManager->InternalRelease();
  return pISimulationEventManager;
}

ISelectionManager* CaApplication::get_Selection()
{
  CA_CHECK_LICENSE(false);

  CaSelectionManager* pCaSelectionManager = (CaSelectionManager*)CaSelectionManager::CreateObject();
  if (pCaSelectionManager == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  ISelectionManager* pISelectionManager = NULL;
  LPDISPATCH pDisp = pCaSelectionManager->GetIDispatch(false);
  pDisp->QueryInterface(IID_ISelectionManager, (void**)&pISelectionManager);
  pCaSelectionManager->InternalRelease();
  return pISelectionManager;
}

ISolverManager* CaApplication::get_Solvers()
{
  CA_CHECK_LICENSE(false);

  CaSolverManager* pCaSolverManager = (CaSolverManager*)CaSolverManager::CreateObject();
  if (pCaSolverManager == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  ISolverManager* pISolverManager = NULL;
  LPDISPATCH pDisp = pCaSolverManager->GetIDispatch(false);
  pDisp->QueryInterface(IID_ISolverManager, (void**)&pISolverManager);
  pCaSolverManager->InternalRelease();
  return pISolverManager;
}

ISeaEnvironment* CaApplication::get_SeaEnvironment()
{
  CA_CHECK_LICENSE(false);

  CaSeaEnvironment* pCaSeaEnvironment = (CaSeaEnvironment*)CaSeaEnvironment::CreateObject();
  if (pCaSeaEnvironment == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  ISeaEnvironment* pISeaEnvironment = NULL;
  LPDISPATCH pDisp = pCaSeaEnvironment->GetIDispatch(false);
  pDisp->QueryInterface(IID_ISeaEnvironment, (void**)&pISeaEnvironment);
  pCaSeaEnvironment->InternalRelease();
  return pISeaEnvironment;
}

BOOL CaApplication::Open(LPCTSTR pszFileName, BOOL bLoadLinks)
{
  CA_CHECK_LICENSE(true);

  if (!FpPM::closeModel())
    return FALSE;

  if (FpPM::vpmModelOpen(CaConvert(pszFileName),bLoadLinks))
    return TRUE;

  FpPM::vpmModelNew();
  return FALSE;
}

BOOL CaApplication::Save()
{
  CA_CHECK_LICENSE(true);

  return FpPM::vpmModelSave();
}

BOOL CaApplication::SaveAs(LPCTSTR pszFileName, BOOL bSaveResults, BOOL bSaveReducedParts)
{
  CA_CHECK_LICENSE(true);

  return FpPM::vpmModelSaveAs(CaConvert(pszFileName), bSaveResults, bSaveReducedParts);
}

BOOL CaApplication::Close()
{
  CA_CHECK_LICENSE(true);

  if (!FpPM::closeModel())
    return FALSE;

  FpPM::vpmModelNew();
  return TRUE;
}

void CaApplication::Exit()
{
  CA_CHECK_LICENSE(true);

  if (FpPM::closeModel(false,true,true))
    PostQuitMessage(0);
}

void CaApplication::Show(BOOL bShowWindow)
{
  CA_CHECK_LICENSE(false);

  if (Fui::hasGUI()) {
    if (bShowWindow)
      Fui::getMainWindow()->popUp();
    else
      Fui::getMainWindow()->popDown();
  }
}

ITriad* CaApplication::CreateTriad(double x, double y, double z,
  const VARIANT FAR& rx, const VARIANT FAR& ry, const VARIANT FAR& rz,
  const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
  if (pCaTriad == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Convert rotations to double
  FaVec3 rot;
  VARIANT varR8;
  VariantInit(&varR8);
  VariantChangeType(&varR8, &rx, 0, VT_R8);
  if (varR8.vt == VT_R8) rot.x(varR8.dblVal);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &ry, 0, VT_R8);
  if (varR8.vt == VT_R8) rot.y(varR8.dblVal);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &rz, 0, VT_R8);
  if (varR8.vt == VT_R8) rot.z(varR8.dblVal);

  // Create triad
  FmTriad* triad = Fedem::createTriad(FaVec3(x,y,z),m_pCurrentSubAssembly);
  if (triad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  if (Desc.vt == VT_BSTR)
    triad->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  if (!rot.isZero()) {
    FaMat33 m;
    triad->setOrientation(m.eulerRotateZYX(rot));
  }

  triad->onChanged();

  pCaTriad->m_pTriad = triad;

  ITriad* pITriad = NULL;
  LPDISPATCH pDisp = pCaTriad->GetIDispatch(false);
  pDisp->QueryInterface(IID_ITriad, (void**)&pITriad);
  pCaTriad->InternalRelease();
  return pITriad;
}

IJoint* CaApplication::CreateJoint(ITriad* MasterTriad, ITriad* SlaveTriad,
  const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaJoint* pCaJoint = (CaJoint*)CaJoint::CreateObject();
  if (pCaJoint == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get triads
  if ((MasterTriad == NULL) || (SlaveTriad == NULL))
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  IDispatch* pDisp2 = NULL;
  MasterTriad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  SlaveTriad->QueryInterface(IID_IDispatch, (void**)&pDisp2);
  if ((pDisp1 == NULL) || (pDisp2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaMasterTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  CaTriad* pCaSlaveTriad  = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp2));
  pDisp1->Release();
  pDisp2->Release();
  if ((pCaMasterTriad == NULL) || (pCaSlaveTriad == NULL))
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* t1 = pCaMasterTriad->m_pTriad;
  FmTriad* t2 = pCaSlaveTriad->m_pTriad;

  // Create joint
  FmJointBase* joint = Fedem::createJoint(FmFreeJoint::getClassTypeID(),
                                          t1,t2,NULL,m_pCurrentSubAssembly);
  if (joint == NULL)
    AfxThrowOleException(E_INVALIDARG);

  if (Desc.vt == VT_BSTR)
    joint->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  t1->onChanged();
  t2->onChanged();
  joint->onChanged();

  pCaJoint->m_pJoint = static_cast<FmFreeJoint*>(joint);

  IJoint* pIJoint = NULL;
  LPDISPATCH pDisp = pCaJoint->GetIDispatch(false);
  pDisp->QueryInterface(IID_IJoint, (void**)&pIJoint);
  pCaJoint->InternalRelease();
  return pIJoint;
}

ICamJoint* CaApplication::CreateCamJoint(ITriad* SlaveTriad, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaCamJoint* pCaCamJoint = (CaCamJoint*)CaCamJoint::CreateObject();
  if (pCaCamJoint == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get slave triad
  if (SlaveTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  SlaveTriad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaSlaveTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaSlaveTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* ts = pCaSlaveTriad->m_pTriad;

  // Create cam joint
  FmCamJoint* joint = Fedem::createCamJoint(ts,m_pCurrentSubAssembly);
  if (joint == NULL)
    AfxThrowOleException(E_INVALIDARG);

  if (Desc.vt == VT_BSTR)
    joint->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  ts->onChanged();
  joint->onChanged();

  pCaCamJoint->m_pCamJoint = joint;

  ICamJoint* pICamJoint = NULL;
  LPDISPATCH pDisp = pCaCamJoint->GetIDispatch(false);
  pDisp->QueryInterface(IID_ICamJoint, (void**)&pICamJoint);
  pCaCamJoint->InternalRelease();
  return pICamJoint;
}

ICylindricJoint* CaApplication::CreateCylindricJoint(ITriad* SlaveTriad, 
  const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaCylindricJoint* pCaCylindricJoint = (CaCylindricJoint*)CaCylindricJoint::CreateObject();
  if (pCaCylindricJoint == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get slave triad
  if (SlaveTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  SlaveTriad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaSlaveTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaSlaveTriad == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* ts = pCaSlaveTriad->m_pTriad;
  if (ts == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Create cylindric joint
  FFaMsg::list("Creating cylindric joint.\n");
  FmCylJoint* joint = new FmCylJoint();
  if (joint == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  joint->setAsSlaveTriad(ts);
  joint->setGlobalCS(ts->getGlobalCS());
  joint->updateLocation();
  ts->onChanged();
  joint->setParentAssembly(m_pCurrentSubAssembly);
  joint->connect();
  pCaCylindricJoint->m_pCylJoint = joint;

  // Create master
  FmStraightMaster* lineMaster = new FmStraightMaster();
  if (lineMaster == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  lineMaster->setParentAssembly(m_pCurrentSubAssembly);
  lineMaster->connect();
  joint->setMaster(lineMaster);

  // Set user description
  if (Desc.vt == VT_BSTR)
    joint->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  joint->onChanged();
  joint->draw();

  // Com wrapper
  ICylindricJoint* pICylindricJoint = NULL;
  LPDISPATCH pDisp = pCaCylindricJoint->GetIDispatch(false);
  pDisp->QueryInterface(IID_ICylindricJoint, (void**)&pICylindricJoint);
  pCaCylindricJoint->InternalRelease();
  return pICylindricJoint;
}

IPart* CaApplication::CreatePart(double x, double y, double z, 
  const VARIANT FAR& rx, const VARIANT FAR& ry, const VARIANT FAR& rz, 
  const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaPart* pCaPart = (CaPart*)CaPart::CreateObject();
  if (pCaPart == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Convert rotations to double
  VARIANT varR8;
  VariantInit(&varR8);
  VariantChangeType(&varR8, &rx, 0, VT_R8);
  double fRx = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &ry, 0, VT_R8);
  double fRy = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &rz, 0, VT_R8);
  double fRz = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);

  // Create rotation matrix
  FaVec3 p(x,y,z);
  FaMat33 m;
  FaVec3 r(fRx,fRy,fRz);
  m.eulerRotateZYX(r);

  // Create triad
  FFaMsg::list("Creating generic part.\n");
  FmPart* part = new FmPart();
  if (part == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  part->useGenericProperties = true;
  part->setTranslation(p);
  part->setOrientation(m);
  part->setParentAssembly(m_pCurrentSubAssembly);
  part->connect();
  pCaPart->m_pGenPart = part;

  // Set user description
  if (Desc.vt == VT_BSTR)
    part->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  part->onChanged();
  part->draw();

  IPart* pIPart = NULL;
  LPDISPATCH pDisp = pCaPart->GetIDispatch(false);
  pDisp->QueryInterface(IID_IPart, (void**)&pIPart);
  pCaPart->InternalRelease();
  return pIPart;
}

IBeam* CaApplication::CreateBeam(ITriad* Triad1, ITriad* Triad2,
    const VARIANT FAR& CrossSection, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaBeam* pCaBeam = (CaBeam*)CaBeam::CreateObject();
  if (pCaBeam == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get triads and cross section
  if ((Triad1 == NULL) || (Triad2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  IDispatch* pDisp2 = NULL;
  IDispatch* pDispCS = NULL;
  Triad1->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  Triad2->QueryInterface(IID_IDispatch, (void**)&pDisp2);
  if (CrossSection.vt == VT_UNKNOWN)
    CrossSection.punkVal->QueryInterface(IID_IDispatch, (void**)&pDispCS);
  else if (CrossSection.vt == VT_DISPATCH)
    CrossSection.pdispVal->QueryInterface(IID_IDispatch, (void**)&pDispCS);
  if ((pDisp1 == NULL) || (pDisp2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad1 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  CaTriad* pCaTriad2 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp2));
  CaCrossSection* pCaCrossSection = NULL;
  if (pDispCS != NULL) {
    pCaCrossSection = dynamic_cast<CaCrossSection*>(CaCrossSection::FromIDispatch(pDispCS));
    pDispCS->Release();
  }
  pDisp1->Release();
  pDisp2->Release();
  if ((pCaTriad1 == NULL) || (pCaTriad2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* t1 = pCaTriad1->m_pTriad;
  FmTriad* t2 = pCaTriad2->m_pTriad;

  // Create beam
  FFaMsg::list("Creating beam.\n");
  FmBeam* beam = Fedem::createBeam(t1,t2,m_pCurrentSubAssembly);
  if (beam == NULL)
    AfxThrowOleException(E_INVALIDARG);

  if (Desc.vt == VT_BSTR)
    beam->setUserDescription(std::string(CW2A(Desc.bstrVal)));
  if (pCaCrossSection != NULL)
    beam->setProperty(pCaCrossSection->m_pCrossSection);

  beam->onChanged();

  pCaBeam->m_pBeam = beam;

  IBeam* pIBeam = NULL;
  LPDISPATCH pDisp = pCaBeam->GetIDispatch(false);
  pDisp->QueryInterface(IID_IBeam, (void**)&pIBeam);
  pCaBeam->InternalRelease();
  return pIBeam;
}

IFunction* CaApplication::CreateFunction(FunctionUsageType Usage, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
  if (pCaFunction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Create function
  FFaMsg::list("Creating function.\n");

  FmEngine* engine = NULL;
  FmMathFuncBase* func = new FmfLinVar();
  if (func == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  switch (Usage)
  {
  case FunctionUsageType::fuCurrentFunction:
    func->setFunctionUse(FmMathFuncBase::FuncUse::CURR_FUNCTION, true);
    break;
  case FunctionUsageType::fuDamperRotCoeff:
    func->setFunctionUse(FmMathFuncBase::FuncUse::DA_ROT_COEFF, true);
    break;
  case FunctionUsageType::fuDamperRotTorque:
    func->setFunctionUse(FmMathFuncBase::FuncUse::DA_ROT_TORQUE, true);
    break;
  case FunctionUsageType::fuDamperTransCoeff:
    func->setFunctionUse(FmMathFuncBase::FuncUse::DA_TRA_COEFF, true);
    break;
  case FunctionUsageType::fuDamperTransForce:
    func->setFunctionUse(FmMathFuncBase::FuncUse::DA_TRA_FORCE, true);
    break;
  case FunctionUsageType::fuDriveFile:
    func->setFunctionUse(FmMathFuncBase::FuncUse::DRIVE_FILE, true);
    break;
  case FunctionUsageType::fuGeneral:
    func->setFunctionUse(FmMathFuncBase::FuncUse::GENERAL, true);
    engine = new FmEngine();
    if (engine == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    engine->setParentAssembly(m_pCurrentSubAssembly);
    engine->connect();
    engine->setFunction(func);
    engine->onChanged();
    break;
  case FunctionUsageType::fuRoadFunction:
    func->setFunctionUse(FmMathFuncBase::FuncUse::ROAD_FUNCTION, true);
    break;
  case FunctionUsageType::fuSpringRotStiffness:
    func->setFunctionUse(FmMathFuncBase::FuncUse::SPR_ROT_STIFF, true);
    break;
  case FunctionUsageType::fuSpringRotTorque:
    func->setFunctionUse(FmMathFuncBase::FuncUse::SPR_ROT_TORQUE, true);
    break;
  case FunctionUsageType::fuSpringTransForce:
    func->setFunctionUse(FmMathFuncBase::FuncUse::SPR_TRA_FORCE, true);
    break;
  case FunctionUsageType::fuSpringTransStiffness:
    func->setFunctionUse(FmMathFuncBase::FuncUse::SPR_TRA_STIFF, true);
    break;
  case FunctionUsageType::fuWaveFunction:
    func->setFunctionUse(FmMathFuncBase::FuncUse::WAVE_FUNCTION, true);
    break;
  default:
    func->setFunctionUse(FmMathFuncBase::FuncUse::NONE, true);
  }

  func->setParentAssembly(m_pCurrentSubAssembly);
  func->connect();

  if (engine)
    pCaFunction->m_ptr = engine;
  else
    pCaFunction->m_ptr = func;
  pCaFunction->m_nUsage = Usage;

  // Set user description
  if (Desc.vt == VT_BSTR)
    pCaFunction->m_ptr->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  func->onChanged();

  IFunction* pIFunction = NULL;
  LPDISPATCH pDisp = pCaFunction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFunction, (void**)&pIFunction);
  pCaFunction->InternalRelease();
  return pIFunction;
}

IStrainRosette* CaApplication::CreateStrainRosette(StrainRosetteType SRType,
    IPart* Part, long Node1, long Node2, long Node3, long Node4,
    const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaStrainRosette* pCaStrainRosette = (CaStrainRosette*)CaStrainRosette::CreateObject();
  if (pCaStrainRosette == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  if (Part == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Part->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaPart* pCaPart = dynamic_cast<CaPart*>(CaPart::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaPart == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Create strain rosette
  FFaMsg::list("Creating strain rosette.\n");
  FmStrainRosette* strainRosette = new FmStrainRosette();
  if (strainRosette == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  if (!strainRosette->setTopology(pCaPart->m_pGenPart,Node1,Node2,Node3,Node4))
  {
    strainRosette->erase();
    AfxThrowOleException(E_INVALIDARG);
  }

  switch (SRType)
  {
  case StrainRosetteType::srDoubleGage90:
    strainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::DOUBLE_GAGE_90);
    break;
  case StrainRosetteType::srTripleGage45:
    strainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::TRIPLE_GAGE_45);
    break;
  case StrainRosetteType::srTripleGage60:
    strainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::TRIPLE_GAGE_60);
    break;
  default:
    strainRosette->rosetteType.setValue(FmStrainRosette::RosetteType::SINGLE_GAGE);
  }

  strainRosette->setParentAssembly(m_pCurrentSubAssembly);
  strainRosette->connect();

  // Set user description
  if (Desc.vt == VT_BSTR)
    strainRosette->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  strainRosette->onChanged();
  strainRosette->syncWithFEModel();
  strainRosette->draw();

  pCaStrainRosette->m_pStrainRosette = strainRosette;

  IStrainRosette* pIStrainRosette = NULL;
  LPDISPATCH pDisp = pCaStrainRosette->GetIDispatch(false);
  pDisp->QueryInterface(IID_IStrainRosette, (void**)&pIStrainRosette);
  pCaStrainRosette->InternalRelease();
  return pIStrainRosette;
}

ICrossSection* CaApplication::CreateCrossSection(CrossSectionType CSType, 
    IMaterial* Material, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaCrossSection* pCaCrossSection = (CaCrossSection*)CaCrossSection::CreateObject();
  if (pCaCrossSection == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get material
  if (Material == NULL)
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  Material->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CaMaterial* pCaMaterial = dynamic_cast<CaMaterial*>(CaMaterial::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaMaterial == NULL)
    AfxThrowOleException(E_INVALIDARG);
  FmMaterialProperty* m = pCaMaterial->m_ptr;
  if (m == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Create cross section
  FFaMsg::list("Creating cross section.\n");
  FmBeamProperty* beamProp = new FmBeamProperty();
  if (beamProp == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  beamProp->crossSectionType.setValue((FmBeamProperty::FmSection)CSType);
  beamProp->material.setRef(m);
  beamProp->setParentAssembly(m_pCurrentSubAssembly);
  beamProp->connect();
  pCaCrossSection->m_pCrossSection = beamProp;

  // Set user description
  if (Desc.vt == VT_BSTR)
    beamProp->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  beamProp->onChanged();

  ICrossSection* pICrossSection = NULL;
  LPDISPATCH pDisp = pCaCrossSection->GetIDispatch(false);
  pDisp->QueryInterface(IID_ICrossSection, (void**)&pICrossSection);
  pCaCrossSection->InternalRelease();
  return pICrossSection;
}

IMaterial* CaApplication::CreateMaterial(double Rho, double E, double nu,
    const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaMaterial* pCaMaterial = (CaMaterial*)CaMaterial::CreateObject();
  if (pCaMaterial == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Create material
  FFaMsg::list("Creating material.\n");
  FmMaterialProperty* beamMat = new FmMaterialProperty();
  if (beamMat == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  if (!beamMat->updateProperties(Rho,E,beamMat->G.getValue(),nu)) {
    beamMat->erase();
    pCaMaterial->InternalRelease();
    AfxThrowOleException(E_INVALIDARG);
  }
  beamMat->setParentAssembly(m_pCurrentSubAssembly);
  beamMat->connect();
  pCaMaterial->m_ptr = beamMat;

  // Set user description
  if (Desc.vt == VT_BSTR)
    beamMat->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  beamMat->onChanged();

  IMaterial* pIMaterial = NULL;
  LPDISPATCH pDisp = pCaMaterial->GetIDispatch(false);
  pDisp->QueryInterface(IID_IMaterial, (void**)&pIMaterial);
  pCaMaterial->InternalRelease();
  return pIMaterial;
}

IFriction* CaApplication::CreateFriction(FrictionType FricType, double PrestressForce, double CoulombCoefficient, 
    double StribeckMagnitude, double CriticalStribeckSpeed, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaFriction* pCaFriction = (CaFriction*)CaFriction::CreateObject();
  if (pCaFriction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Create friction
  FFaMsg::list("Creating friction.\n");

  FmFrictionBase* friction = NULL;

  switch (FricType){
    case FrictionType::TranslationalFriction:
      friction = new FmTransFriction();
      break;
    case FrictionType::RotationalFriction:
      friction = new FmRotFriction();
      break;
    case FrictionType::PrismaticFriction:
      friction = new FmPrismaticFriction();
      break;
    case FrictionType::BearingFriction:
      friction = new FmBearingFriction();
      break;
    case FrictionType::CamFriction:
      friction = new FmCamFriction();
      break;
    default:
      AfxThrowOleException(E_INVALIDARG);
  }

  if (friction == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  friction->setPrestressLoad(PrestressForce);
  friction->setCoulombCoeff(CoulombCoefficient);
  friction->setStribeckMagn(StribeckMagnitude);
  friction->setStribeckSpeed(CriticalStribeckSpeed);
  friction->setParentAssembly(m_pCurrentSubAssembly);
  friction->connect();
  pCaFriction->m_pFriction = friction;

  // Set user description
  if (Desc.vt == VT_BSTR)
    friction->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  friction->onChanged();

  IFriction* pIFriction = NULL;
  LPDISPATCH pDisp = pCaFriction->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFriction, (void**)&pIFriction);
  pCaFriction->InternalRelease();
  return pIFriction;
}

IAxialSpring* CaApplication::CreateAxialSpring(ITriad* Triad1, ITriad* Triad2,
  const VARIANT FAR& Stiffness, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaAxialSpring* pCaAxialSpring = (CaAxialSpring*)CaAxialSpring::CreateObject();
  if (pCaAxialSpring == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get triads
  if ((Triad1 == NULL) || (Triad2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  IDispatch* pDisp2 = NULL;
  Triad1->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  Triad2->QueryInterface(IID_IDispatch, (void**)&pDisp2);
  if ((pDisp1 == NULL) || (pDisp2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad1 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  CaTriad* pCaTriad2 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp2));
  pDisp1->Release();
  pDisp2->Release();
  if ((pCaTriad1 == NULL) || (pCaTriad2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* t1 = pCaTriad1->m_pTriad;
  FmTriad* t2 = pCaTriad2->m_pTriad;

  // Create axial spring
  FmAxialSpring* spring = Fedem::createAxialSpring(t1,t2,m_pCurrentSubAssembly);
  if (spring == NULL)
    AfxThrowOleException(E_INVALIDARG);

  if (Desc.vt == VT_BSTR)
    spring->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  // Stiffness
  double fStiffness = 0.0;
  FmModelMemberBase* pStiffnessFunction = NULL;
  GetFunctionParameter(Stiffness, &fStiffness, &pStiffnessFunction);
  spring->setInitStiff(fStiffness);
  spring->setSpringCharOrStiffFunction(pStiffnessFunction);

  spring->onChanged();

  pCaAxialSpring->m_pAxialSpring = spring;

  IAxialSpring* pIAxialSpring = NULL;
  LPDISPATCH pDisp = pCaAxialSpring->GetIDispatch(false);
  pDisp->QueryInterface(IID_IAxialSpring, (void**)&pIAxialSpring);
  pCaAxialSpring->InternalRelease();
  return pIAxialSpring;
}

IAxialDamper* CaApplication::CreateAxialDamper(ITriad* Triad1, ITriad* Triad2,
  const VARIANT FAR& Damper, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaAxialDamper* pCaAxialDamper = (CaAxialDamper*)CaAxialDamper::CreateObject();
  if (pCaAxialDamper == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get triads
  if ((Triad1 == NULL) || (Triad2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  IDispatch* pDisp1 = NULL;
  IDispatch* pDisp2 = NULL;
  Triad1->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  Triad2->QueryInterface(IID_IDispatch, (void**)&pDisp2);
  if ((pDisp1 == NULL) || (pDisp2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  CaTriad* pCaTriad1 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  CaTriad* pCaTriad2 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp2));
  pDisp1->Release();
  pDisp2->Release();
  if ((pCaTriad1 == NULL) || (pCaTriad2 == NULL))
    AfxThrowOleException(E_INVALIDARG);
  FmTriad* t1 = pCaTriad1->m_pTriad;
  FmTriad* t2 = pCaTriad2->m_pTriad;

  // Create axial damper
  FmAxialDamper* damper = Fedem::createAxialDamper(t1,t2,m_pCurrentSubAssembly);
  if (damper == NULL)
    AfxThrowOleException(E_INVALIDARG);

  if (Desc.vt == VT_BSTR)
    damper->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  // Damping
  double fDamper = 0.0;
  FmModelMemberBase* pDamperFunction = NULL;
  GetFunctionParameter(Damper, &fDamper, &pDamperFunction);
  damper->setInitDamp(fDamper);
  damper->setDampEngine((FmEngine*)pDamperFunction);

  damper->onChanged();

  pCaAxialDamper->m_pAxialDamper = damper;

  IAxialDamper* pIAxialDamper = NULL;
  LPDISPATCH pDisp = pCaAxialDamper->GetIDispatch(false);
  pDisp->QueryInterface(IID_IAxialDamper, (void**)&pIAxialDamper);
  pCaAxialDamper->InternalRelease();
  return pIAxialDamper;
}

IResultExtractor* CaApplication::CreateResultExtractor(LPCTSTR FRSFileNames)
{
  CA_CHECK_LICENSE(true);

#ifdef FT_HAS_GRAPHVIEW
  CaResultExtractor* pCaResultExtractor = (CaResultExtractor*)CaResultExtractor::CreateObject();
  if (pCaResultExtractor == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Get FRS file names
  std::string FRSFileList;
  if (FRSFileNames != NULL)
    FRSFileList = CaConvert(FRSFileNames);
  if (FRSFileList.empty())
    FRSFileList = FmDB::getMechanismObject()->getResultStatusData()->getFileNames("frs");

  // Create result extractor
  FFaMsg::list("Creating result extractor.\n");
  if (!pCaResultExtractor->CreateExtractor(FRSFileList.c_str()))
    return NULL;

  IResultExtractor* pIResultExtractor = NULL;
  LPDISPATCH pDisp = pCaResultExtractor->GetIDispatch(false);
  pDisp->QueryInterface(IID_IResultExtractor, (void**)&pIResultExtractor);
  pCaResultExtractor->InternalRelease();
  return pIResultExtractor;
#else
  return NULL;
#endif
}

ISubAssembly* CaApplication::CreateSubAssembly(SubAssemblyType SAType, const VARIANT FAR& x, const VARIANT FAR& y, const VARIANT FAR& z, 
  const VARIANT FAR& rx, const VARIANT FAR& ry, const VARIANT FAR& rz, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaSubAssembly* pCaSubAssembly = (CaSubAssembly*)CaSubAssembly::CreateObject();
  if (pCaSubAssembly == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Convert positions and rotations to double
  VARIANT varR8;
  VariantInit(&varR8);
  VariantChangeType(&varR8, &x, 0, VT_R8);
  double fX = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &y, 0, VT_R8);
  double fY = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &z, 0, VT_R8);
  double fZ = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &rx, 0, VT_R8);
  double fRx = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &ry, 0, VT_R8);
  double fRy = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);
  VariantInit(&varR8);
  VariantChangeType(&varR8, &rz, 0, VT_R8);
  double fRz = (varR8.vt == VT_R8) ? (varR8.dblVal) : (0);

  // Create subassembly
  FFaMsg::list("Creating subassembly.\n");
  FmAssemblyBase* pSA = NULL;
  if (SAType == saGeneral)
    pSA = new FmStructAssembly();
  else if (SAType == saRiser)
    pSA = new FmRiser();
  else if (SAType == saJacket)
    pSA = new FmJacket();
  else if (SAType == saSoilPile)
    pSA = new FmSoilPile();
  else
    AfxThrowOleException(E_INVALIDARG);

  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set position and rotation
  FFa3DLocation l = pSA->getLocation();
  FaVec3 v = l.translation();
  v.x(fX);
  v.y(fY);
  v.z(fZ);
  l.setPos(FFa3DLocation::CART_X_Y_Z, v);
  FaMat33 m;
  FaVec3 r(fRx,fRy,fRz);
  m.eulerRotateZYX(r);
  l.setRot(FFa3DLocation::EUL_Z_Y_X, m);
  pSA->setLocation(l);

  // Set user description
  if (Desc.vt == VT_BSTR)
    pSA->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  // Connect
  pSA->setParentAssembly(m_pCurrentSubAssembly);
  pSA->connect();
  pSA->onChanged();
  pCaSubAssembly->m_pSubAssembly = pSA;

  ISubAssembly* pISubAssembly = NULL;
  LPDISPATCH pDisp = pCaSubAssembly->GetIDispatch(false);
  pDisp->QueryInterface(IID_ISubAssembly, (void**)&pISubAssembly);
  pCaSubAssembly->InternalRelease();
  return pISubAssembly;
}

void CaApplication::SetCurrentSubAssembly(const VARIANT FAR& SubAssembly)
{
  CA_CHECK_LICENSE(true);

  if (SubAssembly.vt == VT_EMPTY || SubAssembly.vt == VT_NULL || SubAssembly.vt == VT_ERROR)
    // Empty, null or error
    m_pCurrentSubAssembly = NULL;
  else if (SubAssembly.vt == VT_DISPATCH || SubAssembly.vt == VT_UNKNOWN) {
    // IDispatch or IUnknown pointer
    IUnknown* pUnk = SubAssembly.vt == VT_DISPATCH ? SubAssembly.pdispVal : SubAssembly.punkVal;
    IDispatch* pDisp1 = NULL;
    pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(E_INVALIDARG);
    CaSubAssembly* pCaSubAssembly = dynamic_cast<CaSubAssembly*>(CaSubAssembly::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCaSubAssembly == NULL)
      AfxThrowOleException(E_INVALIDARG);
    m_pCurrentSubAssembly = pCaSubAssembly->m_pSubAssembly;
  }
  else
    // Any other variant type
    AfxThrowOleException(E_INVALIDARG);
}

IUserDefinedElement* CaApplication::CreateUserDefinedElement(long ElementType, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  char typeName[64];
  long numElementNodes = FiUserElmPlugin::instance()->getTypeName(ElementType, 64, typeName);
  if (numElementNodes < 0)
    AfxThrowOleException(E_INVALIDARG);

  CaUserDefinedElement* pCaUserDefinedElement = (CaUserDefinedElement*)CaUserDefinedElement::CreateObject();
  if (pCaUserDefinedElement == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  pCaUserDefinedElement->m_numElementNodes = numElementNodes;

  // Create user defined element
  FFaMsg::list("Creating user defined element.\n");
  FmUserDefinedElement* pUDE = new FmUserDefinedElement();
  if (pUDE == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  pUDE->setParentAssembly(m_pCurrentSubAssembly);

  // Set user description
  if (Desc.vt == VT_BSTR)
    pUDE->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  // Connect
  pUDE->connect();
  pCaUserDefinedElement->m_ptr = pUDE;

  std::vector<FmTriad*> dummy;
  pUDE->init(ElementType,typeName,dummy);

  IUserDefinedElement* pIUserDefinedElement = NULL;
  LPDISPATCH pDisp = pCaUserDefinedElement->GetIDispatch(false);
  pDisp->QueryInterface(IID_IUserDefinedElement, (void**)&pIUserDefinedElement);
  pCaUserDefinedElement->InternalRelease();
  return pIUserDefinedElement;
}

IGenericObject* CaApplication::CreateGenericObject(LPCTSTR ObjectType, LPCTSTR ObjectDefinition, const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaGenericObject* pCaGenericObject = (CaGenericObject*)CaGenericObject::CreateObject();
  if (pCaGenericObject == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Create generic object
  FFaMsg::list("Creating generic object.\n");
  FmGenericDBObject* obj = new FmGenericDBObject();
  if (obj == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  obj->setParentAssembly(m_pCurrentSubAssembly);
  obj->objectType.setValue(CaConvert(ObjectType));
  obj->objectDefinition.setValue(CaConvert(ObjectDefinition));
  obj->connect();
  pCaGenericObject->m_pGenericObject = obj;

  // Set user description
  if (Desc.vt == VT_BSTR)
    obj->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  IGenericObject* pIGenericObject = NULL;
  LPDISPATCH pDisp = pCaGenericObject->GetIDispatch(false);
  pDisp->QueryInterface(IID_IGenericObject, (void**)&pIGenericObject);
  pCaGenericObject->InternalRelease();
  return pIGenericObject;
}

IVesselMotion* CaApplication::CreateVesselMotion(const VARIANT FAR& Desc)
{
  CA_CHECK_LICENSE(true);

  CaVesselMotion* pCaVesselMotion = (CaVesselMotion*)CaVesselMotion::CreateObject();
  if (pCaVesselMotion == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Create generic object
  FFaMsg::list("Creating vessel motion.\n");
  FmVesselMotion* obj = new FmVesselMotion();
  if (obj == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  obj->setParentAssembly(m_pCurrentSubAssembly);
  obj->connect();
  obj->createMotions();
  pCaVesselMotion->m_ptr = obj;

  // Set user description
  if (Desc.vt == VT_BSTR)
    obj->setUserDescription(std::string(CW2A(Desc.bstrVal)));

  IVesselMotion* pIVesselMotion = NULL;
  LPDISPATCH pDisp = pCaVesselMotion->GetIDispatch(false);
  pDisp->QueryInterface(IID_IVesselMotion, (void**)&pIVesselMotion);
  pCaVesselMotion->InternalRelease();
  return pIVesselMotion;
}

BOOL CaApplication::CreateMooringLine(ITriad* Triad1, ITriad* Triad2, double Length, double NumSegments, long ElementType)
{
  CA_CHECK_LICENSE(true);

  if (Triad1 == NULL || Triad2 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  IDispatch* pDisp1 = NULL;
  IDispatch* pDisp2 = NULL;
  Triad1->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  Triad2->QueryInterface(IID_IDispatch, (void**)&pDisp2);
  if (pDisp1 == NULL || pDisp2 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  CaTriad* pCaTriad1 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  CaTriad* pCaTriad2 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp2));
  pDisp1->Release();
  pDisp2->Release();
  if (pCaTriad1 == NULL || pCaTriad2 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  return Fedem::createMooringLine(pCaTriad1->m_pTriad, pCaTriad2->m_pTriad, Length,
                                  NumSegments, ElementType, m_pCurrentSubAssembly);
}

long CaApplication::GetCount(BOOL Recursive)
{
  CA_CHECK_LICENSE(false);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID());
  for (FmModelMemberBase* item : items)
    if (Recursive || item->getParentAssembly() == NULL)
      nCount++;

  return nCount;
}

IDispatch* CaApplication::GetItem(long Index, BOOL Recursive)
{
  CA_CHECK_LICENSE(false);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID());
  for (FmModelMemberBase* item : items)
    if (Recursive || item->getParentAssembly() == NULL)
      if (Index == nCount++)
        return CaApplication::CreateCOMObjectWrapper(item);

  return NULL;
}

BSTR CaApplication::GetItemType(long Index, BOOL Recursive)
{
  CA_CHECK_LICENSE(false);

  UINT nCount = 0;
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items, FmModelMemberBase::getClassTypeID());
  for (FmModelMemberBase* item : items)
    if (Recursive || item->getParentAssembly() == NULL)
      if (Index == nCount++)
        return SysAllocString(CA2W(item->getItemName()));

  return NULL;
}

IDispatch* CaApplication::FindByBaseID(long BaseID)
{
  CA_CHECK_LICENSE(false);

  FmModelMemberBase* item = FmDB::findObject(BaseID);
  return item ? CreateCOMObjectWrapper(item) : NULL;
}

IDispatch* CaApplication::FindByDescription(LPCTSTR UserDescription)
{
  CA_CHECK_LICENSE(false);

  std::string descr = CaConvert(UserDescription);
  std::vector<FmModelMemberBase*> items;
  FmDB::getAllOfType(items,FmModelMemberBase::getClassTypeID());
  for (FmModelMemberBase* item : items)
    if (item->getUserDescription().compare(descr) == 0)
      return CreateCOMObjectWrapper(item);

  return NULL;
}

void CaApplication::Remove(IDispatch* Child, BOOL Recursive)
{
  CA_CHECK_LICENSE(true);

  FmModelMemberBase* pChild = CaApplication::GetFromCOMObjectWrapper(Child);

  if (pChild != NULL)
    if (Recursive || !pChild->isOfType(FmSubAssembly::getClassTypeID()))
      pChild->erase();
}

void CaApplication::AttachToGround(ITriad* Triad)
{
  CA_CHECK_LICENSE(true);

  if (Triad == NULL)
    AfxThrowOleException(E_INVALIDARG);

  IDispatch* pDisp1 = NULL;
  Triad->QueryInterface(IID_IDispatch, (void**)&pDisp1);
  if (pDisp1 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  CaTriad* pCaTriad1 = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp1));
  pDisp1->Release();
  if (pCaTriad1 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  FmTriad* t1 = pCaTriad1->m_pTriad;
  if (t1 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  t1->connect(FmDB::getEarthLink());
  t1->onChanged();
  t1->draw();
}

IPart* CaApplication::LoadFEPart(LPCTSTR FileName, BOOL RelativePath)
{
  CA_CHECK_LICENSE(true);

  std::string fileName = CaConvert(FileName);
  if (!FmFileSys::isReadable(fileName))
    return NULL;

  FmPart* part = new FmPart;
  if (part == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  part->setParentAssembly(m_pCurrentSubAssembly);
  part->connect();
  part->myCalculateMass.setValue(FmPart::FROM_FEM);
  if (!part->importPart(fileName,NULL,RelativePath))
  {
    part->erase();
    return NULL;
  }

  part->setUserDescription(FFaFilePath::getBaseName(fileName,true));
  part->onChanged();
  part->draw();

  std::vector<FmTriad*> localTriads;
  part->getTriads(localTriads);
  for (FmTriad* triad : localTriads)
    triad->draw();

  CaPart* pCaPart = (CaPart*)CaPart::CreateObject();
  if (pCaPart == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaPart->m_pGenPart = part;

  IPart* pIPart = NULL;
  LPDISPATCH pDisp = pCaPart->GetIDispatch(false);
  pDisp->QueryInterface(IID_IPart, (void**)&pIPart);
  pCaPart->InternalRelease();

  return pIPart;
}

IFmObject* CaApplication::GetFmObject(IDispatch* Object)
{
  CA_CHECK_LICENSE(false);

  FmModelMemberBase* modelObj = CaApplication::GetFromCOMObjectWrapper(Object);
  if (modelObj == NULL)
    return NULL;

  CaFmObject* pCaFmObject = (CaFmObject*)CaFmObject::CreateObject();
  if (pCaFmObject == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaFmObject->m_ptr = modelObj;

  IFmObject* pIFmObject = NULL;
  LPDISPATCH pDisp = pCaFmObject->GetIDispatch(false);
  pDisp->QueryInterface(IID_IFmObject, (void**)&pIFmObject);
  pCaFmObject->InternalRelease();
  return pIFmObject;
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaApplication::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaApplication::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaApplication::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaApplication::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaApplication::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaApplication::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
                                   lcid, rgdispid);
}
STDMETHODIMP CaApplication::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaApplication, LocalClass)
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

STDMETHODIMP CaApplication::XLocalClass::get_Item(long Index, IDispatch** ppObj)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppObj = pThis->get_Item(Index);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_ExePath(BSTR* pVal)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pVal = pThis->get_ExePath();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_ModelFileName(BSTR* pVal)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pVal = pThis->get_ModelFileName();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_Count(long* pVal)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pVal = pThis->get_Count();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_SimulationEvents(ISimulationEventManager** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->get_SimulationEvents();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_Selection(ISelectionManager** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->get_Selection();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_Solvers(ISolverManager** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->get_Solvers();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::get_SeaEnvironment(ISeaEnvironment** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->get_SeaEnvironment();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::Open(BSTR FileName, VARIANT_BOOL LoadLinks, VARIANT_BOOL* pbRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pbRet = pThis->Open(FileName, LoadLinks);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::Save(VARIANT_BOOL* pbRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pbRet = pThis->Save();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::SaveAs(BSTR FileName, VARIANT_BOOL SaveResults, VARIANT_BOOL SaveReducedParts, VARIANT_BOOL* pbRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pbRet = pThis->SaveAs(FileName, SaveResults, SaveReducedParts);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::Close(VARIANT_BOOL* pbRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pbRet = pThis->Close();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::Exit()
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    pThis->Exit();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::Show(VARIANT_BOOL ShowWindow)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    pThis->Show(ShowWindow);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateTriad(double x, double y, double z, 
  VARIANT rx, VARIANT ry, VARIANT rz, VARIANT Desc, ITriad** ppTriad)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppTriad = pThis->CreateTriad(x,y,z,rx,ry,rz,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateJoint(ITriad* MasterTriad, 
  ITriad* SlaveTriad, VARIANT Desc, IJoint** ppJoint)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppJoint = pThis->CreateJoint(MasterTriad,SlaveTriad,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateCamJoint(ITriad* SlaveTriad, 
  VARIANT Desc, ICamJoint** ppCamJoint)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppCamJoint = pThis->CreateCamJoint(SlaveTriad,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateCylindricJoint(ITriad* SlaveTriad,
  VARIANT Desc, ICylindricJoint** ppCylindricJoint)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppCylindricJoint = pThis->CreateCylindricJoint(SlaveTriad,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreatePart(double x, double y, double z, 
  VARIANT rx, VARIANT ry, VARIANT rz, VARIANT Desc, IPart** ppPart)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppPart = pThis->CreatePart(x,y,z,rx,ry,rz,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateBeam(ITriad* Triad1, 
  ITriad* Triad2, VARIANT CrossSection, VARIANT Desc, IBeam** ppBeam)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppBeam = pThis->CreateBeam(Triad1,Triad2,CrossSection,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateFunction(FunctionUsageType Usage, 
  VARIANT Desc, IFunction** ppFunction)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppFunction = pThis->CreateFunction(Usage, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateStrainRosette(StrainRosetteType SRType, 
    IPart* Part, long Node1, long Node2, long Node3, long Node4,
    VARIANT Desc, IStrainRosette** ppStrainRosette)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppStrainRosette = pThis->CreateStrainRosette(SRType,Part,Node1,Node2,Node3,Node4,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateCrossSection(CrossSectionType CSType,
  IMaterial* Material, VARIANT Desc, ICrossSection** ppCrossSection)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppCrossSection = pThis->CreateCrossSection(CSType,Material,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateMaterial(double Rho, 
  double E, double nu, VARIANT Desc, IMaterial** ppMaterial)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppMaterial = pThis->CreateMaterial(Rho,E,nu,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateFriction(FrictionType FricType, double PrestressForce, double CoulombCoefficient, 
  double StribeckMagnitude, double CriticalStribeckSpeed, VARIANT Desc, IFriction** ppFriction)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppFriction = pThis->CreateFriction(FricType, PrestressForce, CoulombCoefficient,
                                        StribeckMagnitude, CriticalStribeckSpeed, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateAxialSpring(ITriad* Triad1, ITriad* Triad2, 
  VARIANT Stiffness, VARIANT Desc, IAxialSpring** ppAxialSpring)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppAxialSpring = pThis->CreateAxialSpring(Triad1, Triad2, Stiffness, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateAxialDamper(ITriad* Triad1, ITriad* Triad2, 
  VARIANT Damper, VARIANT Desc, IAxialDamper** ppAxialDamper)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppAxialDamper = pThis->CreateAxialDamper(Triad1, Triad2, Damper, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateResultExtractor(BSTR FRSFileNames,
  IResultExtractor** ppResultExtractor)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppResultExtractor = pThis->CreateResultExtractor(FRSFileNames);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateSubAssembly(SubAssemblyType SAType, VARIANT x, VARIANT y, VARIANT z, 
      VARIANT rx, VARIANT ry, VARIANT rz, VARIANT Desc, ISubAssembly** ppSubAssembly)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppSubAssembly = pThis->CreateSubAssembly(SAType, x,y,z,rx,ry,rz,Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::SetCurrentSubAssembly(VARIANT SubAssembly)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    pThis->SetCurrentSubAssembly(SubAssembly);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateUserDefinedElement(long ElementType, VARIANT Desc, IUserDefinedElement** ppUserDefinedElement)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppUserDefinedElement = pThis->CreateUserDefinedElement(ElementType, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateGenericObject(BSTR ObjectType, BSTR ObjectDefinition, VARIANT Desc, IGenericObject** ppGenericObject)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppGenericObject = pThis->CreateGenericObject(ObjectType, ObjectDefinition, Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateVesselMotion(VARIANT Desc, IVesselMotion** ppVesselMotion)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppVesselMotion = pThis->CreateVesselMotion(Desc);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::CreateMooringLine(ITriad* Triad1,
  ITriad* Triad2, double Length, double NumSegments, long ElementType, VARIANT_BOOL* pbRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pbRet = pThis->CreateMooringLine(Triad1, Triad2, Length, NumSegments, ElementType);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::GetCount(VARIANT_BOOL Recursive, long* pnVal)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pnVal = pThis->GetCount(Recursive);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::GetItem(THIS_ long Index, VARIANT_BOOL Recursive, IDispatch** ppObj)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppObj = pThis->GetItem(Index, Recursive);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::GetItemType(THIS_ long Index, VARIANT_BOOL Recursive, BSTR* pVal)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *pVal = pThis->GetItemType(Index, Recursive);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::FindByBaseID(long BaseID, IDispatch** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->FindByBaseID(BaseID);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::FindByDescription(BSTR UserDescription, IDispatch** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->FindByDescription(UserDescription);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::Remove(IDispatch* Child, VARIANT_BOOL Recursive)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    pThis->Remove(Child, Recursive);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::AttachToGround(ITriad* Triad)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    pThis->AttachToGround(Triad);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::LoadFEPart(BSTR FileName, VARIANT_BOOL RelativePath, IPart** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->LoadFEPart(FileName, RelativePath);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaApplication::XLocalClass::GetFmObject(IDispatch* Object, IFmObject** ppRet)
{
  METHOD_PROLOGUE(CaApplication, LocalClass);
  TRY
  {
    *ppRet = pThis->GetFmObject(Object);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Utilities

// Creates a safearray that is to be returned from the COM-API.
// Note:
// - This is designed to work well in both VBScript and .NET.
// - Use "GetRotationMatrix av" without parentheses in VBScript.
//   This means that the parameter is passed ByRef.
void CaApplication::CreateSafeArray(VARIANT* Array3x3, const FaMat33& m)
{
  if (Array3x3 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Create safearray
  SAFEARRAYBOUND bounds[2];
  bounds[0].lLbound = 0;
  bounds[0].cElements = 3;
  bounds[1].lLbound = 0;
  bounds[1].cElements = 3;
  SAFEARRAY* pSA = SafeArrayCreate(VT_VARIANT, 2, bounds);
  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set data
  LONG ix[2];
  VARIANT varR8;
  VariantInit(&varR8);
  varR8.vt = VT_R8;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      // Set array (i,j) index
      ix[0] = i;
      ix[1] = j;
      // Set element data
      varR8.dblVal = m(j+1, i+1);
      SafeArrayPutElement(pSA, ix, &varR8);
    }

  // Set variant
  Array3x3->vt = VT_ARRAY | VT_VARIANT;
  Array3x3->parray = pSA;
}

// Creates a safearray that is to be returned from the COM-API.
void CaApplication::CreateSafeArray(VARIANT* Array4x3, const FaMat34& m)
{
  if (Array4x3 == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Create safearray
  SAFEARRAYBOUND bounds[2];
  bounds[0].lLbound = 0;
  bounds[0].cElements = 4;
  bounds[1].lLbound = 0;
  bounds[1].cElements = 3;
  SAFEARRAY* pSA = SafeArrayCreate(VT_VARIANT, 2, bounds);
  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set data
  LONG ix[2];
  VARIANT varR8;
  VariantInit(&varR8);
  varR8.vt = VT_R8;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 3; j++) {
      // Set array (i,j) index
      ix[0] = i;
      ix[1] = j;
      // Set element data
      varR8.dblVal = m(j+1, i+1);
      SafeArrayPutElement(pSA, ix, &varR8);
    }

  // Set variant
  Array4x3->vt = VT_ARRAY | VT_VARIANT;
  Array4x3->parray = pSA;
}

// Gets data from a 3x3 safearray that is passed to the COM-API.
void CaApplication::GetFromSafeArray(const VARIANT& Array3x3, FaMat33& m)
{
  double d[9];
  GetFromSafeArray(Array3x3, 3, 3, d);
  m = FaMat33(d);
}

// Gets data from a 4x3 safearray that is passed to the COM-API.
void CaApplication::GetFromSafeArray(const VARIANT& Array4x3, FaMat34& m)
{
  double d[12];
  GetFromSafeArray(Array4x3, 4, 3, d);
  m = FaMat34(d);
}

// Gets data from a safearray that is passed to the COM-API.
// - ArrayNxM is a NxM array.
// - N and M are the upper bounds for dimension 1 and 2.
//   I.e. [0 to N-1, 0 to M-1].
//   If M is 0 then we have a 1D array.
// - pData is a pointer to a buffer with NxM double values.
// Note:
// - This is designed to work well in both VBScript and .NET.
// - Use "SetRotationMatrix(ar)" with parentheses in VBScript.
//   This means that the parameter is passed ByVal.
// - Variant types:
//   VBScript: Dim arr(2,2) -> VT_ARRAY|VT_BYREF|VT_VARIANT
//   VBScript: Dim av=arr   -> VT_ARRAY|VT_VARIANT
//   VB.NET:   Dim arr(2,2) -> VT_ARRAY|VT_VARIANT
//   VB.NET:   Dim arr(2,2) As Double -> VT_ARRAY|VT_R8
void CaApplication::GetFromSafeArray(const VARIANT& ArrayNxM, int N, int M, double* pData)
{
  // Get safearray
  SAFEARRAY* pSA = NULL;
  if ((ArrayNxM.vt & VT_ARRAY) && (ArrayNxM.vt & VT_BYREF))
    pSA = *ArrayNxM.pparray;
  else if (ArrayNxM.vt & VT_ARRAY)
    pSA = ArrayNxM.parray;
  if (pSA == NULL)
    AfxThrowOleException(E_INVALIDARG);
  UINT nDims = (M == 0) ? (1) : (2);
  if (SafeArrayGetDim(pSA) != nDims)
    AfxThrowOleException(E_INVALIDARG);
  LONG nLBound1, nUBound1;
  SafeArrayGetLBound(pSA, 1, &nLBound1);
  SafeArrayGetUBound(pSA, 1, &nUBound1);
  if ((nLBound1 != 0) && (nUBound1 != (N - 1)))
    AfxThrowOleException(E_INVALIDARG);
  if (nDims == 2) {
    LONG nLBound2, nUBound2;
    SafeArrayGetLBound(pSA, 2, &nLBound2);
    SafeArrayGetUBound(pSA, 2, &nUBound2);
    if ((nLBound2 != 0) && (nUBound2 != (M - 1)))
      AfxThrowOleException(E_INVALIDARG);
  }
  if (pData == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Get safearray type
  VARTYPE vt = VT_EMPTY;
  SafeArrayGetVartype(pSA, &vt);

  // Convert
  LONG ix[2];
  VARIANT varAny;
  VariantInit(&varAny);
  VARIANT varR8;
  VariantInit(&varR8);
  if (M == 0) M = 1;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++) {
      // Set array (i,j) index
      ix[0] = i;
      ix[1] = j;
      // Get element data to varAny
      if (vt == VT_VARIANT)
        SafeArrayGetElement(pSA, ix, &varAny);
      else {
        varAny.vt = vt;
        SafeArrayGetElement(pSA, ix, &varAny.bVal);
      }
      // Convert varAny to R8
      VariantChangeType(&varR8, &varAny, 0, VT_R8);
      // Set data.
      // Note: Uses "double arr[N][M]" layout
      pData[j + i*M] = varR8.dblVal;
    }
}

// Gets data from a safearray that is passed to the COM-API.
// - ArrayNxM is a NxM array.
// - N and M are the upper bounds for dimension 1 and 2.
//   I.e. [0 to N-1, 0 to M-1].
//   If M is 0 then we have a 1D array.
// - pData is a pointer to a buffer with NxM double values.
// Note:
// - This is designed to work well in both VBScript and .NET.
// - Use "SetRotationMatrix(ar)" with parentheses in VBScript.
//   This means that the parameter is passed ByVal.
// - Variant types:
//   VBScript: Dim arr(2,2) -> VT_ARRAY|VT_BYREF|VT_VARIANT
//   VBScript: Dim av=arr   -> VT_ARRAY|VT_VARIANT
//   VB.NET:   Dim arr(2,2) -> VT_ARRAY|VT_VARIANT
//   VB.NET:   Dim arr(2,2) As Double -> VT_ARRAY|VT_R8
void CaApplication::GetFromSafeArray(const VARIANT& ArrayNxM, int N, int M, long* pData)
{
  // Get safearray
  SAFEARRAY* pSA = NULL;
  if ((ArrayNxM.vt & VT_ARRAY) && (ArrayNxM.vt & VT_BYREF))
    pSA = *ArrayNxM.pparray;
  else if (ArrayNxM.vt & VT_ARRAY)
    pSA = ArrayNxM.parray;
  if (pSA == NULL)
    AfxThrowOleException(E_INVALIDARG);
  UINT nDims = (M == 0) ? (1) : (2);
  if (SafeArrayGetDim(pSA) != nDims)
    AfxThrowOleException(E_INVALIDARG);
  LONG nLBound1, nUBound1;
  SafeArrayGetLBound(pSA, 1, &nLBound1);
  SafeArrayGetUBound(pSA, 1, &nUBound1);
  if ((nLBound1 != 0) && (nUBound1 != (N - 1)))
    AfxThrowOleException(E_INVALIDARG);
  if (nDims == 2) {
    LONG nLBound2, nUBound2;
    SafeArrayGetLBound(pSA, 2, &nLBound2);
    SafeArrayGetUBound(pSA, 2, &nUBound2);
    if ((nLBound2 != 0) && (nUBound2 != (M - 1)))
      AfxThrowOleException(E_INVALIDARG);
  }
  if (pData == NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Get safearray type
  VARTYPE vt = VT_EMPTY;
  SafeArrayGetVartype(pSA, &vt);

  // Convert
  LONG ix[2];
  VARIANT varAny;
  VariantInit(&varAny);
  VARIANT varI4;
  VariantInit(&varI4);
  if (M == 0) M = 1;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++) {
      // Set array (i,j) index
      ix[0] = i;
      ix[1] = j;
      // Get element data to varAny
      if (vt == VT_VARIANT)
        SafeArrayGetElement(pSA, ix, &varAny);
      else {
        varAny.vt = vt;
        SafeArrayGetElement(pSA, ix, &varAny.bVal);
      }
      // Convert varAny to I4
      VariantChangeType(&varI4, &varAny, 0, VT_I4);
      // Set data.
      // Note: Uses "long arr[N][M]" layout
      pData[j + i*M] = varI4.lVal;
    }
}

/*
// Gets a list of triads from the safearray.
// - ArrayN is a N array.
// - pnNumTriads specifies the number of triad pointers in the ppTriads list.
//   If ppTriads is NULL, pnNumTriads returns the number of items in the safearray.
// - ppTriads points to a list of triad pointers that receives the triad pointers.
// - Variant types:
//   VBScript: Dim arr(2) -> VT_ARRAY|VT_BYREF|VT_VARIANT (not verified!)
//   VBScript: Dim av=arr -> VT_ARRAY|VT_VARIANT (not verified!)
//   VB.NET:   Dim arr(2) -> VT_ARRAY|VT_VARIANT
//   VB.NET:   Dim arr(2) As FEDEM.ITriad -> VT_ARRAY|VT_DISPATCH
void CaApplication::GetFromSafeArray(const VARIANT& ArrayN, ULONG* pnNumTriads, FmTriad** ppTriads)
{
  // Get array size
  if (pnNumTriads == NULL)
    AfxThrowOleException(E_INVALIDARG);
  if (ppTriads == NULL) {
    int nDims = 0;
    int nN = 0;
    int nM = 0;
    GetFromSafeArrayDims(ArrayN, &nDims, &nN, &nM);
    if (nDims != 1)
      AfxThrowOleException(E_INVALIDARG);
    if (nN < 1)
      AfxThrowOleException(E_INVALIDARG);
    *pnNumTriads = nN;
    return;
  }

  // Get safearray
  SAFEARRAY* pSA = NULL;
  if ((ArrayN.vt & VT_ARRAY) && (ArrayN.vt & VT_BYREF))
    pSA = *ArrayN.pparray;
  else if (ArrayN.vt & VT_ARRAY)
    pSA = ArrayN.parray;
  if (pSA == NULL)
    AfxThrowOleException(E_INVALIDARG);
  if (SafeArrayGetDim(pSA) != 1)
    AfxThrowOleException(E_INVALIDARG);
  LONG nLBound1, nUBound1;
  SafeArrayGetLBound(pSA, 1, &nLBound1);
  SafeArrayGetUBound(pSA, 1, &nUBound1);
  if ((nLBound1 != 0) && (nUBound1 != (*pnNumTriads - 1)))
    AfxThrowOleException(E_INVALIDARG);

  // Get safearray type
  VARTYPE vt = VT_EMPTY;
  SafeArrayGetVartype(pSA, &vt);

  // Convert
  VARIANT varAny;
  for (long i = 0; i < *pnNumTriads; i++) {
    // Get element data to varAny
    VariantInit(&varAny);
    if (vt == VT_VARIANT) {
      SafeArrayGetElement(pSA, &i, &varAny);
    } else if (vt == VT_DISPATCH) {
      varAny.vt = vt;
      SafeArrayGetElement(pSA, &i, &varAny.pdispVal);
    } else
      AfxThrowOleException(E_INVALIDARG);
    // TODO!
  }
}
*/

void CaApplication::GetFromSafeArrayDims(const VARIANT& ArrayNxM, int* pDims, int* pN, int* pM)
{
  // Get safearray
  SAFEARRAY* pSA = NULL;
  if ((ArrayNxM.vt & VT_ARRAY) && (ArrayNxM.vt & VT_BYREF))
    pSA = *ArrayNxM.pparray;
  else if (ArrayNxM.vt & VT_ARRAY)
    pSA = ArrayNxM.parray;
  if (pSA == NULL)
    AfxThrowOleException(E_INVALIDARG);
  if ((pDims == NULL) || (pN == NULL) || (pM == NULL))
    AfxThrowOleException(E_INVALIDARG);
  *pDims = SafeArrayGetDim(pSA);
  if (*pDims >= 1) {
    LONG nUBound1;
    SafeArrayGetUBound(pSA, 1, &nUBound1);
    *pN = nUBound1 + 1;
  }
  if (*pDims >= 2) {
    LONG nUBound2;
    SafeArrayGetUBound(pSA, 2, &nUBound2);
    *pM = nUBound2 + 1;
  }
}

// Creates a COM object wrapper for the specified FM-object.
// This function can only create a wrapper for a limited set of
// FM-objects (i.e. those that are supported by the COM-API).
// pFmObject This is the FM-object.
// Returns the IDispatch-interface to the COM object wrapper.
// Note: Functions are currently not handled!
IDispatch* CaApplication::CreateCOMObjectWrapper(FmModelMemberBase* item)
{
  // Triad
  if (item->isOfType(FmTriad::getClassTypeID())) {
    CaTriad* pCaTriad = (CaTriad*)CaTriad::CreateObject();
    if (pCaTriad == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaTriad->m_pTriad = dynamic_cast<FmTriad*>(item);
    return pCaTriad->GetIDispatch(false);
  }
  // Joint
  else if (item->isOfType(FmFreeJoint::getClassTypeID())) {
    CaJoint* pCaJoint = (CaJoint*)CaJoint::CreateObject();
    if (pCaJoint == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaJoint->m_pJoint = dynamic_cast<FmFreeJoint*>(item);
    return pCaJoint->GetIDispatch(false);
  }
  // CamJoint
  else if (item->isOfType(FmCamJoint::getClassTypeID())) {
    CaCamJoint* pCaCamJoint = (CaCamJoint*)CaCamJoint::CreateObject();
    if (pCaCamJoint == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaCamJoint->m_pCamJoint = dynamic_cast<FmCamJoint*>(item);
    return pCaCamJoint->GetIDispatch(false);
  }
  // CylindricJoint
  else if (item->isOfType(FmCylJoint::getClassTypeID())) {
    CaCylindricJoint* pCaCylindricJoint = (CaCylindricJoint*)CaCylindricJoint::CreateObject();
    if (pCaCylindricJoint == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaCylindricJoint->m_pCylJoint = dynamic_cast<FmCylJoint*>(item);
    return pCaCylindricJoint->GetIDispatch(false);
  }
  // AxialSpring
  else if (item->isOfType(FmAxialSpring::getClassTypeID())) {
    CaAxialSpring* pCaAxialSpring = (CaAxialSpring*)CaAxialSpring::CreateObject();
    if (pCaAxialSpring == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaAxialSpring->m_pAxialSpring = dynamic_cast<FmAxialSpring*>(item);
    return pCaAxialSpring->GetIDispatch(false);
  }
  // AxialDamper
  else if (item->isOfType(FmAxialDamper::getClassTypeID())) {
    CaAxialDamper* pCaAxialDamper = (CaAxialDamper*)CaAxialDamper::CreateObject();
    if (pCaAxialDamper == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaAxialDamper->m_pAxialDamper = dynamic_cast<FmAxialDamper*>(item);
    return pCaAxialDamper->GetIDispatch(false);
  }
  // Beam
  else if (item->isOfType(FmBeam::getClassTypeID())) {
    CaBeam* pCaBeam = (CaBeam*)CaBeam::CreateObject();
    if (pCaBeam == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaBeam->m_pBeam = dynamic_cast<FmBeam*>(item);
    return pCaBeam->GetIDispatch(false);
  }
  // CrossSection
  else if (item->isOfType(FmBeamProperty::getClassTypeID())) {
    CaCrossSection* pCaCrossSection = (CaCrossSection*)CaCrossSection::CreateObject();
    if (pCaCrossSection == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaCrossSection->m_pCrossSection = dynamic_cast<FmBeamProperty*>(item);
    return pCaCrossSection->GetIDispatch(false);
  }
  // Material
  else if (item->isOfType(FmMaterialProperty::getClassTypeID())) {
    CaMaterial* pCaMaterial = (CaMaterial*)CaMaterial::CreateObject();
    if (pCaMaterial == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaMaterial->m_ptr = dynamic_cast<FmMaterialProperty*>(item);
    return pCaMaterial->GetIDispatch(false);
  }
  // Part
  else if (item->isOfType(FmPart::getClassTypeID())) {
    CaPart* pCaPart = (CaPart*)CaPart::CreateObject();
    if (pCaPart == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaPart->m_pGenPart = dynamic_cast<FmPart*>(item);
    return pCaPart->GetIDispatch(false);
  }
  // SubAssembly
  else if (item->isOfType(FmAssemblyBase::getClassTypeID())) {
    CaSubAssembly* pCaSubAssembly = (CaSubAssembly*)CaSubAssembly::CreateObject();
    if (pCaSubAssembly == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaSubAssembly->m_pSubAssembly = dynamic_cast<FmAssemblyBase*>(item);
    return pCaSubAssembly->GetIDispatch(false);
  }
  // GenericObject
  else if (item->isOfType(FmGenericDBObject::getClassTypeID())) {
    CaGenericObject* pCaGenericObject = (CaGenericObject*)CaGenericObject::CreateObject();
    if (pCaGenericObject == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaGenericObject->m_pGenericObject = dynamic_cast<FmGenericDBObject*>(item);
    return pCaGenericObject->GetIDispatch(false);
  }
  // Friction
  else if (item->isOfType(FmFrictionBase::getClassTypeID())) {
    CaFriction* pCaFriction = (CaFriction*)CaFriction::CreateObject();
    if (pCaFriction == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaFriction->m_pFriction = dynamic_cast<FmFrictionBase*>(item);
    return pCaFriction->GetIDispatch(false);
  }
  // Function
  else if (item->isOfType(FmEngine::getClassTypeID())) {
    CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
    if (pCaFunction == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaFunction->m_ptr = dynamic_cast<FmEngine*>(item);
    return pCaFunction->GetIDispatch(false);
  }
  // Function definition
  else if (item->isOfType(FmMathFuncBase::getClassTypeID())) {
    CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
    if (pCaFunction == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaFunction->m_ptr = dynamic_cast<FmMathFuncBase*>(item);
    return pCaFunction->GetIDispatch(false);
  }
  // Adv. spring characteristics
  else if (item->isOfType(FmSpringChar::getClassTypeID())) {
    CaFunction* pCaFunction = (CaFunction*)CaFunction::CreateObject();
    if (pCaFunction == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaFunction->m_ptr = dynamic_cast<FmSpringChar*>(item);
    return pCaFunction->GetIDispatch(false);
  }
  // Strain rosette
  else if (item->isOfType(FmStrainRosette::getClassTypeID())) {
    CaStrainRosette* pCaRosette = (CaStrainRosette*)CaStrainRosette::CreateObject();
    if (pCaRosette == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaRosette->m_pStrainRosette = dynamic_cast<FmStrainRosette*>(item);
    return pCaRosette->GetIDispatch(false);
  }
  // Graph
  else if (item->isOfType(FmGraph::getClassTypeID())) {
    CaGraph* pCaGraph = (CaGraph*)CaGraph::CreateObject();
    if (pCaGraph == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaGraph->m_ptr = dynamic_cast<FmGraph*>(item);
    return pCaGraph->GetIDispatch(false);
  }
  // Curve
  else if (item->isOfType(FmCurveSet::getClassTypeID())) {
    CaCurve* pCaCurve = (CaCurve*)CaCurve::CreateObject();
    if (pCaCurve == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaCurve->m_ptr = dynamic_cast<FmCurveSet*>(item);
    return pCaCurve->GetIDispatch(false);
  }
  // VesselMotion
  else if (item->isOfType(FmVesselMotion::getClassTypeID())) {
    CaVesselMotion* pCaVesselMotion = (CaVesselMotion*)CaVesselMotion::CreateObject();
    if (pCaVesselMotion == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaVesselMotion->m_ptr = dynamic_cast<FmVesselMotion*>(item);
    return pCaVesselMotion->GetIDispatch(false);
  }
  // UserDefinedElement
  else if (item->isOfType(FmUserDefinedElement::getClassTypeID())) {
    CaUserDefinedElement* pCaUserDefinedElement = (CaUserDefinedElement*)CaUserDefinedElement::CreateObject();
    if (pCaUserDefinedElement == NULL)
      AfxThrowOleException(E_OUTOFMEMORY);
    pCaUserDefinedElement->m_ptr = dynamic_cast<FmUserDefinedElement*>(item);
    return pCaUserDefinedElement->GetIDispatch(false);
  }

  // All other types return as a generic FmObject
  CaFmObject* pCaFmObject = (CaFmObject*)CaFmObject::CreateObject();
  if (pCaFmObject == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  pCaFmObject->m_ptr = item;
  return pCaFmObject->GetIDispatch(false);
}

// Gets the Fm-object that is referenced by the COM object wrapper.
// pDisp is the IDispach-interface pointer to the COM object.
// Returns the FM-object.
FmModelMemberBase* CaApplication::GetFromCOMObjectWrapper(IDispatch* pDisp)
{
  // Triad
  CaTriad* pCaTriad = dynamic_cast<CaTriad*>(CaTriad::FromIDispatch(pDisp));
  if (pCaTriad != NULL)
    return pCaTriad->m_pTriad;
  // Joint
  CaJoint* pCaJoint = dynamic_cast<CaJoint*>(CaJoint::FromIDispatch(pDisp));
  if (pCaJoint != NULL)
    return pCaJoint->m_pJoint;
  // CamJoint
  CaCamJoint* pCaCamJoint = dynamic_cast<CaCamJoint*>(CaCamJoint::FromIDispatch(pDisp));
  if (pCaCamJoint != NULL)
    return pCaCamJoint->m_pCamJoint;
  // CylindricJoint
  CaCylindricJoint* pCaCylindricJoint = dynamic_cast<CaCylindricJoint*>(CaCylindricJoint::FromIDispatch(pDisp));
  if (pCaCylindricJoint != NULL)
    return pCaCylindricJoint->m_pCylJoint;
  // AxialSpring
  CaAxialSpring* pCaAxialSpring = dynamic_cast<CaAxialSpring*>(CaAxialSpring::FromIDispatch(pDisp));
  if (pCaAxialSpring != NULL)
    return pCaAxialSpring->m_pAxialSpring;
  // AxialDamper
  CaAxialDamper* pCaAxialDamper = dynamic_cast<CaAxialDamper*>(CaAxialDamper::FromIDispatch(pDisp));
  if (pCaAxialDamper != NULL)
    return pCaAxialDamper->m_pAxialDamper;
  // Beam
  CaBeam* pCaBeam = dynamic_cast<CaBeam*>(CaBeam::FromIDispatch(pDisp));
  if (pCaBeam != NULL)
    return pCaBeam->m_pBeam;
  // CrossSection
  CaCrossSection* pCaCrossSection = dynamic_cast<CaCrossSection*>(CaCrossSection::FromIDispatch(pDisp));
  if (pCaCrossSection != NULL)
    return pCaCrossSection->m_pCrossSection;
  // Material
  CaMaterial* pCaMaterial = dynamic_cast<CaMaterial*>(CaMaterial::FromIDispatch(pDisp));
  if (pCaMaterial != NULL)
    return pCaMaterial->m_ptr;
  // Part
  CaPart* pCaPart = dynamic_cast<CaPart*>(CaPart::FromIDispatch(pDisp));
  if (pCaPart != NULL)
    return pCaPart->m_pGenPart;
  // SubAssembly
  CaSubAssembly* pCaSubAssembly = dynamic_cast<CaSubAssembly*>(CaSubAssembly::FromIDispatch(pDisp));
  if (pCaSubAssembly != NULL)
    return pCaSubAssembly->m_pSubAssembly;
  // GenericObject
  CaGenericObject* pCaGenericObject = dynamic_cast<CaGenericObject*>(CaGenericObject::FromIDispatch(pDisp));
  if (pCaGenericObject != NULL)
    return pCaGenericObject->m_pGenericObject;
  // Friction
  CaFriction* pCaFriction = dynamic_cast<CaFriction*>(CaFriction::FromIDispatch(pDisp));
  if (pCaFriction != NULL)
    return pCaFriction->m_pFriction;
  // Function
  CaFunction* pCaFunction = dynamic_cast<CaFunction*>(CaFunction::FromIDispatch(pDisp));
  if (pCaFunction != NULL)
    return pCaFunction->m_ptr;
  // Strain rosette
  CaStrainRosette* pCaRosette = dynamic_cast<CaStrainRosette*>(CaStrainRosette::FromIDispatch(pDisp));
  if (pCaRosette != NULL)
    return pCaRosette->m_pStrainRosette;
  // Graph
  CaGraph* pCaGraph = dynamic_cast<CaGraph*>(CaGraph::FromIDispatch(pDisp));
  if (pCaGraph != NULL)
    return pCaGraph->m_ptr;
  // Curve
  CaCurve* pCaCurve = dynamic_cast<CaCurve*>(CaCurve::FromIDispatch(pDisp));
  if (pCaCurve != NULL)
    return pCaCurve->m_ptr;
  // VesselMotion
  CaVesselMotion* pCaVesselMotion = dynamic_cast<CaVesselMotion*>(CaVesselMotion::FromIDispatch(pDisp));
  if (pCaVesselMotion != NULL)
    return pCaVesselMotion->m_ptr;
  // UserDefinedElement
  CaUserDefinedElement* pCaUserDefinedElement = dynamic_cast<CaUserDefinedElement*>(CaUserDefinedElement::FromIDispatch(pDisp));
  if (pCaUserDefinedElement != NULL)
    return pCaUserDefinedElement->m_ptr;
  // FmObject
  CaFmObject* pCaFmObject = dynamic_cast<CaFmObject*>(CaFmObject::FromIDispatch(pDisp));
  if (pCaFmObject != NULL)
    return pCaFmObject->m_ptr;
  // Unexpected
  return NULL;
}

// Gets the contents of a function parameter.
// FunctionParam can be a double, a function name or an IFunction.
// pVal returns the double value.
// ppFunction returns the Fedem function object.
void CaApplication::GetFunctionParameter(const VARIANT FAR& FunctionParam,
                                         double* pVal, FmModelMemberBase** ppFunction)
{
  // Initialize
  *pVal = 0.0;
  *ppFunction = NULL;

  // String with a function name
  if (FunctionParam.vt == VT_BSTR) {
    std::string strFunctionParam(CW2A(FunctionParam.bstrVal));
    if (strFunctionParam.empty()) return;

    std::vector<FmEngine*> engines;
    FmDB::getAllEngines(engines);
    for (FmEngine* engine : engines)
      if (engine->getUserDescription().compare(strFunctionParam) == 0) {
        *ppFunction = engine;
        return;
      }

    std::vector<FmMathFuncBase*> functions;
    FmDB::getAllFunctions(functions);
    for (FmMathFuncBase* func : functions)
      if (func->getUserDescription().compare(strFunctionParam) == 0) {
        *ppFunction = func;
        return;
      }

    std::vector<FmSpringChar*> springChars;
    FmDB::getAllSpringChars(springChars);
    for (FmSpringChar* sprch : springChars)
      if (sprch->getUserDescription().compare(strFunctionParam) == 0) {
        *ppFunction = sprch;
        return;
      }
  }

  // IDispatch or IUnknown pointer
  else if (FunctionParam.vt == VT_DISPATCH || FunctionParam.vt == VT_UNKNOWN) {
    IUnknown* pUnk = FunctionParam.vt == VT_DISPATCH ? FunctionParam.pdispVal : FunctionParam.punkVal;
    IDispatch* pDisp1 = NULL;
    pUnk->QueryInterface(IID_IDispatch, (void**)&pDisp1);
    if (pDisp1 == NULL)
      AfxThrowOleException(CO_E_ERRORINAPP);
    CaFunction* pCaFunction = dynamic_cast<CaFunction*>(CaFunction::FromIDispatch(pDisp1));
    pDisp1->Release();
    if (pCaFunction == NULL)
      AfxThrowOleException(CO_E_ERRORINAPP);
    *ppFunction = pCaFunction->m_ptr;
    if (*ppFunction == NULL)
      AfxThrowOleException(CO_E_ERRORINAPP);
  }

  // Double value
  else {
    VARIANT varR8;
    VariantInit(&varR8);
    VariantChangeType(&varR8, &FunctionParam, 0, VT_R8);
    *pVal = varR8.vt == VT_R8 ? varR8.dblVal : 0.0;
  }
}
