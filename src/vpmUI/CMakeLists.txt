# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

set ( LIB_ID vpmUI )
set ( LIB_ID_LIST vpmUIComponents vpmUITopLevels )


option ( USE_FREQDOMAIN "Build Fedem GUI with frequency-domain settings" true )

if ( USE_WINDPOW )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_WND" )
endif ( USE_WINDPOW )


foreach ( FOLDER ${LIB_ID_LIST} )
  add_subdirectory ( ${FOLDER} )
endforeach ( FOLDER ${LIB_ID_LIST} )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST FuiCtrlModes FuiModes FuiMsg Fui )
## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST )
## Pure implementation files, i.e., source files without corresponding header
set ( SOURCE_FILE_LIST )


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


# Build the ${LIB_ID} library

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )


# Install StartGuide files

file ( GLOB HTM_FILES StartGuide/*.htm )
file ( GLOB IMG_FILES StartGuide/img/*.* )
if ( FTENV_VERBOSE )
  message ( STATUS "Installing ${HTM_FILES} to ${CMAKE_INSTALL_PREFIX}" )
endif ( FTENV_VERBOSE )
install ( FILES ${HTM_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Doc" )
install ( FILES ${IMG_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Doc/img" )
