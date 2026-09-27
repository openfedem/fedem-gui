// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_MODES_ANIMATION_H
#define FAP_MODES_ANIMATION_H

#include <vector>

class FmModelMemberBase;
class FmAnimation;
class FdAnimateModel;
class FFrExtractor;


namespace Fap
{
  bool modesAnimation(FmAnimation* animation,
                      FdAnimateModel* animator,
                      bool& userCancelled);
  int modesInitAnim(FmAnimation* animation,
                    FdAnimateModel* animator,
                    FFrExtractor* rdb,
                    const std::vector<FmModelMemberBase*>& objs,
                    double& ksi, double& omega, double& Tmax, double& Tinc);
}

#endif
