// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef BLADE_DRAWER_H
#define BLADE_DRAWER_H

#include <QWidget>

class Blade;
class BladeView;
class AirfoilView;


class BladeDrawer : public QWidget
{
public:
  BladeDrawer(QWidget* parent = NULL);

  void setBlade(Blade* aBlade);
  Blade* getBlade() const { return apBlade; }

  AirfoilView* apAirfoilView;
  BladeView* apBladeView;

private:
  Blade* apBlade;
};

#endif
