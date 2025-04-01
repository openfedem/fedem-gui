# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID FFuQtComponents )


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
                           FFuQtTableView
                           FFuQtToggleButton
                           FFuQtToolBar
                           FFuQtToolButton
                           FFuQtUserDialog
                           FFuQtDropDownBox
                           FFuQ3ScrolledList
)
if ( Qwt_LIBRARY )
  list ( APPEND COMPONENT_FILE_LIST FFuQt2DPlotter )
endif ( Qwt_LIBRARY )

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
                               FFuQtTableView
                               FFuQtToggleButton
                               FFuQtToolButton
                               FFuQtLabel
                               FFuQ3ScrolledList
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
QT4_WRAP_CPP ( QT_MOC_SOURCE_FILES ${QT_MOC_HEADER_FILES} )


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
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

set ( DEPENDENCY_LIST FFuLib )
if ( TARGET Qwt )
  list ( APPEND DEPENDENCY_LIST Qwt )
elseif ( Qwt_LIBRARY )
  list ( APPEND DEPENDENCY_LIST ${Qwt_LIBRARY} )
  if ( WIN )
    string ( APPEND CMAKE_CXX_FLAGS " -DQWT_DLL" )
  endif ( WIN )
endif ( TARGET Qwt )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${SOURCE_FILES} ${HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
