# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID FFuQtComponents )

find_package ( Qt6 REQUIRED COMPONENTS Core )
if ( Qt6Core_VERSION VERSION_GREATER_EQUAL 6.3 )
  qt_standard_project_setup ()
endif ( Qt6Core_VERSION VERSION_GREATER_EQUAL 6.3 )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FFuQtButton
                           FFuQtColorComboBox
                           FFuQtColorDialog
                           FFuQtColorSelector
                           FFuQtDialogButtons
                           FFuQtFileBrowseField
                           FFuQtFileDialog
                           FFuQtFrame
                           FFuQtIOField
                           FFuQtLabel
                           FFuQtLabelField
                           FFuQtLabelFrame
                           FFuQtListView
                           FFuQtListViewItem
                           FFuQtMemo
                           FFuQtMenuBar
                           FFuQtMenuBase
                           FFuQtOptionMenu
                           FFuQtPopUpMenu
                           FFuQtProgressDialog
                           FFuQtPushButton
                           FFuQtRadioButton
                           FFuQtScale
                           FFuQtScrolledList
                           FFuQtScrolledListDialog
                           FFuQtSelectionDialog
                           FFuQtSpinBox
                           FFuQtSplitter
                           FFuQtTabbedWidgetStack
                           FFuQtTable
                           FFuQtToggleButton
                           FFuQtToolBar
                           FFuQtToolButton
                           FFuQtUserDialog
                           FFuQtDropDownBox
    )

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST )
## Pure implementation files, i.e., source files without corresponding header
set ( SOURCE_FILE_LIST FFuQtPushButton )

## Header files to be moc'ed
set ( QT_MOC_HEADER_FILE_LIST  FFuQtColorComboBox
                               FFuQtIOField
                               FFuQtListView
                               FFuQtMemo
                               FFuQtMenuBar
                               FFuQtOptionMenu
                               FFuQtPopUpMenu
                               FFuQtProgressDialog
                               FFuQtPushButton
                               FFuQtRadioButton
                               FFuQtScale
                               FFuQtScrolledList
                               FFuQtSelectionDialog
                               FFuQtSpinBox
                               FFuQtTabbedWidgetStack
                               FFuQtTable
                               FFuQtToggleButton
                               FFuQtToolButton
                               FFuQtLabel
    )

if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FFuQt2DPlotter )
  list ( APPEND QT_MOC_HEADER_FILE_LIST FFuQt2DPlotter )
endif ( Qwt_LIBRARY )


foreach ( FILE ${QT_MOC_HEADER_FILE_LIST} )
  set ( QT_MOC_HEADER_FILES ${QT_MOC_HEADER_FILES} ${FILE}.H )
endforeach ( FILE ${QT_MOC_HEADER_FILE_LIST} )


include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )
# Named moc_<filename>.cxx where input is filname.H
QT6_WRAP_CPP ( QT_MOC_SOURCE_FILES ${QT_MOC_HEADER_FILES} )


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

if ( WIN )
  string ( APPEND CMAKE_CXX_FLAGS " -D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING" )
endif ( WIN )


#
# Dependencies and system libraries
#

set ( DEPENDENCY_LIST FFuLib )
if ( TARGET Qwt )
  list ( APPEND DEPENDENCY_LIST Qwt )
elseif ( Qwt_LIBRARY )
  list ( APPEND DEPENDENCY_LIST ${Qwt_LIBRARY} )
  if ( WIN )
    string ( REPLACE ".lib" ".dll" QWT_LIBRARY ${Qwt_LIBRARY} )
    string ( APPEND CMAKE_CXX_FLAGS " -DQWT_DLL" )
  else ( WIN )
    set ( QWT_LIBRARY ${Qwt_LIBRARY} )
  endif ( WIN )
  if ( FTENV_VERBOSE )
    message ( STATUS "Installing ${QWT_LIBRARY} to ${CMAKE_INSTALL_PREFIX}" )
  endif ( FTENV_VERBOSE )
  install ( FILES ${QWT_LIBRARY} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
endif ( TARGET Qwt )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${SOURCE_FILES} ${HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
