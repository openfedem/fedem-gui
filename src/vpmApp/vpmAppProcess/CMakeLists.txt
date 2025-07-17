# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

# Builds the LIB_ID library
set ( LIB_ID vpmAppProcess )


## Files with header and source with same name
set ( COMPONENT_FILE_LIST  FapDamageRecovery FapDynamicSolver
                           FapGageRecovery FapLinkReducer FapLinkSolver
                           FapModeShapeExpander
                           FapRecoveryBase FapSimEventHandler FapSolutionProcessMgr
                           FapSolverBase FapStressExpander
    )

## Pure header files, i.e., header files without a corresponding source file
set ( HEADER_FILE_LIST FapSolverID )
## Pure implementation files, i.e., source files without corresponding header
set ( SOURCE_FILE_LIST )


foreach ( FILE ${COMPONENT_FILE_LIST} )
  list ( APPEND CPP_SOURCE_FILES ${FILE}.C )
  list ( APPEND HPP_HEADER_FILES ${FILE}.H )
endforeach ( FILE ${COMPONENT_FILE_LIST} )


foreach ( FILE ${HEADER_FILE_LIST} )
  list ( APPEND HPP_HEADER_FILES ${FILE}.H )
endforeach ( FILE ${HEADER_FILE_LIST} )


foreach ( FILE ${SOURCE_FILE_LIST} )
  list ( APPEND CPP_SOURCE_FILES ${FILE}.C )
endforeach ( FILE ${SOURCE_FILE_LIST} )


set ( DEPENDENCY_LIST vpmPM vpmDB vpmApp FFaCmdLineArg )

message ( STATUS "Building library ${LIB_ID}" )
add_library ( ${LIB_ID} ${CPP_SOURCE_FILES} ${HPP_HEADER_FILES} )
target_link_libraries ( ${LIB_ID} ${DEPENDENCY_LIST} )
