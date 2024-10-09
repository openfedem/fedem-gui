REM SPDX-FileCopyrightText: 2023 SAP SE
REM
REM SPDX-License-Identifier: Apache-2.0
REM
REM This file is part of FEDEM - https://openfedem.org

REM Installs the FEDEM desktop application for Windows.
SET PWD=%~dp0
CD %PWD%\bin

ECHO(
ECHO Installing FEDEM version %VERSION%
ECHO(
SET INST_DIR=C:\Program Files\FEDEM\Fedem-%VERSION%
SET /P _inst_dir="Installation folder [ %INST_DIR% ]: "
if "%_inst_dir%" == "" (
  SET _inst_dir=%INST_DIR%
)

IF EXIST "%_inst_dir%" (
  ECHO %_inst_dir% already exists, deleting ...
  RMDIR /S "%_inst_dir%" || GOTO Abort:
)
MKDIR "%_inst_dir%"
IF %ERRORLEVEL% NEQ 0 (
  GOTO Abort:
)

ECHO(
ECHO Installing to %_inst_dir% ...
CSCRIPT /B /NOLOGO unzip.vbs ..\FedemFiles.zip "%_inst_dir%"
ECHO(
ECHO Setting model file association ...
SFTA --reg "%_inst_dir%\Fedem.exe" ".fmm" "FedemGUI"
ECHO(
SET /P _create_lnk="Create Desktop shortcut (Y/N): "
IF /I "%_create_lnk%" == "Y" (
  CSCRIPT /B /NOLOGO makelnk.vbs %VERSION% "%_inst_dir%" "D"
)
ECHO(
SET /P _create_lnk="Create Start menu shortcut (Y/N): "
IF /I "%_create_lnk%" == "Y" (
  CSCRIPT /B /NOLOGO makelnk.vbs %VERSION% "%_inst_dir%" "S"
)
ECHO(
ECHO Cleaning up ...
CD %PWD%\..
RMDIR /S %PWD%
ECHO(
ECHO Installation completed.
PAUSE
EXIT

:Abort
ECHO(
ECHO Installation aborted.
PAUSE
EXIT /B %ERRORLEVEL%
