# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Build setup

include_directories ( "${CMAKE_CURRENT_SOURCE_DIR}" "${MDB_DIR}"
                      "${MDB_DIR}/fedem-foundation/src" )

set ( CXX_FLAGS "-DFFR_LISTVIEW -DFT_USE_VERTEX" )
if ( WIN )
  set ( CXX_FLAGS "/bigobj ${CXX_FLAGS}" )
  if ( PLATFORM_BITSIZE_IS_64_BIT )
    set ( LINKER_FLAGS "/STACK:4000000" )
  endif ( PLATFORM_BITSIZE_IS_64_BIT )
endif ( WIN )
if ( USE_PROFILER )
  string ( APPEND CXX_FLAGS " -DFT_USE_PROFILER" )
endif ( USE_PROFILER )

if ( WIN )
  find_library ( Coin_library coin4 $ENV{COIN_ROOT}/lib )
elseif ( DEFINED ENV{COIN_ROOT} )
  find_library ( Coin_library Coin $ENV{COIN_ROOT}/lib )
else ( WIN )
  find_library ( Coin_library Coin )
endif ( WIN )

if ( Coin_library )
  message ( STATUS "Found Coin library: ${Coin_library}" )
  string ( APPEND CXX_FLAGS " -DUSE_INVENTOR" )
else ( Coin_library )
  message ( STATUS "Coin library not available, configuring without 3D view" )
endif ( Coin_library )

set ( USE_QWTLIB "OFF" CACHE STRING "Build Fedem GUI with Qwt curve plotting" )
set_property ( CACHE USE_QWTLIB PROPERTY STRINGS Internal External OFF )
if ( "${USE_QWTLIB}" STREQUAL "External" )
  find_package ( Qwt )
elseif ( "${USE_QWTLIB}" STREQUAL "Internal" )
  set ( Qwt_LIBRARY TRUE )
elseif ( "${USE_QWTLIB}" STREQUAL "ON" )
  set ( USE_QWTLIB "Internal" )
  set ( Qwt_LIBRARY TRUE )
endif ( "${USE_QWTLIB}" STREQUAL "External" )

#
# List all subfolders from the fedem-foundation repository in this project
#

set ( USE_QT Qt5 )
set ( USE_VISUALS TRUE )
set ( USE_VERTEXOBJ TRUE )
set ( USE_REFCOUNT TRUE )
set ( USE_CONNECTORS TRUE )
set ( USE_CMDLINEARG TRUE )
set ( SUBFOLDER_LIST Admin FFaLib FFlLib FFrLib
                     FFlrLib FFaFunctionLib FFaMathExpr
                     FiDeviceFunctions FiUserElmPlugin )

if ( Qwt_LIBRARY )
  list ( INSERT SUBFOLDER_LIST 4 FFpLib )
else ( Qwt_LIBRARY )
  set ( NO_FATIGUE TRUE )
endif ( Qwt_LIBRARY )

foreach ( FOLDER ${SUBFOLDER_LIST} )
  add_subdirectory ( ${MDB_DIR}/fedem-foundation/src/${FOLDER}
                     "${CMAKE_CURRENT_BINARY_DIR}/fedem-foundation/${FOLDER}" )
endforeach ( FOLDER ${SUBFOLDER_LIST} )


if ( WIN )

# Check if the COM-API should be included
# or if the program should be run with console window
  option ( USE_COMAPI "Build Fedem with the COM-API" ON )
  option ( NO_CONSOLE_WINDOW "Build fedem without console window" ON )
  mark_as_advanced ( USE_COMAPI NO_CONSOLE_WINDOW )

  if ( USE_COMAPI )
    file ( GLOB COM_FILES comApi/*.idl comApi/*.cpp comApi/*.h )
    if ( NOT Qwt_LIBRARY )
      list ( REMOVE_ITEM COM_FILES
             ${CMAKE_CURRENT_SOURCE_DIR}/comApi/CaResultExtractor.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/comApi/CaResultExtractor.h )
    endif ( NOT Qwt_LIBRARY )
    include_directories ( "${CMAKE_CURRENT_BINARY_DIR}/$(IntDir)" )
    string ( APPEND CXX_FLAGS " -DFT_HAS_COM" )
#   Use "/subsystem:console" to avoid AfxWinMain when building with COM-API
    string ( APPEND LINKER_FLAGS " /subsystem:console" )
  elseif ( NO_CONSOLE_WINDOW )
    string ( APPEND LINKER_FLAGS " /subsystem:windows /entry:mainCRTStartup" )
  endif ( USE_COMAPI )

# Resource file for the executable
  set ( APP_ICON "vpmUI/Icons/win32AppIcon/fedem_app_icon.rc" )
endif ( WIN )

if ( Qwt_LIBRARY )
  string ( APPEND CXX_FLAGS " -DFT_HAS_GRAPHVIEW" )
  if ( "${USE_QWTLIB}" STREQUAL "Internal" )
    include_directories ( "${CMAKE_CURRENT_SOURCE_DIR}/Qwt/src" )
  endif ( "${USE_QWTLIB}" STREQUAL "Internal" )
elseif ( NOT "${USE_QWTLIB}" STREQUAL "OFF" )
  message ( STATUS "Qwt library not available, configuring without graph view" )
  set ( USE_QWTLIB "OFF" )
endif ( Qwt_LIBRARY )

string ( APPEND CMAKE_CXX_FLAGS " ${CXX_FLAGS}" )
string ( APPEND CMAKE_EXE_LINKER_FLAGS " ${LINKER_FLAGS}" )

#
# All folders to use in the build process
#
set ( SUBFOLDER_LIST FFuLib vpmUI vpmApp vpmPM )
if ( Coin_library )
  list ( APPEND SUBFOLDER_LIST vpmDisplay FFdCadModel )
endif ( Coin_library )
if ( "${USE_QWTLIB}" STREQUAL "Internal" )
  list ( INSERT SUBFOLDER_LIST 0 Qwt )
endif ( "${USE_QWTLIB}" STREQUAL "Internal" )

foreach ( FOLDER ${SUBFOLDER_LIST} )
  add_subdirectory ( ${FOLDER} )
endforeach ( FOLDER ${SUBFOLDER_LIST} )

if ( USE_FORTRAN AND USE_CHSHAPE )
  add_subdirectory ( ${MDB_DIR}/chainShape "${CMAKE_CURRENT_BINARY_DIR}/chainShape" )
endif ( USE_FORTRAN AND USE_CHSHAPE )

add_subdirectory ( ${MDB_DIR}/vpmDB "${CMAKE_CURRENT_BINARY_DIR}/vpmDB" )

#
# Collect all internal and external libraries
#
set ( DEPENDENCY_LIST vpmUI vpmPM )
if ( Coin_library )
  list ( APPEND DEPENDENCY_LIST vpmDisplay )
endif ( Coin_library )
list ( APPEND DEPENDENCY_LIST vpmDB FiUserElmPlugin FFaOperation )

message ( STATUS "Building executable Fedem" )
string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_CONNECTORS" )
add_executable ( Fedem WIN32 vpm_main.C vpm_main_init.C ${COM_FILES} ${APP_ICON} )
target_link_libraries ( Fedem ${DEPENDENCY_LIST} )

#
# Install the Fedem binary
#
if ( FTENV_VERBOSE )
  message ( STATUS "Installing Fedem to ${CMAKE_INSTALL_PREFIX}" )
endif ( FTENV_VERBOSE )
install ( TARGETS Fedem RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
