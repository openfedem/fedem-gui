# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID FFdCadModel )

if ( Coin_library )
  if ( WIN )
    string ( APPEND CMAKE_CXX_FLAGS " -DCOIN_DLL" )
  endif ( WIN )
  if ( DEFINED ENV{COIN_ROOT} )
    include_directories ( "$ENV{COIN_ROOT}/include" )
  endif ( DEFINED ENV{COIN_ROOT} )
endif ( Coin_library )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST FdCadHandler FdCadEdge FdCadFace
                          FdCadSolid FdCadSolidWire
    )
## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FdCadInfo FFdUtils/FFdLook )


foreach ( FILE ${COMPONENT_FILE_LIST} )
  list ( APPEND CPP_SOURCE_FILES ${FILE}.C )
  list ( APPEND HPP_HEADER_FILES ${FILE}.H )
endforeach ( FILE ${COMPONENT_FILE_LIST} )

foreach ( FILE ${HEADER_FILE_LIST} )
  list ( APPEND HPP_HEADER_FILES ${FILE}.H )
endforeach ( FILE ${HEADER_FILE_LIST} )


message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} FFaAlgebra FFaContainers ${Coin_library} )
