# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmAppUAMap )

if ( USE_MEMPOOL )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_MEMPOOL" )
endif ( USE_MEMPOOL )
if ( USE_FORTRAN )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_PREVIEW" )
endif ( USE_FORTRAN )
if ( USE_SOLVERS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_SOLVERS" )
endif ( USE_SOLVERS )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FapUAAdvAnalysisOptions FapUAAnimationControl
                           FapUAAnimationDefine FapUAAppearance
                           FapUACtrlElemProperties FapUACtrlGridAttributes
                           FapUACtrlModeller FapUACSSelector FapUACSListView
                           FapUAEigenOptions
                           FapUAFppOptions FapUAFunctionProperties FapUAGageOptions
                           FapUAItemsListView FapUALinkRamSettings FapUAMainWindow
                           FapUAMiniFileBrowser FapUAModeller
                           FapUAModelPreferences FapUAModMemListView
                           FapUAOutputList FapUAPreferences FapUAProperties
                           FapUAQuery FapUAQueryInputField
                           FapUARDBListView FapUARDBSelector FapUARDBMEFatigue FapUAResultListView
                           FapUASimModelListView FapUASimModelRDBListView FapUAStressOptions
                           FapUAViewSettings
                           FapUASeaEnvironment FapUACreateBeamstringPair
                           FapUAObjectBrowser FapUAModelExport
                           vpmAppUAMapHandlers/FapUACommandHandler
                           vpmAppUAMapHandlers/FapUADataHandler
                           vpmAppUAMapHandlers/FapUAExistenceHandler
                           vpmAppUAMapHandlers/FapUAFinishHandler
                           vpmAppUAMapHandlers/FapUAItemsViewHandler
    )

if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FapUACurveDefine FapUACurveAxisDefinition
                                    FapUAGraphDefine FapUAGraphView FapUAGraphViewTLS
                                    FapUARDBMEFatigue )
endif ( Qwt_LIBRARY )

if ( USE_WINDPOW )
  list ( APPEND COMPONENT_FILE_LIST FapUAAirEnvironment
                                    FapUACreateTurbineAssembly
                                    FapUACreateTurbineTower )
endif ( USE_WINDPOW )

if ( USE_EXT_CTRLSYS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_EXTCTRL" )
  list ( APPEND COMPONENT_FILE_LIST FapUAExtCtrlSysProperties )
endif ( USE_EXT_CTRLSYS )

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FapUAQueryMatch )
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


set ( DEPENDENCY_LIST vpmPM vpmUI vpmUITopLevels vpmUIComponents
                      vpmApp vpmAppCmds vpmAppProcess vpmAppDisplay
                      vpmDB FFrLib )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
