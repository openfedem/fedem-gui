# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

unset ( ZLIB_INCLUDE CACHE )
unset ( ZLIB_LIBRARY CACHE )

find_path ( ZLIB_INCLUDE
            NAMES zlib.h
            PATHS C:/Zlib/include
           )

if ( ZLIB_INCLUDE )
  message ( STATUS "Found Zlib: ${ZLIB_INCLUDE}" )
  include_directories ( ${ZLIB_INCLUDE} )

  find_library ( ZLIB_LIBRARY
                 NAMES z zlib
                 PATHS C:/Zlib/lib
               )
  find_library ( MINIZIP_LIBRARY
                 NAMES zip minizip
                 PATHS C:/Zlib/lib
               )

  if ( ZLIB_LIBRARY AND WIN )
    get_filename_component ( ZLIB_DIR ${ZLIB_LIBRARY} DIRECTORY )
    string ( REGEX REPLACE "lib$" "bin" ZLIB_BIN ${ZLIB_DIR} )
    find_file ( ZLIB_DLL zlib.dll PATHS ${ZLIB_DIR} ${ZLIB_BIN} )
  endif ( ZLIB_LIBRARY AND WIN )

endif ( ZLIB_INCLUDE )

if ( ZLIB_LIBRARY AND MINIZIP_LIBRARY )
  message ( STATUS "Found Zlib: ${ZLIB_LIBRARY} ${MINIZIP_LIBRARY}" )
  if ( ZLIB_DLL )
    message ( STATUS "Found Zlib: ${ZLIB_DLL}" )
  endif ( ZLIB_DLL )
  include_directories ( ${ZLIB_INCLUDE} )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_ZLIB" )
else ( ZLIB_LIBRARY )
  message ( WARNING "Did NOT find Zlib, configuring without it." )
endif ( ZLIB_LIBRARY AND MINIZIP_LIBRARY )
