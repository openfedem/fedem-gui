// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtDialogButtons.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtTabbedWidgetStack.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"

#include "FuiQtAdvAnalysisOptions.H"


class FuiQtAdvAnalysisOptionsSheet : public QWidget, public FFuQtComponentBase
{
public:
  FuiQtAdvAnalysisOptionsSheet() { this->setWidget(this); }
};


// Qt implementation of static create method in FuiAdvAnalysisOptions
FuiAdvAnalysisOptions* FuiAdvAnalysisOptions::create(int xpos, int ypos,
                                                     int width, int height,
                                                     bool basicMode)
{
  return new FuiQtAdvAnalysisOptions(xpos,ypos,width,height,basicMode);
}


FuiQtAdvAnalysisOptions::FuiQtAdvAnalysisOptions(int xpos, int ypos,
                                                 int width, int height,
                                                 bool basicMode)
  : FFuQtTopLevelShell(NULL,xpos,ypos,width,height,
                       "Solver", "AnalysisOptions")
{
  // Widget creation

  tabStack = new FFuQtTabbedWidgetStack(NULL,true);
  dialogButtons = new FFuQtDialogButtons(NULL,false);
  btnRunCloud = new FFuQtPushButton();
  btnHelp = new FFuQtPushButton();
  labImgTop = new FFuQtLabel();
  btnAdvanced = new FFuQtPushButton(this);

#ifdef FT_HAS_SOLVERS
  std::array<FuiQtAdvAnalysisOptionsSheet*,NOPTIONS> myOptions;
#else
  std::array<FuiQtAdvAnalysisOptionsSheet*,OUTPUTOPTIONS> myOptions;
#endif
  for (size_t i = 0; i < myOptions.size(); i++)
    options[i] = myOptions[i] = new FuiQtAdvAnalysisOptionsSheet();

  // Time options
  labelFrames[TIMEOPTIONS][TIME_FRAME]    = new FFuQtLabelFrame();
  labelFrames[TIMEOPTIONS][CUTBACK_FRAME] = new FFuQtLabelFrame();
  labelFrames[TIMEOPTIONS][RESTART_FRAME] = new FFuQtLabelFrame();

  doubleFields[TIMEOPTIONS][START]         = new FFuQtIOField();
  doubleFields[TIMEOPTIONS][STOP]          = new FFuQtIOField();
  myAdvTimeIncQueryField                   = new FuiQtQueryInputField(NULL);
  doubleFields[TIMEOPTIONS][MIN_TIME_INCR] = new FFuQtIOField();

  toggleButtons[TIMEOPTIONS][CUTBACK]       = new FFuQtToggleButton();
  doubleFields[TIMEOPTIONS][CUTBACK_FACTOR] = new FFuQtIOField();
  integerFields[TIMEOPTIONS][CUTBACK_STEPS] = new FFuQtIOField();

  toggleButtons[TIMEOPTIONS][RESTART]     = new FFuQtToggleButton();
  doubleFields[TIMEOPTIONS][RESTART_TIME] = new FFuQtIOField();

  labels[TIMEOPTIONS][START]          = new FFuQtLabel();
  labels[TIMEOPTIONS][STOP]           = new FFuQtLabel();
  labels[TIMEOPTIONS][INCREMENT]      = new FFuQtLabel();
  labels[TIMEOPTIONS][MIN_TIME_INCR]  = new FFuQtLabel();
  labels[TIMEOPTIONS][CUTBACK_FACTOR] = new FFuQtLabel();
  labels[TIMEOPTIONS][CUTBACK_STEPS]  = new FFuQtLabel();
  labels[TIMEOPTIONS][RESTART_TIME]   = new FFuQtLabel();

  degradeSoilButton = new FFuQtPushButton();

#ifndef FT_HAS_SOLVERS
  labelFrames[TIMEOPTIONS][ADDITIONAL_FRAME] = new FFuQtLabelFrame();
  labelFrames[TIMEOPTIONS][ADDITIONAL_FRAME]->setLabel("Additional Solver Options");
  addOptions = new FFuQtIOField();
#endif

  // Integration options
  labelFrames[INTOPTIONS][INT_ALG_FRAME]       = new FFuQtLabelFrame();
  labelFrames[INTOPTIONS][ITERATION_FRAME]     = new FFuQtLabelFrame();
  labelFrames[INTOPTIONS][MATRIX_UPDATE_FRAME] = new FFuQtLabelFrame();

  radioButtons[INTOPTIONS][NEWMARK]                   = new FFuQtRadioButton();
  radioButtons[INTOPTIONS][HHT_ALPHA]                 = new FFuQtRadioButton();
  radioButtons[INTOPTIONS][GENERALIZED_ALPHA]         = new FFuQtRadioButton();
  radioButtons[INTOPTIONS][RADIO_USE_TOL]             = new FFuQtRadioButton();
  radioButtons[INTOPTIONS][RADIO_IGNORE_TOL]          = new FFuQtRadioButton();
  radioButtons[INTOPTIONS][RADIO_FIXED_MATRIX_UPDATE] = new FFuQtRadioButton();
  radioButtons[INTOPTIONS][RADIO_VAR_MATRIX_UPDATE]   = new FFuQtRadioButton();
  doubleFields[INTOPTIONS][HHT_ALPHA]                 = new FFuQtIOField();
  doubleFields[INTOPTIONS][GENERALIZED_ALPHA]         = new FFuQtIOField();
  doubleFields[INTOPTIONS][TOL_MATRIX_UPDATE]         = new FFuQtIOField();

  toggleButtons[INTOPTIONS][DYN_STRESS_STIFF]  = new FFuQtToggleButton();
  toggleButtons[INTOPTIONS][MOMENT_CORRECTION] = new FFuQtToggleButton();

  integerFields[INTOPTIONS][NUM_ITERS]            = new FFuQtIOField();
  integerFields[INTOPTIONS][MAX_NUM_ITERS]        = new FFuQtIOField();
  integerFields[INTOPTIONS][MIN_NUM_ITERS]        = new FFuQtIOField();
  integerFields[INTOPTIONS][MIN_MATRIX_UPDATE]    = new FFuQtIOField();
  integerFields[INTOPTIONS][MAX_NO_MATRIX_UPDATE] = new FFuQtIOField();

  labels[INTOPTIONS][HHT_ALPHA]            = new FFuQtLabel();
  labels[INTOPTIONS][GENERALIZED_ALPHA]    = new FFuQtLabel();
  labels[INTOPTIONS][NUM_ITERS]            = new FFuQtLabel();
  labels[INTOPTIONS][MAX_NUM_ITERS]        = new FFuQtLabel();
  labels[INTOPTIONS][MIN_NUM_ITERS]        = new FFuQtLabel();
  labels[INTOPTIONS][MIN_MATRIX_UPDATE]    = new FFuQtLabel();
  labels[INTOPTIONS][MAX_NO_MATRIX_UPDATE] = new FFuQtLabel();
  labels[INTOPTIONS][TOL_MATRIX_UPDATE]    = new FFuQtLabel();
  labels[INTOPTIONS][SHADOW_POS_ALG_LABEL] = new FFuQtLabel();

  shadowPosAlgMenu = new FFuQtOptionMenu();

  // Convergence options
  for (int i = 0; i < 3; i++) {
    radioButtons[CONVOPTIONS][SV_DIS_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][MT_DIS_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][MR_DIS_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][SV_VEL_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][SV_RES_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][MT_RES_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][MR_RES_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][AVG_EN_1+i] = new FFuQtRadioButton();
    radioButtons[CONVOPTIONS][MAX_EN_1+i] = new FFuQtRadioButton();
    labels[CONVOPTIONS][AOI_DIS_1+i]      = new FFuQtLabel();
    labels[CONVOPTIONS][AOI_VEL_1+i]      = new FFuQtLabel();
    labels[CONVOPTIONS][AOI_RES_1+i]      = new FFuQtLabel();
    labels[CONVOPTIONS][AOI_EN_1+i]       = new FFuQtLabel();
  }

  doubleFields[CONVOPTIONS][SV_DIS] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][MT_DIS] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][MR_DIS] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][SV_VEL] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][SV_RES] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][MT_RES] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][MR_RES] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][AVG_EN] = new FFuQtIOField();
  doubleFields[CONVOPTIONS][MAX_EN] = new FFuQtIOField();

  labels[CONVOPTIONS][SV_DIS]  = new FFuQtLabel();
  labels[CONVOPTIONS][MT_DIS]  = new FFuQtLabel();
  labels[CONVOPTIONS][MR_DIS]  = new FFuQtLabel();
  labels[CONVOPTIONS][SV_VEL]  = new FFuQtLabel();
  labels[CONVOPTIONS][SV_RES]  = new FFuQtLabel();
  labels[CONVOPTIONS][MT_RES]  = new FFuQtLabel();
  labels[CONVOPTIONS][MR_RES]  = new FFuQtLabel();
  labels[CONVOPTIONS][AVG_EN]  = new FFuQtLabel();
  labels[CONVOPTIONS][MAX_EN]  = new FFuQtLabel();
  labels[CONVOPTIONS][A_DESCR] = new FFuQtLabel();
  labels[CONVOPTIONS][O_DESCR] = new FFuQtLabel();
  labels[CONVOPTIONS][I_DESCR] = new FFuQtLabel();

  labelFrames[CONVOPTIONS][DIS_FRAME]    = new FFuQtLabelFrame();
  labelFrames[CONVOPTIONS][VEL_FRAME]    = new FFuQtLabelFrame();
  labelFrames[CONVOPTIONS][RES_FRAME]    = new FFuQtLabelFrame();
  labelFrames[CONVOPTIONS][ENERGY_FRAME] = new FFuQtLabelFrame();

  // Eigen options
  toggleButtons[EIGENOPTIONS][EMODE_SOL]          = new FFuQtToggleButton();
  toggleButtons[EIGENOPTIONS][EMODE_DAMPED]       = new FFuQtToggleButton();
  toggleButtons[EIGENOPTIONS][EMODE_BC]           = new FFuQtToggleButton();
  toggleButtons[EIGENOPTIONS][EMODE_STRESS_STIFF] = new FFuQtToggleButton();

  doubleFields[EIGENOPTIONS][EMODE_INTV]       = new FFuQtIOField();
  doubleFields[EIGENOPTIONS][EMODE_SHIFT_FACT] = new FFuQtIOField();

  integerFields[EIGENOPTIONS][NUM_EMODES] = new FFuQtIOField();

  labels[EIGENOPTIONS][EMODE_INTV]       = new FFuQtLabel();
  labels[EIGENOPTIONS][NUM_EMODES]       = new FFuQtLabel();
  labels[EIGENOPTIONS][EMODE_SHIFT_FACT] = new FFuQtLabel();

  // Equilibrium options
  toggleButtons[EQOPTIONS][EQL_ITER]         = new FFuQtToggleButton();
  toggleButtons[EQOPTIONS][EQL_STRESS_STIFF] = new FFuQtToggleButton();
  doubleFields[EQOPTIONS][EQL_ITER_TOL]      = new FFuQtIOField();
  doubleFields[EQOPTIONS][ITER_STEP_SIZE]    = new FFuQtIOField();

  labels[EQOPTIONS][EQL_ITER_TOL]   = new FFuQtLabel();
  labels[EQOPTIONS][ITER_STEP_SIZE] = new FFuQtLabel();
  labels[EQOPTIONS][INFO_FIELD]     = new FFuQtLabel();

  toggleButtons[EQOPTIONS][RAMP_UP]    = new FFuQtToggleButton();
  toggleButtons[EQOPTIONS][RAMP_GRAV]  = new FFuQtToggleButton();
  integerFields[EQOPTIONS][RAMP_STEPS] = new FFuQtIOField();
  doubleFields[EQOPTIONS][RAMP_VMAX]   = new FFuQtIOField();
  doubleFields[EQOPTIONS][RAMP_LENGTH] = new FFuQtIOField();
  doubleFields[EQOPTIONS][RAMP_DELAY]  = new FFuQtIOField();

  labels[EQOPTIONS][RAMP_STEPS]  = new FFuQtLabel();
  labels[EQOPTIONS][RAMP_VMAX]   = new FFuQtLabel();
  labels[EQOPTIONS][RAMP_LENGTH] = new FFuQtLabel();
  labels[EQOPTIONS][RAMP_DELAY]  = new FFuQtLabel();

  // Output options
#ifdef FT_HAS_SOLVERS
  labels[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]        = new FFuQtLabel();
  toggleButtons[OUTPUTOPTIONS][AUTO_CURVE_EXPORT] = new FFuQtToggleButton();
  toggleButtons[OUTPUTOPTIONS][AUTO_VTF_EXPORT]   = new FFuQtToggleButton();
  toggleButtons[OUTPUTOPTIONS][AUTO_ANIM]         = new FFuQtToggleButton();
  toggleButtons[OUTPUTOPTIONS][OVERWRITE]         = new FFuQtToggleButton();
  autoCurveExportField = new FFuQtFileBrowseField(NULL);
  autoVTFField         = new FFuQtFileBrowseField(NULL);
#endif

  // Basic options
  labelFrames[BASICOPTIONS][IEQ_FRAME]      = new FFuQtLabelFrame();
  toggleButtons[BASICOPTIONS][IEQ_TOGGLE]   = new FFuQtToggleButton();

  labelFrames[BASICOPTIONS][TIME_FRAME]     = new FFuQtLabelFrame();
  toggleButtons[BASICOPTIONS][TIME_TOGGLE]  = new FFuQtToggleButton();
  doubleFields[BASICOPTIONS][START]         = new FFuQtIOField();
  doubleFields[BASICOPTIONS][STOP]          = new FFuQtIOField();
  myBasTimeIncQueryField                    = new FuiQtQueryInputField(NULL);
  labels[BASICOPTIONS][START]               = new FFuQtLabel();
  toggleButtons[BASICOPTIONS][STOP]         = new FFuQtToggleButton();
  labels[BASICOPTIONS][INCREMENT]           = new FFuQtLabel();
  toggleButtons[BASICOPTIONS][QS_TOGGLE]    = new FFuQtToggleButton();
  radioButtons[BASICOPTIONS][QS_COMPLETE]   = new FFuQtRadioButton();
  radioButtons[BASICOPTIONS][QS_UPTOTIME]   = new FFuQtRadioButton();
  doubleFields[BASICOPTIONS][QS_UPTOTIME]   = new FFuQtIOField();

  labelFrames[BASICOPTIONS][MODES_FRAME]    = new FFuQtLabelFrame();
  toggleButtons[BASICOPTIONS][MODES_TOGGLE] = new FFuQtToggleButton();
  integerFields[BASICOPTIONS][MODES_COUNT]  = new FFuQtIOField();
  labels[BASICOPTIONS][MODES_COUNT]         = new FFuQtLabel();

#ifdef FT_HAS_FREQDOMAIN
  labelFrames[BASICOPTIONS][FRA_FRAME]      = new FFuQtLabelFrame();
  toggleButtons[BASICOPTIONS][FRA_TOGGLE]   = new FFuQtToggleButton();
#endif

  // Initialize the widget components
  // and their positioning using layout managers

  this->initWidgets(basicMode);
  for (size_t iOpt = 0; iOpt < myOptions.size(); iOpt++)
    this->initLayout(myOptions[iOpt],iOpt);

  QBoxLayout* layout = new QHBoxLayout();
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(dialogButtons->getQtWidget());
  layout->addWidget(btnRunCloud->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(btnHelp->getQtWidget());
  QBoxLayout* layout2 = new QVBoxLayout();
  layout2->setContentsMargins(10,5,10,10);
  layout2->setSpacing(5);
  layout2->addWidget(new FFuQtSeparator());
  layout2->addLayout(layout);

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(labImgTop->getQtWidget());
  layout->addWidget(myOptions[BASICOPTIONS],1);
  layout->addWidget(tabStack->getQtWidget(),1);
  layout->addLayout(layout2);
}


void FuiQtAdvAnalysisOptions::initLayout(QWidget* parentSheet, int iOpt)
{
  auto&& toggleButton = [iOpt,this](int iField)
  {
    return toggleButtons[iOpt][iField]->getQtWidget();
  };
  auto&& radioButton = [iOpt,this](int iField)
  {
    return radioButtons[iOpt][iField]->getQtWidget();
  };
  auto&& integerField = [iOpt,this](int iField)
  {
    return integerFields[iOpt][iField]->getQtWidget();
  };
  auto&& doubleField = [iOpt,this](int iField)
  {
    return doubleFields[iOpt][iField]->getQtWidget();
  };
  auto&& label = [iOpt,this](int iField)
  {
    return labels[iOpt][iField]->getQtWidget();
  };

  LabelFrameMap& frame = labelFrames[iOpt];
  auto&& frameLayout = [&frame](int i, int topMarg = 0, int bottomMarg = 10)
  {
    QGridLayout* gl = new QGridLayout(frame[i]->getQtWidget());
    gl->setContentsMargins(10,topMarg,10,bottomMarg);
    return gl;
  };

  QGridLayout* gl = NULL;
  QGridLayout* gl2 = NULL;
  QBoxLayout* layout = NULL;
  int labelColWidth = 200;

  switch (iOpt) {
  case BASICOPTIONS:
    layout = new QHBoxLayout(frame[IEQ_FRAME]->getQtWidget());
    layout->setContentsMargins(10,5,10,5);
    layout->addWidget(toggleButton(IEQ_TOGGLE));

    gl = new QGridLayout();
    gl->setContentsMargins(18,0,0,0);
    gl->setVerticalSpacing(3);
    gl->addWidget(label(START), 0,0);
    gl->addWidget(doubleField(START), 0,1);
    gl->addWidget(toggleButton(STOP), 1,0);
    gl->addWidget(doubleField(STOP), 1,1);
    gl->addWidget(label(INCREMENT), 2,0);
    gl->addWidget(myBasTimeIncQueryField->getQtWidget(), 2,1);
    gl->setRowMinimumHeight(2,20);
    gl->setColumnMinimumWidth(0,labelColWidth-18);

    gl2 = new QGridLayout();
    gl2->setContentsMargins(18,0,0,0);
    gl2->setVerticalSpacing(2);
    gl2->addWidget(radioButton(QS_COMPLETE), 0,0);
    gl2->addWidget(radioButton(QS_UPTOTIME), 1,0);
    gl2->addWidget(doubleField(QS_UPTOTIME), 1,1);
    gl2->setColumnMinimumWidth(0,labelColWidth-18);

    layout = new QVBoxLayout(frame[TIME_FRAME]->getQtWidget());
    layout->setContentsMargins(10,5,10,5);
    layout->addWidget(toggleButton(TIME_TOGGLE));
    layout->addLayout(gl);
    layout->addWidget(toggleButton(QS_TOGGLE));
    layout->addLayout(gl2);

    gl = frameLayout(MODES_FRAME,5,5);
    gl->setVerticalSpacing(5);
    gl->addWidget(toggleButton(MODES_TOGGLE), 0,0);
    gl->addWidget(label(MODES_COUNT), 1,0);
    gl->addWidget(integerField(MODES_COUNT), 1,1);
    gl->setColumnMinimumWidth(0,labelColWidth);

#ifdef FT_HAS_FREQDOMAIN
    layout = new QHBoxLayout(frame[FRA_FRAME]->getQtWidget());
    layout->setContentsMargins(10,5,10,5);
    layout->addWidget(toggleButton(FRA_TOGGLE));
#endif

    layout = new QVBoxLayout(parentSheet);
    layout->addWidget(frame[IEQ_FRAME]->getQtWidget());
    layout->addWidget(frame[TIME_FRAME]->getQtWidget());
    layout->addWidget(frame[MODES_FRAME]->getQtWidget());
#ifdef FT_HAS_FREQDOMAIN
    layout->addWidget(frame[FRA_FRAME]->getQtWidget());
#endif
    layout->addStretch();
    layout->addWidget(new FFuQtNotes(NULL,
                      "Click \"Advanced\" for more settings."));
    break;

  case TIMEOPTIONS:
    gl = frameLayout(TIME_FRAME,5);
    gl->addWidget(label(START), 0,0);
    gl->addWidget(label(STOP), 1,0);
    gl->addWidget(label(INCREMENT), 2,0);
    gl->addWidget(label(MIN_TIME_INCR), 3,0);
    gl->addWidget(doubleField(START), 0,1);
    gl->addWidget(doubleField(STOP), 1,1);
    gl->addWidget(myAdvTimeIncQueryField->getQtWidget(), 2,1);
    gl->addWidget(doubleField(MIN_TIME_INCR), 3,1);
    gl->setRowMinimumHeight(2,20);
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(CUTBACK_FRAME,5);
    gl->addWidget(toggleButton(CUTBACK), 0,0);
    gl->addWidget(label(CUTBACK_FACTOR), 1,0);
    gl->addWidget(label(CUTBACK_STEPS), 2,0);
    gl->addWidget(doubleField(CUTBACK_FACTOR), 1,1);
    gl->addWidget(integerField(CUTBACK_STEPS), 2,1);
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(RESTART_FRAME,5);
    gl->addWidget(toggleButton(RESTART), 0,0);
    gl->addWidget(label(RESTART_TIME), 1,0);
    gl->addWidget(doubleField(RESTART_TIME), 1,1);
    gl->setColumnMinimumWidth(0,labelColWidth);

#ifndef FT_HAS_SOLVERS
    layout = new QHBoxLayout(frame[ADDITIONAL_FRAME]->getQtWidget());
    layout->addWidget(addOptions->getQtWidget());
#endif

    layout = new QVBoxLayout(parentSheet);
    layout->addWidget(frame[TIME_FRAME]->getQtWidget());
    layout->addWidget(frame[CUTBACK_FRAME]->getQtWidget());
    layout->addWidget(frame[RESTART_FRAME]->getQtWidget());
#ifndef FT_HAS_SOLVERS
    layout->addWidget(frame[ADDITIONAL_FRAME]->getQtWidget());
#endif
    layout->addWidget(degradeSoilButton->getQtWidget());
    layout->addStretch();
    break;

  case INTOPTIONS:
    gl = frameLayout(INT_ALG_FRAME,5);
    gl->addWidget(radioButton(NEWMARK), 0,0,1,-1);
    gl->addWidget(radioButton(HHT_ALPHA), 1,0);
    gl->addWidget(radioButton(GENERALIZED_ALPHA), 2,0);
    gl->addWidget(label(HHT_ALPHA), 1,1, Qt::AlignRight);
    gl->addWidget(label(GENERALIZED_ALPHA), 2,1, Qt::AlignRight);
    gl->addWidget(doubleField(HHT_ALPHA), 1,2);
    gl->addWidget(doubleField(GENERALIZED_ALPHA), 2,2);
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(ITERATION_FRAME,5);
    gl->setHorizontalSpacing(20);
    gl->addWidget(radioButton(RADIO_USE_TOL), 0,0,1,-1);
    gl->addWidget(radioButton(RADIO_IGNORE_TOL), 1,0,1,-1);
    gl->addWidget(label(NUM_ITERS), 2,0);
    gl->addWidget(label(MAX_NUM_ITERS), 3,0);
    gl->addWidget(label(MIN_NUM_ITERS), 4,0);
    gl->addWidget(integerField(NUM_ITERS), 2,1);
    gl->addWidget(integerField(MAX_NUM_ITERS), 3,1);
    gl->addWidget(integerField(MIN_NUM_ITERS), 4,1);
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(MATRIX_UPDATE_FRAME,5);
    gl->setHorizontalSpacing(10);
    gl->addWidget(radioButton(RADIO_FIXED_MATRIX_UPDATE), 0,0,1,-1);
    gl->addWidget(radioButton(RADIO_VAR_MATRIX_UPDATE), 1,0,1,-1);
    gl->addWidget(label(MIN_MATRIX_UPDATE), 2,0);
    gl->addWidget(label(MAX_NO_MATRIX_UPDATE), 3,0);
    gl->addWidget(label(TOL_MATRIX_UPDATE), 4,0);
    gl->addWidget(integerField(MIN_MATRIX_UPDATE), 2,1);
    gl->addWidget(integerField(MAX_NO_MATRIX_UPDATE), 3,1);
    gl->addWidget(doubleField(TOL_MATRIX_UPDATE), 4,1);

    layout = new QVBoxLayout(parentSheet);
    layout->addWidget(frame[INT_ALG_FRAME]->getQtWidget());
    layout->addWidget(frame[ITERATION_FRAME]->getQtWidget());
    layout->addWidget(frame[MATRIX_UPDATE_FRAME]->getQtWidget());
    layout->addSpacing(10);
    layout->addWidget(labels[iOpt][SHADOW_POS_ALG_LABEL]->getQtWidget());
    layout->addWidget(shadowPosAlgMenu->getQtWidget());
    layout->addSpacing(10);
    layout->addWidget(toggleButtons[iOpt][DYN_STRESS_STIFF]->getQtWidget());
    layout->addWidget(toggleButtons[iOpt][MOMENT_CORRECTION]->getQtWidget());
    layout->addStretch();
    break;

  case CONVOPTIONS:
    labelColWidth = 120;
    gl = frameLayout(DIS_FRAME);
    gl->addWidget(label(AOI_DIS_1)   , 0,2, Qt::AlignHCenter);
    gl->addWidget(label(AOI_DIS_2)   , 0,3, Qt::AlignHCenter);
    gl->addWidget(label(AOI_DIS_3)   , 0,4, Qt::AlignHCenter);
    gl->addWidget(label(SV_DIS)      , 1,0);
    gl->addWidget(label(MT_DIS)      , 2,0);
    gl->addWidget(label(MR_DIS)      , 3,0);
    gl->addWidget(doubleField(SV_DIS), 1,1);
    gl->addWidget(doubleField(MT_DIS), 2,1);
    gl->addWidget(doubleField(MR_DIS), 3,1);
    for (int i = 1; i <= 3; i++)
    {
      gl->addWidget(radioButton(SV_DIS+i), 1,1+i, Qt::AlignHCenter);
      gl->addWidget(radioButton(MT_DIS+i), 2,1+i, Qt::AlignHCenter);
      gl->addWidget(radioButton(MR_DIS+i), 3,1+i, Qt::AlignHCenter);
      gl->setColumnMinimumWidth(1+i, i == 2 ? 25 : 30);
    }
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(VEL_FRAME);
    gl->addWidget(label(AOI_VEL_1)   , 0,2, Qt::AlignHCenter);
    gl->addWidget(label(AOI_VEL_2)   , 0,3, Qt::AlignHCenter);
    gl->addWidget(label(AOI_VEL_3)   , 0,4, Qt::AlignHCenter);
    gl->addWidget(label(SV_VEL)      , 1,0);
    gl->addWidget(doubleField(SV_VEL), 1,1);
    for (int i = 1; i <= 3; i++)
    {
      gl->addWidget(radioButton(SV_VEL+i), 1,1+i, Qt::AlignHCenter);
      gl->setColumnMinimumWidth(1+i, i == 2 ? 25 : 30);
    }
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(RES_FRAME);
    gl->addWidget(label(AOI_RES_1)   , 0,2, Qt::AlignHCenter);
    gl->addWidget(label(AOI_RES_2)   , 0,3, Qt::AlignHCenter);
    gl->addWidget(label(AOI_RES_3)   , 0,4, Qt::AlignHCenter);
    gl->addWidget(label(SV_RES)      , 1,0);
    gl->addWidget(label(MT_RES)      , 2,0);
    gl->addWidget(label(MR_RES)      , 3,0);
    gl->addWidget(doubleField(SV_RES), 1,1);
    gl->addWidget(doubleField(MT_RES), 2,1);
    gl->addWidget(doubleField(MR_RES), 3,1);
    for (int i = 1; i <= 3; i++)
    {
      gl->addWidget(radioButton(SV_RES+i), 1,1+i, Qt::AlignHCenter);
      gl->addWidget(radioButton(MT_RES+i), 2,1+i, Qt::AlignHCenter);
      gl->addWidget(radioButton(MR_RES+i), 3,1+i, Qt::AlignHCenter);
      gl->setColumnMinimumWidth(1+i, i == 2 ? 25 : 30);
   }
    gl->setColumnMinimumWidth(0,labelColWidth);

    gl = frameLayout(ENERGY_FRAME);
    gl->addWidget(label(AOI_EN_1)    , 0,2, Qt::AlignHCenter);
    gl->addWidget(label(AOI_EN_2)    , 0,3, Qt::AlignHCenter);
    gl->addWidget(label(AOI_EN_3)    , 0,4, Qt::AlignHCenter);
    gl->addWidget(label(AVG_EN)      , 1,0);
    gl->addWidget(label(MAX_EN)      , 2,0);
    gl->addWidget(doubleField(AVG_EN), 1,1);
    gl->addWidget(doubleField(MAX_EN), 2,1);
    for (int i = 1; i <= 3; i++)
    {
      gl->addWidget(radioButton(AVG_EN+i), 1,1+i, Qt::AlignHCenter);
      gl->addWidget(radioButton(MAX_EN+i), 2,1+i, Qt::AlignHCenter);
      gl->setColumnMinimumWidth(1+i, i == 2 ? 25 : 30);
    }
    gl->setColumnMinimumWidth(0,labelColWidth);

    layout = new QVBoxLayout(parentSheet);
    layout->addWidget(frame[DIS_FRAME]->getQtWidget());
    layout->addWidget(frame[VEL_FRAME]->getQtWidget());
    layout->addWidget(frame[RES_FRAME]->getQtWidget());
    layout->addWidget(frame[ENERGY_FRAME]->getQtWidget());
    layout->addWidget(labels[iOpt][A_DESCR]->getQtWidget());
    layout->addWidget(labels[iOpt][O_DESCR]->getQtWidget());
    layout->addWidget(labels[iOpt][I_DESCR]->getQtWidget());
    layout->addStretch();
    break;

  case EIGENOPTIONS:
    gl = new QGridLayout();
    gl->setContentsMargins(0,0,0,0);
    gl->addWidget(label(NUM_EMODES), 0,0);
    gl->addWidget(label(EMODE_INTV), 1,0);
    gl->addWidget(label(EMODE_SHIFT_FACT), 2,0);
    gl->addWidget(integerField(NUM_EMODES), 0,1);
    gl->addWidget(doubleField(EMODE_INTV), 1,1);
    gl->addWidget(doubleField(EMODE_SHIFT_FACT), 2,1);
    gl->setColumnMinimumWidth(0,labelColWidth);

    layout = new QVBoxLayout(parentSheet); 
    layout->addWidget(toggleButton(EMODE_SOL));
    layout->addLayout(gl);
    layout->addWidget(toggleButton(EMODE_BC));
    layout->addWidget(toggleButton(EMODE_DAMPED));
    layout->addWidget(toggleButton(EMODE_STRESS_STIFF));
    layout->addStretch();
    break;

  case EQOPTIONS:
    gl = new QGridLayout(parentSheet);
    gl->addWidget(toggleButton(EQL_ITER), 0,0,1,-1);
    gl->addWidget(label(EQL_ITER_TOL)   , 1,0);
    gl->addWidget(label(ITER_STEP_SIZE) , 2,0);
    gl->addWidget(label(INFO_FIELD)     , 3,0,1,-1);
    gl->addWidget(toggleButton(EQL_STRESS_STIFF), 4,0,1,-1);
    gl->addWidget(doubleField(EQL_ITER_TOL)  , 1,1);
    gl->addWidget(doubleField(ITER_STEP_SIZE), 2,1);
    gl->addWidget(toggleButton(RAMP_UP)  , 6,0,1,-1, Qt::AlignTop);
    gl->addWidget(toggleButton(RAMP_GRAV), 7,0,1,-1);
    gl->addWidget(label(RAMP_STEPS) , 8,0);
    gl->addWidget(label(RAMP_VMAX)  , 9,0);
    gl->addWidget(label(RAMP_LENGTH),10,0);
    gl->addWidget(label(RAMP_DELAY) ,11,0);
    gl->addWidget(integerField(RAMP_STEPS), 8,1);
    gl->addWidget(doubleField(RAMP_VMAX)  , 9,1);
    gl->addWidget(doubleField(RAMP_LENGTH),10,1);
    gl->addWidget(doubleField(RAMP_DELAY) ,11,1);
    gl->setRowStretch(12,10);
    gl->setRowStretch(6,1);
    gl->setColumnMinimumWidth(0,labelColWidth);
    break;

  case OUTPUTOPTIONS:
    layout = new QVBoxLayout(parentSheet); 
    layout->addWidget(toggleButton(AUTO_CURVE_EXPORT));
    layout->addWidget(autoCurveExportField->getQtWidget());
    layout->addWidget(label(AUTO_CURVE_EXPORT), 0, Qt::AlignRight);
    layout->addSpacing(5);
    layout->addWidget(toggleButton(AUTO_VTF_EXPORT));
    layout->addWidget(autoVTFField->getQtWidget());
    layout->addSpacing(10);
    layout->addWidget(toggleButton(AUTO_ANIM));
    layout->addWidget(toggleButton(OVERWRITE));
    layout->addStretch();
    break;
  }
}


void FuiQtAdvAnalysisOptions::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);
  this->placeAdvancedButton(this->width(),this->height());
}
