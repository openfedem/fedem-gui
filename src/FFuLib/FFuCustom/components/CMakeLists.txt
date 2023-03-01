# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

set ( LIB_ID FFuComponents )

if ( USE_WINDPOW )
  file ( GLOB_RECURSE CPP_SOURCE_FILES *.C )
  file ( GLOB_RECURSE HPP_HEADER_FILES *.H )

  set ( QT_MOC_HEADER_FILES
	BladeDrawer.H
	guiComponents/AirfoilSelector.H
	guiComponents/BladeSelector.H
	renderers/AirfoilView.H
	renderers/BladeView.H
      ) ## H files to be moc'ed
else ( USE_WINDPOW )
  set ( CPP_SOURCE_FILES Event.C EventObject.C EventParameter.C MultiEventModel.C
                         ../inputTables/delegates/HeaderDelegate.C )
  set ( HPP_HEADER_FILES Event.H EventObject.H EventParameter.H MultiEventModel.H
                         ../inputTables/delegates/HeaderDelegate.H )
  set ( QT_MOC_HEADER_FILES ../inputTables/delegates/HeaderDelegate.H )
endif ( USE_WINDPOW )

include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )
QT5_WRAP_CPP ( QT_MOC_SOURCE_FILES ${QT_MOC_HEADER_FILES} )

set ( SOURCE_FILES ${CPP_SOURCE_FILES} ${QT_MOC_SOURCE_FILES} )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} FFuAuxClasses Qt5::Widgets Qt5::Core )
