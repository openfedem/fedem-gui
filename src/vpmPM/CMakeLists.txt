# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmPM )

find_package ( Qt6 REQUIRED COMPONENTS Core )
if ( Qt6Core_VERSION VERSION_GREATER_EQUAL 6.3 )
  qt_standard_project_setup ()
endif ( Qt6Core_VERSION VERSION_GREATER_EQUAL 6.3 )

if ( USE_WINDPOW )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_WND" )
endif ( USE_WINDPOW )
if ( USE_EXT_CTRLSYS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_EXTCTRL" )
endif ( USE_EXT_CTRLSYS )
if ( USE_PROFILER )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_PROFILER" )
endif ( USE_PROFILER )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST FpBatchProcess FpModelRDBHandler
                          FpPM FpProcess FpProcessBase FpProcessManager
                          FpRDBExtractorManager FpExtractor
)
## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FpFileSys FpProcessOptions )
## Pure implementation files, i.e., source files without corresponding header
set ( SOURCE_FILE_LIST FpRDBHandler )

#
# Qt moc handling, see also section just above the dependency setup
#
set ( QT_MOC_HEADER_FILE_LIST FpProcess ) ## H files to be moc'ed


foreach ( FILE ${QT_MOC_HEADER_FILE_LIST} )
  set ( QT_MOC_HEADER_FILES ${QT_MOC_HEADER_FILES} ${FILE}.H )
endforeach ( FILE ${QT_MOC_HEADER_FILE_LIST} )


include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )
QT6_WRAP_CPP ( QT_MOC_SOURCE_FILES ${QT_MOC_HEADER_FILES} ) # Named moc_<filename>.cxx where input is filname.H


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


set ( HEADER_FILES ${HPP_HEADER_FILES} )
set ( SOURCE_FILES ${CPP_SOURCE_FILES} ${QT_MOC_SOURCE_FILES} )


#
# Set library dependencies
#
set ( FT_COMMON_LIBRARIES FFrLib FFlLib FFaCmdLineArg FFaDefinitions FFaDynCalls FFaOS FFaString )
set ( FT_KERNEL_LIBRARIES FFaFunctionLib )
if ( Qwt_LIBRARY )
  list ( APPEND FT_KERNEL_LIBRARIES FFpFatigue )
endif ( Qwt_LIBRARY )
set ( FT_GUI_LIBRARIES vpmApp vpmAppCmds vpmAppProcess vpmAppUAMap vpmUI FFuAuxClasses )
set ( DEPENDENCY_LIST ${FT_GUI_LIBRARIES} ${FT_KERNEL_LIBRARIES} ${FT_COMMON_LIBRARIES} )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${SOURCE_FILES} ${HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
