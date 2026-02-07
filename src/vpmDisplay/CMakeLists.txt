# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

set ( LIB_ID vpmDisplay )

# Dependencies and system libraries

if ( WIN )
  find_package ( Qt6 REQUIRED COMPONENTS Widgets Core OpenGL OpenGLWidgets )
else ( WIN )
  find_package ( Qt6 REQUIRED COMPONENTS Widgets Core OpenGL )
endif ( WIN )

if ( WIN )
  set ( SDK_ROOT "C:/Program Files (x86)/Windows Kits/10/Lib" )
  find_library ( DSO_LIBGL OpenGL32 HINTS ${SDK_ROOT}/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/um/x64 )
  find_library ( SmallChange_library SmallChange1 $ENV{COIN_ROOT}/lib )
  find_library ( SoQt_library SoQt1 REQUIRED PATHS $ENV{COIN_ROOT}/lib )
  find_library ( Simage_library simage1 $ENV{COIN_ROOT}/lib )
elseif ( DEFINED ENV{COIN_ROOT} )
  find_library ( DSO_LIBGL GL /usr/lib64 )
  find_library ( SmallChange_library SmallChange $ENV{COIN_ROOT}/lib )
  find_library ( SoQt_library SoQt REQUIRED PATHS $ENV{COIN_ROOT}/lib )
  find_library ( Simage_library simage $ENV{COIN_ROOT}/lib )
else ( WIN )
  find_library ( DSO_LIBGL GL /usr/lib64 )
  find_library ( SmallChange_library SmallChange )
  find_library ( SoQt_library SoQt REQUIRED )
  find_library ( Simage_library simage )
endif ( WIN )

# Include this to test the obj-file parser
#add_subdirectory ( vpmDisplayTests )
# Include this to build the viewer test application
#add_subdirectory ( qtViewers/qtViewersTests )

set ( EXTERNAL_LIBRARIES ${Coin_library} ${SoQt_library} Qt6::OpenGL ${DSO_LIBGL} )

if ( WIN )
  string ( APPEND CMAKE_CXX_FLAGS " -Dwin32 -DCOIN_DLL -DSOQT_DLL" )
endif ( WIN )
if ( DEFINED ENV{COIN_ROOT} )
  include_directories ( "$ENV{COIN_ROOT}/include" )
endif ( DEFINED ENV{COIN_ROOT} )

if ( SmallChange_library )
  message ( STATUS "Found SmallChange library: ${SmallChange_library}" )
  list ( INSERT EXTERNAL_LIBRARIES 2 ${SmallChange_library} )
  string ( APPEND CMAKE_CXX_FLAGS " -DUSE_SMALLCHANGE" )
  if ( WIN )
    string ( APPEND CMAKE_CXX_FLAGS " -DSMALLCHANGE_DLL" )
  endif ( WIN )
else ( SmallChange_library )
  message ( STATUS "SmallChange library not available, configuring without it" )
endif ( SmallChange_library )

if ( Simage_library )
  message ( STATUS "Found Simage library: ${Simage_library}" )
  string ( APPEND CMAKE_CXX_FLAGS " -DUSE_SIMAGE" )
  if ( WIN )
    string ( APPEND CMAKE_CXX_FLAGS " -DSIMAGE_DLL" )
  endif ( WIN )
  list ( APPEND EXTERNAL_LIBRARIES ${Simage_library} )
else ( Simage_library )
  message ( STATUS "Simage library not available, configuring without it" )
  if ( FTENV_WARNINGS AND LINUX )
    string ( APPEND CMAKE_CXX_FLAGS " -Wno-unused-parameter" )
  endif ( FTENV_WARNINGS AND LINUX )
endif ( Simage_library )

if ( USE_MEMPOOL )
  string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_MEMPOOL" )
endif ( USE_MEMPOOL )

string ( APPEND CMAKE_CXX_FLAGS " -DFT_USE_VISUALS" )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FaDOF Fd2dPictureNode Fd2DPoints
                           FdAnimateModel FdAnimationInfo FdAppearanceKit
                           FdAxialSprDa FdAxisCross FdBackPointer
                           FdBase FdBeam FdCamJoint FdCamJointKit FdCtrlDB
                           FdCtrlElement FdCtrlElemKit FdCtrlGrid FdCtrlGridKit FdCtrlKit
                           FdCtrlLine FdCtrlLineKit FdCtrlObject FdCtrlSymbolKit
                           FdCtrlSymDef FdCurveKit FdDB FdDBPointSelectionData
                           FdEvent FdExportIv FdExtraGraphics
                           FdFEGroupPart FdFEGroupPartKit FdFEModel FdFEModelKit
                           FdFEVisControl FdFreeJoint FdHP FdLabelKit
                           FdLinJoint FdLinJointKit
                           FdLink FdLoad FdLoadDirEngine FdLoadTransformKit
                           FdMechanismKit FdMultiplyTransforms FdObjParser FdPart
                           FdPickedPoints FdPickFilter FdPipeSurface FdPipeSurfaceKit
                           FdPrismJoint FdPtPMoveAnimator FdRefPlane FdRefPlaneKit
                           FdRevJoint FdSeaState FdSeaStateKit
                           FdSelector FdSensor FdSimpleJoint FdSimpleJointKit
                           FdSprDaPlacer FdSprDaTransformKit FdSticker FdStrainRosette
                           FdStrainRosetteKit FdSymbolDefs FdSymbolKit
                           FdTire FdTransformKit FdTriad FdTriadSwKit
                           FdUserDefinedElement qtViewers/FdQtViewer
)
## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FdLogoPicture FdMarkerPicture FdAnimatedBase FdConverter FdViewer )
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


# Build the library ${LIB_ID}

set ( DEPENDENCY_LIST vpmUI vpmApp FFdCadModel vpmDB
                      FFlVisualization FFaAlgebra FFaDefinitions FFaGeometry
                      ${EXTERNAL_LIBRARIES} )
if ( LINUX )
  list ( APPEND DEPENDENCY_LIST -lX11 )
endif ( LINUX )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )


# Install the dependent shared object libraries

list ( REMOVE_ITEM EXTERNAL_LIBRARIES ${DSO_LIBGL} )
if ( WIN )
  list ( APPEND EXTERNAL_LIBRARIES Qt6::OpenGLWidgets )
else ( WIN )
  list ( REMOVE_ITEM EXTERNAL_LIBRARIES Qt6::OpenGL )
endif ( WIN )
foreach ( LIB ${EXTERNAL_LIBRARIES} )
  if ( TARGET ${LIB} )
    get_target_property ( LIB_PATH ${LIB} LOCATION )
  else ( TARGET ${LIB} )
    set ( LIB_PATH ${LIB} )
  endif ( TARGET ${LIB} )
  if ( WIN )
    string ( REPLACE "/lib/" "/bin/" DLL_PATH ${LIB_PATH} )
    string ( REPLACE ".lib" ".dll" DLL_PATH ${DLL_PATH} )
    if ( FTENV_VERBOSE )
      message ( STATUS "Installing ${DLL_PATH} to ${CMAKE_INSTALL_PREFIX}" )
    endif ( FTENV_VERBOSE )
    install ( FILES ${DLL_PATH} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
              CONFIGURATIONS Release RelWithDebInfo )
  else ( WIN )
    # On Linux, we need to install all symbolic links
    # (Is there a better way to accomplish this?)
    file ( GLOB LIB_FILES ${LIB_PATH}* )
    if ( FTENV_VERBOSE )
      message ( STATUS "Installing ${LIB_FILES} to ${CMAKE_INSTALL_PREFIX}" )
    endif ( FTENV_VERBOSE )
    install ( FILES ${LIB_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" )
  endif ( WIN )
endforeach ( LIB ${EXTERNAL_LIBRARIES} )
