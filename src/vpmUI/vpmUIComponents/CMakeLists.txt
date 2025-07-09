# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmUIComponents )

if ( USE_FORTRAN )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_PREVIEW" )
endif ( USE_FORTRAN )
if ( USE_FREQDOMAIN )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_FREQDOMAIN" )
endif ( USE_FREQDOMAIN )
if ( USE_EXT_CTRLSYS )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_HAS_EXTCTRL" )
endif ( USE_EXT_CTRLSYS )

add_subdirectory ( vpmUIQtComponents )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  Fui3DPoint FuiAnimationDefine
                           FuiCtrlElemProperties FuiFunctionProperties
                           FuiInputSelector FuiItemsListView FuiItemsListViews
                           FuiJointDOF FuiJointSummary FuiJointTabAdvanced
                           FuiLinkTabs FuiMotionType FuiPlayPanel FuiPointEditor
                           FuiPositionData FuiQueryInputField FuiScaleShiftWidget
                           FuiSimpleLoad FuiSprDaForce FuiSpringChar FuiSpringDefCalc
                           FuiSummaryTable FuiTimeInterval FuiTopologyView
                           FuiTriadDOF FuiTriadSummary FuiVariableType
                           FuiBeamPropSummary FuiBeamPropHydro FuiResultTabs
                           FuiThreshold FuiParameterView FuiWorkSpace
)

if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FuiCurveAxisDefinition FuiCurveDefine
                                    FuiCurveTimeRange FuiScaleShiftWidget
                                    FuiGraphDefine FuiSNCurveSelector )
endif ( Qwt_LIBRARY )

if ( USE_EXT_CTRLSYS )
  list ( APPEND COMPONENT_FILE_LIST FuiExtCtrlSysProperties )
endif ( USE_EXT_CTRLSYS )

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


#
# Dependencies and system libraries
#
set ( FT_KERNEL_LIBRARIES FFaAlgebra )
set ( FT_COMMON_LIBRARIES )
set ( FT_GUI_LIBRARIES vpmUIQtComponents )
set ( DEPENDENCY_LIST ${FT_GUI_LIBRARIES} ${FT_KERNEL_LIBRARIES} ${FT_COMMON_LIBRARIES} )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
