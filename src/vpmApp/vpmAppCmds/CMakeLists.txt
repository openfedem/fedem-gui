# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmAppCmds )


if ( USE_SEVENTS OR USE_RISER OR USE_WINDPOW )
  set ( INCLUDE_ASSEMBLIES TRUE )
endif ( USE_SEVENTS OR USE_RISER OR USE_WINDPOW )
if ( USE_OILWELL OR USE_RISER OR USE_WINDPOW )
  set ( INCLUDE_OILWELLCMD TRUE )
endif ( USE_OILWELL OR USE_RISER OR USE_WINDPOW )

if ( INCLUDE_ASSEMBLIES )
  add_subdirectory ( ../../../fedem-mdb/assemblyCreators "${CMAKE_CURRENT_BINARY_DIR}/assemblyCreators" )
endif ( INCLUDE_ASSEMBLIES )

if ( USE_DIGTWIN )
  find_package ( Zlib )
endif ( USE_DIGTWIN )

if ( USE_EXT_CTRLSYS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_EXTCTRL" )
endif ( USE_EXT_CTRLSYS )
if ( USE_NCODE_INTERFACE )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_NCODE" )
endif ( USE_NCODE_INTERFACE )
if ( USE_MEMPOOL )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_MEMPOOL" )
endif ( USE_MEMPOOL )

string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_VISUALS" )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FapAddResProbeCmd FapAllignCSCmds
                           FapAnimationCmds FapCmdsBase
                           FapCreateSensorCmd FapCtrl3DObjCmds
                           FapCylinderSpiderCmds FapDBCreateCmds FapEditCmds
                           FapEditStrainRosetteDirCmd FapEditStrainRosetteNodesCmd
                           FapExportCmds FapFileCmds FapFunctionCmds
                           FapGeneralSpiderCmds FapGraphCmds FapHelpCmds
                           FapLinkVisualCmds FapListViewCmds FapMainWinCmds
                           FapMech3DObjCmds FapMoveToCenterCmds
                           FapOutputListCmds FapSelectionCmds
                           FapSolveCmds FapStrainCoatCmds FapStrainRosetteCmds
                           FapToolsCmds FapViewCtrlCmds FapWorkSpaceCmds
)
if ( INCLUDE_OILWELLCMD )
  list ( APPEND COMPONENT_FILE_LIST FapOilWellCmds )
endif ( INCLUDE_OILWELLCMD )


## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST )

## Pure implementation files, i.e., source files without corresponding header
if ( ZLIB_LIBRARY )
  set ( SOURCE_FILE_LIST FapZip )
else ( ZLIB_LIBRARY )
  set ( SOURCE_FILE_LIST )
endif ( ZLIB_LIBRARY )


foreach ( FILE ${COMPONENT_FILE_LIST} )
  set ( CPP_SOURCE_FILES ${CPP_SOURCE_FILES} ${FILE}.C )
  set ( HPP_HEADER_FILES ${HPP_HEADER_FILES} ${FILE}.H )
endforeach ( FILE ${COMPONENT_FILE_LIST} )


foreach ( FILE ${HEADER_FILE_LIST} )
  set ( HPP_HEADER_FILES ${HPP_HEADER_FILES} ${FILE}.H )
endforeach ( FILE ${HEADER_FILE_LIST} )


foreach ( FILE ${SOURCE_FILE_LIST} )
  set ( CPP_SOURCE_FILES ${CPP_SOURCE_FILES} ${FILE}.C )
endforeach ( FILE ${SOURCE_FILE_LIST} )


set ( DEPENDENCY_LIST vpmAppDisplay vpmAppProcess FFlLib )
if ( Qwt_LIBRARY )
  list ( APPEND DEPENDENCY_LIST FFpCurveData )
endif ( Qwt_LIBRARY )
if ( WIN AND Coin_library )
  include_directories ( "$ENV{COIN_ROOT}/include" )
endif ( WIN AND Coin_library )
if ( USE_PROFILER )
  list ( APPEND DEPENDENCY_LIST FFaProfiler )
endif ( USE_PROFILER )
if ( INCLUDE_ASSEMBLIES )
  list ( INSERT DEPENDENCY_LIST 0 assemblyCreators )
endif ( INCLUDE_ASSEMBLIES )

if ( ZLIB_LIBRARY )
  list ( APPEND DEPENDENCY_LIST ${MINIZIP_LIBRARY} ${ZLIB_LIBRARY} )
endif ( ZLIB_LIBRARY )


message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )


if ( ZLIB_DLL )
  if ( FTENV_VERBOSE )
    message ( STATUS "Installing ${ZLIB_DLL} to ${CMAKE_INSTALL_PREFIX}" )
  endif ( FTENV_VERBOSE )
  install ( FILES ${ZLIB_DLL} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
            CONFIGURATIONS Release RelWithDebInfo )
endif ( ZLIB_DLL )
