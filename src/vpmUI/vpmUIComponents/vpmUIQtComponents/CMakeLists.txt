# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmUIQtComponents )

find_package ( Qt6 REQUIRED COMPONENTS Core )
if ( Qt6Core_VERSION VERSION_GREATER_EQUAL 6.3 )
  qt_standard_project_setup ()
endif ( Qt6Core_VERSION VERSION_GREATER_EQUAL 6.3 )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FuiQt3DPoint FuiQtAnimationDefine FuiQtDynamicProperties
                           FuiQtCurveAxisDefinition FuiQtCurveDefine
                           FuiQtCtrlElemProperties FuiQtFunctionProperties
                           FuiQtGraphDefine FuiQtInputSelector FuiQtItemsListView
                           FuiQtItemsListViews FuiQtJointDOF FuiQtJointSummary FuiQtJointTabAdvanced
                           FuiQtLinkTabs FuiQtMotionType FuiQtPlayPanel FuiQtPointEditor
                           FuiQtPositionData FuiQtQueryInputField
                           FuiQtSimpleLoad FuiQtSprDaForce FuiQtSpringChar
                           FuiQtSpringDefCalc FuiQtTimeInterval FuiQtTopologyView
                           FuiQtTriadDOF FuiQtTriadSummary FuiQtVariableType FuiQtWorkSpace
                           FuiQtBeamPropSummary FuiQtBeamPropHydro FuiQtResultTabs FuiQtThreshold
)

if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FuiQtCurveAxisDefinition FuiQtCurveDefine
                                    FuiQtGraphDefine FuiQtScaleShiftWidget
				    FuiQtSNCurveSelector )
endif ( Qwt_LIBRARY )

if ( USE_EXT_CTRLSYS )
  list ( APPEND COMPONENT_FILE_LIST FuiQtExtCtrlSysProperties )
endif ( USE_EXT_CTRLSYS )

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST )
## Pure implementation files, i.e., source files without corresponding header
set ( SOURCE_FILE_LIST FuiQtSummaryTable FuiQtParameterView )


## Header files to be moc'ed
set ( QT_MOC_HEADER_FILE_LIST FuiQtItemsListView FuiQtScaleShiftWidget FuiQtWorkSpace )


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
# Dependencies and system libraries
#

set ( FT_GUI_LIBRARIES vpmUIComponents FFuQtComponents FFuQtBaseClasses )
set ( DEPENDENCY_LIST ${FT_GUI_LIBRARIES} )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${SOURCE_FILES} ${HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
