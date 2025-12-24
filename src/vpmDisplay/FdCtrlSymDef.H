// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 *
 * CLASS: FdCtrSymDef
 * AUTHOR: Magnus Svanberg
 * LAST MODIFIED: 19 sept 1997
 *
 * DESCRIPTION:
 * The class describes the graphic symbols of the control system.
 *
 **********************************************************************/

#ifndef FD_CTRL_SYM_DEF_H
#define FD_CTRL_SYM_DEF_H

class SoMaterial;
class FdCtrlSymbolKit;


namespace FdCtrlSymDef
{
  void init();

  SoMaterial* getDefaultMaterial();
  SoMaterial* getBackMaterial();
  SoMaterial* getHighlightMaterial();

  FdCtrlSymbolKit* adderSym();
  FdCtrlSymbolKit* comparatorSym();
  FdCtrlSymbolKit* outputSym();
  FdCtrlSymbolKit* inputSym();
  FdCtrlSymbolKit* deadZoneSym();
  FdCtrlSymbolKit* amplifierSym();
  FdCtrlSymbolKit* sampleHoldSym();
  FdCtrlSymbolKit* limDerivatorSym();
  FdCtrlSymbolKit* multiplierSym();
  FdCtrlSymbolKit* logicalSwitchSym();
  FdCtrlSymbolKit* hysteresisSym();
  FdCtrlSymbolKit* limitationSym();

  FdCtrlSymbolKit* stdSym(int nInp, int nOutp = 1);
  inline FdCtrlSymbolKit* std2pSym() { return stdSym(1); }
  inline FdCtrlSymbolKit* std3pSym() { return stdSym(2); }
};

#endif
