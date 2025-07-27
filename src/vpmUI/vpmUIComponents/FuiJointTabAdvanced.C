// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiJointTabAdvanced.H"
#include "FFuLib/FFuOptionMenu.H"


void FuiJointTabAdvanced::setOptionChangedCB(const FFaDynCB1<std::string>& cb)
{
  myRotFormulationMenu->setOptionChangedCB(cb);
  myRotSequenceMenu->setOptionChangedCB(cb);

  myRotSpringCplMenu->setOptionChangedCB(cb);
  myTranSpringCplMenu->setOptionChangedCB(cb);
}


void FuiJointTabAdvanced::setSensitivity(bool isSensitive)
{
  myRotFormulationMenu->setSensitivity(isSensitive);
  myRotSequenceMenu->setSensitivity(isSensitive);

  myTranSpringCplMenu->setSensitivity(isSensitive);
  myRotSpringCplMenu->setSensitivity(isSensitive);
}
