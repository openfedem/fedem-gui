// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"

#include "FuiQtPreferences.H"


FuiPreferences* FuiPreferences::create(int xpos, int ypos, int width, int height,
                                       const char* title, const char* name)
{
  return new FuiQtPreferences(xpos,ypos,width,height,title,name);
}


FuiQtPreferences::FuiQtPreferences(int xpos, int ypos, int width, int height,
                                   const char* title, const char* name)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,title,name)
{
  for (int iField = 0; iField < SOLVER_PATH; iField++) {
    optFields[iField] = new FFuQtIOField();
    labels[iField] = new FFuQtLabel();
  }
  optFields[SOLVER_PATH] = new FFuQtIOField();
  reducerFrame = new FFuQtLabelFrame();
  eqSolverToggle = new FFuQtToggleButton();
  eqSolverAutomatic = new FFuQtRadioButton();
  eqSolverManual = new FFuQtRadioButton();
  recMatrixToggle = new FFuQtToggleButton();
  recMatrixAutomatic = new FFuQtRadioButton();
  recMatrixManual = new FFuQtRadioButton();
  remoteSolveFrame = new FFuQtLabelFrame();
  solverPrefixToggle = new FFuQtToggleButton();
  solverPathToggle = new FFuQtToggleButton();
  dialogButtons = new FFuQtDialogButtons(NULL,false);

  this->initWidgets();

  QWidget* maxCPfield = new QWidget();
  QBoxLayout* layout = new QHBoxLayout(maxCPfield);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(static_cast<FFuQtLabel*>(labels[MAX_CONC_PROC]));
  layout->addWidget(static_cast<FFuQtIOField*>(optFields[MAX_CONC_PROC]));

  QWidget* eqsToggles = new QWidget();
  layout = new QHBoxLayout(eqsToggles);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("      Cache size:"));
  layout->addSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtRadioButton*>(eqSolverAutomatic));
  layout->addSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtRadioButton*>(eqSolverManual));
  layout->addWidget(static_cast<FFuQtIOField*>(optFields[EQ_SOLVER_SWAP]));
  layout->addWidget(static_cast<FFuQtLabel*>(labels[EQ_SOLVER_SWAP]));

  QWidget* recToggles = new QWidget();
  layout = new QHBoxLayout(recToggles);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("      Cache size:"));
  layout->addSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtRadioButton*>(recMatrixAutomatic));
  layout->addSpacing(5);
  layout->addWidget(dynamic_cast<FFuQtRadioButton*>(recMatrixManual));
  layout->addWidget(static_cast<FFuQtIOField*>(optFields[REC_MATRIX_SWAP]));
  layout->addWidget(static_cast<FFuQtLabel*>(labels[REC_MATRIX_SWAP]));

  // Layout for the reducer out-of-core options frame
  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(reducerFrame));
  layout->setContentsMargins(10,2,10,5);
  layout->setSpacing(0);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(eqSolverToggle));
  layout->addWidget(eqsToggles);
  layout->addSpacing(10);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(recMatrixToggle));
  layout->addWidget(recToggles);

  // Layout for the remote solve options frame
  layout = new QVBoxLayout(static_cast<FFuQtLabelFrame*>(remoteSolveFrame));
  layout->setContentsMargins(10,2,10,10);
  layout->setSpacing(0);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(solverPrefixToggle));
  layout->addWidget(static_cast<FFuQtLabel*>(labels[SOLVER_PREFIX]));
  layout->addWidget(static_cast<FFuQtIOField*>(optFields[SOLVER_PREFIX]));
  layout->addSpacing(10);
  layout->addWidget(dynamic_cast<FFuQtToggleButton*>(solverPathToggle));
  layout->addWidget(static_cast<FFuQtIOField*>(optFields[SOLVER_PATH]));

  // Layout for the main widget
  layout = new QVBoxLayout(this);
  layout->setSpacing(0);
  for (int iField = REDUCER; iField <= FPP; iField++)
  {
    layout->addWidget(static_cast<FFuQtLabel*>(labels[iField]));
    layout->addWidget(static_cast<FFuQtIOField*>(optFields[iField]));
    layout->addSpacing(5);
  }
  layout->addWidget(maxCPfield);
  layout->addSpacing(5);
  layout->addWidget(static_cast<FFuQtLabelFrame*>(reducerFrame));
  layout->addSpacing(5);
  layout->addWidget(static_cast<FFuQtLabelFrame*>(remoteSolveFrame));
  layout->addWidget(static_cast<FFuQtDialogButtons*>(dialogButtons),
                    0, Qt::AlignBottom);
}
