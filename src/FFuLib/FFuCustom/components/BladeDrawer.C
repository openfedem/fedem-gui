// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>

#include "FFuLib/FFuCustom/components/renderers/BladeView.H"
#include "FFuLib/FFuCustom/components/renderers/AirfoilView.H"
#include "FFuLib/FFuCustom/components/BladeDrawer.H"


BladeDrawer::BladeDrawer(QWidget* parent) : QWidget(parent)
{
  apBlade = NULL;
  apBladeView = new BladeView(this);
  apAirfoilView = new AirfoilView(this);

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(apBladeView);
  layout->addWidget(apAirfoilView);

  this->setFixedHeight(170);
  apAirfoilView->setFixedSize(170,160);
  apBladeView->setFixedHeight(160);

  this->setStyleSheet("border: 4px solid qradialgradient(cx:0.5, cy:0.5, radius: 1, fx:0.5, fy:0.5, stop:0 #fdd000, stop:1 #f77500); "
                      "border-left: 0px solid; border-right: 0px solid; border-top: 0px solid");
  apAirfoilView->setStyleSheet("border-left: 2px solid qradialgradient(cx:0.5, cy:0.5, radius: 1, fx:0.5, fy:0.5, stop:0 #fdd000, stop:1 #f77500)");
}


void BladeDrawer::setBlade(Blade* aBlade)
{
  apBlade = aBlade;

  apBladeView->setBlade(apBlade);
  apAirfoilView->setBlade(apBlade);
}
