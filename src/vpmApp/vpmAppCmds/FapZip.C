// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////
/*
 * This is an adaption of minizip.c of the Minizip project
 * http://www.winimage.com/zLibDll/minizip.html
 * Copyright (C) 1998-2010 Gilles Vollant
 *
 * Rewritten for Fedem usage 2022 by K. M. Okstad
 */

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <ctime>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "zip.h"
#ifdef _WIN32
#include "iowin32.h"
#endif


namespace Fap
{
  void filetime (const char* f, tm_zip& tmzip, uLong* dt)
  {
#ifdef _WIN32
    tmzip.tm_sec  = tmzip.tm_min = tmzip.tm_hour = 0;
    tmzip.tm_mday = tmzip.tm_mon = tmzip.tm_year = 0;
    WIN32_FIND_DATAA ff32;
    HANDLE hFind = FindFirstFileA(f,&ff32);
    if (hFind != INVALID_HANDLE_VALUE)
    {
      FILETIME ftLocal;
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
      FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
      FindClose(hFind);
    }
#else
    struct stat s;
    time_t tm_t = stat(f,&s) == 0 ? s.st_mtime : 0;
    struct tm* filedate = localtime(&tm_t);
    tmzip.tm_sec  = filedate->tm_sec;
    tmzip.tm_min  = filedate->tm_min;
    tmzip.tm_hour = filedate->tm_hour;
    tmzip.tm_mday = filedate->tm_mday;
    tmzip.tm_mon  = filedate->tm_mon ;
    tmzip.tm_year = filedate->tm_year;
    *dt = 0;
#endif
  }

  bool make_zip (const std::string& zipName,
                 const std::vector<std::string>& fileNames)
  {
#ifdef _WIN32
    zlib_filefunc64_def ffunc;
    fill_win32_filefunc64A(&ffunc);
    zipFile zf = zipOpen2_64(zipName.c_str(),0,NULL,&ffunc);
#else
    zipFile zf = zipOpen64(zipName.c_str(),0);
#endif
    if (!zf)
    {
      std::cerr <<" *** Failed to open zip-file "<< zipName << std::endl;
      return false;
    }

    const int compress_level = Z_DEFAULT_COMPRESSION;
    const int size_buf = 16384;
    char buf[size_buf];
    zip_fileinfo zi;
    zi.internal_fa = zi.external_fa = 0;
    size_t archive = 0;

    for (const std::string& fileName : fileNames)
    {
      std::string unixName(fileName); // Ensure '/' as path separator
      std::replace(unixName.begin(), unixName.end(), '\\', '/');
      filetime(fileName.c_str(),zi.tmz_date,&zi.dosDate);
      int err = zipOpenNewFileInZip3_64(zf,unixName.c_str(),&zi,
                                        NULL,0,NULL,0,NULL,
                                        compress_level ? Z_DEFLATED : 0,
                                        compress_level,0,
                                        -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                        NULL,0,0);
      if (err)
      {
        std::cerr <<"  ** Failed to open "<< fileName <<" in zip-file."<< std::endl;
        continue;
      }

      FILE* fin = fopen(fileName.c_str(),"rb");
      if (!fin)
      {
        std::cerr <<"  ** Failed to open "<< fileName <<" for reading."<< std::endl;
        continue;
      }

      int size_read = 0;
      while ((size_read = (int)fread(buf,1,size_buf,fin)) > 0)
      {
        err = feof(fin) == 0 ? size_read - size_buf : 0;
        if (err < 0)
        {
          std::cerr <<"  ** Failure reading "<< fileName << std::endl;
          break;
        }

        err = zipWriteInFileInZip(zf,buf,size_read);
        if (err < 0)
        {
          std::cerr <<"  ** Failure writing "<< fileName <<" in the zip-file."<< std::endl;
          break;
        }
      }

      fclose(fin);
      if (err < 0)
        err = Z_ERRNO;
      else if ((err = zipCloseFileInZip(zf)))
        std::cerr <<"  ** Failed to close "<< fileName <<" in the zip-file."<< std::endl;
      else
        archive++;
    }

    int err = zipClose(zf,NULL);
    if (archive < fileNames.size())
      std::cerr <<" *** The zip-file "<< zipName <<" is incomplete, "
                << fileNames.size()-archive <<" files could not be written."<< std::endl;
    else if (err)
      std::cerr <<" *** Failed to close zip-file "<< zipName << std::endl;
    else
      return true;

    return false;
  }
}
