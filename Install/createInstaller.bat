@echo off
REM SPDX-FileCopyrightText: 2023 SAP SE
REM
REM SPDX-License-Identifier: Apache-2.0
REM
REM This file is part of FEDEM - https://openfedem.org

REM This script creates an installer as a ZIP file
REM for the complete FEDEM installation on the Win64 platform.
REM Edit the following line according to the version to create installer for:
SET VERSION=8.1.0
REM All binaries are assumed to have been built and placed in folder %INST_DIR%.
SET INST_DIR=%USERPROFILE%\Fedem-install\4.0.6\bin
REM ============================================================================
REM No editing after this point.

mkdir %TMP%\Fedem
copy README.txt %TMP%\Fedem
echo @ECHO OFF> %TMP%\Fedem\INSTALL.bat
echo SET VERSION=%VERSION%>> %TMP%\Fedem\INSTALL.bat
type INSTALL.bat >> %TMP%\Fedem\INSTALL.bat
cscript zip.vbs %INST_DIR% %TMP%\Fedem\FedemFiles.zip 80000
mkdir %TMP%\Fedem\bin
copy unzip.vbs %TMP%\Fedem\bin
copy SFTA.exe %TMP%\Fedem\bin
copy makelnk.vbs %TMP%\Fedem\bin
cscript zip.vbs %TMP%\Fedem FedemInstaller.zip 5000
rmdir /s /q %TMP%\Fedem
pause
