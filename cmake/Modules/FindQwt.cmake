# SPDX-FileCopyrightText: 2023 SAP SE
#
# SPDX-License-Identifier: Apache-2.0
#
# This file is part of FEDEM - https://openfedem.org

unset ( Qwt_INCLUDE CACHE )
unset ( Qwt_LIBRARY CACHE )

find_path ( Qwt_INCLUDE
            NAMES qwt.h
            PATHS /usr/local/qwt-6.3.0-dev/include C:/Qwt-6.3.0-dev/include
          )

if ( Qwt_INCLUDE )
  message ( STATUS "Found Qwt: ${Qwt_INCLUDE}" )

  find_library ( Qwt_LIBRARY
                 NAMES qwt
                 PATHS /usr/local/qwt-6.3.0-dev/lib C:/Qwt-6.3.0-dev/lib
               )

endif ( Qwt_INCLUDE )

if ( Qwt_LIBRARY )
  message ( STATUS "Found Qwt: ${Qwt_LIBRARY}" )
  include_directories ( ${Qwt_INCLUDE} )
elseif ( Qwt_FIND_REQUIRED )
  message ( WARNING "Required package Qwt not found." )
  message ( FATAL_ERROR "Fix this and try again." )
else ( Qwt_LIBRRY )
  message ( WARNING "Did NOT find Qwt, configuring without it." )
endif ( Qwt_LIBRARY )
