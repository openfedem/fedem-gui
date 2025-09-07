// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_BEAM_PROP_HYDRO_H
#define FUI_QT_BEAM_PROP_HYDRO_H

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "vpmUI/vpmUIComponents/FuiBeamPropHydro.H"


class FuiQtBeamPropHydro : public FFuQtWidget, public FuiBeamPropHydro
{
public:
  FuiQtBeamPropHydro(QWidget* parent, const char* name = "FuiQtBeamPropHydro");
};

#endif
