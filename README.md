<!---
  SPDX-FileCopyrightText: 2023 SAP SE

  SPDX-License-Identifier: Apache-2.0

  This file is part of FEDEM - https://openfedem.org
--->

[![REUSE status](https://api.reuse.software/badge/github.com/openfedem/fedem-gui)](https://api.reuse.software/info/github.com/openfedem/fedem-gui)

# FEDEM GUI

![Fedem Logo](https://github.com/openfedem/.github/blob/main/profile/FedemLogo.png "Welcome to FEDEM")

**Welcome to FEDEM! - Finite Element Dynamics in Elastic Mechanisms.**

## About this project

This project contains the complete source code of
the Graphical User Interface of FEDEM. It also uses elements from
the [fedem-mdb](https://github.com/openfedem/fedem-mdb) project,
which is consumed as a submodule by this repository.
See [src/README.md](src/README.md) for details on the source code organization.

The FEDEM solvers themselves are maintained in a parallel project
[fedem-solvers](https://github.com/openfedem/fedem-solvers).

Refer to our web page [openfedem.org](https://openfedem.org/)
for overall information on the FEDEM project.

## Requirements and Setup

The FEDEM GUI project relies on the following third-party modules,
which are not included in this repository:

* [Qt 6.8](https://download.qt.io/archive/qt/6.8/) -
  Cross-platform graphical user interface framework
* [Coin 4.0](https://github.com/coin3d/coin) -
  OpenGL-based 3D graphics library
* [SoQt 1.6](https://github.com/coin3d/soqt) -
  Coupling module between Coin and Qt
* [SmallChange 1.0](https://github.com/coin3d/smallchange) -
  Some Open Inventor extensions for Coin
* [Simage 1.8](https://github.com/coin3d/simage) -
  Library for saving images and animations from the Coin view
* [Qwt 6.3](https://qwt.sourceforge.io/index.html) -
  Framework for 2D curve plotting widgets
* [zlib 1.2](https://zlib.net/) -
  File compression library

You need to build and install these modules separately,
before building the FEDEM GUI application itself,
see [here](BUILD.md#build-of-external-3rd-party-modules) for details.
The first four of the packages above are mandatory,
whereas the latter three are optional.
This means the build will work without them,
but with some features being absent.

Provided you have installed the third-party modules listed above,
you can configure and build the FEDEM GUI from the sources by following
the instructions given [here](BUILD.md#build-the-fedem-gui-application).

### Activation of features compile-time

Many of the features in the FEDEM GUI can be activated/deactivated compile-time
using command-line options on the form `-DUSE_(feature)=ON/OFF`
when running cmake, where `=ON` activates a feature which is off by default,
and `=OFF` deactivates a feature which is on by default.
Currently, the following options can be specified to toggle features on or off,
with their default value in brackets [].

* USE_CHSHAPE - Build with mooring line calculation support [OFF]
* USE_COMAPI - Build with the COM API [ON]
* USE_CONTROL - Build with control system support [ON]
* USE_DIGTWIN - Build with some Digitial Twin features [OFF]
* USE_FATIGUE - Build with durability calculation support [OFF]
* USE_FREQDOMAIN - Build with frequency-domain support [ON]
* USE_MESHING - Build with FE meshing support [OFF]
* USE_OILWELL - Build with oil well modeling support [OFF]
* USE_QWTLIB - Build with Qwt curve plotting library [OFF]
* USE_RISER - Build with marine structures modeling support [OFF]
* USE_SEVENTS - Build with simulation event support [OFF]
* USE_SOLVERS - Build with solver support [ON]
* USE_TIREAPI - Build with the Tire model API [OFF]
* USE_VTF - Build with VTF export support [OFF]
* USE_WINDPOW - Build with Wind turbine modeling support [OFF]

## Installation

You can install a pre-compiled release of FEDEM for the Windows platform
(64-bit only) by downloading any of the zip-files `FedemInstaller-*.zip`
from the [Releases](https://github.com/openfedem/fedem-gui/releases) page.
Unzip this file in an arbitrary location, and execute the file `INSTALL.bat`
as administrator to install the software on your PC.
See also [Install/README.txt](Install/README.txt).

The Compiled HTML Help file (Fedem.chm) embedded with the installation
will normally be blocked by the Windows operation system,
such that only its table of contents is shown when opening it.
To avoid this, you may need to unblock it in the file Properties,
as explained [here](https://support.helpndoc.com/en/support/solutions/articles/43000549682-the-chm-documentation-format-only-displays-the-table-of-contents-without-any-topic-content).

Unless you already have Microsoft Visual Studio installed (2015 or later),
you may also need to download and install some C++ runtime libraries from
[Microsoft](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
before you can run the installed FEDEM GUI or solvers on Windows.
Simply download the file [vc_redist.x64.exe](https://aka.ms/vs/17/release/vc_redist.x64.exe)
and execute it as administrator.

## Contributing

This project is open to feature requests, suggestions, bug reports, etc.,
via [GitHub issues](https://github.com/openfedem/fedem-gui/issues).
Contributions and feedback are encouraged and always welcome.
For more information about how to contribute,
see our [Contribution Guidelines](.github/CONTRIBUTING.md).

## Licensing

Copyright 2023 SAP SE or an SAP affiliate company and fedem-gui contributors.
Please see our [LICENSE](LICENSE) for copyright and license information.
Detailed information including third-party components and their licensing/copyright information
is available via the [REUSE tool](https://api.reuse.software/info/github.com/openfedem/fedem-gui).
