// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtTriadDOF.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtMotionType.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtVariableType.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtSimpleLoad.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#ifdef FT_HAS_FREQDOMAIN
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#endif


FuiQtTriadDOF::FuiQtTriadDOF(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  motionType   = new FuiQtMotionType(NULL,3);
  variableType = new FuiQtVariableType(NULL);
  loadField    = new FuiQtSimpleLoad(NULL);
  initialVel   = new FFuQtLabelField();
#ifdef FT_HAS_FREQDOMAIN
  freqToggle   = new FFuQtToggleButton();
#endif

  this->initWidgets();

  QWidget* qtmp = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(qtmp);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(20);
  layout->addWidget(static_cast<FuiQtVariableType*>(variableType),1);
#ifdef FT_HAS_FREQDOMAIN
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(freqToggle));
#endif

  QWidget* qProp = new QWidget();
  layout = new QVBoxLayout(qProp);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FuiQtSimpleLoad*>(loadField));
  layout->addWidget(qtmp);
  layout->addWidget(static_cast<FFuQtLabelField*>(initialVel));

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FuiQtMotionType*>(motionType),2);
  layout->addWidget(qProp,3);
}
