// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAdvAnalysisOptions.H"
#include "vpmUI/vpmUITopLevels/FuiPreferences.H"
#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Fui.H"
#include "vpmUI/Pixmaps/solverSetup.xpm"

#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuLabel.H"
#include "FFuLib/FFuLabelFrame.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuToggleButton.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFuLib/FFuFileBrowseField.H"
#include "FFuLib/FFuTabbedWidgetStack.H"
#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "FFaLib/FFaString/FFaStringExt.H"

#include "vpmPM/FpPM.H"

#include "vpmApp/FapLicenseManager.H"
#include "vpmApp/vpmAppCmds/FapSolveCmds.H"

#include <iostream>


Fmd_SOURCE_INIT(FUI_ADVANALYSISOPTIONS,FuiAdvAnalysisOptions,FFuTopLevelShell);

//----------------------------------------------------------------------------

FuiAdvAnalysisOptions::FuiAdvAnalysisOptions(bool basicMode)
{
  Fmd_CONSTRUCTOR_INIT(FuiAdvAnalysisOptions);

  this->labImgTop = NULL;
  this->notesLabel = NULL;
  this->labNotesText = NULL;
  this->btnAdvanced = this->btnRunCloud = this->btnHelp = NULL;

  this->tabStack = NULL;
  this->options.fill(NULL);

  this->myAdvTimeIncQueryField = this->myBasTimeIncQueryField = NULL;

  this->addOptions = NULL;

  this->degradeSoilButton = NULL;
  this->autoCurveExportField = this->autoVTFField = NULL;

  this->myBasicMode = basicMode;
  this->myBasicTab = -1;
  this->myCurrentTab = 0;
  this->hasVTFfield = false;
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::initWidgets()
{
  this->dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
						     onDialogButtonClicked,int));

  // Run in the cloud button
  this->btnRunCloud->setLabel("Run in cloud");
  this->btnRunCloud->setActivateCB(FFaDynCB0M(FuiAdvAnalysisOptions,this,onRunCloudBtnClicked));
  if (FapSolveCmds::haveCloud())
    this->btnRunCloud->popUp();
  else
    this->btnRunCloud->popDown();

  // Help button
  this->btnHelp->setLabel("Help");
  this->btnHelp->setActivateCB(FFaDynCB0M(FuiAdvAnalysisOptions,this,onHelpBtnClicked));

  // Set basic mode stuff
  this->labImgTop->setPixMap(solverSetup_xpm);
  this->labImgTop->setSizeGeometry(0,0,421,106);

  this->labNotesText->setLabel("Click \"Advanced\" for more settings");

  if (this->myBasicMode) {
    this->btnAdvanced->setLabel("Advanced");
    this->btnAdvanced->setToolTip("Go to Advanced dynamics solver settings");
  }
  else {
    this->btnAdvanced->setLabel("Basic");
    this->btnAdvanced->setToolTip("Go to Basic dynamics solver settings");
    this->labImgTop->popDown();
    this->notesLabel->popDown();
    this->labNotesText->popDown();
  }

  this->btnAdvanced->setActivateCB(FFaDynCB0M(FuiAdvAnalysisOptions,this,onAdvBtnClicked));

  // Set Tabs
  this->tabStack->addTabPage(this->options[TIMEOPTIONS],"Time");
  this->tabStack->addTabPage(this->options[INTOPTIONS],"Integration");
  this->tabStack->addTabPage(this->options[CONVOPTIONS],"Tolerances");
  this->tabStack->addTabPage(this->options[EIGENOPTIONS],"Eigenmode");
  this->tabStack->addTabPage(this->options[EQOPTIONS],"Initial Equilibrium");
#ifdef FT_HAS_SOLVERS
  this->tabStack->addTabPage(this->options[OUTPUTOPTIONS],"Output");
  this->myBasicTab = 6;
#else
  this->myBasicTab = 5;
#endif
  if (this->myBasicMode)
    this->tabStack->addTabPage(this->options[BASICOPTIONS],"Basic");

  // Dialog buttons labels
  this->dialogButtons->setButtonLabel(FFuDialogButtons::LEFTBUTTON,"Run!");
  this->dialogButtons->setButtonLabel(FFuDialogButtons::MIDBUTTON,"Apply");
  this->dialogButtons->setButtonLabel(FFuDialogButtons::RIGHTBUTTON,"Cancel");
#ifndef FT_HAS_SOLVERS
  // Deactivate the Run button if no solver available
  this->dialogButtons->setButtonSensitivity(FFuDialogButtons::LEFTBUTTON,false);
#endif

  // Setting of toggle button callbacks

  // Time options
  this->doubleFields[TIMEOPTIONS][START]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								   onStartTimeAccepted,double));
  this->doubleFields[TIMEOPTIONS][STOP]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								  onStopTimeAccepted,double));

  this->toggleButtons[TIMEOPTIONS][CUTBACK]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								    onCutbackToggled,bool));
  this->toggleButtons[TIMEOPTIONS][RESTART]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								    onRestartToggled,bool));

  // Integration options
  this->newmarkButtonGroup.insert(this->radioButtons[INTOPTIONS][NEWMARK]);
  this->newmarkButtonGroup.insert(this->radioButtons[INTOPTIONS][HHT_ALPHA]);
  this->newmarkButtonGroup.insert(this->radioButtons[INTOPTIONS][GENERALIZED_ALPHA]);
  this->newmarkButtonGroup.setGroupToggleCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
                                                       onNewmarkToggled,int,bool));
  this->doubleFields[INTOPTIONS][HHT_ALPHA]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                      onAlphafAccepted,double));
  this->doubleFields[INTOPTIONS][GENERALIZED_ALPHA]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                              onAlphamAccepted,double));

  this->toleranceButtonGroup.insert(this->radioButtons[INTOPTIONS][RADIO_USE_TOL]);
  this->toleranceButtonGroup.insert(this->radioButtons[INTOPTIONS][RADIO_IGNORE_TOL]);
  this->toleranceButtonGroup.setGroupToggleCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
                                                         onToleranceToggled,int,bool));

  this->matrixUpdateButtonGroup.insert(this->radioButtons[INTOPTIONS][RADIO_FIXED_MATRIX_UPDATE]);
  this->matrixUpdateButtonGroup.insert(this->radioButtons[INTOPTIONS][RADIO_VAR_MATRIX_UPDATE]);
  this->matrixUpdateButtonGroup.setGroupToggleCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
                                                            onMatrixUpdateToggled,int,bool));

  // Convergence options
  this->radioButtons[CONVOPTIONS][SV_DIS_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_DIS1Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_DIS_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_DIS2Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_DIS_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_DIS3Toggled,bool));
  this->radioButtons[CONVOPTIONS][MT_DIS_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMT_DIS1Toggled,bool));
  this->radioButtons[CONVOPTIONS][MT_DIS_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMT_DIS2Toggled,bool));
  this->radioButtons[CONVOPTIONS][MT_DIS_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMT_DIS3Toggled,bool));
  this->radioButtons[CONVOPTIONS][MR_DIS_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMR_DIS1Toggled,bool));
  this->radioButtons[CONVOPTIONS][MR_DIS_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMR_DIS2Toggled,bool));
  this->radioButtons[CONVOPTIONS][MR_DIS_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMR_DIS3Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_VEL_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_VEL1Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_VEL_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_VEL2Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_VEL_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_VEL3Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_RES_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_RES1Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_RES_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_RES2Toggled,bool));
  this->radioButtons[CONVOPTIONS][SV_RES_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onSV_RES3Toggled,bool));
  this->radioButtons[CONVOPTIONS][MT_RES_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMT_RES1Toggled,bool));
  this->radioButtons[CONVOPTIONS][MT_RES_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMT_RES2Toggled,bool));
  this->radioButtons[CONVOPTIONS][MT_RES_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMT_RES3Toggled,bool));
  this->radioButtons[CONVOPTIONS][MR_RES_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMR_RES1Toggled,bool));
  this->radioButtons[CONVOPTIONS][MR_RES_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMR_RES2Toggled,bool));
  this->radioButtons[CONVOPTIONS][MR_RES_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMR_RES3Toggled,bool));
  this->radioButtons[CONVOPTIONS][AVG_EN_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onAVG_EN1Toggled,bool));
  this->radioButtons[CONVOPTIONS][AVG_EN_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onAVG_EN2Toggled,bool));
  this->radioButtons[CONVOPTIONS][AVG_EN_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onAVG_EN3Toggled,bool));
  this->radioButtons[CONVOPTIONS][MAX_EN_1]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMAX_EN1Toggled,bool));
  this->radioButtons[CONVOPTIONS][MAX_EN_2]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMAX_EN2Toggled,bool));
  this->radioButtons[CONVOPTIONS][MAX_EN_3]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this, onMAX_EN3Toggled,bool));

  // Eigenvalue solution options
  this->toggleButtons[EIGENOPTIONS][EMODE_SOL]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								       onEModeSolToggled,bool));
  this->integerFields[EIGENOPTIONS][NUM_EMODES]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
									  onNumModesAccepted,int));

  // Initial Equilibrium Options
  this->toggleButtons[EQOPTIONS][EQL_ITER]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                   onEqlIterToggled,bool));
  this->toggleButtons[EQOPTIONS][RAMP_UP]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                  onRampUpToggled,bool));

  // Basic options
  this->toggleButtons[BASICOPTIONS][IEQ_TOGGLE]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                        onEqlIterToggled,bool));
  this->toggleButtons[BASICOPTIONS][TIME_TOGGLE]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                         onDSToggled,bool));
  this->toggleButtons[BASICOPTIONS][STOP]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                  onStopToggled,bool));
  this->toggleButtons[BASICOPTIONS][QS_TOGGLE]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                       onQSToggled,bool));
  this->radioButtons[BASICOPTIONS][QS_COMPLETE]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                        onQRadio1Toggled,bool));
  this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                        onQRadio2Toggled,bool));
  this->toggleButtons[BASICOPTIONS][MODES_TOGGLE]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                          onEModeSolToggled,bool));

  // Field check modes

  for (int iOpt = 0; iOpt < NOPTIONS; iOpt++) {
    for (const IOFieldMap::value_type& io : this->doubleFields[iOpt])
      io.second->setInputCheckMode(FFuIOField::DOUBLECHECK);
    for (const IOFieldMap::value_type& io : this->integerFields[iOpt])
      io.second->setInputCheckMode(FFuIOField::INTEGERCHECK);
  }

  // Query field
  this->myAdvTimeIncQueryField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  this->myAdvTimeIncQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->myAdvTimeIncQueryField->setTextForNoRefSelected("Constant");
  this->myAdvTimeIncQueryField->setChangedCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
							onTimeIncQueryChanged,int,double));

  // Label definitions

  // Time options
  this->labelFrames[TIMEOPTIONS][TIME_FRAME]->setLabel("Simulation time");
  this->labels[TIMEOPTIONS][START]->setLabel("Start time");
  this->labels[TIMEOPTIONS][STOP]->setLabel("Stop time");
  this->labels[TIMEOPTIONS][INCREMENT]->setLabel("Time increment");
  this->labels[TIMEOPTIONS][MIN_TIME_INCR]->setLabel("Minimum time increment");

  this->labelFrames[TIMEOPTIONS][CUTBACK_FRAME]->setLabel("Iteration cut-back");
  this->toggleButtons[TIMEOPTIONS][CUTBACK]->setLabel("Perform cut-back on divergence");
  this->labels[TIMEOPTIONS][CUTBACK_FACTOR]->setLabel("Step size reduction factor");
  this->labels[TIMEOPTIONS][CUTBACK_STEPS]->setLabel("Number of steps with reduced size");

  this->labelFrames[TIMEOPTIONS][RESTART_FRAME]->setLabel("Restart");
  this->toggleButtons[TIMEOPTIONS][RESTART]->setLabel("Perform restart");
  this->labels[TIMEOPTIONS][RESTART_TIME]->setLabel("Restart time");

  // Integration options
  this->labelFrames[INTOPTIONS][INT_ALG_FRAME]->setLabel("Integration algorithm");
  this->labelFrames[INTOPTIONS][ITERATION_FRAME]->setLabel("Integration convergence control");
  this->labelFrames[INTOPTIONS][MATRIX_UPDATE_FRAME]->setLabel("System matrix updates");
  this->radioButtons[INTOPTIONS][NEWMARK]->setLabel("Standard Newmark integration without numerical damping");
  this->radioButtons[INTOPTIONS][HHT_ALPHA]->setLabel("HHT-alpha");
  this->radioButtons[INTOPTIONS][GENERALIZED_ALPHA]->setLabel("Generalized-alpha");
  this->radioButtons[INTOPTIONS][RADIO_FIXED_MATRIX_UPDATE]->setLabel("Fixed number of matrix updates");
  this->radioButtons[INTOPTIONS][RADIO_VAR_MATRIX_UPDATE]->setLabel("Variable number of matrix updates");
  this->radioButtons[INTOPTIONS][RADIO_USE_TOL]->setLabel("Use integration tolerances");
  this->radioButtons[INTOPTIONS][RADIO_IGNORE_TOL]->setLabel("Ignore integration tolerances");
  this->toggleButtons[INTOPTIONS][DYN_STRESS_STIFF]->setLabel("Geometric stiffness contribution");
  this->toggleButtons[INTOPTIONS][MOMENT_CORRECTION]->setLabel("Centripetal force correction");

  this->setAlphaLabel(HHT_ALPHA,"H",0.0,0.3333);
  this->setAlphaLabel(GENERALIZED_ALPHA,"m",-1.0,0.5);
  this->labels[INTOPTIONS][NUM_ITERS]->setLabel("Number of iterations");
  this->labels[INTOPTIONS][MAX_NUM_ITERS]->setLabel("Maximum number of iterations");
  this->labels[INTOPTIONS][MIN_NUM_ITERS]->setLabel("Minimum number of iterations");
  this->labels[INTOPTIONS][MIN_MATRIX_UPDATE]->setLabel("Number of initial iterations with matrix updates");
  this->labels[INTOPTIONS][MAX_NO_MATRIX_UPDATE]->setLabel("Maximum sequential iterations with no matrix update");
  this->labels[INTOPTIONS][TOL_MATRIX_UPDATE]->setLabel("Convergence tolerance factor for matrix updates");
  this->labels[INTOPTIONS][SHADOW_POS_ALG_LABEL]->setLabel("Default positioning algorithm for co-rotated reference coordinate systems");

  shadowPosAlgMenu->addOption("Max triangle (with unit offset, depreciated)");
  shadowPosAlgMenu->addOption("Max triangle (with scaled offset if needed)");
  shadowPosAlgMenu->addOption("Mass-based nodal average");
  shadowPosAlgMenu->addOption("Max triangle for Parts, mass-based nodal average for Beams");
  shadowPosAlgMenu->addOption("Mass-based nodal average for Parts only");

  this->degradeSoilButton->setLabel("Degrade soil springs at current stop time");
  this->degradeSoilButton->setActivateCB(FFaDynCB0M(FuiAdvAnalysisOptions,this,onDegradeSoilActivated));
  if (FapLicenseManager::hasFeature("FA-WND") || FapLicenseManager::hasFeature("FA-RIS"))
    this->degradeSoilButton->popUp();
  else
    this->degradeSoilButton->popDown();

  // Convergence options
  const char* lbl[3] = { "A", "O", "I" };

  for (int i = 0; i < 3; i++) {
    this->labels[CONVOPTIONS][AOI_DIS_1+i]->setLabel(lbl[i]);
    this->labels[CONVOPTIONS][AOI_VEL_1+i]->setLabel(lbl[i]);
    this->labels[CONVOPTIONS][AOI_RES_1+i]->setLabel(lbl[i]);
    this->labels[CONVOPTIONS][AOI_EN_1+i]->setLabel(lbl[i]);
    this->isTopOfRadioLabel[CONVOPTIONS][AOI_DIS_1+i] = true;
    this->isTopOfRadioLabel[CONVOPTIONS][AOI_VEL_1+i] = true;
    this->isTopOfRadioLabel[CONVOPTIONS][AOI_RES_1+i] = true;
    this->isTopOfRadioLabel[CONVOPTIONS][AOI_EN_1+i]  = true;
  }
  this->labels[CONVOPTIONS][A_DESCR]->setLabel("A: Set of tests where ALL must be satisfied");
  this->labels[CONVOPTIONS][O_DESCR]->setLabel("O: Set of tests where ONE must be satisfied");
  this->labels[CONVOPTIONS][I_DESCR]->setLabel("I: Ignored tests");

  this->labelFrames[CONVOPTIONS][DIS_FRAME]->setLabel("Displacement iteration correction");
  this->labelFrames[CONVOPTIONS][VEL_FRAME]->setLabel("Velocity iteration correction");
  this->labelFrames[CONVOPTIONS][RES_FRAME]->setLabel("Unbalanced forces (residual)");
  this->labelFrames[CONVOPTIONS][ENERGY_FRAME]->setLabel("Iteration energy change");
  this->labelFrames[CONVOPTIONS][DIS_FRAME]->toBack();
  this->labelFrames[CONVOPTIONS][VEL_FRAME]->toBack();
  this->labelFrames[CONVOPTIONS][RES_FRAME]->toBack();
  this->labelFrames[CONVOPTIONS][ENERGY_FRAME]->toBack();

  this->labels[CONVOPTIONS][SV_DIS]->setLabel("Scaled vector norm");
  this->labels[CONVOPTIONS][MT_DIS]->setLabel("Max disp");
  this->labels[CONVOPTIONS][MR_DIS]->setLabel("Max rotation");
  this->labels[CONVOPTIONS][SV_VEL]->setLabel("Scaled vector norm");
  this->labels[CONVOPTIONS][SV_RES]->setLabel("Scaled vector norm");
  this->labels[CONVOPTIONS][MT_RES]->setLabel("Max force");
  this->labels[CONVOPTIONS][MR_RES]->setLabel("Max torque");
  this->labels[CONVOPTIONS][AVG_EN]->setLabel("Max average");
  this->labels[CONVOPTIONS][MAX_EN]->setLabel("Max DOF energy");

  // Eigenvalue solution options
  this->toggleButtons[EIGENOPTIONS][EMODE_SOL]->setLabel("Perform eigenmode solution");
  this->toggleButtons[EIGENOPTIONS][EMODE_DAMPED]->setLabel("Damped eigenmode solution");
  this->toggleButtons[EIGENOPTIONS][EMODE_BC]->setLabel("Use additional boundary conditions in eigenmode solution");
  this->toggleButtons[EIGENOPTIONS][EMODE_STRESS_STIFF]->setLabel("Geometric stiffness contribution");
  this->labels[EIGENOPTIONS][EMODE_INTV]->setLabel("Eigenmode solution interval");
  this->labels[EIGENOPTIONS][NUM_EMODES]->setLabel("Number of eigenmodes to solve");
  this->labels[EIGENOPTIONS][EMODE_SHIFT_FACT]->setLabel("Eigenvalue shift [Hz]");

  // Initial Equilibrium Options
  this->toggleButtons[EQOPTIONS][EQL_ITER]->setLabel("Perform static equilibrium iterations at initial position");
  this->toggleButtons[EQOPTIONS][EQL_STRESS_STIFF]->setLabel("Geometric stiffness contribution");
  this->labels[EQOPTIONS][EQL_ITER_TOL]->setLabel("Equilibrium iteration tolerance");
  this->labels[EQOPTIONS][ITER_STEP_SIZE]->setLabel("Iteration step size limit");
  this->labels[EQOPTIONS][INFO_FIELD]->setLabel("Numbers refer to the vector norm of incremental displacements");

  this->toggleButtons[EQOPTIONS][RAMP_UP]->setLabel("Perform dynamic ramp-up of loads/motions");
  this->toggleButtons[EQOPTIONS][RAMP_GRAV]->setLabel("Ramp-up gravity forces as well");
  this->labels[EQOPTIONS][RAMP_STEPS]->setLabel("Number of increments in ramp-up stage");
  this->labels[EQOPTIONS][RAMP_VMAX]->setLabel("Maximum gradient during ramp-up stage");
  this->labels[EQOPTIONS][RAMP_LENGTH]->setLabel("Total length (in time) of ramp-up stage");
  this->labels[EQOPTIONS][RAMP_DELAY]->setLabel("Time with constant loads after ramp-up");

  // Output options
#ifdef FT_HAS_SOLVERS
  this->toggleButtons[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]->setLabel("Automatic curve export");
  this->toggleButtons[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]->setToggleCB(FFaDynCB1M(FFuComponentBase,autoCurveExportField,
                                                                                setSensitivity,bool));

  this->autoCurveExportField->setAbsToRelPath("yes");
  this->autoCurveExportField->setDialogType(FFuFileDialog::FFU_SAVE_FILE);
  this->autoCurveExportField->setDialogRememberKeyword("AutoCurveExportField");
#endif

  // Basic options
  this->labelFrames[BASICOPTIONS][IEQ_FRAME]->setLabel("Initial equilibrium");
  this->toggleButtons[BASICOPTIONS][IEQ_TOGGLE]->setLabel("Enable");

  this->labelFrames[BASICOPTIONS][TIME_FRAME]->setLabel("Time history response analysis");
  this->toggleButtons[BASICOPTIONS][TIME_TOGGLE]->setLabel("Enable");
  this->labels[BASICOPTIONS][START]->setLabel("Start time");
  this->toggleButtons[BASICOPTIONS][STOP]->setLabel("Stop time");
  this->labels[BASICOPTIONS][INCREMENT]->setLabel("Time increment");
  this->toggleButtons[BASICOPTIONS][QS_TOGGLE]->setLabel("Quasistatic analysis");
  this->radioButtons[BASICOPTIONS][QS_COMPLETE]->setLabel("Complete interval");
  this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->setLabel("Up to time");

  this->labelFrames[BASICOPTIONS][MODES_FRAME]->setLabel("Natural frequency analysis");
  this->toggleButtons[BASICOPTIONS][MODES_TOGGLE]->setLabel("Enable");
  this->labels[BASICOPTIONS][MODES_COUNT]->setLabel("Number of modes");

#ifdef FT_HAS_FREQDOMAIN
  this->labelFrames[BASICOPTIONS][FRA_FRAME]->setLabel("Frequency response analysis");
  this->toggleButtons[BASICOPTIONS][FRA_TOGGLE]->setLabel("Enable");
#endif

  this->doubleFields[BASICOPTIONS][START]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								    onStartTimeAccepted,double));
  this->doubleFields[BASICOPTIONS][STOP]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
								   onStopTimeAccepted,double));
  this->integerFields[BASICOPTIONS][MODES_COUNT]->setAcceptedCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
									   onNumModesAccepted,int));

  this->myBasTimeIncQueryField->setBehaviour(FuiQueryInputField::REF_NUMBER);
  this->myBasTimeIncQueryField->setButtonMeaning(FuiQueryInputField::EDIT);
  this->myBasTimeIncQueryField->setTextForNoRefSelected("Constant");
  this->myBasTimeIncQueryField->setChangedCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
							onTimeIncQueryChanged,int,double));

#ifdef FT_HAS_SOLVERS
  std::vector<std::string> rpcExts = { "rsp", "drv", "tim" };
  this->autoCurveExportField->addDialogFilter("MTS RPC file, PC formatting", rpcExts, false, FuaAdvAnalysisOptionsValues::RPC_LITTLE_ENDIAN);
  this->autoCurveExportField->addDialogFilter("MTS RPC file, UNIX formatting", rpcExts, false, FuaAdvAnalysisOptionsValues::RPC_BIG_ENDIAN);
  this->autoCurveExportField->addDialogFilter("ASCII, tab separated", "asc", true, FuaAdvAnalysisOptionsValues::ASCII_MULTI_COLUMN);
  this->autoCurveExportField->setFileOpenedCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
                                                         onAutoCurveExportFileChanged,const std::string&,int));

  this->toggleButtons[OUTPUTOPTIONS][AUTO_VTF_EXPORT]->setLabel("Automatic export to GLview VTF file");
  this->toggleButtons[OUTPUTOPTIONS][AUTO_VTF_EXPORT]->setToggleCB(FFaDynCB1M(FFuComponentBase,autoVTFField,
                                                                              setSensitivity,bool));
  this->autoVTFField->setLabel("Express File");
  this->autoVTFField->setAbsToRelPath("yes");
  this->autoVTFField->setDialogType(FFuFileDialog::FFU_SAVE_FILE);
  this->autoVTFField->setDialogRememberKeyword("AutoVTFField");

  this->autoVTFField->addDialogFilter("Express VTF file","vtf",true,0);
  this->autoVTFField->addDialogFilter("Binary VTF file","vtf",false,1);
  this->autoVTFField->addDialogFilter("ASCII VTF file","vtf",false,2);
  this->autoVTFField->setFileOpenedCB(FFaDynCB2M(FuiAdvAnalysisOptions,this,
						 onAutoVTFFileChanged,const std::string&,int));

  this->toggleButtons[OUTPUTOPTIONS][AUTO_ANIM]->setLabel("Automatically start a simultaneous Rigid Body animation on solver start");

  this->toggleButtons[OUTPUTOPTIONS][OVERWRITE]->setLabel("Overwrite existing results, if any");
  this->toggleButtons[OUTPUTOPTIONS][OVERWRITE]->setToggleCB(FFaDynCB1M(FuiAdvAnalysisOptions,this,
                                                                        onOverwriteToggled,bool));
#endif

  // Set minimum TLS size to avoid hiding info
  Fui::Geo myGeo = Fui::getGeo(Fui::ADVANALYSISOPTIONS_GEO);
  this->setMinWidth(myGeo.width);
  this->setMinHeight(myGeo.height);
  this->placeAdvancedButton(myGeo.width,myGeo.height);

  // Set value accept policy
  this->setAllFieldAcceptPolicy(FFuIOField::ENTERONLY);

  // Set double display mode
  this->setAllDoubleDisplayMode(FFuIOField::AUTO,12,1);

  // Create the UA-object of this UI
  FFuUAExistenceHandler::invokeCreateUACB(this);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::setAlphaLabel(int iop, const char* subs, double a1, double a2)
{
  this->labels[INTOPTIONS][iop]->setLabel("<font face='Symbol' size='+1'>a</font><sub><i>" +
                                          std::string(subs) + "</i></sub> -factor  [" +
                                          FFaNumStr(a1,-1,3) + "," + FFaNumStr(a2,-1,3) + "]");
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::setSensitivity(bool sens)
{
#ifdef FT_HAS_SOLVERS
  bool restartIsOn = FpPM::isModelRestartable();
#else
  bool restartIsOn = false;
#endif
  if (restartIsOn)
  {
    // Restart toggle always sensitive when we have results
    restartIsOn = this->toggleButtons[TIMEOPTIONS][RESTART]->getValue();
    this->toggleButtons[TIMEOPTIONS][RESTART]->setSensitivity(true);
    this->doubleFields[TIMEOPTIONS][RESTART_TIME]->setSensitivity(restartIsOn);
    this->degradeSoilButton->setSensitivity(true);
  }
  else
  {
    this->toggleButtons[TIMEOPTIONS][RESTART]->setSensitivity(false);
    this->doubleFields[TIMEOPTIONS][RESTART_TIME]->setSensitivity(false);
    this->degradeSoilButton->setSensitivity(false);
  }

  // Cloud execution not yet allowed with restart
  this->btnRunCloud->setSensitivity(sens && !restartIsOn);

#ifdef FT_HAS_SOLVERS
  // Bugfix #126: If the model has results, we should still be able
  // to solve the active event, in case that one has no results
  if (!sens) sens = FpPM::isModelRunable();

  // The dialog buttons must be sensitive whenever restart is toggled on
  this->FuiTopLevelDialog::setSensitivity(sens || restartIsOn);

  if (this->toggleButtons[OUTPUTOPTIONS][OVERWRITE]->getValue())
  {
    this->dialogButtons->setButtonSensitivity(FFuDialogButtons::LEFTBUTTON,true);
    if (!restartIsOn) this->btnRunCloud->setSensitivity(true);
  }
#else
  this->dialogButtons->setButtonSensitivity(FFuDialogButtons::MIDBUTTON,sens);
#endif
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::placeAdvancedButton(int width, int height)
{
  // The Advanced/Basic button still needs to be positioned explicitly,
  // since we want it to appear "inside" frame of the tabbed sheet
  int w = btnHelp->getWidthHint();
  int h = btnHelp->getHeightHint();
  int x = width - w - 10;
  int y = height - dialogButtons->getHeightHint() - h - 30;
  btnAdvanced->setSizeGeometry(x,y,w,h);
  btnAdvanced->toFront();
}
//-----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::setAllFieldAcceptPolicy(int policy)
{
  for (int iOpt = 0; iOpt < NOPTIONS; iOpt++)
    for (IOFieldMap::value_type& io : this->doubleFields[iOpt])
      io.second->setAcceptPolicy(policy);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::setAllDoubleDisplayMode(int mode, int precision, int zeroPrecision)
{
  for (int iOpt = 0; iOpt < NOPTIONS; iOpt++)
    for (IOFieldMap::value_type& io : this->doubleFields[iOpt]) {
      io.second->setDoubleDisplayMode(mode);
      io.second->setDoubleDisplayPrecision(precision);
      io.second->setZeroDisplayPrecision(zeroPrecision);
    }
}
//----------------------------------------------------------------------------

bool FuiAdvAnalysisOptions::updateDBValues(bool)
{
  int iOpt;

  //unhighligh fields since qt enables mult field highlighting (qt bug ?)
  for (iOpt = 0; iOpt < NOPTIONS; iOpt++) {
    for (IOFieldMap::value_type& io : this->doubleFields[iOpt])
      io.second->unHighlight();
    for (IOFieldMap::value_type& io : this->integerFields[iOpt])
      io.second->unHighlight();
  }

  //get ui values
  FuaAdvAnalysisOptionsValues values;
  if (this->getMyUIValues(&values)) {
    //all values are ok for the ui
    if (myBasicMode) {
      // Copy some basic field values wich are also present in the advanced dialog
      values.doubleValues[TIMEOPTIONS][START] = values.doubleValues[BASICOPTIONS][START];
      values.valueStatus[TIMEOPTIONS][START]  = values.valueStatus[BASICOPTIONS][START];
      values.doubleValues[TIMEOPTIONS][STOP]  = values.doubleValues[BASICOPTIONS][STOP];
      values.valueStatus[TIMEOPTIONS][STOP]   = values.valueStatus[BASICOPTIONS][STOP];
      values.integerValues[EIGENOPTIONS][NUM_EMODES] = values.integerValues[BASICOPTIONS][MODES_COUNT];
      values.valueStatus[EIGENOPTIONS][NUM_EMODES]   = values.valueStatus[BASICOPTIONS][MODES_COUNT];
    }

    //invoke callback
    this->invokeSetAndGetDBValuesCB(&values);

    //Re-set values to ensure value correspondance between ui and db
    this->setMyUIValues(&values,false);
  }

  // Check validity of the field values in UI or DB
  bool status = true;
  for (iOpt = 0; iOpt < NOPTIONS && status; iOpt++) {
    for (IOFieldMap::value_type& io : this->doubleFields[iOpt])
      if (!values.valueStatus[iOpt][io.first]) {
        this->tabStack->setCurrentTab(iOpt > 0 ? iOpt-1 : myBasicTab);
        io.second->highlight();
        std::cout << '\a' << std::flush; // bleep
        status = false;
      }

    for (IOFieldMap::value_type& io : this->integerFields[iOpt])
      if (!values.valueStatus[iOpt][io.first]) {
        this->tabStack->setCurrentTab(iOpt > 0 ? iOpt-1 : myBasicTab);
        io.second->highlight();
        std::cout << '\a' << std::flush; // bleep
        status = false;
      }
  }

  // Update sensitivity of the additional solver option field
  // in case restart is toggled ON or OFF
  FuiPreferences* pref = dynamic_cast<FuiPreferences*>(FFuTopLevelShell::getInstanceByType(FuiPreferences::getClassTypeID()));
  if (pref) pref->onRestartToggeled(this->toggleButtons[TIMEOPTIONS][RESTART]->getValue());

  return status;
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onDialogButtonClicked(int button)
{
  switch (button)
    {
    case FFuDialogButtons::LEFTBUTTON:

      if (myBasicMode)
        this->myBasTimeIncQueryField->accept();
      else
        this->myAdvTimeIncQueryField->accept();
      if (this->updateDBValues())
        FapSolveCmds::solveDynamics(); // Start the dynamics solver
      break;

    case FFuDialogButtons::MIDBUTTON:
      this->updateDBValues();
      break;

    case FFuDialogButtons::RIGHTBUTTON:
      this->invokeFinishedCB();
      break;
    }
}

void FuiAdvAnalysisOptions::onRunCloudBtnClicked()
{
  if (myBasicMode)
    this->myBasTimeIncQueryField->accept();
  else
    this->myAdvTimeIncQueryField->accept();
  if (this->updateDBValues())
    FapSolveCmds::solveInCloud(); // Start the solver in the cloud
}

//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onStartTimeAccepted(double startTime)
{
  if (myBasicMode)
    this->doubleFields[TIMEOPTIONS][START]->setValue(startTime);
  else
    this->doubleFields[BASICOPTIONS][START]->setValue(startTime);
}

void FuiAdvAnalysisOptions::onStopTimeAccepted(double stopTime)
{
  if (myBasicMode)
    this->doubleFields[TIMEOPTIONS][STOP]->setValue(stopTime);
  else
    this->doubleFields[BASICOPTIONS][STOP]->setValue(stopTime);
}

void FuiAdvAnalysisOptions::onTimeIncQueryChanged(int, double)
{
  FmModelMemberBase* selected;
  if (myBasicMode) {
    selected = this->myBasTimeIncQueryField->getSelectedRef();
    this->myAdvTimeIncQueryField->setValue(this->myBasTimeIncQueryField->getValue());
  }
  else {
    selected = this->myAdvTimeIncQueryField->getSelectedRef();
    this->myBasTimeIncQueryField->setValue(this->myAdvTimeIncQueryField->getValue());
  }

  if (selected)
    this->doubleFields[TIMEOPTIONS][MIN_TIME_INCR]->setSensitivity(true);
  else if (!this->toggleButtons[TIMEOPTIONS][CUTBACK]->getValue())
    this->doubleFields[TIMEOPTIONS][MIN_TIME_INCR]->setSensitivity(false);

  if (myBasicMode)
    this->myAdvTimeIncQueryField->setSelectedRef(selected);
  else
    this->myBasTimeIncQueryField->setSelectedRef(selected);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onCutbackToggled(bool onOrOff)
{
  if (this->myAdvTimeIncQueryField->getSelectedRef())
    this->doubleFields[TIMEOPTIONS][MIN_TIME_INCR]->setSensitivity(true);
  else
    this->doubleFields[TIMEOPTIONS][MIN_TIME_INCR]->setSensitivity(onOrOff);
  this->doubleFields[TIMEOPTIONS][CUTBACK_FACTOR]->setSensitivity(onOrOff);
  this->integerFields[TIMEOPTIONS][CUTBACK_STEPS]->setSensitivity(onOrOff);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onRestartToggled(bool onOrOff)
{
  this->doubleFields[TIMEOPTIONS][RESTART_TIME]->setSensitivity(onOrOff);

  // The dialog buttons must be sensitive whenever restart is toggled on.
  // This enables us to modify any solution parameter in restart runs.
  // The dialog buttons are also made insensitive if restart is toggled off.
  // That means, you cannot turn off the restart toggle as long as there are
  // results that were produced by a restart run. Restart is switched off
  // only when the simulation results are deleted.
  this->FuiTopLevelDialog::setSensitivity(onOrOff);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onNewmarkToggled(int button, bool onOrOff)
{
  // Lambda function for setting the field sensitivities
  auto&& setFieldSensitivity = [this](int iop, bool sensitive)
  {
    this->labels[INTOPTIONS][iop]->setSensitivity(sensitive);
    this->doubleFields[INTOPTIONS][iop]->setSensitivity(sensitive);
  };

  switch (button) {
  case 0:
    setFieldSensitivity(HHT_ALPHA,false);
    setFieldSensitivity(GENERALIZED_ALPHA,false);
    break;
  case 1:
    this->setAlphaLabel(HHT_ALPHA,"H",0.0,0.3333);
    setFieldSensitivity(HHT_ALPHA,onOrOff);
    setFieldSensitivity(GENERALIZED_ALPHA,false);
    break;
  case 2:
    this->setAlphaLabel(HHT_ALPHA,"f",-1.0,0.5);
    setFieldSensitivity(HHT_ALPHA,onOrOff);
    this->setAlphaLabel(GENERALIZED_ALPHA,"m",-1.0,0.5);
    setFieldSensitivity(GENERALIZED_ALPHA,onOrOff);
    break;
  }
}

void FuiAdvAnalysisOptions::onAlphafAccepted(double alpha)
{
  if (this->radioButtons[INTOPTIONS][GENERALIZED_ALPHA]->getValue())
    if (alpha >= -1.0 && alpha <= 0.5)
      this->setAlphaLabel(GENERALIZED_ALPHA,"m",3.0*alpha-1.0,alpha);
}

void FuiAdvAnalysisOptions::onAlphamAccepted(double alpha)
{
  if (this->radioButtons[INTOPTIONS][GENERALIZED_ALPHA]->getValue())
    if (alpha >= -1.0 && alpha <= 0.5)
      this->setAlphaLabel(HHT_ALPHA,"f",alpha,0.5);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onToleranceToggled(int button, bool onOrOff)
{
  if (button == 0) onOrOff = !onOrOff;

  this->integerFields[INTOPTIONS][NUM_ITERS]->setSensitivity(onOrOff);
  this->labels[INTOPTIONS][NUM_ITERS]->setSensitivity(onOrOff);

  this->integerFields[INTOPTIONS][MAX_NUM_ITERS]->setSensitivity(!onOrOff);
  this->labels[INTOPTIONS][MAX_NUM_ITERS]->setSensitivity(!onOrOff);

  this->integerFields[INTOPTIONS][MIN_NUM_ITERS]->setSensitivity(!onOrOff);
  this->labels[INTOPTIONS][MIN_NUM_ITERS]->setSensitivity(!onOrOff);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onMatrixUpdateToggled(int button, bool onOrOff)
{
  if (button == 1) onOrOff = !onOrOff;

  this->integerFields[INTOPTIONS][MIN_MATRIX_UPDATE]->setSensitivity(true);
  this->labels[INTOPTIONS][MIN_MATRIX_UPDATE]->setSensitivity(true);

  this->doubleFields[INTOPTIONS][TOL_MATRIX_UPDATE]->setSensitivity(!onOrOff);
  this->labels[INTOPTIONS][TOL_MATRIX_UPDATE]->setSensitivity(!onOrOff);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onAOItoggled(int iNorm, int i, bool onOrOff)
{
  if (onOrOff)
  {
    int j = 1 + i%3;
    int k = 1 + j%3;
    this->radioButtons[CONVOPTIONS][iNorm+j]->setValue(false);
    this->radioButtons[CONVOPTIONS][iNorm+k]->setValue(false);
    this->doubleFields[CONVOPTIONS][iNorm]->setSensitivity(i < 3);
  }
  else
    this->radioButtons[CONVOPTIONS][iNorm+i]->setValue(true);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onAutoCurveExportFileChanged(const std::string&, int id)
{
  switch (id)
    {
    case FuaAdvAnalysisOptionsValues::RPC_LITTLE_ENDIAN:
      this->labels[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]->setLabel("MTS RPC, PC formatting");
      break;
    case FuaAdvAnalysisOptionsValues::RPC_BIG_ENDIAN:
      this->labels[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]->setLabel("MTS RPC, UNIX formatting");
      break;
    case FuaAdvAnalysisOptionsValues::ASCII_MULTI_COLUMN:
      this->labels[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]->setLabel("ASCII, tab separated");
      break;
    default:
      this->labels[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]->setLabel("Unknown file format");
      break;
    }
}
//-----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onAutoVTFFileChanged(const std::string&, int id)
{
  const char* vtf_label[3] = { "Express File",
			       " Binary File",
			       "  ASCII File" };

  if (id >= 0 && id < 3)
    this->autoVTFField->setLabel(vtf_label[id]);
  else
    this->autoVTFField->setLabel("File");
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onEModeSolToggled(bool onOrOff)
{
  if (myBasicMode)
    this->toggleButtons[EIGENOPTIONS][EMODE_SOL]->setValue(onOrOff);
  else
    this->toggleButtons[BASICOPTIONS][MODES_TOGGLE]->setValue(onOrOff);

  this->integerFields[EIGENOPTIONS][NUM_EMODES]->setSensitivity(onOrOff);
  this->labels[EIGENOPTIONS][NUM_EMODES]->setSensitivity(onOrOff);

  this->integerFields[BASICOPTIONS][MODES_COUNT]->setSensitivity(onOrOff);
  this->labels[BASICOPTIONS][MODES_COUNT]->setSensitivity(onOrOff);

  this->doubleFields[EIGENOPTIONS][EMODE_INTV]->setSensitivity(onOrOff);
  this->labels[EIGENOPTIONS][EMODE_INTV]->setSensitivity(onOrOff);

  this->doubleFields[EIGENOPTIONS][EMODE_SHIFT_FACT]->setSensitivity(onOrOff);
  this->labels[EIGENOPTIONS][EMODE_SHIFT_FACT]->setSensitivity(onOrOff);

  this->toggleButtons[EIGENOPTIONS][EMODE_DAMPED]->setSensitivity(onOrOff);
  this->toggleButtons[EIGENOPTIONS][EMODE_BC]->setSensitivity(onOrOff);
  this->toggleButtons[EIGENOPTIONS][EMODE_STRESS_STIFF]->setSensitivity(onOrOff);
}

void FuiAdvAnalysisOptions::onNumModesAccepted(int nMode)
{
  if (myBasicMode)
    this->integerFields[EIGENOPTIONS][NUM_EMODES]->setValue(nMode);
  else
    this->integerFields[BASICOPTIONS][MODES_COUNT]->setValue(nMode);
}

//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onEqlIterToggled(bool onOrOff)
{
  if (myBasicMode)
    this->toggleButtons[EQOPTIONS][EQL_ITER]->setValue(onOrOff);
  else
    this->toggleButtons[BASICOPTIONS][IEQ_TOGGLE]->setValue(onOrOff);

  this->doubleFields[EQOPTIONS][EQL_ITER_TOL]->setSensitivity(onOrOff);
  this->labels[EQOPTIONS][EQL_ITER_TOL]->setSensitivity(onOrOff);
  this->doubleFields[EQOPTIONS][ITER_STEP_SIZE]->setSensitivity(onOrOff);
  this->labels[EQOPTIONS][ITER_STEP_SIZE]->setSensitivity(onOrOff);

  this->toggleButtons[EQOPTIONS][EQL_STRESS_STIFF]->setSensitivity(onOrOff);
}

void FuiAdvAnalysisOptions::onRampUpToggled(bool onOrOff)
{
  this->toggleButtons[EQOPTIONS][RAMP_GRAV]->setSensitivity(onOrOff);

  this->integerFields[EQOPTIONS][RAMP_STEPS]->setSensitivity(onOrOff);
  this->labels[EQOPTIONS][RAMP_STEPS]->setSensitivity(onOrOff);
  this->doubleFields[EQOPTIONS][RAMP_VMAX]->setSensitivity(onOrOff);
  this->labels[EQOPTIONS][RAMP_VMAX]->setSensitivity(onOrOff);
  this->doubleFields[EQOPTIONS][RAMP_LENGTH]->setSensitivity(onOrOff);
  this->labels[EQOPTIONS][RAMP_LENGTH]->setSensitivity(onOrOff);
  this->doubleFields[EQOPTIONS][RAMP_DELAY]->setSensitivity(onOrOff);
  this->labels[EQOPTIONS][RAMP_DELAY]->setSensitivity(onOrOff);
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onDSToggled(bool onOrOff)
{
  bool useStop = onOrOff && this->toggleButtons[BASICOPTIONS][STOP]->getValue();

  this->doubleFields[BASICOPTIONS][START]->setSensitivity(onOrOff);
  this->doubleFields[BASICOPTIONS][STOP]->setSensitivity(useStop);
  this->myBasTimeIncQueryField->setSensitivity(onOrOff);
  this->toggleButtons[BASICOPTIONS][STOP]->setSensitivity(onOrOff);
  this->toggleButtons[BASICOPTIONS][QS_TOGGLE]->setSensitivity(onOrOff);

  this->onQSToggled(onOrOff && this->toggleButtons[BASICOPTIONS][QS_TOGGLE]->getValue());

  this->doubleFields[TIMEOPTIONS][START]->setSensitivity(onOrOff);
  this->doubleFields[TIMEOPTIONS][STOP]->setSensitivity(useStop);
  this->myAdvTimeIncQueryField->setSensitivity(onOrOff);
}

void FuiAdvAnalysisOptions::onStopToggled(bool onOrOff)
{
  this->doubleFields[BASICOPTIONS][STOP]->setSensitivity(onOrOff);
  this->doubleFields[TIMEOPTIONS][STOP]->setSensitivity(onOrOff);
}

void FuiAdvAnalysisOptions::onQSToggled(bool onOrOff)
{
  this->radioButtons[BASICOPTIONS][QS_COMPLETE]->setSensitivity(onOrOff);
  this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->setSensitivity(onOrOff);
  bool b = this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->getValue();
  this->doubleFields[BASICOPTIONS][QS_UPTOTIME]->setSensitivity(b && onOrOff);
}

void FuiAdvAnalysisOptions::onQRadio1Toggled(bool onOrOff)
{
  this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->setValue(!onOrOff);
  this->doubleFields[BASICOPTIONS][QS_UPTOTIME]->setSensitivity(!onOrOff);
}

void FuiAdvAnalysisOptions::onQRadio2Toggled(bool onOrOff)
{
  this->radioButtons[BASICOPTIONS][QS_COMPLETE]->setValue(!onOrOff);
  this->doubleFields[BASICOPTIONS][QS_UPTOTIME]->setSensitivity(onOrOff);
}

void FuiAdvAnalysisOptions::onOverwriteToggled(bool onOrOff)
{
  if (onOrOff)
    this->dialogButtons->setButtonSensitivity(FFuDialogButtons::LEFTBUTTON,true);
  else
    this->dialogButtons->setButtonSensitivity(FFuDialogButtons::LEFTBUTTON,FpPM::isModelEditable());
}

void FuiAdvAnalysisOptions::onAdvBtnClicked()
{
  // Toggle basic mode
  this->myBasicMode = !this->myBasicMode;

  // Pop down/up basic mode stuff
  if (this->myBasicMode) {
    myCurrentTab = this->tabStack->getCurrentTabPosIdx();
    this->tabStack->addTabPage(this->options[BASICOPTIONS],"Basic");
    this->tabStack->setCurrentTab(myBasicTab);
    this->labImgTop->popUp();
    this->notesLabel->popUp();
    this->labNotesText->popUp();
    this->btnAdvanced->setLabel("Advanced");
    this->btnAdvanced->setToolTip("Go to Advanced dynamics solver settings");
  }
  else {
    this->tabStack->removeTabPage(this->options[BASICOPTIONS]);
    this->tabStack->setCurrentTab(myCurrentTab);
    this->labImgTop->popDown();
    this->notesLabel->popDown();
    this->labNotesText->popDown();
    this->btnAdvanced->setLabel("Basic");
    this->btnAdvanced->setToolTip("Return to Basic dynamics solver settings");
  }

  this->placeAdvancedButton(this->getWidth(), this->getHeight());
}

void FuiAdvAnalysisOptions::onHelpBtnClicked()
{
  // Display the topic in the help file
  std::string chmTopic("dialogbox/solver-");
  if (myBasicMode)
    chmTopic += "basic.htm";
  else
    chmTopic += "advanced.htm";
  Fui::showCHM(chmTopic.c_str());
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::setUIValues(const FFuaUIValues* values)
{
  FuaAdvAnalysisOptionsValues* advValues = (FuaAdvAnalysisOptionsValues*) values;
  this->setMyUIValues(advValues,true);
  this->setSensitivity(FpPM::isModelEditable());
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::setMyUIValues(FuaAdvAnalysisOptionsValues* advValues,
                                          bool invalidValuesAlso)
{
  for (int iOpt = 0; iOpt < NOPTIONS; iOpt++)
  {
    // field values
    for (IOFieldMap::value_type& io : this->doubleFields[iOpt])
      if (invalidValuesAlso || advValues->valueStatus[iOpt][io.first]) {
        DoubleIter vi = advValues->doubleValues[iOpt].find(io.first);
        if (vi != advValues->doubleValues[iOpt].end())
          io.second->setValue(vi->second);
      }
    for (IOFieldMap::value_type& io : this->integerFields[iOpt])
      if (invalidValuesAlso || advValues->valueStatus[iOpt][io.first]) {
        IntegerIter vi = advValues->integerValues[iOpt].find(io.first);
        if (vi != advValues->integerValues[iOpt].end())
          io.second->setValue(vi->second);
      }

    // toggle values
    for (RadioButtonMap::value_type& r : this->radioButtons[iOpt]) {
      BoolIter vi = advValues->toggleValues[iOpt].find(r.first);
      if (vi != advValues->toggleValues[iOpt].end())
        r.second->setValue(vi->second);
    }
    for (ToggleButtonMap::value_type& t : this->toggleButtons[iOpt]) {
      BoolIter vi = advValues->toggleValues[iOpt].find(t.first);
      if (vi != advValues->toggleValues[iOpt].end())
        t.second->setValue(vi->second);
    }
  }

  IntegerIter vi = advValues->optionMenuValues[INTOPTIONS].find(SHADOW_POS_ALG);
  if (vi != advValues->optionMenuValues[INTOPTIONS].end())
    shadowPosAlgMenu->selectOption(vi->second);

  this->myAdvTimeIncQueryField->setValue(advValues->myTimeIncValue);
  this->myAdvTimeIncQueryField->setQuery(advValues->myTimeIncQuery);
  this->myAdvTimeIncQueryField->setSelectedRef(advValues->mySelectedTimeEngine);
  this->myAdvTimeIncQueryField->setButtonCB(advValues->myEditTimeIncEngineCB);

  if (this->myAdvTimeIncQueryField->getSelectedRef() || this->toggleButtons[TIMEOPTIONS][CUTBACK]->getValue())
    this->doubleFields[TIMEOPTIONS][MIN_TIME_INCR]->setSensitivity(true);
  else
    this->doubleFields[TIMEOPTIONS][MIN_TIME_INCR]->setSensitivity(false);

  if (addOptions)
    addOptions->setValue(advValues->addOptions);

  if (this->autoCurveExportField) {
    this->autoCurveExportField->setAbsToRelPath(advValues->modelFilePath);
    this->autoCurveExportField->setFileName(advValues->autoCurveExportFileName);
    this->autoCurveExportField->setFilterID(advValues->curveFileFormat);
    this->autoCurveExportField->setSensitivity(advValues->toggleValues[OUTPUTOPTIONS][AUTO_CURVE_EXPORT]);
    this->onAutoCurveExportFileChanged(advValues->autoCurveExportFileName,advValues->curveFileFormat);
  }

  if ((this->hasVTFfield = advValues->autoVTFFileType >= 0) && this->autoVTFField) {
    this->autoVTFField->setAbsToRelPath(advValues->modelFilePath);
    this->autoVTFField->setFileName(advValues->autoVTFFileName);
    this->autoVTFField->setFilterID(advValues->autoVTFFileType);
    this->autoVTFField->setSensitivity(advValues->toggleValues[OUTPUTOPTIONS][AUTO_VTF_EXPORT]);
    this->onAutoVTFFileChanged(advValues->autoVTFFileName,advValues->autoVTFFileType);
  }

#ifdef FT_HAS_SOLVERS
  if (this->hasVTFfield) {
    this->toggleButtons[OUTPUTOPTIONS][AUTO_VTF_EXPORT]->popUp();
    this->autoVTFField->popUp();
  }
  else {
    this->toggleButtons[OUTPUTOPTIONS][AUTO_VTF_EXPORT]->popDown();
    this->autoVTFField->popDown();
  }
#endif

  // tab
  this->tabStack->setCurrentTab(this->myBasicMode ? myBasicTab : myCurrentTab);

  // set sensitivities
  this->onCutbackToggled(advValues->toggleValues[TIMEOPTIONS][CUTBACK]);
  if (advValues->toggleValues[INTOPTIONS][HHT_ALPHA])
    this->onNewmarkToggled(1,true);
  else if (advValues->toggleValues[INTOPTIONS][GENERALIZED_ALPHA])
    this->onNewmarkToggled(2,true);
  this->onToleranceToggled(1,advValues->toggleValues[INTOPTIONS][RADIO_IGNORE_TOL]);
  this->onMatrixUpdateToggled(0,advValues->toggleValues[INTOPTIONS][RADIO_FIXED_MATRIX_UPDATE]);
  this->onEModeSolToggled(advValues->toggleValues[EIGENOPTIONS][EMODE_SOL]);
  this->onEqlIterToggled(advValues->toggleValues[EQOPTIONS][EQL_ITER]);
  this->onRampUpToggled(advValues->toggleValues[EQOPTIONS][RAMP_UP]);

  this->doubleFields[CONVOPTIONS][SV_DIS]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][SV_DIS_3]);
  this->doubleFields[CONVOPTIONS][MT_DIS]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][MT_DIS_3]);
  this->doubleFields[CONVOPTIONS][MR_DIS]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][MR_DIS_3]);
  this->doubleFields[CONVOPTIONS][SV_VEL]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][SV_VEL_3]);
  this->doubleFields[CONVOPTIONS][SV_RES]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][SV_RES_3]);
  this->doubleFields[CONVOPTIONS][MT_RES]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][MT_RES_3]);
  this->doubleFields[CONVOPTIONS][MR_RES]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][MR_RES_3]);
  this->doubleFields[CONVOPTIONS][AVG_EN]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][AVG_EN_3]);
  this->doubleFields[CONVOPTIONS][MAX_EN]->setSensitivity(!advValues->toggleValues[CONVOPTIONS][MAX_EN_3]);

  // basic options

  bool onOrOffTime = advValues->toggleValues[BASICOPTIONS][TIME_TOGGLE];
  bool onOrOffStop = onOrOffTime && advValues->toggleValues[BASICOPTIONS][STOP];
  this->doubleFields[TIMEOPTIONS][START]->setSensitivity(onOrOffTime);
  this->doubleFields[TIMEOPTIONS][STOP]->setSensitivity(onOrOffStop);

  this->doubleFields[BASICOPTIONS][START]->setSensitivity(onOrOffTime);
  this->doubleFields[BASICOPTIONS][STOP]->setSensitivity(onOrOffStop);

  this->toggleButtons[BASICOPTIONS][QS_TOGGLE]->setSensitivity(onOrOffTime);
  bool onOrOffQS = advValues->toggleValues[BASICOPTIONS][QS_TOGGLE];
  this->radioButtons[BASICOPTIONS][QS_COMPLETE]->setSensitivity(onOrOffQS && onOrOffTime);
  this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->setSensitivity(onOrOffQS && onOrOffTime);
  bool b = this->radioButtons[BASICOPTIONS][QS_UPTOTIME]->getValue();
  this->doubleFields[BASICOPTIONS][QS_UPTOTIME]->setSensitivity(b && onOrOffQS && onOrOffTime);

  bool onOrOffModes = advValues->toggleValues[BASICOPTIONS][MODES_TOGGLE];
  this->integerFields[BASICOPTIONS][MODES_COUNT]->setSensitivity(onOrOffModes);

#ifdef FT_HAS_FREQDOMAIN
  this->toggleButtons[BASICOPTIONS][FRA_TOGGLE]->setSensitivity(advValues->haveFreqDomainLoads);
#endif

  this->myBasTimeIncQueryField->setValue(advValues->myTimeIncValue);
  this->myBasTimeIncQueryField->setQuery(advValues->myTimeIncQuery);
  this->myBasTimeIncQueryField->setSelectedRef(advValues->mySelectedTimeEngine);
  this->myBasTimeIncQueryField->setButtonCB(advValues->myEditTimeIncEngineCB);
}
//----------------------------------------------------------------------------

bool FuiAdvAnalysisOptions::getMyUIValues(FuaAdvAnalysisOptionsValues* values)
{
  bool validValues = true;
  for (int iOpt = 0; iOpt < NOPTIONS; iOpt++)
  {
    // field values
    for (const IOFieldMap::value_type& io : this->doubleFields[iOpt])
      if ((values->valueStatus[iOpt][io.first] = io.second->isDouble()))
        values->doubleValues[iOpt][io.first] = io.second->getDouble();
      else {
	values->doubleValues[iOpt][io.first] = 0.0;
        validValues = false;
      }

    for (const IOFieldMap::value_type& io : this->integerFields[iOpt])
      if ((values->valueStatus[iOpt][io.first] = io.second->isInt()))
        values->integerValues[iOpt][io.first] = io.second->getInt();
      else {
	values->integerValues[iOpt][io.first] = 0;
	validValues = false;
      }

    // toggle values
    for (const RadioButtonMap::value_type& r : this->radioButtons[iOpt])
      values->toggleValues[iOpt][r.first] = r.second->getValue();
    for (const ToggleButtonMap::value_type& t : this->toggleButtons[iOpt])
      values->toggleValues[iOpt][t.first] = t.second->getValue();
  }

  values->optionMenuValues[INTOPTIONS][SHADOW_POS_ALG] = shadowPosAlgMenu->getSelectedOption();

  if (addOptions)
    values->addOptions = addOptions->getValue();

  if (this->myBasicMode)
    values->myTimeIncValue = this->myBasTimeIncQueryField->getValue();
  else
    values->myTimeIncValue = this->myAdvTimeIncQueryField->getValue();
  values->mySelectedTimeEngine = this->myAdvTimeIncQueryField->getSelectedRef();

  if (this->autoCurveExportField) {
    values->autoCurveExportFileName = this->autoCurveExportField->getFileName();
    values->curveFileFormat         = this->autoCurveExportField->getFilterID();
  }
  else
    values->curveFileFormat = -1;

  if (this->hasVTFfield && this->autoVTFField) {
    values->autoVTFFileName = this->autoVTFField->getFileName();
    values->autoVTFFileType = this->autoVTFField->getFilterID();
  }
  else
    values->autoVTFFileType = -1;

  if (!this->myBasicMode)
    myCurrentTab = this->tabStack->getCurrentTabPosIdx();

  return validValues;
}
//----------------------------------------------------------------------------

void FuiAdvAnalysisOptions::onDegradeSoilActivated()
{
  FapSolveCmds::degradeSoil(this->doubleFields[TIMEOPTIONS][STOP]->getDouble());
}
