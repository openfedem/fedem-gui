// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_SELECTOR_H
#define FD_SELECTOR_H

#include <vector>

class FdObject;
class FaVec3;
class FaDOF;


namespace FdSelector
{
  void getSelectedObjects(std::vector<FdObject*>& toBeFilled);

  void smartMoveSelection(const FaVec3& firstPoint,
                          const FaVec3& secondPoint,
                          const FaDOF& dof);

  FaDOF getDegOfFreedom();

  void selectExpanded(FdObject* object);
  void deselectExpandedLast();
}

#endif
