// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_ADV_ANALYSIS_OPTIONS_H
#define FUI_ADV_ANALYSIS_OPTIONS_H

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include <array>
#include <map>

class FFuIOField;
class FFuLabel;
class FFuLabelFrame;
class FFuRadioButton;
class FFuToggleButton;
class FFuPushButton;
class FFuOptionMenu;
class FFuFileBrowseField;
class FFuTabbedWidgetStack;
class FuaAdvAnalysisOptionsValues;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmModelMemberBase;
class FFuaQueryBase;

typedef std::map<int,FFuLabelFrame*> LabelFrameMap;
typedef LabelFrameMap::iterator LabelFrameIter;

typedef std::map<int,double> DoubleMap;
typedef DoubleMap::iterator DoubleIter;

typedef std::map<int,int> IntegerMap;
typedef IntegerMap::iterator IntegerIter;

typedef std::map<int,bool> BoolMap;
typedef BoolMap::iterator BoolIter;

typedef std::map<int,FFuRadioButton*> RadioButtonMap;
typedef RadioButtonMap::iterator RadioIter;

typedef std::map<int,FFuToggleButton*> ToggleButtonMap;
typedef ToggleButtonMap::iterator ToggleIter;

typedef std::map<int,FFuIOField*> IOFieldMap;
typedef IOFieldMap::iterator IOFieldIter;

typedef std::map<int,FFuLabel*> LabelMap;
typedef LabelMap::iterator LabelIter;


class FuiAdvAnalysisOptions : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

protected:
  FuiAdvAnalysisOptions();

public:
  // Static create method to use instead of constructor
  // Implementation in GUILib-dependent implementation file
  static FuiAdvAnalysisOptions* create(int xpos, int ypos,
                                       int width, int height,
                                       bool basicMode = false);

  enum { // Options
    BASICOPTIONS,
    TIMEOPTIONS,
    INTOPTIONS,
    CONVOPTIONS,
    EIGENOPTIONS,
    EQOPTIONS,
    OUTPUTOPTIONS,
    NOPTIONS
  };

  enum { // Basic & Time options
    IEQ_FRAME,
    IEQ_FRAME_END,
    IEQ_TOGGLE,

    TIME_FRAME,
    TIME_FRAME_END,
    TIME_TOGGLE,
    START,
    STOP,
    INCREMENT,
    MIN_TIME_INCR,

    QS_TOGGLE,
    QS_COMPLETE,
    QS_UPTOTIME,

    MODES_FRAME,
    MODES_FRAME_END,
    MODES_TOGGLE,
    MODES_COUNT,

    FRA_FRAME,
    FRA_FRAME_END,
    FRA_TOGGLE,

    CUTBACK_FRAME,
    CUTBACK_FRAME_END,
    CUTBACK,
    CUTBACK_FACTOR,
    CUTBACK_STEPS,

    RESTART_FRAME,
    RESTART_FRAME_END,
    RESTART,
    RESTART_TIME,

    ADDITIONAL_FRAME,
    ADDITIONAL_FRAME_END,
    ADDITIONAL
  };

  enum { // Integration options
    INT_ALG_FRAME,
    INT_ALG_FRAME_END,
    NEWMARK,
    HHT_ALPHA,
    GENERALIZED_ALPHA,

    ITERATION_FRAME,
    ITERATION_FRAME_END,
    RADIO_USE_TOL,
    RADIO_IGNORE_TOL,
    NUM_ITERS,
    MAX_NUM_ITERS,
    MIN_NUM_ITERS,

    MATRIX_UPDATE_FRAME,
    MATRIX_UPDATE_FRAME_END,
    RADIO_FIXED_MATRIX_UPDATE,
    RADIO_VAR_MATRIX_UPDATE,
    MIN_MATRIX_UPDATE,
    MAX_NO_MATRIX_UPDATE,
    TOL_MATRIX_UPDATE,

    DYN_STRESS_STIFF,
    MOMENT_CORRECTION,
    SHADOW_POS_ALG_LABEL,
    SHADOW_POS_ALG
  };

  enum { // Convergence options
    // Displacement
    DIS_FRAME, DIS_FRAME_END,
    AOI_DIS_1, AOI_DIS_2, AOI_DIS_3,
    SV_DIS, SV_DIS_1, SV_DIS_2, SV_DIS_3,
    MT_DIS, MT_DIS_1, MT_DIS_2, MT_DIS_3,
    MR_DIS, MR_DIS_1, MR_DIS_2, MR_DIS_3,
    // Velocity
    VEL_FRAME, VEL_FRAME_END,
    AOI_VEL_1, AOI_VEL_2, AOI_VEL_3,
    SV_VEL, SV_VEL_1, SV_VEL_2, SV_VEL_3,
    // Residual
    RES_FRAME, RES_FRAME_END,
    AOI_RES_1, AOI_RES_2, AOI_RES_3,
    SV_RES, SV_RES_1, SV_RES_2, SV_RES_3,
    MT_RES, MT_RES_1, MT_RES_2, MT_RES_3,
    MR_RES, MR_RES_1, MR_RES_2, MR_RES_3,
    // Energy
    ENERGY_FRAME, ENERGY_FRAME_END,
    AOI_EN_1, AOI_EN_2, AOI_EN_3,
    AVG_EN, AVG_EN_1, AVG_EN_2, AVG_EN_3,
    MAX_EN, MAX_EN_1, MAX_EN_2, MAX_EN_3,
    // Additional labels
    A_DESCR, O_DESCR, I_DESCR
  };

  enum { // Eigen options
    // toggle
    EMODE_SOL,
    EMODE_BC,
    EMODE_DAMPED,
    EMODE_STRESS_STIFF,
    // double fields
    EMODE_INTV,
    EMODE_SHIFT_FACT,
    // integer fields
    NUM_EMODES
  };

  enum { // Equilibrium options
    // toggle
    EQL_ITER,
    EQL_STRESS_STIFF,
    // double fields
    EQL_ITER_TOL,
    ITER_STEP_SIZE,
    // label
    INFO_FIELD,
    // ramp-up toggles and fields
    RAMP_UP,
    RAMP_GRAV,
    RAMP_STEPS,
    RAMP_VMAX,
    RAMP_LENGTH,
    RAMP_DELAY
  };

  enum { // Output options
    AUTO_CURVE_EXPORT,
    AUTO_VTF_EXPORT,
    AUTO_ANIM,
    OVERWRITE
  };

  void setBasicMode(bool basicMode);

  //Field Settings
  //see FFuIOField for mode and policy
  void setAllFieldAcceptPolicy(int policy);
  void setAllDoubleDisplayMode(int mode, int precision, int zeroPrecision);
  virtual void setUIValues(const FFuaUIValues* values);
  // sensitivities
  virtual void setSensitivity(bool sens);

protected:
  void initWidgets(bool basicMode);
  void showBasicMode(bool doShow);
  void placeAdvancedButton(int width, int height);
  void onAdvBtnClicked();

private:
  void setAlphaLabel(int iop, const char* sub, double a1, double a2);
  void setMyUIValues(FuaAdvAnalysisOptionsValues* values, bool invalidValuesAlso);
  bool getMyUIValues(FuaAdvAnalysisOptionsValues* values);

  // actions
  virtual bool updateDBValues(bool = true);

  // callbacks
  void onDialogButtonClicked(int button);
  void onRunCloudBtnClicked();
  void onHelpBtnClicked();

  // Time toggles
  void onStartTimeAccepted(double);
  void onStopTimeAccepted(double);
  void onTimeIncQueryChanged(int,double);
  void onCutbackToggled(bool onOrOff);
  void onRestartToggled(bool onOrOff);
  void onDegradeSoilActivated();

  // Integration toggles
  void onNewmarkToggled(int button, bool onOrOff);
  void onAlphafAccepted(double alpha);
  void onAlphamAccepted(double alpha);
  void onToleranceToggled(int button, bool onOrOff);
  void onMatrixUpdateToggled(int buttom, bool onOrOff);

  // Convergence norm toggles (All-of, One-of, Ignore)
  void onAOItoggled(int iNorm, int i, bool onOrOff);

  // Displacement toggles
  void onSV_DIS1Toggled(bool onOrOff) { this->onAOItoggled(SV_DIS,1,onOrOff); }
  void onSV_DIS2Toggled(bool onOrOff) { this->onAOItoggled(SV_DIS,2,onOrOff); }
  void onSV_DIS3Toggled(bool onOrOff) { this->onAOItoggled(SV_DIS,3,onOrOff); }
  void onMT_DIS1Toggled(bool onOrOff) { this->onAOItoggled(MT_DIS,1,onOrOff); }
  void onMT_DIS2Toggled(bool onOrOff) { this->onAOItoggled(MT_DIS,2,onOrOff); }
  void onMT_DIS3Toggled(bool onOrOff) { this->onAOItoggled(MT_DIS,3,onOrOff); }
  void onMR_DIS1Toggled(bool onOrOff) { this->onAOItoggled(MR_DIS,1,onOrOff); }
  void onMR_DIS2Toggled(bool onOrOff) { this->onAOItoggled(MR_DIS,2,onOrOff); }
  void onMR_DIS3Toggled(bool onOrOff) { this->onAOItoggled(MR_DIS,3,onOrOff); }

  // Velocity toggles
  void onSV_VEL1Toggled(bool onOrOff) { this->onAOItoggled(SV_VEL,1,onOrOff); }
  void onSV_VEL2Toggled(bool onOrOff) { this->onAOItoggled(SV_VEL,2,onOrOff); }
  void onSV_VEL3Toggled(bool onOrOff) { this->onAOItoggled(SV_VEL,3,onOrOff); }

  // Residual toggles
  void onSV_RES1Toggled(bool onOrOff) { this->onAOItoggled(SV_RES,1,onOrOff); }
  void onSV_RES2Toggled(bool onOrOff) { this->onAOItoggled(SV_RES,2,onOrOff); }
  void onSV_RES3Toggled(bool onOrOff) { this->onAOItoggled(SV_RES,3,onOrOff); }
  void onMT_RES1Toggled(bool onOrOff) { this->onAOItoggled(MT_RES,1,onOrOff); }
  void onMT_RES2Toggled(bool onOrOff) { this->onAOItoggled(MT_RES,2,onOrOff); }
  void onMT_RES3Toggled(bool onOrOff) { this->onAOItoggled(MT_RES,3,onOrOff); }
  void onMR_RES1Toggled(bool onOrOff) { this->onAOItoggled(MR_RES,1,onOrOff); }
  void onMR_RES2Toggled(bool onOrOff) { this->onAOItoggled(MR_RES,2,onOrOff); }
  void onMR_RES3Toggled(bool onOrOff) { this->onAOItoggled(MR_RES,3,onOrOff); }

  // Energy toggles
  void onAVG_EN1Toggled(bool onOrOff) { this->onAOItoggled(AVG_EN,1,onOrOff); }
  void onAVG_EN2Toggled(bool onOrOff) { this->onAOItoggled(AVG_EN,2,onOrOff); }
  void onAVG_EN3Toggled(bool onOrOff) { this->onAOItoggled(AVG_EN,3,onOrOff); }
  void onMAX_EN1Toggled(bool onOrOff) { this->onAOItoggled(MAX_EN,1,onOrOff); }
  void onMAX_EN2Toggled(bool onOrOff) { this->onAOItoggled(MAX_EN,2,onOrOff); }
  void onMAX_EN3Toggled(bool onOrOff) { this->onAOItoggled(MAX_EN,3,onOrOff); }

  // Eigenvalue solution toggles
  void onEModeSolToggled(bool onOrOff);
  void onNumModesAccepted(int);

  // Initial Equilibrium toggles
  void onEqlIterToggled(bool onOrOff);
  void onRampUpToggled(bool onOrOff);

  // Output toggles
  void onAutoCurveExportFileChanged(const std::string&, int);
  void onAutoVTFFileChanged(const std::string&, int);
  void onOverwriteToggled(bool);

  // Basic toggles
  void onDSToggled(bool onOrOff);
  void onStopToggled(bool onOrOff);
  void onQSToggled(bool onOrOff);
  void onQRadio1Toggled(bool onOrOff);
  void onQRadio2Toggled(bool onOrOff);

protected:
  FFuLabel* labImgTop;
  FFuPushButton* btnAdvanced;
  FFuPushButton* btnRunCloud;
  FFuPushButton* btnHelp;
  FFuIOField* addOptions;

  FFuTabbedWidgetStack* tabStack;
  std::array<FFuComponentBase*,NOPTIONS> options;
  std::array<RadioButtonMap   ,NOPTIONS> radioButtons;
  std::array<ToggleButtonMap  ,NOPTIONS> toggleButtons;
  std::array<IOFieldMap       ,NOPTIONS> doubleFields;
  std::array<IOFieldMap       ,NOPTIONS> integerFields;
  std::array<LabelMap         ,NOPTIONS> labels;
  std::array<LabelFrameMap    ,NOPTIONS> labelFrames;
  std::array<BoolMap          ,NOPTIONS> isTopOfRadioLabel;

  FuiQueryInputField* myAdvTimeIncQueryField;
  FuiQueryInputField* myBasTimeIncQueryField;

  FFuLogicButtonGroup newmarkButtonGroup;
  FFuLogicButtonGroup toleranceButtonGroup;
  FFuLogicButtonGroup matrixUpdateButtonGroup;

  FFuOptionMenu*      shadowPosAlgMenu;
  FFuPushButton*      degradeSoilButton;

  FFuFileBrowseField* autoCurveExportField;
  FFuFileBrowseField* autoVTFField;

private:
  bool myBasicMode; //!< If \e true, the Basic options sheet is shown
  bool hasVTFfield; //!< If \e true, automatic VTF export is licensed
};


class FuaAdvAnalysisOptionsValues : public FFuaUIValues
{
public:
  // These members correspond with the ui widgets with similar names.
  // Since they use maps they have a special rule:
  // When used in the ui code, the ui widgets have control,
  // i.e., they ask for their values, not the other way round.

  std::array<DoubleMap ,FuiAdvAnalysisOptions::NOPTIONS> doubleValues;
  std::array<IntegerMap,FuiAdvAnalysisOptions::NOPTIONS> integerValues;
  std::array<BoolMap   ,FuiAdvAnalysisOptions::NOPTIONS> valueStatus;
  std::array<BoolMap   ,FuiAdvAnalysisOptions::NOPTIONS> toggleValues;
  std::array<IntegerMap,FuiAdvAnalysisOptions::NOPTIONS> optionMenuValues;

  double myTimeIncValue = 0.0;
  FFuaQueryBase* myTimeIncQuery = NULL;
  FmModelMemberBase* mySelectedTimeEngine = NULL;
  FFaDynCB1<FuiQueryInputFieldValues&> myEditTimeIncEngineCB;

  bool haveFreqDomainLoads = false;

  std::string addOptions;

  std::string modelFilePath;
  std::string autoCurveExportFileName;

  enum {
    RPC_LITTLE_ENDIAN,
    RPC_BIG_ENDIAN,
    ASCII_MULTI_COLUMN
  };

  int         curveFileFormat = RPC_LITTLE_ENDIAN;
  int         autoVTFFileType = 0;
  std::string autoVTFFileName;
};

#endif
