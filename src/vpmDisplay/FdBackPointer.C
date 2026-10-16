// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdBackPointer.H"


SO_NODE_SOURCE(FdBackPointer);


void FdBackPointer::init()
{
  SO_NODE_INIT_CLASS(FdBackPointer, SoNode, "Node");
}


FdBackPointer::FdBackPointer(FdObject* pt) : structPt(pt)
{
  SO_NODE_CONSTRUCTOR(FdBackPointer);
}
