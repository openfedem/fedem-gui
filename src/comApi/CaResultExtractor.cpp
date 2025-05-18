// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "CaResultExtractor.h"
#include "CaApplication.h"
#include "CaMacros.h"
#include "CaStrConv.h"

#include "vpmDB/FmDB.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmCurveSet.H"

#include "vpmApp/vpmAppDisplay/FapReadCurveData.H"
#include "FFpLib/FFpCurveData/FFpReadResults.H"
#include "FFpLib/FFpFatigue/FFpSNCurveLib.H"
#include "FFpLib/FFpFatigue/FFpFatigue.H"
#include "FFrLib/FFrExtractor.H"


//////////////////////////////////////////////////////////////////////
// Constructors and set-up

IMPLEMENT_DYNCREATE(CaResultExtractor, CCmdTarget)

BEGIN_MESSAGE_MAP(CaResultExtractor, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CaResultExtractor, CCmdTarget)
  DISP_PROPERTY_EX_ID(CaResultExtractor, "FileNames", dispidFileNames, get_FileNames, put_FileNames, VT_BSTR)
  DISP_PROPERTY_EX_ID(CaResultExtractor, "ReadTime", dispidReadTime, get_ReadTime, put_ReadTime, VT_BOOL)
  DISP_FUNCTION_ID(CaResultExtractor, "Read", dispidRead, Read, VT_VARIANT, VTS_BSTR VTS_BSTR VTS_BSTR VTS_PR8 VTS_PR8)
  DISP_FUNCTION_ID(CaResultExtractor, "ReadCurve", dispidReadCurve, ReadCurve, VT_VARIANT, VTS_BSTR VTS_PR8 VTS_PR8 VTS_UNKNOWN)
  DISP_FUNCTION_ID(CaResultExtractor, "CalcRainflow", dispidCalcRainflow, CalcRainflow, VT_VARIANT, VTS_VARIANT VTS_R8)
  DISP_FUNCTION_ID(CaResultExtractor, "OpenSNCurveLib", dispidOpenSNCurveLib, OpenSNCurveLib, VT_BOOL, VTS_BSTR)
  DISP_FUNCTION_ID(CaResultExtractor, "CalcDamage", dispidCalcDamage, CalcDamage, VT_R8, VTS_VARIANT VTS_I4 VTS_I4)
  DISP_FUNCTION_ID(CaResultExtractor, "GetSNStdCount", dispidGetSNStdCount, GetSNStdCount, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CaResultExtractor, "GetSNStdName", dispidGetSNStdName, GetSNStdName, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CaResultExtractor, "GetSNCurveCount", dispidGetSNCurveCount, GetSNCurveCount, VT_I4, VTS_I4)
  DISP_FUNCTION_ID(CaResultExtractor, "GetSNCurveName", dispidGetSNCurveName, GetSNCurveName, VT_BSTR, VTS_I4 VTS_I4)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CaResultExtractor, CCmdTarget)
  INTERFACE_PART(CaResultExtractor, IID_IResultExtractor, LocalClass)
END_INTERFACE_MAP()

// {FF66E75D-BCA2-4ec7-A41F-CFDA50B5F87E}
IMPLEMENT_OLECREATE(CaResultExtractor, "FEDEM.ResultExtractor",
0xff66e75d, 0xbca2, 0x4ec7, 0xa4, 0x1f, 0xcf, 0xda, 0x50, 0xb5, 0xf8, 0x7e);


CaResultExtractor::CaResultExtractor(void)
{
  EnableAutomation();
  ::AfxOleLockApp();

  m_pExtractor = NULL;
  m_readTime = false;
}

CaResultExtractor::~CaResultExtractor(void)
{
  ::AfxOleUnlockApp();

  if (m_pExtractor != NULL)
    delete m_pExtractor;
}


//////////////////////////////////////////////////////////////////////
// Methods

BSTR CaResultExtractor::get_FileNames()
{
  CA_CHECK(m_pExtractor);

  std::set<std::string> fileNames = m_pExtractor->getAllResultContainerFiles();
  if (fileNames.empty())
    return SysAllocString(CA2W(""));

  std::set<std::string>::const_iterator sit = fileNames.begin();
  std::string strFileList(*sit);
  for (++sit; sit != fileNames.end(); ++sit)
    strFileList.append(";"+(*sit));

  return SysAllocString(CA2W(strFileList.c_str()));
}

void CaResultExtractor::put_FileNames(LPCTSTR val)
{
  if (val == NULL)
    AfxThrowOleException(E_INVALIDARG);
  CreateExtractor(CW2A(val));
}

bool CaResultExtractor::CreateExtractor(LPCSTR szFileNames)
{
  if (m_pExtractor != NULL)
    delete m_pExtractor;

  // Create new extractor
  bool success = true;
  m_pExtractor = new FFrExtractor();
  char* fcopy = strdup(szFileNames);
  char* pstr = strtok(fcopy,";");
  for (; pstr; pstr = strtok(NULL,";"))
    success &= m_pExtractor->addFile(pstr,true);
  free(fcopy);

  return success;
}

BOOL CaResultExtractor::get_ReadTime()
{
  return m_readTime;
}

void CaResultExtractor::put_ReadTime(BOOL val)
{
  m_readTime = (val != 0);
}

VARIANT CaResultExtractor::Read(LPCTSTR ObjectType, LPCTSTR BaseIDs, LPCTSTR Variables,
                                double* StartTime, double* EndTime)
{
  CA_CHECK(m_pExtractor);

  if (Variables == NULL || StartTime == NULL || EndTime == NULL)
    AfxThrowOleException(E_INVALIDARG);

  VARIANT ArrayNxM;
  ArrayNxM.vt = VT_EMPTY;

  // Get base IDs, if any
  std::vector<int> vnBaseIds;
  if (BaseIDs != NULL)
  {
    if (ObjectType == NULL)
      AfxThrowOleException(E_INVALIDARG);

    std::string bcopy = CaConvert(BaseIDs);
    char* pstr = strtok(const_cast<char*>(bcopy.c_str()), ";");
    for (; pstr; pstr = strtok(NULL,";"))
      vnBaseIds.push_back(atol(pstr));
  }
  else if (ObjectType != NULL)
    AfxThrowOleException(E_INVALIDARG);

  // Get variable description
  std::vector<std::string> vstr;
  std::string vcopy = CaConvert(Variables);
  char* pstr = strtok(const_cast<char*>(vcopy.c_str()), ";");
  for (; pstr; pstr = strtok(NULL,";"))
    vstr.push_back(pstr);

  std::vector<FFpVar> vVariables;
  vVariables.reserve(vstr.size());
  for (std::string& variable : vstr)
  {
    FFpVar var;
    pstr = strtok(const_cast<char*>(variable.c_str()),":");
    for (int count = 0; pstr; pstr = strtok(NULL,":"), count++)
      switch (count) {
      case 0: var.name = pstr; break;
      case 1: var.type = pstr; break;
      case 2: var.oper = pstr; break;
      }
    vVariables.push_back(var);
  }

  // Read from FRS
  std::string objType = CaConvert(ObjectType);
  DoubleVectors values;
  std::string errorMsg;
  bool status = FFp::readHistories(objType.c_str(), vnBaseIds, vVariables,
                                   m_pExtractor, *StartTime, *EndTime,
                                   m_readTime, values, errorMsg);
  if (!errorMsg.empty()) std::cout << errorMsg << std::endl;
  if (!status || values.empty() || values.front().empty())
    return ArrayNxM; // unexpected

  int nCX = values.front().size();
  int nCY = values.size();

  // Create safearray
  SAFEARRAYBOUND bounds[2];
  bounds[0].lLbound = 0;
  bounds[0].cElements = nCX;
  bounds[1].lLbound = 0;
  bounds[1].cElements = nCY;
  SAFEARRAY* pSA = SafeArrayCreate(VT_VARIANT, 2, bounds);
  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set data
  LONG ix[2];
  VARIANT varR8;
  VariantInit(&varR8);
  varR8.vt = VT_R8;
  for (int i = 0; i < nCX; i++) {
    for (int j = 0; j < nCY; j++) {
      // Set array (i,j) index
      ix[0] = i;
      ix[1] = j;
      // Set element data
      varR8.dblVal = values[j][i];
      SafeArrayPutElement(pSA, ix, &varR8);
    }
  }

  // Set variant
  ArrayNxM.vt = VT_ARRAY | VT_VARIANT;
  ArrayNxM.parray = pSA;
  return ArrayNxM;
}

VARIANT CaResultExtractor::CalcRainflow(VARIANT ArrayNx2, double GateValueMPa)
{
  VARIANT ArrayRx1;
  ArrayRx1.vt = VT_EMPTY;

  // Get array size
  int nDims = 0, nN = 0, nM = 0;
  CaApplication::GetFromSafeArrayDims(ArrayNx2, &nDims, &nN, &nM);
  if (nDims != 1 || nN < 1)
    AfxThrowOleException(E_INVALIDARG);

  // Get array data
  double* pData = (double*)malloc(nN * sizeof(double));
  if (pData == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  CaApplication::GetFromSafeArray(ArrayNx2, nN, nDims == 2 ? nM : 0, pData);

  // Convert to vector
  std::vector<double> data(pData,pData+nN);
  free(pData);

  double MPa = 1.0e-6; // Stress scaling factor to MPa
  FmDB::getMechanismObject()->modelDatabaseUnits.getValue().convert(MPa,"FORCE/AREA");

  // Calculate rainflow
  std::vector<double> ranges;
  FFpFatigue::calcRainFlowAndDamage(data,ranges,GateValueMPa,MPa);
  if (ranges.empty())
    return ArrayRx1; // unexpected

  // Create safearray
  SAFEARRAYBOUND bounds[1];
  bounds[0].lLbound = 0;
  bounds[0].cElements = ranges.size();
  SAFEARRAY* pSA = SafeArrayCreate(VT_VARIANT, 1, bounds);
  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set data
  LONG ix = 0;
  VARIANT varR8;
  VariantInit(&varR8);
  varR8.vt = VT_R8;
  for (double val : ranges) {
    // Set element data
    varR8.dblVal = val;
    SafeArrayPutElement(pSA, &ix, &varR8);
    ix++;
  }

  // Set variant
  ArrayRx1.vt = VT_ARRAY | VT_VARIANT;
  ArrayRx1.parray = pSA;
  return ArrayRx1;
}

BOOL CaResultExtractor::OpenSNCurveLib(LPCTSTR SNCurveFile)
{
  if (SNCurveFile == NULL)
    AfxThrowOleException(E_INVALIDARG);

  return FFpFatigue::readSNCurves(CaConvert(SNCurveFile));
}

double CaResultExtractor::CalcDamage(VARIANT ArrayRx1, long SNStd, long SNCurve)
{
  // Get array size
  int nDims = 0, nN = 0, nM = 0;
  CaApplication::GetFromSafeArrayDims(ArrayRx1, &nDims, &nN, &nM);
  if (nDims != 1)
    AfxThrowOleException(E_INVALIDARG);
  else if (nN < 1)
    return 0.0; // No cycles ==> zero damage

  // Get array data
  double* pData = (double*)malloc(nN * sizeof(double));
  if (pData == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);
  CaApplication::GetFromSafeArray(ArrayRx1, nN, 0, pData);

  // Convert to vector
  std::vector<double> ranges(pData,pData+nN);
  free(pData);

  // Calculate damage
  return FFpFatigue::getDamage(ranges, SNStd, SNCurve);
}

long CaResultExtractor::GetSNStdCount()
{
  return FFpSNCurveLib::instance()->getNoCurveStds();
}

BSTR CaResultExtractor::GetSNStdName(long SNStd)
{
  return SysAllocString(CA2W(FFpSNCurveLib::instance()->getCurveStd(SNStd).c_str()));
}

long CaResultExtractor::GetSNCurveCount(long SNStd)
{
  return FFpSNCurveLib::instance()->getNoCurves(SNStd);
}

BSTR CaResultExtractor::GetSNCurveName(long SNStd, long SNCurve)
{
  return SysAllocString(CA2W(FFpSNCurveLib::instance()->getCurveName(SNStd,SNCurve).c_str()));
}

/*!
  Note: This method is mis-placed, as it does not read data from the extractor
  object maintained by this class, but instead the internal extractor of the
  main GUI-application (they do not necessarily contain the same things).
  Consider moving it to CaApplication (kmo 27/01/16).
*/

VARIANT CaResultExtractor::ReadCurve(LPCTSTR BaseIDs, double* StartTime, double* EndTime)
{
  if (BaseIDs == NULL || StartTime == NULL || EndTime == NULL)
    AfxThrowOleException(E_INVALIDARG);

  VARIANT ArrayNx2;
  ArrayNx2.vt = VT_EMPTY;

  std::vector<FmCurveSet*> curves;
  std::string baseIDstring = CaConvert(BaseIDs);

  // Skip delimiters at beginning.
  std::string::size_type lastPos = baseIDstring.find_first_not_of(";", 0);
  // Find first "non-delimiter".
  std::string::size_type pos = baseIDstring.find_first_of(";", lastPos);

  while (pos != std::string::npos || lastPos != std::string::npos)
  {
    // Found a token, get the baseID
    int baseID = atol(baseIDstring.substr(lastPos,pos-lastPos).c_str());
    // Skip delimiters. Note the "not_of"
    lastPos = baseIDstring.find_first_not_of(";", pos);
    // Find next "non-delimiter"
    pos = baseIDstring.find_first_of(";", lastPos);

    // Get the curve object
    FmCurveSet* curve = dynamic_cast<FmCurveSet*>(FmDB::findObject(baseID));
    if (curve != NULL) curves.push_back(curve);
  }

  if (curves.empty())
    return ArrayNx2; // No curves found

  // Read curve data
  DoubleVectors values;
  int nCX = Fap::readCurveData(curves,values,*StartTime,*EndTime);
  int nCY = values.size();
  if (nCY == 0)
    return ArrayNx2; // All curves are empty

  // Create safearray
  SAFEARRAYBOUND bounds[2];
  bounds[0].lLbound = 0;
  bounds[0].cElements = nCX;
  bounds[1].lLbound = 0;
  bounds[1].cElements = nCY;
  SAFEARRAY* pSA = SafeArrayCreate(VT_VARIANT, 2, bounds);
  if (pSA == NULL)
    AfxThrowOleException(E_OUTOFMEMORY);

  // Set data
  LONG ix[2];
  VARIANT varR8;
  VariantInit(&varR8);
  varR8.vt = VT_R8;
  for (int i = 0; i < nCX; i++)
    for (int j = 0; j < nCY; j++) {
      // Set array (i,j) index
      ix[0] = i;
      ix[1] = j;
      // Set element data
      varR8.dblVal = values[j][i];
      SafeArrayPutElement(pSA, ix, &varR8);
    }

  // Set variant
  ArrayNx2.vt = VT_ARRAY | VT_VARIANT;
  ArrayNx2.parray = pSA;
  return ArrayNx2;
}


//////////////////////////////////////////////////////////////////////
// Implementation

STDMETHODIMP_(ULONG) CaResultExtractor::XLocalClass::AddRef()
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
  return pThis->ExternalAddRef();
}
STDMETHODIMP_(ULONG) CaResultExtractor::XLocalClass::Release()
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
  return pThis->ExternalRelease();
}
STDMETHODIMP CaResultExtractor::XLocalClass::QueryInterface(
  REFIID iid, LPVOID* ppvObj)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
  return pThis->ExternalQueryInterface(&iid, ppvObj);
}
STDMETHODIMP CaResultExtractor::XLocalClass::GetTypeInfoCount(
  UINT FAR* pctinfo)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CaResultExtractor::XLocalClass::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CaResultExtractor::XLocalClass::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
}
STDMETHODIMP CaResultExtractor::XLocalClass::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass)
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

STDMETHODIMP CaResultExtractor::XLocalClass::get_FileNames(BSTR* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->get_FileNames();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::put_FileNames(BSTR val)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    pThis->put_FileNames(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::get_ReadTime(VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->get_ReadTime();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::put_ReadTime(VARIANT_BOOL val)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    pThis->put_ReadTime(val);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::Read(BSTR ObjectType, BSTR BaseIDs, BSTR Variables,
  double* StartTime, double* EndTime, VARIANT* ArrayNxM)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *ArrayNxM = pThis->Read(ObjectType, BaseIDs, Variables, StartTime, EndTime);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::ReadCurve(BSTR BaseID, double* StartTime, double* EndTime, VARIANT* ArrayNx2)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *ArrayNx2 = pThis->ReadCurve(BaseID, StartTime, EndTime);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::CalcRainflow(VARIANT ArrayNx2,
  double GateValueMPa, VARIANT* ArrayRx1)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *ArrayRx1 = pThis->CalcRainflow(ArrayNx2, GateValueMPa);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::OpenSNCurveLib(BSTR SNCurveFile,
  VARIANT_BOOL* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->OpenSNCurveLib(SNCurveFile);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::CalcDamage(VARIANT ArrayRx1,
  long SNStd, long SNCurve, double* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->CalcDamage(ArrayRx1, SNStd, SNCurve);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::GetSNStdCount(long* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->GetSNStdCount();
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::GetSNStdName(long SNStd, BSTR* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->GetSNStdName(SNStd);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::GetSNCurveCount(long SNStd, long* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->GetSNCurveCount(SNStd);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}

STDMETHODIMP CaResultExtractor::XLocalClass::GetSNCurveName(long SNStd, long SNCurve, BSTR* pVal)
{
  METHOD_PROLOGUE(CaResultExtractor, LocalClass);
  TRY
  {
    *pVal = pThis->GetSNCurveName(SNStd, SNCurve);
  }
  CATCH_ALL(e)
  {
    return ResultFromScode(COleException::Process(e));
  }
  END_CATCH_ALL
  return S_OK;
}
