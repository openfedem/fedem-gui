# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID FFuLib )


set ( SUBFOLDER_LIST FFuAuxClasses FFuBase FFuQtBaseClasses
                     FFuQtComponents FFuCustom
    )

foreach ( FOLDER ${SUBFOLDER_LIST} )
  add_subdirectory ( ${FOLDER} )
endforeach ( FOLDER ${SUBFOLDER_LIST} )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FFuLogicButtonGroup
                           FFuListView
                           FFuColorSelector
                           FFuMenuBase
                           FFuColorDialog
                           FFuIOField
                           FFuOptionMenu
                           FFuScrolledListDialog
                           FFuDialogButtons
                           FFuLabelField
                           FFuFileBrowseField
                           FFuFileDialog
                           FFuListViewItem
                           FFuButton
                           FFuUserDialog
                           FFuTable
)

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST     FFu2DPlotter
                           FFuColor
                           FFuColorChooser
                           FFuFrame
                           FFuLabel
                           FFuLabelFrame
                           FFuMemo
                           FFuMenuBar
                           FFuPopUpMenu
                           FFuProgressDialog
                           FFuPushButton
                           FFuRadioButton
                           FFuScale
                           FFuScrolledList
                           FFuSpinBox
                           FFuTabbedWidgetStack
                           FFuTableView
                           FFuToggleButton
                           FFuToolBar
                           FFuToolButton
)

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
set ( DEPENDENCY_LIST FFuQtComponents FFuQtBaseClasses FFuBase FFuAuxClasses FFaString )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
