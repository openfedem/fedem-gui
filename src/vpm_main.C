// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/Fui.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#endif
#include "vpmDB/FmDB.H"
#include "vpmPM/FpPM.H"
#include "vpmPM/FpBatchProcess.H"
#include "vpmUI/Icons/FuiIconPixmapsMain.H"
#include "FFuLib/FFuUserDialog.H"
#include "FFuLib/FFuAuxClasses/FFuaApplication.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"
#include "FFaLib/FFaDefinitions/FFaAppInfo.H"
#include "Admin/FedemAdmin.H"
#include "vpmApp/vpmAppCmds/FapToolsCmds.H"
#include "vpmApp/FapLicenseManager.H"
#ifdef FT_REDIRECT_COUT
#include <fstream> // For std::cout redirection
#endif

#include <cstring>
#if defined(win32) || defined(win64)
#define strcasecmp _stricmp
#define NOGDI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef FT_HAS_SEH
// Needed by Structured Exception Handling (SEH)
#include <dbghelp.h>
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
  HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
  CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
  CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
  CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
bool getRegUseSEH();
int doMainLoopSEH();
#endif
#else
#undef FT_HAS_COM
#undef FT_HAS_SEH
#endif
#ifdef FT_HAS_COM
extern bool CaInit();
#endif


int doMainLoop ()
{
  if (FFaAppInfo::isConsole())
    if (!FpBatchProcess::setupBatch())
      return 0;

  return FFuaApplication::mainLoop();
}


int main (int argc, char** argv)
{
#if defined(_MSV_VER) && _MSC_VER < 1900
  // Use two digit exponent in [s]printf(..)
  _set_output_format(_TWO_DIGIT_EXPONENT);
#endif

  const char* ext = argc == 2 ? strrchr(argv[1],'.') : NULL;
  if (ext && strcasecmp(ext,".fmm") == 0)
  {
    // Invoked as `Fedem <modelfile>` possibly via file-association, etc.
    // Insert the "-f" option such that the command-line handler works.
    char* args[3] = { argv[0], (char*)"-f", argv[1] };
    FFaCmdLineArg::init(3,args);
  }
  else
    FFaCmdLineArg::init(argc,argv);

  FFaCmdLineArg::instance()->addOption("help",false,"Print out this help text");
  FFaCmdLineArg::instance()->addOption("helpAll",false,"Print out this help text"
				       "\nincluding the private options, if any",false);
  FFaCmdLineArg::instance()->addOption("console",false,"Enable console window");
  FFaCmdLineArg::instance()->addOption("version",false,"Print out program version");

  // Private options (hidden in -help)
  FFaCmdLineArg::instance()->addOption("logCmds",false,"Print application commands to console",false);
  FFaCmdLineArg::instance()->addOption("memPoll",false,"Stop execution for memory polling",false);
  FFaCmdLineArg::instance()->addOption("allow3DofAttach",true,"Allow triads to be attached to 3-DOF nodes",false);
  FFaCmdLineArg::instance()->addOption("allowDepAttach",false,"Allow triads to be attached to dependent RGD nodes",false);
  FFaCmdLineArg::instance()->addOption("convertToLinear",1,"Convert parabolic shell and beam elements to linears"
				       "\n0: No conversion, 1: Ignore mid-side nodes, 2: Sub-divide",false);
  FFaCmdLineArg::instance()->addOption("ID_increment",0,"User ID increment on read",false);
  FFaCmdLineArg::instance()->addOption("reUseUserID",false,"Fill holes in user ID range when creating new objects",false);
#ifdef FT_HAS_COM
  FFaCmdLineArg::instance()->addOption("Embedding",false,"Run embedded using COM-API",false);
  FFaCmdLineArg::instance()->addOption("Automation",false,"Run automated using COM-API",false);
  FFaCmdLineArg::instance()->addOption("regserver",false,"Register the COM-API interface",false);
  FFaCmdLineArg::instance()->addOption("unregister",false,"Unregister the COM-API interface",false);
#endif

  // Public options
  FFaCmdLineArg::instance()->addOption("f","untitled.fmm","Model file to open");
  FFaCmdLineArg::instance()->addOption("feRepository","","FE model repository");
  FFaCmdLineArg::instance()->addOption("debug",false,"Run in debug mode");
  FFaCmdLineArg::instance()->addOption("plotNodes",false,"Enable plotting of nodal results");
  FFaCmdLineArg::instance()->addOption("plotElements",false,"Enable plotting of element results");
  FFaCmdLineArg::instance()->addOption("logFile",true,"Write all Output List contents to log-file."
				       "\nLog-file name: <modelFilePrefix>.log");
  FFaCmdLineArg::instance()->addOption("noFEData",false,"Load model file without FE models and FE results."
				       "\nUse together with -f, implicit when running batch.");
  FFaCmdLineArg::instance()->addOption("purgeOnSave",false,"Purge inactive mechanism objects on Save");
  FFaCmdLineArg::instance()->addOption("checkRDBinterval",500,"Time [ms] between each RDB check/update during solve");
  FFaCmdLineArg::instance()->addOption("checkCloudInterval",1000,"Time [ms] between each status check during cloud solve");
  FFaCmdLineArg::instance()->addOption("exportCurves","","Auto-export curves on batch solve."
				       "\nSpecify folder to export curve files to.");
  FFaCmdLineArg::instance()->addOption("exportAnimations",false,"Auto-export animations to VTF on batch solve");

  FFaCmdLineArg::instance()->addOption("solve","","Start given solver(s) in batch mode."
				       "\nThis option can have the following values:"
				       "\nall        = all solvers"
				       "\nevents     = all solvers on all simulation events"
				       "\nreducer    = reduction of all parts"
				       "\ndynamics   = dynamics solver"
				       "\nstress     = stress recovery in all parts"
				       "\nmodes      = mode shape expansion in all parts"
				       "\nstraingage = strain gage recovery in all parts"
				       "\nstraincoat = strain coat recovery in all parts");
  FFaCmdLineArg::instance()->addOption("prepareBatch","","Prepare for batch execution"
				       "\nThis option can have the following values:"
				       "\nall        = all solvers"
				       "\nreducer    = reduction of all parts"
				       "\ndynamics   = dynamics solver"
				       "\nstress     = stress recovery in all parts"
				       "\nmodes      = mode shape expansion in all parts"
				       "\nstraingage = strain gage recovery in all parts"
				       "\nstraincoat = strain coat recovery in all parts");
  FFaCmdLineArg::instance()->addOption("events","","Simulation event definition file");
  FFaCmdLineArg::instance()->addOption("timerange","","Time specification for batch stress recovery."
				       "\nFormat: [startTime,stopTime(,timeInc)]."
				       "\ntimeInc is optional.");

  // Get program version and build date
  const char* fedem_version = FedemAdmin::getVersion();
  const char* build_date = FedemAdmin::getBuildDate();

  // Print help and info text, if wanted
  bool wantHelp, wantAllHelp, logCmds;
  FFaCmdLineArg::instance()->getValue("help",wantHelp);
  FFaCmdLineArg::instance()->getValue("helpAll",wantAllHelp);
  if (wantHelp || wantAllHelp)
  {
    std::string hlpText;
    FFaCmdLineArg::instance()->composeHelpText(hlpText,wantAllHelp);
    FFaAppInfo::openConsole();
    std::cout <<"fedem version "<< fedem_version <<" "<< build_date
	      <<"\n\nAvailable command-line options:\n"<< hlpText;
    FFaAppInfo::closeConsole();
    exit(0);
  }

  FFaCmdLineArg::instance()->getValue("version",wantHelp);
  if (wantHelp)
  {
    FFaAppInfo::openConsole();
    std::cout <<"fedem version "<< fedem_version <<" "<< build_date << std::endl;
    FFaAppInfo::closeConsole();
    exit(0);
  }

  // Check if the console window is needed
  FFaCmdLineArg::instance()->getValue("logCmds",logCmds);
  FFaCmdLineArg::instance()->getValue("console",wantAllHelp);
  if (FpBatchProcess::userWantsBatch())
    FFaAppInfo::openConsole(false); // batch execution, only console
  else if (logCmds || wantAllHelp)
    FFaAppInfo::openConsole(true);  // GUI and console window
#ifdef FT_HAS_COM
  else // only GUI
  {
    // Use "/subsystem:console" to avoid AfxWinMain when building with COM-API
    // Hide the console window here, unless wanted (-console or batch execution)
    HWND hWndConsole = GetConsoleWindow();
    if (hWndConsole) ShowWindow(hWndConsole,SW_HIDE);
    // TODO: this not so good; if starting from an existing terminal window,
    // that one is closed also when launched. Find a way to detect that..
  }
#endif

  // Check for expiration (for internal releases)
  int expired = FedemAdmin::getExpireAfter();
  if (expired > 0)
  {
    int myAge = FedemAdmin::getDaysSinceBuilt();
    if (myAge >= expired)
    {
      FFaAppInfo::openConsole();
      std::cout <<"fedem version "<< fedem_version <<" "<< build_date <<"\n\n"
		<<"*** This version expired "<< myAge-expired <<" days ago ***\n";
      FFaAppInfo::closeConsole();
      exit(1);
    }
    else
      expired -= myAge;
  }

  FpPM::init(argv[0]);
  Fui::init(argc,argv);

  // Change splash screen based on activated feature set
  if (FapLicenseManager::checkWindpowerLicense())
    FFuaApplication::splashScreenWindpower();
  else if (FapLicenseManager::checkOffshoreLicense())
    FFuaApplication::splashScreenOffshore();

  FFuaApplication::splashMessage("Initializing database");
#ifdef USE_INVENTOR
  FdDB::init();
#endif
  FmDB::init();

  FpPM::CopyrightNotice();
  if (expired > 0)
    std::cout <<"*** This is an internal (non-public) version that will expire in "
	      << expired <<" days ***\n\n\n";

  // Register file associations
  if (!FapToolsCmds::checkFileAssociations())
    FapToolsCmds::setFileAssociationsEx();

#ifdef FT_HAS_COM
  // Initialize the COM handling
  if (!CaInit())
    return 0;
#endif

  FpPM::loadUnitConvertionFile();
  FpPM::loadResultPosFiles();
  FpPM::loadSNCurveFile();
  FpPM::loadAllPlugins();

  Fui::noUserInputPlease();
  Fui::start();
#ifdef USE_INVENTOR
  FdDB::start();
#endif
  FpPM::start();

  FFuaApplication::splashMessage("Loading user interface");
  Fui::mainUI();
  Fui::okToGetUserInput();

  FFuaApplication::splashMessage("Loading model file");
  FpPM::openCmdLineFile();
  FFuaCmdItem::enableCmdLogging(logCmds);

  FFuUserDialog* memExhaustWarning = NULL;
  if (!FFaAppInfo::isConsole())
  {
    const char* buttonTexts[1] = {"Quit"};
    memExhaustWarning = FFuUserDialog::create("Fedem could not allocate the requested memory, and must shut down",
					      FFuDialog::ERROR,
					      buttonTexts,
					      1);
    memExhaustWarning->popDown();
  }

#ifdef FT_HAS_UPDATER
  // Check for updates (silent).
  Fui::runUpdater("/silent");
#endif

#ifdef FT_REDIRECT_COUT
  // Redirect all std::cout print to a file
  std::ofstream redir("fedem_console.txt");
  std::cout.rdbuf(redir.rdbuf());
#endif

  try
  {
#ifdef FT_HAS_SEH
    // Main loop, with SEH (unless debug mode)
    bool debugMode = false;
    FFaCmdLineArg::instance()->getValue("debug",debugMode);
    if (!debugMode && getRegUseSEH())
      return doMainLoopSEH();
#endif
    // Main loop (without SEH)
    return doMainLoop();
  }
  catch (std::bad_alloc&)
  {
    if (FFaAppInfo::isConsole())
      std::cerr <<"------------------------------------------------------------------\n"
		<<"Fedem could not allocate the requested memory, and must shut down.\n"
		<<"------------------------------------------------------------------\n";
    else
    {
      memExhaustWarning->popUp();
      memExhaustWarning->execute();
    }
  }
}


#ifdef FT_HAS_SEH

/*!
  Structured Exception Handling (SEH).
*/

int filterExceptionSEH (int code, PEXCEPTION_POINTERS ex)
{
  // Output crash to FedemCrash.log (in the temp-folder)
  SYSTEMTIME stLocalTime;
  DWORD dwBufferSize = MAX_PATH;
  CHAR szPath[MAX_PATH];
  ::GetLocalTime(&stLocalTime);
  ::GetTempPath(dwBufferSize, szPath);
  CHAR szCrashLog[MAX_PATH];
  strcpy(szCrashLog, szPath);
  strcat(szCrashLog, "FedemCrash.log");
  FILE* fpLog = fopen(szCrashLog, "ab");
  CHAR buf[4096];
  sprintf(buf, "Crash %d-%d-%d %d:%d:%d\r\n",
    stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
    stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
  fwrite(buf, 1, strlen(buf), fpLog);
  sprintf(buf, "ExceptionCode=0x%x\r\n",
    ex->ExceptionRecord->ExceptionCode);
  fwrite(buf, 1, strlen(buf), fpLog);
  sprintf(buf, "ExceptionFlags=0x%x\r\n",
    ex->ExceptionRecord->ExceptionFlags);
  fwrite(buf, 1, strlen(buf), fpLog);
  sprintf(buf, "ExceptionAddress=0x%x\r\n",
    ex->ExceptionRecord->ExceptionAddress);
  fwrite(buf, 1, strlen(buf), fpLog);
  sprintf(buf, "ModuleHandle=0x%x\r\n\r\n",
    ::GetModuleHandle(NULL));
  fwrite(buf, 1, strlen(buf), fpLog);
  fclose(fpLog);

  // Output dump file to FedemCrash.dmp (in the temp-folder)
  HMODULE mhLib = ::LoadLibrary("dbghelp.dll");
  if (mhLib != NULL) {
    MINIDUMPWRITEDUMP pDump =
      (MINIDUMPWRITEDUMP)::GetProcAddress(mhLib, "MiniDumpWriteDump");
    if (pDump != NULL) {
      strcpy(szCrashLog, szPath);
      sprintf(buf, "FedemCrash_%d-%d-%d_%d-%d-%d.dmp",
        stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
        stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);
      strcat(szCrashLog, buf);
      HANDLE hFile = ::CreateFile(szCrashLog,
        GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
      _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
      ExInfo.ThreadId = ::GetCurrentThreadId();
      ExInfo.ExceptionPointers = ex;
      ExInfo.ClientPointers = FALSE;
      pDump(GetCurrentProcess(), GetCurrentProcessId(),
        hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
      ::CloseHandle(hFile);
    }
  }
  return EXCEPTION_EXECUTE_HANDLER;
}


/*!
  Main loop function with Structured Exception Handling (SEH).
*/

int doMainLoopSEH ()
{
  __try { return doMainLoop(); }
  __except(filterExceptionSEH(GetExceptionCode(),GetExceptionInformation()))
  {
    // Notify the user about the execption
    const char* msg =
      "FEDEM has experienced an unrecoverable error and must shut down.\n\n"
      "Note: Two log-files have been created in the Windows temp-directory.\n"
      "We would appreciate it very much if you could send these files to\n"
      "developers@openfedem.org. The files are FedemCrash.log and FedemCrash*.dmp.\n"
      "They are usually located in: C:\\Users\\(username)\\AppData\\Local\\Temp\n"
      "The files do not contain any personal data or information about the FEDEM model content.";
    if (FFaAppInfo::isConsole())
      std::cerr << msg << std::endl;
    else
      ::MessageBox(NULL, msg, "FEDEM Error", MB_OK | MB_ICONEXCLAMATION);
  }

  return 0;
}


/*!
  This function checks if SEH is turned on or off.
  Default is on. Developers may wish to turn it off.
*/

bool getRegUseSEH ()
{
  // Open registry key
  HKEY hk;
  LONG err = ::RegOpenKeyEx(HKEY_CLASSES_ROOT,
			    "FMM-file\\internal",
			    0, KEY_QUERY_VALUE, &hk);
  if (err != ERROR_SUCCESS)
    return true;

  // Get registry value
  DWORD data = 1;
  DWORD count = 4;
  err = ::RegQueryValueEx(hk,
			  "useSEH",
			  NULL, NULL,
			  (LPBYTE)&data,
			  &count);
  ::RegCloseKey(hk);
  if (err == ERROR_SUCCESS)
    return (bool)data;

  return true;
}

#endif
