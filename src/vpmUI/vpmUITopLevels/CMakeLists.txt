# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmUITopLevels )

if ( USE_SOLVERS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_SOLVERS" )
endif ( USE_SOLVERS )
if ( USE_FREQDOMAIN )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_FREQDOMAIN" )
endif ( USE_FREQDOMAIN )

add_subdirectory ( vpmUIQtTopLevels )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FuiAdvAnalysisOptions FuiAnimationControl FuiAnimExportSetup
                           FuiAppearance FuiCtrlModeller FuiCtrlGridAttributes
                           FuiCSSelector
                           FuiEigenOptions FuiFppOptions FuiGageOptions
                           FuiLinkRamSettings
                           FuiMainWindow FuiMiniFileBrowser FuiModeller
                           FuiModelPreferences FuiOutputList
                           FuiPlugins FuiPreferences FuiProperties FuiRDBSelector
                           FuiRepeatCurve FuiStressOptions FuiViewSettings
                           FuiSeaEnvironment FuiEventDefinition
                           FuiSplitBeam FuiCreateBeamstringPair FuiObjectBrowser
                           FuiTopLevelDialog FuiMooringLine FuiModelExport )

if ( USE_WINDPOW )
  list ( APPEND COMPONENT_FILE_LIST FuiAirEnvironment FuiAirfoilDefinition FuiBladeDefinition
                                    FuiCreateTurbineAssembly FuiCreateTurbineTower FuiTurbWind )
endif ( USE_WINDPOW )

if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FuiGraphView FuiRDBMEFatigue )
endif ( Qwt_LIBRARY )

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FuiModelManager )
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

set ( FT_KERNEL_LIBRARIES vpmDB )
set ( FT_COMMON_LIBRARIES FFaAlgebra FFaCmdLineArg FFaOS FFaString )
set ( FT_GUI_LIBRARIES vpmPM vpmApp vpmUI vpmUIComponents FFuLib vpmUIQtTopLevels )

set ( DEPENDENCY_LIST ${FT_GUI_LIBRARIES} ${FT_KERNEL_LIBRARIES} ${FT_COMMON_LIBRARIES} )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
