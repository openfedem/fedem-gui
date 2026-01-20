# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmAppDisplay )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST FapAnimationCreator FFaLegendMapper
                          FapVTFFile FapCGeoFile )
if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FapGraphDataMap )
endif ( Qwt_LIBRARY )

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FapReadCurveData )
## Pure implementation files, i.e., source files without corresponding header
set ( SOURCE_FILE_LIST FapModesAnimation )


find_path ( STB_IMAGE_PATH stb_image.h PATHS "$ENV{STB_ROOT}" )

if ( STB_IMAGE_PATH )
  include_directories ( ${STB_IMAGE_PATH} )
  string ( APPEND CMAKE_CXX_FLAGS " -DSTB_IMAGE=1" )
endif ( STB_IMAGE_PATH )

if ( USE_REFCOUNT )
  string ( APPEND CMAKE_CXX_FLAGS " -DFFL_REFCOUNT" )
endif ( USE_REFCOUNT )

if ( USE_MEMPOOL )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_MEMPOOL" )
endif ( USE_MEMPOOL )

if ( USE_PROFILER )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_PROFILER" )
  list ( APPEND SOURCE_FILE_LIST FapMemoryProfiler )
endif ( USE_PROFILER )

string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_VISUALS" )


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

if ( VTFAPI_FOUND )
  include_directories ( ${VTF_INCLUDE} )
endif ( VTFAPI_FOUND )

if ( DEFINED ENV{COIN_ROOT} AND Coin_library )
  include_directories ( "$ENV{COIN_ROOT}/include" )
endif ( DEFINED ENV{COIN_ROOT} AND Coin_library )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HEADER_FILES} )
target_link_libraries ( ${LIB_ID} FFlrLib FFaOperation ${VTF_LIBRARY} )
