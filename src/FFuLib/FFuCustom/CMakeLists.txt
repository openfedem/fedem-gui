# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

find_package ( Qt5 REQUIRED COMPONENTS Gui Widgets Core )

set ( SUBFOLDER_LIST components )

if ( USE_WINDPOW )
  list ( APPEND SUBFOLDER_LIST inputTables mvcModels )
endif ( USE_WINDPOW )

foreach ( FOLDER ${SUBFOLDER_LIST} )
  add_subdirectory ( ${FOLDER} )
endforeach ( FOLDER ${SUBFOLDER_LIST} )
