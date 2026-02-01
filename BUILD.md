<!---
  SPDX-FileCopyrightText: 2023 SAP SE

  SPDX-License-Identifier: Apache-2.0

  This file is part of FEDEM - https://openfedem.org
--->

# FEDEM GUI Build

Currently, we support building on Windows
using Microsoft Visual Studio 2022 or later,
and with Intel&reg; Fortran Compilers for the Fortran modules.
The build system is based on the [CMake](https://cmake.org/) tool,
which is embedded in the Visual Studio 2022 installation.

The GUI application can also be built on Linux, using the description
shown [below](#build-fedem-gui-application-on-linux).

## Build of external 3rd-party modules

Before you can build the FEDEM GUI itself, you first need to build and install
some 3rd-party open source components which are not included in this repository.
Detailed instructions on how to do that for each package, is provided below.

### Qt

The [Qt framework](https://www.qt.io/product/framework) is a huge package,
and FEDEM only uses a small part of it. The following description therefore
focuses on the necessary steps to build only what is needed for FEDEM usage.

To build and install the components needed by FEDEM on Windows,
proceed as follows. The description below assumes you're using version 6.8.2
for which the FEDEM build process has been tested, but it may also apply
for later versions with minor modifications.

- Clone the top-level Qt module from the git repository:

      cd ~/Fedem-src
      git clone git://code.qt.io/qt/qt5.git Qt6

- Checkout the 6.8.2 branch:

      cd Qt6
      git checkout 6.8.2

- FEDEM only requires the `qtbase` submodule of Qt,
  so to clone the corresponding branch for that:

      ./init-repository --branch --module-subset=qtbase

- With Visual Studio 2022 Community, configure the Qt build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title Qt6 configuration
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      mkdir %USERPROFILE%\Qt-build
      mkdir %USERPROFILE%\Qt-build\qtbase
      cd %USERPROFILE%\Qt-build\qtbase
      call %USERPROFILE%\Fedem-src\Qt6\qtbase\configure.bat ^
      -top-level -release -prefix C:\Qt-6.8.2 ^
      -nomake examples -nomake tests ^
      -no-dbus -ssl -no-openssl -no-harfbuzz -no-gif -no-libjpeg ^
      -no-freetype -qt-zlib
      pause

- Build and install Qt:

      cd %USERPROFILE%\Qt-build
      cmake --build . --parallel
      cmake --install .

- Edit the file `C:\Qt-6.8.2\lib\cmake\Qt6Core\Qt6CoreMacros.cmake` to make it accept `.H`
  as extensions for header files:

      230c230
      <         set(HEADER_REGEX "(h|hh|h\\+\\+|hm|hpp|hxx|in|txx|inl)$")
      ---
      >         set(HEADER_REGEX "(H|h|hh|h\\+\\+|hm|hpp|hxx|in|txx|inl)$")

### Coin

FEDEM uses the [Coin3D](https://www.coin3d.org/) package,
to implement its *3D Modeler view*.
It consists of several github projects, of which four are used in FEDEM.

To build and install the main component (Coin), proceed as follows:

- Download the sources from [github](https://github.com/coin3d/coin).
  We have been using the release
  [Coin 4.0.3](https://github.com/coin3d/coin/releases/tag/v4.0.3).
  Choose the file `coin-4.0.3-src.zip`.

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\Coin-4.0.3`.

- Download the latest [boost](https://www.boost.org/) package.
  Currently, this is
  [boost 1.87.0](https://www.boost.org/users/history/version_1_87_0.html).
  Choose the file `boost_1_87_0.zip`.
  The whole package contains more than 80000 files, but Coin uses only a small
  fraction of it. So to save time (and disk space), you may choose to extract
  only the necessary parts. Using the `unzip` tool in a bash shell, that is:

      cd ~/Fedem-src
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/*.hpp'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/assert/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/concept/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/container/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/config/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/core/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/detail/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/exception/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/iterator/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/lexical_cast/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/move/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/mpl/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/numeric/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/preprocessor/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/range/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/smart_ptr/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/type_traits/**'
      unzip ~/Downloads/boost_1_87_0.zip 'boost_1_87_0/boost/utility/**'

  This will extract less than 3000 files, which is sufficient for building Coin.

- With Visual Studio 2022 Community, configure the Coin build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title Coin configuration
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 17 2022" ^
      -S %USERPROFILE%\Fedem-src\Coin-4.0.3 ^
      -B %USERPROFILE%\Coin-build\Coin4 ^
      -DBOOST_ROOT=%USERPROFILE%\Fedem-src\boost_1_87.0 ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4.0.3 ^
      -DCOIN_BUILD_TESTS=OFF -DHAVE_SOUND_OFF
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\Coin4\Coin.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

### SoQt

This is a coupling module between Coin and Qt,
and can therefore be built only *after* you have installed those two modules.

To build and install SoQt, proceed as follows:

- Download the sources from [github](https://github.com/coin3d/soqt).
  Use the latest release [SoQt 1.6.3](https://github.com/coin3d/soqt/releases/tag/v1.6.3),
  and choose the file `soqt-1.6.3-src.zip`.

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\SoQt-1.6.3`.

- With Visual Studio 2022 Community, configure the SoQt build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title SoQt configuration
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 17 2022" ^
      -S %USERPROFILE%\Fedem-src\SoQt-1.6.3 ^
      -B %USERPROFILE%\Coin-build\SoQt1 ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4.0.3 ^
      -DCMAKE_PREFIX_PATH=C:\Qt-6.8.2 ^
      -DSOQT_BUILD_TESTS=OFF
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\SoQt\SoQt.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

### SmallChange

The lastest release on github is not compatible with Qt 6.
Therefore, we need to build this package from HEAD of the master branch.
To do this proceeed as follows:

- Clone the sources from the github repository:

      cd ~/Fedem-src
      git clone git@github.com:coin3d/smallchange.git SmallChange

- With Visual Studio 2022 Community, configure the SmallChange build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title SmallChange configuration
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 17 2022" ^
      -S %USERPROFILE%\Fedem-src\SmallChange ^
      -B %USERPROFILE%\Coin-build\SmallChange1 ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4.0.3 ^
      -DCMAKE_PREFIX_PATH=C:\Qt-6.8.2
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\SoQt\SmallChange.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

### Simage

This library is only used for exporting images and animations from FEDEM. If
those features are not important, you may skip the following installation steps:

- Download the sources from [github](https://github.com/coin3d/simage).
  Use the latest release
  [Simage 1.8.3](https://github.com/coin3d/simage/releases/tag/v1.8.3),
  and choose the file `simage-1.8.3-src.zip`.

- Unzip the sources in arbitrary location,
  e.g., `%USERPROFILE%\Fedem-src\Simage-1.8.3`.

- With Visual Studio 2022 Community, configure the Simage build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title Simage configuration
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 17 2022" ^
      -S %USERPROFILE%\Fedem-src\Simage-1.8.3 ^
      -B %USERPROFILE%\Coin-build\Simage1 ^
      -DCMAKE_INSTALL_PREFIX=C:\Coin-4.0.3 ^
      -DSIMAGE_LIBSNDFILE_SUPPORT=OFF ^
      -DSIMAGE_OGGVORBIS_SUPPORT=OFF ^
      -DSIMAGE_BUILD_EXAMPLES=OFF ^
      -DSIMAGE_BUILD_TESTS=OFF
      pause

- Open the generated solution file `%USERPROFILE%\Coin-build\Simage\simage.sln`
  in Visual Studio and build the `INSTALL` target for `Release` configuration.

- Move the `simage.h` header file into right place:

      cd C:\Coin-4.0.3\include
      mkdir Simage
      move simage.h Simage

### Qwt

  This library implements the graph view widget which is used for doing the
  curve plotting in the FEDEM GUI. It is needed only if want to build FEDEM
  with the curve plotting feature, which is activated by the cmake
  command-line option `-DUSE_QWTLIB=External`
  (see [here](README.md#activation-of-features-compile-time)).

  To build and install the Qwt library, proceed as follows:

- Clone the sources from the git repository:

      cd ~/Fedem-src
      git clone https://git.code.sf.net/p/qwt/git Qwt
      git checkout v6.3.0

- To reduce the size of the build, some features not used in FEDEM
  may be deactivated by commenting out the following lines in the file
  [qwtconfig.pri](https://sourceforge.net/p/qwt/git/ci/develop/tree/qwtconfig.pri):

      89c89
      < QWT_CONFIG       += QwtPolar
      ---
      > #QWT_CONFIG       += QwtPolar
      103c103
      < QWT_CONFIG     += QwtSvg
      ---
      > #QWT_CONFIG     += QwtSvg
      117c117
      < QWT_CONFIG     += QwtDesigner
      ---
      > #QWT_CONFIG     += QwtDesigner
      140c140
      < QWT_CONFIG     += QwtExamples
      ---
      > #QWT_CONFIG     += QwtExamples
      151c151
      < QWT_CONFIG     += QwtPlayground
      ---
      > #QWT_CONFIG     += QwtPlayground
      158c158
      < QWT_CONFIG     += QwtTests
      ---
      > #QWT_CONFIG     += QwtTests

- With Visual Studio 2022 Community, configure and build Qwt
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title Qwt build
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      mkdir %USERPROFILE%\Qwt-build
      cd %USERPROFILE%\Qwt-build
      C:\Qt-6.8.2\bin\qmake6 %USERPROFILE%\Fedem-src\Qwt\qwt.pro
      REM Deactivate Debug build
      (
        echo all:
        echo install:
      ) > src\Makefile.Debug
      nmake install
      pause

  This will install Qwt in the default location `C:\Qwt-6.3.0-dev`.
  Notice the 5 lines before the `nmake` command overwriting the generated
  `src\Makefile.Debug` file. This is needed in order to build for Release only.
  Otherwise, also a Debug build will be attempted, which will fail unless
  you have installed the Debug version of the Qt libraries.

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
      )
      set(ZIP_PRIVATE_HDRS
          contrib/minizip/crypt.h
      )
      set(ZIP_SRCS
          contrib/minizip/zip.c
          contrib/minizip/ioapi.c
      )
      if(WIN32)
        list(APPEND ZIP_PUBLIC_HDRS contrib/minizip/iowin32.h)
        list(APPEND ZIP_SRC contrib/minizip/iowin32.c)
      endif()
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

- With Visual Studio 2022 Community, configure the zlib build
  by executing the following commands from a DOS shell
  (put this in a bat-file for the convenience):

      @echo off
      title zlib configuration
      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
      "%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 17 2022" ^
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

- Configure for build in Visual Studio 2022 using `CMake`,
  e.g., execute the following bat script:

      @echo off
      title Fedem GUI configuration
      call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat" intel64 vs2022
      set COIN_ROOT=C:\Coin-4.0.3
      set QT_ROOT=C:\Qt-6.8.2
      set /p VERSION=<%USERPROFILE%\Fedem-src\fedem-gui\cfg\VERSION
      "%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
      -G "Visual Studio 17 2022" ^
      -S %USERPROFILE%\Fedem-src\fedem-gui ^
      -B %USERPROFILE%\Fedem-build\fedemGUI ^
      -DCMAKE_INSTALL_PREFIX=%USERPROFILE%\Fedem-install\%VERSION% ^
      -DCMAKE_PREFIX_PATH=%QT_ROOT% ^
      -DUSE_FORTRAN=ON
      pause

  If you don't have the Intel&reg; Fortran Compilers installed,
  you have to replace the call statement above by

      call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

  and remove the option `-DUSE_FORTRAN=ON`.
  It will then configure without Fortran support.
  This implies that the *Previewing of Functions* feature will not be available
  in your build of the FEDEM GUI. You can also specify any of the options listed
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
  The documentation installation can switched off by specifying the cmake option
  `-DINSTALL_ARTIFACTS=OFF`.

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

## Build FEDEM GUI application on Linux

It is possible to configure and build the GUI application on Linux as well.
The following configuration steps have been used so far:

- System: Ubuntu 22.04 with gcc 11.4

- Qt packages: Install Qt 6.2 and dependencies using the package manager

      sudo apt install libqt6core6 libqt6gui6 libqt6widgets6 libqt6opengl6
      sudo apt install libvulkan-dev libxkbcommon-dev

- Coin3D: Download the latest release from github. The binary package
  [coin-4.0.2-Ubuntu2204-gcc11-x64](https://github.com/coin3d/coin/releases/download/v4.0.2/coin-4.0.2-Ubuntu2204-gcc11-x64.tar.gz)
  can be used, unless you want to compile from the sources
  [coin-4.0.2-src](https://github.com/coin3d/coin/releases/download/v4.0.2/coin-4.0.2-src.tar.gz).
  Extract the binary package in arbitrary location, e.g., under `/usr/local/Coin3D`

- SoQt: Download the latest version from github.
  No binary package built with Qt 6 for Ubuntu is available, so here you need to build from the sources
  [soqt-1.6.2-src](https://github.com/coin3d/soqt/releases/download/v1.6.2/soqt-1.6.2-src.tar.gz):

      tar zxfv ~/Downloads/soqt-1.6.2-src.tar.gz
      mkdir soqt/Release
      cd soqt/Release
      cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/Coin3D -DSOQT_BUILD_TESTS=OFF
      make
      sudo make install

  The `CMAKE_INSTALL_PREFIX` variable has to point to the location where you chose to extract the Coin package.

- Smallchange: The lastest release on github is not compatible with Qt 6.
  Therefore, we need to build this package from HEAD of the master branch:

      git clone git@github.com:coin3d/smallchange.git
      mkdir smallchange/Release
      cd smallchange/Release
      cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/Coin3D
      make
      sudo make install

- Qwt (optional): Obtain the sources of this module following the first two steps
  described [above](#qwt) for the Windows build.
  Then configure, build and install the Qwt package through the commands:

      cd Qwt
      qmake6 qwt.pro
      make
      sudo make install

  This will install Qwt in the default location, which is `/usr/local/qwt-6.3.0-dev/`.
  You may use any other location by editing the `QWT_INSTALL_PREFIX` variable in the `qwtconfig.pri` file,
  before running the `qmake6` command.
  In that case, remember to also modify [FindQwt.cmake](cmake/Modules/FindQwt.cmake) before proceeding.

- Zlib (optional): Obtain the sources of this module following the first three steps
  described [above](#zlib) for the Windows build.
  Then configure, build and install the Zlib package through the commands:

      mkdir ~/zlib-1.2.13/Release
      cd ~/zlib-1.2.13/Release
      cmake ..-DCMAKE_BUILD_TYPE=Release -DZLIB_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/usr/local
      make
      sudo make install

- Configure and build the FEDEM GUI:

      git clone --recurse-submodules git@github.com:openfedem/fedem-gui.git
      mkdir fedem-gui/Release fedem-gui/Debug
      cd fedem-gui/Release
      COIN_ROOT=/usr/local/Coin3D cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_QWTLIB=External
      make
      cd ../Debug
      COIN_ROOT=/usr/local/Coin3D cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_QWTLIB=External
      make

  The option `-DUSE_QWTLIB=External` can be omitted if you didn't install the Qwt library
  and chose to build FEDEM without the curve plotting capabilities.
