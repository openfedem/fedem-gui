<!---
  SPDX-FileCopyrightText: 2023 SAP SE

  SPDX-License-Identifier: Apache-2.0

  This file is part of FEDEM - https://openfedem.org
--->

# FEDEM install scripts

This folder contains some BAT-scripts for packing the Windows desktop application
of FEDEM into a zip-archive for external distribution.

The installation process uses the SFTA tool which is downloaded from
[dansys.com](https://danysys.com/set-file-type-association-default-application-command-line-windows-10-userchoice-hash-internal-method),
to set proper file association for the FEDEM model file type (`.fmm`).

Edit the VERSION tag in the `createInstaller.bat` file to reflect the actual version
the installer is to be built for, and execute the file after the build has completed.
