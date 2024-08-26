# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Library setup

set ( LIB_ID vpmApp )
set ( LIB_ID_LIST )

string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_CONNECTORS" )

add_subdirectory ( vpmAppProcess )


# Options for inclusion of various features at compile time
option ( USE_CONTROL "Build with Control system modeling" ON )
option ( USE_TIREAPI "Build with Tire/Road modeling" OFF )
option ( USE_FATIGUE "Build with Fatigue/life calculation" OFF )
option ( USE_MESHING "Build with FE meshing support" OFF )
option ( USE_OILWELL "Build with Oil well modeling" OFF )
option ( USE_RISER   "Build with Offshore modeling" OFF )
option ( USE_SEVENTS "Build with Simulation Event support" OFF )
option ( USE_DIGTWIN "Build with Digital Twin features" OFF )
option ( USE_VTF     "Use the VTF API (Ceetron)" OFF )
mark_as_advanced ( USE_TIREAPI USE_MESHING USE_OILWELL USE_VTF )

if ( USE_WINDPOW )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_WND" )
endif ( USE_WINDPOW )

add_subdirectory ( vpmAppUAMap )

if ( USE_VTF )
  find_package ( VTFAPI )
  if ( VTFAPI_FOUND )
    string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_VTF" )
  endif ( VTFAPI_FOUND )
endif ( USE_VTF )

add_subdirectory ( vpmAppDisplay )

if ( USE_OILWELL )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_OWL" )
endif ( USE_OILWELL )
if ( USE_RISER )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_RIS" )
endif ( USE_RISER )
if ( USE_SEVENTS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_SEV" )
endif ( USE_SEVENTS )
if ( USE_DIGTWIN )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_SAP" )
endif ( USE_DIGTWIN )

add_subdirectory ( vpmAppCmds )

if ( USE_CONTROL )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_CTR" )
endif ( USE_CONTROL )
if ( USE_TIREAPI )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_TII" )
endif ( USE_TIREAPI )
if ( USE_FATIGUE )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_DRB" )
endif ( USE_FATIGUE )
if ( USE_MESHING )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_MSH" )
endif ( USE_MESHING )


## Files with header and source with same name
set ( CPP_COMPONENT_FILE_LIST FapEventManager FapLicenseManager )
## Pure header files, i.e., header files without a corresponding source file
set ( CPP_HEADER_FILE_LIST FapInit )
## Pure implementation files, i.e., source files without corresponding header
set ( CPP_SOURCE_FILE_LIST )


foreach ( FILE ${CPP_COMPONENT_FILE_LIST} )
  list ( APPEND CPP_SOURCE_FILES ${FILE}.C )
  list ( APPEND CPP_HEADER_FILES ${FILE}.H )
endforeach ( FILE ${CPP_COMPONENT_FILE_LIST} )


foreach ( FILE ${CPP_HEADER_FILE_LIST} )
  list ( APPEND CPP_HEADER_FILES ${FILE}.H )
endforeach ( FILE ${CPP_HEADER_FILE_LIST} )


foreach ( FILE ${CPP_SOURCE_FILE_LIST} )
  list ( APPEND CPP_SOURCE_FILES ${FILE}.C )
endforeach ( FILE ${CPP_SOURCE_FILE_LIST} )


message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${CPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} vpmAppCmds vpmAppUAMap )
