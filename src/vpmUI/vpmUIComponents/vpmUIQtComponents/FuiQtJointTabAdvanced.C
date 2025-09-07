// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointTabAdvanced.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FuiQtJointTabAdvanced::FuiQtJointTabAdvanced(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myRotFormulationFrame = new QGroupBox("Rotation formulation");
  QGridLayout* gl = new QGridLayout(myRotFormulationFrame);

  myRotFormulationMenu = new FFuQtOptionMenu();
  gl->addWidget(new QLabel("Formulation"), 0,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(myRotFormulationMenu), 0,1);

  myRotSequenceMenu = new FFuQtOptionMenu();
  gl->addWidget(new QLabel("Sequence"), 1,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(myRotSequenceMenu), 1,1);
  gl->addWidget(myRotExplainLabel = new QLabel(), 2,0, 1,2);

  mySpringCouplingFrame = new QGroupBox("Spring inter-connectivity");
  gl = new QGridLayout(mySpringCouplingFrame);

  myTranSpringCplMenu = new FFuQtOptionMenu();
  gl->addWidget(new QLabel("Translation"), 0,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(myTranSpringCplMenu), 0,1);

  myRotSpringCplMenu = new FFuQtOptionMenu();
  gl->addWidget(new QLabel("Rotation"), 1,0);
  gl->addWidget(static_cast<FFuQtOptionMenu*>(myRotSpringCplMenu),1,1);

  QLayout* layout = new QHBoxLayout(this);
  layout->addWidget(myRotFormulationFrame);
  layout->addWidget(mySpringCouplingFrame);
}


void FuiQtJointTabAdvanced::showRotFormulation(bool yesOrNo)
{
  if (yesOrNo)
    myRotFormulationFrame->show();
  else
    myRotFormulationFrame->hide();
}


void FuiQtJointTabAdvanced::showSpringCpl(int rotOrYesOrNo)
{
  if (rotOrYesOrNo)
  {
    mySpringCouplingFrame->show();
    QLayout* layout = mySpringCouplingFrame->layout();
    for (int i = 0; i < 2; i++)
      if (rotOrYesOrNo == 2) // show rotation menu only
        layout->itemAt(i)->widget()->hide();
      else
        layout->itemAt(i)->widget()->show();
  }
  else
    mySpringCouplingFrame->hide();
}


void FuiQtJointTabAdvanced::setRotExplain(const char* text)
{
  myRotExplainLabel->setText(text);
}
