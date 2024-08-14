// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstring>

#include "vpmApp/FapLicenseManager.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#endif
#include "vpmPM/FpPM.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"


bool FapLicenseManager::hasCtrlLicense(int creating, int /*msgType*/)
{
  if (FapLicenseManager::checkCtrlLicense(true,creating)) return true;

  ListUI <<"\n===> This model contains control system elements\n"
         <<"     but the control system feature is not available in this version.\n"
         <<"     Manipulation of control elements is disabled.\n\n";
  return false;
}


bool FapLicenseManager::checkCtrlLicense(bool checkOut, int /*creating*/)
{
  return FapLicenseManager::checkLicense("FA-CTR",checkOut);
}


bool FapLicenseManager::checkExtCtrlLicense(bool checkOut)
{
  return FapLicenseManager::checkLicense("FA-CTI",checkOut);
}


bool FapLicenseManager::hasTireLicense(int /*msgType*/)
{
  if (FapLicenseManager::checkTireLicense(true)) return true;

  ListUI <<"\n===> Tire and road objects are present in this model\n"
         <<"     but that feature is not available in this version.\n"
         <<"     Manipulation of tire and road objects is disabled.\n\n";
  return false;
}


bool FapLicenseManager::checkTireLicense(bool checkOut)
{
  return FapLicenseManager::checkLicense("FA-TII",checkOut);
}


bool FapLicenseManager::checkWindpowerLicense(bool checkOut)
{
  return FapLicenseManager::checkLicense("FA-WND",checkOut);
}


bool FapLicenseManager::checkOffshoreLicense(bool checkOut)
{
  return FapLicenseManager::checkLicense("FA-RIS",checkOut);
}


bool FapLicenseManager::checkSimEventLicense(bool checkOut)
{
  return FapLicenseManager::checkLicense("FA-SEV",checkOut);
}


bool FapLicenseManager::checkLicense(const char* feature, bool checkOut)
{
  // Using preprocessor defines to activate features compile time
#ifdef FT_HAS_CTR
  if (!strcmp(feature,"FA-CTR")) return true;
#endif
#ifdef FT_HAS_TII
  if (!strcmp(feature,"FA-TII")) return true;
#endif
#ifdef FT_HAS_DRB
  if (!strcmp(feature,"FA-DRB")) return true;
#endif
#ifdef FT_HAS_VTF
  if (!strcmp(feature,"FA-VTF")) return true;
#endif
#ifdef FT_HAS_MSH
  if (!strcmp(feature,"FA-MSH")) return true;
#endif
#ifdef FT_HAS_OWL
  if (!strcmp(feature,"FA-OWL")) return true;
#endif
#ifdef FT_HAS_RIS
  if (!strcmp(feature,"FA-RIS")) return true;
#endif
#ifdef FT_HAS_WND
  if (!strcmp(feature,"FA-WND")) return true;
#endif
#ifdef FT_HAS_SEV
  if (!strcmp(feature,"FA-SEV")) return true;
#endif
#ifdef FT_HAS_COM
  if (!strcmp(feature,"FA-COM")) return true;
#endif
#ifdef FT_HAS_SAP
  if (!strcmp(feature,"FA-SAP")) return true;
#endif
  // Dummy statement to suppress compiler warning
  if (feature && checkOut) return false;

  return false;
}


bool FapLicenseManager::hasDurabilityLicense(int /*msgType*/)
{
  if (FapLicenseManager::checkLicense("FA-DRB")) return true;

  ListUI <<"\n===> Durability calculation objects are present in this model\n"
         <<"     but that feature is not available in this version.\n"
         <<"     Manipulation of durability calculation objects is disabled.\n\n";
  return false;
}


bool FapLicenseManager::hasVTFExportLicense(int /*msgType*/)
{
  if (FapLicenseManager::checkVTFExportLicense(true)) return true;

  ListUI <<"\n===> Auto-export to VTF files is turned on in this model\n"
         <<"     but that feature is not available in this version.\n"
         <<"     Auto-export to VTF is disabled.\n\n";
  return false;
}


bool FapLicenseManager::checkVTFExportLicense(bool checkOut)
{
  return FapLicenseManager::checkLicense("FA-VTF",checkOut);
}


bool FapLicenseManager::hasUserGuide()
{
  return !FpPM::getFullFedemPath("Doc/FedemUsersGuide.pdf",'e').empty();
}


bool FapLicenseManager::hasTheoryGuide()
{
  return !FpPM::getFullFedemPath("Doc/FedemTheoryGuide.pdf",'e').empty();
}


bool FapLicenseManager::hasReferenceGuide()
{
  return !FpPM::getFullFedemPath("Doc/Fedem.chm",'e').empty();
}


bool FapLicenseManager::hasComAPIGuide()
{
  return !FpPM::getFullFedemPath("Doc/ComAPI.chm",'e').empty();
}
