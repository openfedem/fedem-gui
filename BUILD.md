<!---
  SPDX-FileCopyrightText: 2023 SAP SE

  SPDX-License-Identifier: Apache-2.0

  This file is part of FEDEM - https://openfedem.org
--->

# FEDEM GUI Build

Currently, we support building on Windows only,
using Microsoft Visual Studio 2019 or later,
and with Intel&reg; Fortran Compilers for the Fortran modules.
The build system is based on the [CMake](https://cmake.org/) tool,
which is embedded in the Visual Studio 2019 installation.

## Build of external 3rd-party modules

Before you can build the FEDEM GUI itself, you first need to build and install
some 3rd-party open source components which are not included in this repository.
Detailed instructions on how to do that for each package, is provided below.

### Qt

The [Qt framework](https://www.qt.io/product/framework) is a huge package,
and FEDEM only uses a small part of it. The following description therefore
focuses on the necessary steps to build only what is needed for FEDEM usage.

The current version of FEDEM still uses
[Qt 4.8](https://doc.qt.io/archives/qt-4.8/index.html).
However, the work of porting to Qt 6 has started,
and these instructions will be updated when that effort is completed.

To build and install the Qt components needed in FEDEM on Windows,
proceed as follows:

- Download the open source version of
  [Qt 4.8.7](https://download.qt.io/archive/qt/4.8/4.8.7/).
  Choose the file `qt-everywhere-opensource-src-4.8.7.zip`
  (or the equivalent `.tar.gz` file).

- Unzip the sources in some arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\qt-everywhere-opensource-src-4.8.7`.
  To save time (and disk space), you can unzip only the sources
  (skipping the documentation, demos, examples, etc.).
  Using the `unzip` tool in a bash shell, that is:

      mkdir ~/Fedem-src
      cd ~/Fedem-src
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/*'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/configure.*'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/bin/**'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/include/**'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/lib/**'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/mkspecs/**'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/qmake/**'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/src/**'
      unzip ~/Downloads/qt-everywhere-opensource-src-4.8.7.zip 'qt-everywhere-opensource-src-4.8.7/tools/**'

- Open the `x64 Native Tools Command Prompt for VS 2019`
  Windows app to configure Qt:

      cd %USERPROFILE%\Fedem-src\qt-everywhere-opensource-src-4.8.7
      configure -opensource -fast -release -platform win32-msvc2015 ^
      -no-declarative -no-gif -no-multimedia -no-libjpeg -no-libmng -no-libtiff -no-openssl -no-phonon -no-script -no-scripttools -no-webkit -no-xmlpatterns

  You need to press `y` to accept the Open Source licensing terms.
  Then to build the Qt modules, execute:

      nmake

  This will run for several minutes, so go and take a coffee in the meantime..
  After a while, the build process will probably fail due to a compiler error,
  but only after all libraries needed by FEDEM has been successfully built,
  so don't worry about that.

- Install the Qt binaries required for building FEDEM.
  Open a command prompt in administrator mode, then:

      mkdir C:\Qt-4.8.7
      cd C:\Qt-4.8.7
      mkdir bin lib
      cd %USERPROFILE%\Fedem-src\qt-everywhere-opensource-src-4.8.7
      copy bin\qmake.exe C:\Qt-4.8.7\bin\
      copy bin\moc.exe C:\Qt-4.8.7\bin\
      copy bin\rcc.exe C:\Qt-4.8.7\bin\
      copy bin\uic.exe C:\Qt-4.8.7\bin\
      copy lib\qtmain.lib C:\Qt-4.8.7\lib
      copy lib\Qt3Support4.* C:\Qt-4.8.7\lib
      copy lib\QtCore4.* C:\Qt-4.8.7\lib
      copy lib\QtGui4.* C:\Qt-4.8.7\lib
      copy lib\QtNetwork4.* C:\Qt-4.8.7\lib
      copy lib\QtOpenGL4.* C:\Qt-4.8.7\lib
      copy lib\QtSql4.* C:\Qt-4.8.7\lib
      copy lib\QtSvg4.* C:\Qt-4.8.7\lib
      copy lib\QtXml4.* C:\Qt-4.8.7\lib

- Install the Qt header files required for building FEDEM.
  Open a bash shell in administrator mode,
  then run the script (or do the equivalent in DOS):

      #!/bin/bash
      for dir in Qt3Support QtCore QtGui QtNetwork QtOpenGL QtSql; do
        mkdir -p C:/Qt-4.8.7/include/$dir/private
        cd ~/Fedem-src/qt-everywhere-opensource-src-4.8.7/include/$dir
        cp Q* C:/Qt-4.8.7/include/$dir/
        for file in `cat *.h | awk -F'"' '{print$2}'`; do
          cp $file C:/Qt-4.8.7/include/$dir/
        done
        cd private
        for file in `cat *.h | awk -F'"' '{print$2}'`; do
          cp $file C:/Qt-4.8.7/include/$dir/private
        done
      done

### Coin

FEDEM uses the [Coin3D](https://www.coin3d.org/) package,
to implement its *3D Modeler view*.
It consists of several github projects, of which four are used in FEDEM.

To build and install the main component (Coin), proceed as follows:

- Download the sources from [github](https://github.com/coin3d/coin).
  We have been using
  [Coin 4.0.0](https://github.com/coin3d/coin/releases/tag/Coin-4.0.0),
  but recently two updates were released (4.0.1 and 4.0.2), so you may want
  to try out the latest release instead.
  Choose the file `coin-4.0.0-src.zip`
  (or the equivalent `.tar.gz` file) if you go for the 4.0.0 version.

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\Coin-4.0.0`.

- Download the latest [boost](https://www.boost.org/) package.
  Currently, this is
  [boost 1.83.0](https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/).
  Choose the file `boost_1_83_0.zip` (or the equivalent `.tar.gz` file).
  The whole package contains more than 80000 files, but Coin uses only a small
  fraction of it. So to save time (and disk space), you may choose to extract
  only the necessary parts. Using the `unzip` tool in a bash shell, that is:

      cd ~/Fedem-src
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/*.hpp'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/assert/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/concept/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/container/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/config/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/core/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/detail/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/exception/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/iterator/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/lexical_cast/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/move/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/mpl/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/numeric/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/preprocessor/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/range/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/smart_ptr/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/type_traits/**'
      unzip ~/Downloads/boost_1_83_0.zip 'boost_1_83_0/boost/utility/**'

  This will extract less than 3000 files, which is sufficient for building Coin.

- Comment out line 818 in the file `~/Fedem-src/Coin-4.0.0/CMakeLists.txt`
  which adds the sub-directory `cpack.d` to the build, that is:

      818c818
      < add_subdirectory(cpack.d)
      ---
      > #add_subdirectory(cpack.d)

- With Visual Studio 2019, configure the Coin build by executing the following
  commands from a DOS shell (put this in a bat-file for the convenience):

      @echo off
      title Coin configuration
      call "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VS2019INSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 16 2019" ^
      -S %USERPROFILE%\Fedem-src\Coin-4.0.0 ^
      -B %USERPROFILE%\Coin-build\Coin ^
      -DBOOST_ROOT=%USERPROFILE%\Fedem-src\boost_1_83.0 ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4 ^
      -DCOIN_BUILD_TESTS=OFF
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\Coin\Coin.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

### SoQt

This is a coupling module between Coin and Qt,
and can therefore be built only *after* you have installed those two modules.

To build and install SoQt, proceed as follows:

- Download the sources from [github](https://github.com/coin3d/soqt).
  Since we still are building with Qt 4.8, you have to use
  [SoQt 1.6.0](https://github.com/coin3d/soqt/releases/tag/SoQt-1.6.0),
  and not the more recent 1.6.1 release (which contains porting to Qt 5).
  Choose the file `soqt-1.6.0-src.zip` (or the equivalent `.tar.gz` file).

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\SoQt-1.6.0`.

- Comment out lines 392 and 396 in the file
  `~/Fedem-src/SoQt-1.6.0/CMakeLists.txt` which add the sub-directories
  `test-code` and `cpack.d` to the build, that is:

      392c392
      < #add_subdirectory(test-code)
      ---
      > add_subdirectory(test-code)
      396c396
      < #add_subdirectory(cpack.d)
      ---
      > add_subdirectory(cpack.d)

- With Visual Studio 2019, configure the SoQt build by executing the following
  commands from a DOS shell (put this in a bat-file for the convenience):

      @echo off
      title SoQt configuration
      call "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VS2019INSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 16 2019" ^
      -S %USERPROFILE%\Fedem-src\SoQt-1.6.0 ^
      -B %USERPROFILE%\Coin-build\SoQt ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4 ^
      -DCMAKE_PREFIX_PATH=C:\Qt-4.8.7
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\SoQt\SoQt.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

### SmallChange

To build and install SmallChange, proceed as follows:

- Download the sources from [github](https://github.com/coin3d/smallchange).
  Use the latest (and so far the only) release
  [SmallChange 1.0.0](https://github.com/coin3d/smallchange/releases/tag/smallchange-1.0.0),
  and choose the file `smallchange-1.0.0-src.zip`
  (or the equivalent `.tar.gz` file).

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\SmallChange-1.0.0`.

- Comment out lines 186, 187 and 228 in the file
  `~/Fedem-src/SmallChange-1.0.0/CMakeLists.txt` which add the sub-directories
  `test-code`, `docs` and `cpack.d` to the build, that is:

      186,187c186,187
      < #add_subdirectory(test-code)
      < #add_subdirectory(docs)
      ---
      > add_subdirectory(test-code)
      > add_subdirectory(docs)
      228c228
      < #add_subdirectory(cpack.d)
      ---
      > add_subdirectory(cpack.d)

- With Visual Studio 2019, configure the SmallChange build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title SmallChange configuration
      call "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VS2019INSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 16 2019" ^
      -S %USERPROFILE%\Fedem-src\SmallChange-1.0.0 ^
      -B %USERPROFILE%\Coin-build\SmallChange ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4 ^
      -DCMAKE_PREFIX_PATH=C:\Qt-4.8.7
      pause

- Open the generated solution file
  `%USERPROFILE%\Coin-build\SoQt\SmallChange.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

### Simage

This library is only used for exporting images and animations from FEDEM. If
those features are not important, you may skip the following installation steps:

- Download the sources from [github](https://github.com/coin3d/simage).
  Use the latest release
  [Simage 1.8.3](https://github.com/coin3d/simage/releases/tag/v1.8.3),
  and choose the file `simage-1.8.3-src.zip` (or the equivalent `.tar.gz` file).

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\Simage-1.8.3`.

- With Visual Studio 2019, configure the Simage build by executing the following
  commands from a DOS shell (put this in a bat-file for the convenience):

      @echo off
      title Simage configuration
      call "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VS2019INSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 16 2019" ^
      -S %USERPROFILE%\Fedem-src\Simage-1.8.3 ^
      -B %USERPROFILE%\Coin-build\Simage ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4 ^
      -DSIMAGE_LIBSNDFILE_SUPPORT=OFF ^
      -DSIMAGE_OGGVORBIS_SUPPORT=OFF ^
      -DSIMAGE_BUILD_EXAMPLES=OFF ^
      -DSIMAGE_BUILD_TESTS=OFF
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\Simage\simage.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

- Move the `simage.h` header file into right place:

      cd C:\Coin-4.0.0\include
      mkdir Simage
      move simage.h Simage

### Zlib

The [zlib](https://zlib.net/) package is needed only if you activate building
with Digital Twin features, using the command-line option `-DUSE_DIGTWIN=ON`,
as explained [here](README.md#activation-of-features-compile-time). It is used
for exporting models to *FMUs* and *EPD Connected Products* apps. So if you
don't need this feature, you can skip the following installation procedure:

- Download the zlib sources from [github](https://github.com/madler/zlib).
  We have tested this with
  [zlib 1.2.13](https://github.com/madler/zlib/releases/tag/v1.2.13),
  but any newer releases will probably work as well.

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\zlib-1.2.13`

- Append the following to their `CMakeLists.txt` file:

      #=========================================================================
      # minizip
      #=========================================================================
      set(ZIP_PUBLIC_HDRS
          contrib/minizip/zip.h
          contrib/minizip/ioapi.h
          contrib/minizip/iowin32.h
      )
      set(ZIP_PRIVATE_HDRS
          contrib/minizip/crypt.h
      )
      set(ZIP_SRCS
          contrib/minizip/zip.c
          contrib/minizip/ioapi.c
          contrib/minizip/iowin32.c
      )
      message(STATUS "Adding zip library")
      add_library(zip STATIC ${ZIP_SRCS} ${ZIP_PUBLIC_HDRS} ${ZIP_PRIVATE_HDRS})
      if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
        install(TARGETS zip
                RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
                ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
                LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
      endif()
      if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
        install(FILES ${ZIP_PUBLIC_HDRS} DESTINATION "${INSTALL_INC_DIR}")
      endif()

  This will configure for building the `zip` library based on the `minizip`
  sources, in addition to the `zlib` library itself, when `cmake` is executed.

- With Visual Studio 2019, configure the zlib build by executing the following
  commands from a DOS shell (put this in a bat-file for the convenience):

      @echo off
      title zlib configuration
      call "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VS2019INSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 16 2019" ^
      -S %USERPROFILE%\Fedem-src\zlib-1.2.13 ^
      -B %USERPROFILE%\zlib-build ^
      -DCMAKE_INSTALL_PREFIX=C:\Zlib
      pause

- Open the generated solution file `%USERPROFILE%\zlib-build\zlib.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

## Build the FEDEM GUI application

With the necessary third-party modules installed,
as described [above](#build-of-external-3rd-party-modules),
it is straight forward to configure and build the FEDEM GUI itself,
Proceed as follows:

- From a bash shell or command prompt, clone the sources of this repository:

      cd ~/Fedem-src
      git clone --recurse-submodules git@github.com:openfedem/fedem-gui.git

- Configure for build in Visual Studio 2019 using `CMake`,
  e.g., execute the following bat script:

      @echo off
      title Fedem GUI configuration
      call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat" intel64 vs2019
      set COIN_ROOT=C:\Coin-4
      set QT_ROOT=C:\Qt-4.8.7
      set PATH=%PATH%;%QT_ROOT%\bin
      set /p VERSION=<%USERPROFILE%\Fedem-src\fedem-gui\cfg\VERSION
      "%VS2019INSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 16 2019" ^
      -S %USERPROFILE%\Fedem-src\fedem-gui ^
      -B %USERPROFILE%\Fedem-build\fedemGUI ^
      -DCMAKE_INSTALL_PREFIX=%USERPROFILE%\Fedem-install\%VERSION% ^
      -DUSE_FORTRAN=ON
      pause

  If you don't have the Intel&reg; Fortran Compilers installed,
  you have to replace the call statement above by

      call "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64

  and remove the option `-DUSE_FORTRAN=ON`.
  It will then configure without Fortran support.
  This implies that the *Previewing of Functions* feature will not be available
  in your build of FEDEM GUI. You can also specify any of the options listed
  [here](README.md#activation-of-features-compile-time) in a similar way,
  to tailor your build further.

- Open the generated solution file
  `%USERPROFILE%\Fedem-build\fedemGUI\fedemGUI.sln`
  in Visual Studio and build the `Fedem` target for `Release` configuration
  to compile all source files.

  Build the `INSTALL` target to install the resulting binaries
  (`.exe` file and dependent `.dll` files) and some other files in the folder
  `${CMAKE_INSTALL_PREFIX}\bin` where `CMAKE_INSTALL_PREFIX` is specified
  on the `cmake` command (see above). The binaries will then be installed
  in a subfolder named after the current version stored in the
  [cfg\VERSION](cfg/VERSION) file.
  Notice that the end-user documentation also will be installed, by downloading
  artifacts from the [fedem-docs](https://github.com/openfedem/fedem-docs) repository.
  Remember to update the `DOC_VER` variable in the [CMakeLists.txt](CMakeLists.txt)
  file if the documentation has been updated, before building this project.

## Build new FEDEM installation

  To build a complete installer for FEDEM with the GUI and solver modules,
  you first need to build the `INSTALL` target of this project
  as described [above](#build-the-fedem-gui-application), as well as for
  the parallel projects [fedem-solvers](https://github.com/openfedem/fedem-solvers)
  and [fedem-mdb](https://github.com/openfedem/fedem-mdb).

  Make sure that all three projects use the same installation destination,
  by setting the `CMAKE_INSTALL_PREFIX` variable to the same value for all
  when running the `cmake` configuration steps.

  When you have completed these tasks, you can build a zip-file installer
  for distribution of the FEDEM package by executing the script
  [Install\createInstaller.bat](Install/createInstaller.bat).
  You may need to edit this script first to specify the version tag and
  location of the compiled binaries (i.e., line 11 and 13):

      10 REM Edit the following line according to the version to create installer for:
      11 SET VERSION=8.0.0
      12 REM All binaries are assumed to have been built and placed in folder %INST_DIR%.
      13 SET INST_DIR=%USERPROFILE%\Fedem-install\3.0.1\bin
