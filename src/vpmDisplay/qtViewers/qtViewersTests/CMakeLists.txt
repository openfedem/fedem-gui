# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

set ( LIB_ID qtViewersTests )

if ( WIN )
  string ( APPEND CMAKE_CXX_FLAGS " -DCOIN_DLL -DSOQT_DLL" )
  string ( REGEX REPLACE ".subsystem:console" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" )
endif ( WIN )
string ( APPEND CMAKE_CXX_FLAGS " -DNO_FFU" )

if ( DEFINED ENV{COIN_ROOT} )
  include_directories ( "$ENV{COIN_ROOT}/include" )
endif ( DEFINED ENV{COIN_ROOT} )

message ( STATUS "Building executable ${LIB_ID}" )

add_executable ( ${LIB_ID} WIN32 viewerTest.C
                 ../FdQtViewer.C ../../FdMultiplyTransforms.C
                 ../FdQtViewer.H ../../FdMultiplyTransforms.H
               )

target_link_libraries ( ${LIB_ID} ${Coin_library} ${SoQt_library} Qt6::Widgets Qt6::OpenGL ${DSO_LIBGL} )
if ( LINUX )
  target_link_libraries ( ${LIB_ID} -lX11 )
endif ( LINUX )
