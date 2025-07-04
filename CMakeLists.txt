# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

################################################################################
# This is the top-level cmake project file for the Fedem GUI sources.
################################################################################

cmake_minimum_required ( VERSION 2.8...3.5 )

# Project setup

set ( APPLICATION_ID fedemGui )
set ( DOMAIN_ID FEDEM )
set ( PACKAGE_ID GUI )

option ( USE_SOLVERS "Build Fedem GUI with solver support" ON )
option ( USE_WINDPOW "Build Fedem GUI with Windpower support" OFF )
option ( USE_FORTRAN "Build Fedem GUI with Fortran libraries" OFF )
option ( USE_CHSHAPE "Use ChainShape library for mooring line calculation" OFF )
option ( USE_MEMPOOL "Use memory pool for heap allocation in FE library" ON )
option ( USE_PROFILER "Use CPU and Memory profiler" OFF )
mark_as_advanced ( USE_FORTRAN USE_CHSHAPE USE_MEMPOOL USE_PROFILER )

if ( USE_FORTRAN )
  project ( ${APPLICATION_ID} CXX C Fortran )
else ( USE_FORTRAN )
  project ( ${APPLICATION_ID} CXX C )
endif ( USE_FORTRAN )
message ( STATUS "Generating build project for ${PROJECT_SOURCE_DIR}" )

set ( MDB_DIR ${PROJECT_SOURCE_DIR}/fedem-mdb )

find_path ( _MODULES FedemConfig.cmake
            PATHS $ENV{CMAKE_MODULES}
            "${MDB_DIR}/fedem-foundation/cmake/Modules/"
          )
if ( _MODULES )
  message ( STATUS "NOTE : Using ${_MODULES}" )
  list ( APPEND CMAKE_MODULE_PATH ${_MODULES} )
else ( _MODULES )
  message ( STATUS "ERROR : Missing path to FedemConfig.cmake" )
  message ( FATAL_ERROR "Set environment variable CMAKE_MODULES and try again" )
endif ( _MODULES )
set ( _MODULES "${MDB_DIR}/cmake/Modules" )
if ( IS_DIRECTORY ${_MODULES} )
  message ( STATUS "NOTE : Using ${_MODULES}" )
  list ( APPEND CMAKE_MODULE_PATH ${_MODULES} )
endif ( IS_DIRECTORY ${_MODULES} )
set ( _MODULES "${PROJECT_SOURCE_DIR}/cmake/Modules" )
if ( IS_DIRECTORY ${_MODULES} )
  message ( STATUS "NOTE : Using ${_MODULES}" )
  list ( APPEND CMAKE_MODULE_PATH ${_MODULES} )
endif ( IS_DIRECTORY ${_MODULES} )
unset ( _MODULES CACHE )

include ( FedemConfig )

add_subdirectory ( src )

option ( INSTALL_ARTIFACTS "Install externally built dependent artifacts" ON )
mark_as_advanced ( INSTALL_ARTIFACTS )
if ( INSTALL_ARTIFACTS )

# Install some resource files

file ( GLOB RESOURCES resources/*.f* )
if ( FTENV_VERBOSE )
  message ( STATUS "Installing ${RESOURCES} to ${CMAKE_INSTALL_PREFIX}" )
endif ( FTENV_VERBOSE )
set ( BINARY_DIR "${CMAKE_INSTALL_PREFIX}/bin" )
set ( PROPERTY_DIR "${BINARY_DIR}/Properties" )
install ( FILES ${RESOURCES} DESTINATION "${BINARY_DIR}/resources" )
install ( DIRECTORY Addons DESTINATION "${BINARY_DIR}" )
install ( DIRECTORY Plugin DESTINATION "${BINARY_DIR}" )
install ( DIRECTORY Templates DESTINATION "${BINARY_DIR}" )
install ( DIRECTORY Properties/CrossSections DESTINATION "${PROPERTY_DIR}" )
install ( FILES Properties/README.md DESTINATION "${PROPERTY_DIR}" )

if ( USE_WINDPOW )
  # Install the Fedem Windpower property files
  if ( FTENV_VERBOSE )
    message ( STATUS "Installing Windpower properties to ${PROPERTY_DIR}" )
  endif ( FTENV_VERBOSE )
  file ( GLOB SAMPLES Properties/*.fmm )
  file ( GLOB AERODAT Properties/AeroData/*.* )
  install ( FILES ${SAMPLES} DESTINATION "${PROPERTY_DIR}" )
  install ( FILES ${AERODAT} DESTINATION "${PROPERTY_DIR}/AeroData" )
  foreach ( FILE ${SAMPLES} )
    get_filename_component ( MODEL ${FILE} NAME_WLE )
    install ( FILES ${AERODAT} DESTINATION "${PROPERTY_DIR}/${MODEL}_airfoils" )
  endforeach ( FILE ${SAMPLES} )
endif ( USE_WINDPOW )

# Download and install the latest documentation

set ( DOC_VER fedem-8.0.9 )
set ( DOC_URL https://github.com/openfedem/fedem-docs/releases/download/${DOC_VER} )
set ( DOC_DIR "${CMAKE_CURRENT_BINARY_DIR}/Doc" )
set ( DOC_FILES Fedem.chm FedemUsersGuide.pdf FedemTheoryGuide.pdf )
set ( FEDEM_DOCS )
foreach ( FILE ${DOC_FILES} )
  list ( APPEND FEDEM_DOCS "${DOC_DIR}/${FILE}" )
  if ( FTENV_VERBOSE )
    message ( STATUS "DOWNLOAD ${DOC_URL}/${FILE} --> ${DOC_DIR}/${FILE}" )
  endif ( FTENV_VERBOSE )
  file ( DOWNLOAD ${DOC_URL}/${FILE} "${DOC_DIR}/${FILE}" )
endforeach ( FILE ${DOC_FILES} )
if ( FTENV_VERBOSE )
  message ( STATUS "Installing ${FEDEM_DOCS} to ${BINARY_DIR}/Doc" )
endif ( FTENV_VERBOSE )
install ( FILES ${FEDEM_DOCS} DESTINATION "${BINARY_DIR}/Doc" )

# Download and install the FMU libraries

set ( FMU_VER fmu-8.0.6 )
set ( FMU_URL https://github.com/openfedem/fedem-solvers/releases/download/${FMU_VER} )
set ( FMU_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
set ( FMU_FILES libfedem_fmu.so fedem_fmu.dll )
set ( FEDEM_FMUS )
foreach ( FILE ${FMU_FILES} )
  list ( APPEND FEDEM_FMUS "${FMU_DIR}/${FILE}" )
  if ( FTENV_VERBOSE )
    message ( STATUS "DOWNLOAD ${FMU_URL}/${FILE} --> ${FMU_DIR}/${FILE}" )
  endif ( FTENV_VERBOSE )
  file ( DOWNLOAD ${FMU_URL}/${FILE} "${FMU_DIR}/${FILE}" )
endforeach ( FILE ${FMU_FILES} )
if ( FTENV_VERBOSE )
  message ( STATUS "Installing ${FEDEM_FMUS} to ${BINARY_DIR}/Templates/cloudsim" )
endif ( FTENV_VERBOSE )
install ( FILES ${FEDEM_FMUS} DESTINATION "${BINARY_DIR}/Templates/cloudsim" )

endif ( INSTALL_ARTIFACTS )
