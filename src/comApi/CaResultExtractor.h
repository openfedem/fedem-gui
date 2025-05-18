/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
#pragma once

#include "CaCommonHeaders.h"
#include <vector>
#include <string>

class FFrExtractor;

class CaResultExtractor : public CCmdTarget
{
  typedef std::pair<std::string,std::string> StringPair;
  typedef std::vector<double>  Dvector;
  typedef std::vector<Dvector> Dvectors;

public:
  CaResultExtractor(void);
  ~CaResultExtractor(void);

  virtual void OnFinalRelease()
  {
    CCmdTarget::OnFinalRelease();
  }

  DECLARE_DYNCREATE(CaResultExtractor)
  DECLARE_MESSAGE_MAP()
  DECLARE_OLECREATE(CaResultExtractor)
  DECLARE_DISPATCH_MAP()
  DECLARE_INTERFACE_MAP()

  enum
  {
    dispidFileNames = 1L,
    dispidReadTime = 2L,
    dispidRead = 100L,
    dispidCalcRainflow = 101L,
    dispidOpenSNCurveLib = 102L,
    dispidCalcDamage = 103L,
    dispidGetSNStdCount = 200L,
    dispidGetSNStdName = 201L,
    dispidGetSNCurveCount = 202L,
    dispidGetSNCurveName = 203L,
	dispidReadCurve = 204L
  };

  // Methods
  BSTR get_FileNames();
  void put_FileNames(LPCTSTR val);
  BOOL get_ReadTime();
  void put_ReadTime(BOOL val);
  VARIANT Read(LPCTSTR ObjectType, LPCTSTR BaseIDs, LPCTSTR Variables,
    double* StartTime, double* EndTime);
  VARIANT CalcRainflow(VARIANT ArrayNx2, double GateValueMPa);
  BOOL OpenSNCurveLib(LPCTSTR SNCurveFile);
  double CalcDamage(VARIANT ArrayRx1, long SNStd, long SNCurve);
  long GetSNStdCount();
  BSTR GetSNStdName(long SNStd);
  long GetSNCurveCount(long SNStd);
  BSTR GetSNCurveName(long SNStd, long SNCurve);
  VARIANT ReadCurve(LPCTSTR BaseIDs, double* StartTime, double* EndTime);


  BEGIN_INTERFACE_PART(LocalClass, IResultExtractor)
    STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
    STDMETHOD(GetTypeInfo)(
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo);
    STDMETHOD(GetIDsOfNames)(
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid);
    STDMETHOD(Invoke)(
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr);
    STDMETHOD(get_FileNames)(THIS_ BSTR* pVal);
    STDMETHOD(put_FileNames)(THIS_ BSTR val);
    STDMETHOD(get_ReadTime)(THIS_ VARIANT_BOOL* pVal);
    STDMETHOD(put_ReadTime)(THIS_ VARIANT_BOOL val);
    STDMETHOD(Read)(THIS_ BSTR ObjectType, BSTR BaseIDs, BSTR Variables,
      double* StartTime, double* EndTime, VARIANT* ArrayNxM);
	STDMETHOD(ReadCurve)(THIS_ BSTR BaseIDs, double* StartTime, double* EndTime, VARIANT* ArrayNx2);
    STDMETHOD(CalcRainflow)(THIS_ VARIANT ArrayNx2, double GateValueMPa,
      VARIANT* ArrayRx1);
    STDMETHOD(OpenSNCurveLib)(THIS_ BSTR SNCurveFile, VARIANT_BOOL* pVal);
    STDMETHOD(CalcDamage)(THIS_ VARIANT ArrayRx1, long SNStd, long SNCurve,
      double* pVal);
    STDMETHOD(GetSNStdCount)(THIS_ long* pVal);
    STDMETHOD(GetSNStdName)(THIS_ long SNStd, BSTR* pVal);
    STDMETHOD(GetSNCurveCount)(THIS_ long SNStd, long* pVal);
    STDMETHOD(GetSNCurveName)(THIS_ long SNStd, long SNCurve, BSTR* pVal);
  END_INTERFACE_PART(LocalClass)

public:
  // Implementation
  bool CreateExtractor(LPCSTR szFileNames);

private:
  // Properties
  FFrExtractor* m_pExtractor;
  bool          m_readTime;
};
