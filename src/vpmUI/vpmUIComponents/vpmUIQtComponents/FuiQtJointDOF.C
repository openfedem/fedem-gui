// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtJointDOF.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtMotionType.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtVariableType.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSimpleLoad.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSpringDefCalc.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSprDaForce.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#ifdef FT_HAS_FREQDOMAIN
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#endif


FuiQtJointDOF::FuiQtJointDOF(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  motionType   = new FuiQtMotionType(NULL);
  variableType = new FuiQtVariableType(NULL);
  simpleLoad   = new FuiQtSimpleLoad(NULL);
  springDC     = new FuiQtSpringDefCalc(NULL);
  springFS     = new FuiQtSprDaForce(NULL);
  damperFS     = new FuiQtSprDaForce(NULL);
  initialVel   = new FFuQtLabelField();
#ifdef FT_HAS_FREQDOMAIN
  freqToggle   = new FFuQtToggleButton();
#endif

  this->initWidgets();

  QWidget* qLeft = new QWidget();
  QBoxLayout* layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FuiQtMotionType*>(motionType));
  layout->addWidget(static_cast<FuiQtSimpleLoad*>(simpleLoad));
  layout->addWidget(static_cast<FuiQtVariableType*>(variableType));
  layout->addWidget(static_cast<FFuQtLabelField*>(initialVel));

  QWidget* qRight = new QWidget();
  layout = new QVBoxLayout(qRight);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FuiQtSprDaForce*>(springFS));
#ifdef FT_HAS_FREQDOMAIN
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(freqToggle));
#endif
  layout->addWidget(static_cast<FuiQtSprDaForce*>(damperFS));

  layout = new QHBoxLayout(this);
  layout->addWidget(qLeft);
  layout->addWidget(static_cast<FuiQtSpringDefCalc*>(springDC));
  layout->addWidget(qRight);
}
