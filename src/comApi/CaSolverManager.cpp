// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaSolverManager.h"
#include "CaApplication.h"
#include "CaSimulationEvent.h"
#include "CaStrConv.h"

#include "vpmApp/vpmAppCmds/FapSolveCmds.H"
#include "vpmDB/FmDB.H"
#include "vpmDB/FmAnalysis.H"
#include "vpmDB/FmResultStatusData.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmSimulationEvent.H"
#include "vpmDB/FmSolverParser.H"

//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaSolverManager, CCmdTarget)

BEGIN_MESSAGE_MAP(CaSolverManager, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaSolverManager, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaSolverManager, "StartTime", dispidStartTime, get_StartTime, put_StartTime, VT_R8)
  DISP_PROPERTY_EX_ID(CaSolverManager, "StopTime", dispidStopTime, get_StopTime, put_StopTime, VT_R8)
  DISP_PROPERTY_EX_ID(CaSolverManager, "TimeIncrement", dispidTimeIncrement, get_TimeIncrement, put_TimeIncrement, VT_R8)
  DISP_PROPERTY_EX_ID(CaSolverManager, "QuasiStatic", dispidQuasiStatic, get_QuasiStatic, put_QuasiStatic, VT_BOOL)
  DISP_PROPERTY_EX_ID(CaSolverManager, "InitEquil", dispidInitEquil, get_InitEquil, put_InitEquil, VT_BOOL)
  DISP_FUNCTION_ID(CaSolverManager, "GetFRSFileNames", dispidGetFRSFileNames, GetFRSFileNames, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CaSolverManager, "SolveDynamics", dispidSolveDynamics, SolveDynamics, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSolverManager, "SolveAll", dispidSolveAll, SolveAll, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSolverManager, "SolveEvents", dispidSolveEvents, SolveEvents, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSolverManager, "EraseSimulationResults", dispidEraseSimulationResults, EraseSimulationResults, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSolverManager, "EraseEventResults", dispidEraseEventResults, EraseEventResults, VT_EMPTY, VTS_NONE)
  DISP_FUNCTION_ID(CaSolverManager, "WriteSolverInputFile", dispidWriteSolverInputFile, WriteSolverInputFile, VT_BOOL, VTS_BSTR)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaSolverManager, CCmdTarget)
	INTERFACE_PART(CaSolverManager, IID_ISolverManager, LocalClass)
END_INTERFACE_MAP()

// {A5874AC8-ED53-405A-ABCC-CC8869B44F34}
IMPLEMENT_OLECREATE(CaSolverManager, "FEDEM.SolverManager",
    0xa5874ac8, 0xed53, 0x405a, 0xab, 0xcc, 0xcc, 0x88, 0x69, 0xb4, 0x4f, 0x34);


CaSolverManager::CaSolverManager(void)
{
  EnableAutomation();
  ::AfxOleLockApp();
}

CaSolverManager::~CaSolverManager(void)
{
  ::AfxOleUnlockApp();
}


//////////////////////////////////////////////////////////////////////
// Methods

double CaSolverManager::get_StartTime()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis(false);
  return analysis ? analysis->startTime.getValue() : 0.0;
}

void CaSolverManager::put_StartTime(double val)
{
  FmDB::getActiveAnalysis()->setStartTime(val);
}

double CaSolverManager::get_StopTime()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis(false);
  return analysis ? analysis->stopTime.getValue() : 0.0;
}

void CaSolverManager::put_StopTime(double val)
{
  FmDB::getActiveAnalysis()->setEndTime(val);
}

double CaSolverManager::get_TimeIncrement()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis(false);
  return analysis ? analysis->timeIncr.getValue() : 0.0;
}

void CaSolverManager::put_TimeIncrement(double val)
{
  FmDB::getActiveAnalysis()->setTimeIncrement(val);
}

BOOL CaSolverManager::get_QuasiStatic()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis(false);
  return analysis ? analysis->quasistaticEnable.getValue() : false;
}

void CaSolverManager::put_QuasiStatic(BOOL val)
{
  FmDB::getActiveAnalysis()->quasistaticEnable.setValue(val);
}

BOOL CaSolverManager::get_InitEquil()
{
  FmAnalysis* analysis = FmDB::getActiveAnalysis(false);
  return analysis ? analysis->solveInitEquil.getValue() : false;
}

void CaSolverManager::put_InitEquil(BOOL val)
{
  FmDB::getActiveAnalysis()->solveInitEquil.setValue(val);
}

void CaSolverManager::SolveDynamics()
{
  FapSolveCmds::solveDynamics();
}

void CaSolverManager::SolveAll()
{
  FapSolveCmds::solveAll(false);
}

void CaSolverManager::SolveEvents()
{
  FapSolveCmds::solveEvents();
}

BSTR CaSolverManager::GetFRSFileNames()
{
  FmMechanism* mech = FmDB::getMechanismObject(false);
  FmResultStatusData* rsd = mech ? mech->getResultStatusData() : NULL;
  return SysAllocString(CA2W(rsd ? rsd->getFileNames("frs").c_str() : ""));
}

void CaSolverManager::EraseSimulationResults()
{
  FapSolveCmds::eraseSimuleRes();
}

void CaSolverManager::EraseEventResults()
{
  FapSolveCmds::eraseEventRes();
}

BOOL CaSolverManager::WriteSolverInputFile(LPCTSTR FileName)
{
  std::string fileName = CaConvert(FileName);
  FmSolverParser solverParser(fileName.c_str());
  return solverParser.writeFullFile();
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaSolverManager::XLocalClass::AddRef()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
		return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaSolverManager::XLocalClass::Release()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
		return pThis->ExternalRelease();
}
STDMETHODIMP CaSolverManager::XLocalClass::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
		return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaSolverManager::XLocalClass::GetTypeInfoCount(
	UINT FAR* pctinfo)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
		LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
	ASSERT(lpDispatch != NULL);
	return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaSolverManager::XLocalClass::GetTypeInfo(
	UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
		LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
	ASSERT(lpDispatch != NULL);
	return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaSolverManager::XLocalClass::GetIDsOfNames(
	REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
	LCID lcid, DISPID FAR* rgdispid)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
		LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
	ASSERT(lpDispatch != NULL);
	return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames,
		lcid, rgdispid);
}
STDMETHODIMP CaSolverManager::XLocalClass::Invoke(
	DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
	EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass)
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

STDMETHODIMP CaSolverManager::XLocalClass::get_StartTime(double* pVal)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        *pVal = pThis->get_StartTime();
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::put_StartTime(double val)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        pThis->put_StartTime(val);
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::get_StopTime(double* pVal)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        *pVal = pThis->get_StopTime();
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::put_StopTime(double val)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        pThis->put_StopTime(val);
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::get_TimeIncrement(double* pVal)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        *pVal = pThis->get_TimeIncrement();
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::put_TimeIncrement(double val)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        pThis->put_TimeIncrement(val);
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::get_QuasiStatic(VARIANT_BOOL* pVal)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        *pVal = pThis->get_QuasiStatic();
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::put_QuasiStatic(VARIANT_BOOL val)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        pThis->put_QuasiStatic(val);
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::get_InitEquil(VARIANT_BOOL* pVal)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        *pVal = pThis->get_InitEquil();
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::put_InitEquil(VARIANT_BOOL val)
{
    METHOD_PROLOGUE(CaSolverManager, LocalClass);
    TRY
    {
        pThis->put_InitEquil(val);
    }
    CATCH_ALL(e)
    {
        return ResultFromScode(COleException::Process(e));
    }
    END_CATCH_ALL
        return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::GetFRSFileNames(BSTR* pVal)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		*pVal = pThis->GetFRSFileNames();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::SolveDynamics()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		pThis->SolveDynamics();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::SolveAll()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		pThis->SolveAll();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::SolveEvents()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		pThis->SolveEvents();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::EraseSimulationResults()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		pThis->EraseSimulationResults();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::EraseEventResults()
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		pThis->EraseEventResults();
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}

STDMETHODIMP CaSolverManager::XLocalClass::WriteSolverInputFile(BSTR FileName, VARIANT_BOOL* pVal)
{
	METHOD_PROLOGUE(CaSolverManager, LocalClass);
	TRY
	{
		*pVal = pThis->WriteSolverInputFile(FileName);
	}
	CATCH_ALL(e)
	{
		return ResultFromScode(COleException::Process(e));
	}
	END_CATCH_ALL
		return S_OK;
}
