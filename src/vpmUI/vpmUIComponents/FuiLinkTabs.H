// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_LINK_TABS_H
#define FUI_LINK_TABS_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuLogicButtonGroup.H"
#include "FFaLib/FFaAlgebra/FFaTensor3.H"
#include "FFaLib/FFaAlgebra/FFaVec3.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <array>

class FFuLabel;
class FFuLabelFrame;
class FFuLabelField;
class FFuPushButton;
class FFuRadioButton;
class FFuToggleButton;
class FFuOptionMenu;
class FFuFileBrowseField;
class FFuIOField;
class FFuTable;
class FFuScale;
class FFuScrolledList;
class FFuaQueryBase;
class FuiPositionData;
class Fui3DPoint;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FuiDynamicProperties;
class FmIsPositionedBase;
class FmModelMemberBase;


struct FuiLinkValues
{
  // model tab
  bool    locked = false;
  bool    suppressInSolver = false;
  bool    useGenericPart = false;
  bool    usingFEModelViz = false;
  std::string repositoryFile;
  std::string importedFile;
  std::string vizFile;
  std::string unitConversion = "No unit conversion";
  int    reducedVersionNumber = 0;
  double massDamping = 0.0;
  double stiffDamping = 0.0;
  double stiffScale = 1.0;
  double massScale = 1.0;
  bool   allowChange = true;
  bool   allowChangeViz = true;

  // reduction options
  double singularityCrit = 1.0e-12;
  double eigValTolerance = 1.0e-8;
  bool   consistentMassMx = false;
  bool   ignoreCheckSum   = false;
  bool   expandModeShapes = false;
  bool   factorStiffMx    = false;
  int    compModes = 0;
  int    recoveryMxPrec = 2;

  // load options
  std::vector<int>                loadCases;
  std::vector<double>             loadFactors;
  std::vector<double>             loadDelays;
  std::vector<FmModelMemberBase*> selectedLoadEngines;
  FFuaQueryBase*                  loadEngineQuery = NULL;

  // generic part options
  bool       genericPartNoCGTriad = false;
  double     genericPartMass = 0.0;
  int        useCalculatedMass = 0;
  char       canCalculateMass = false;
  FFaTensor3 genericPartInertia;
  int        inertiaRef = 0;
  int        genericPartStiffType = 0;
  double     genericPartKT = 0.0;
  double     genericPartKR = 0.0;

  FFuaQueryBase*     materialQuery = NULL;
  FmModelMemberBase* selectedMaterial = NULL;

  // hydrodynamics options
  bool   buoyancy = false;

  // meshing options
  bool   enableMeshing = false;
  double quality = 0.0;
  int    minSize = 0;
  bool   parabolic = false;
  int    noNodes = 0;
  int    noElms = 0;

  // advanced options
  int    coordSysOption = 0;
  int    centripOption = 0;
  int    recoveryOption = 0;
  bool   ignoreRecovery = false;
  char   extResSwitch = false;
  std::string extResFileName;
  std::string modelFilePath;

  // nonlinear link options
  int    numNonlinear = 0;
  bool   useNonlinearSwitch = false;
  std::string nonlinearInputFile;

  // FE node data
  int    feNode = -1;
  FaVec3 feNodePos;
  std::vector<std::string> elmList;

  // callback functions
  FFaDynCB1<FuiQueryInputFieldValues&> editLoadEngineCB;
  FFaDynCB1<bool>                      linkMeshCB;
  FFaDynCB0                            linkChangeCB;
  FFaDynCB2<const std::string&,bool>   linkChangeVizCB;
};


class FuiReductionFrame : virtual public FFuComponentBase
{
public:
  virtual void setReducedVersion(int redVer) = 0;
};


class FuiLinkModelSheet : virtual public FFuComponentBase
{
public:
  FuiLinkModelSheet() { IAmSensitive = ICanChange = ICanChangeViz = true; }

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);
  void setChangeLinkCB(const FFaDynCB0& aDynCB);
  void setChangeGPVizCB(const FFaDynCB2<const std::string&,bool>& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuRadioButton*     feModelBtn;
  FFuRadioButton*     genPartBtn;
  FFuLogicButtonGroup linkTypeBtnGroup;
  FFuToggleButton*    suppressInSolverToggle;
  FFuLabel*           suppressInSolverLabel;

  FFuLabelFrame* feModelFrame;
  FFuLabelField* repositoryFileField;
  FFuLabelField* importedFileField;
  FFuLabel*      unitConversionLabel;
  FFuPushButton* changeLinkBtn;

  FFuLabelFrame* vizFrame;
  FFuLabelField* vizField;
  FFuPushButton* vizChangeBtn;
  FFuLabel*      vizLabel;

  FuiReductionFrame* reductionFrame;
  FuiDynamicProperties* dynamicProps;

private:
  void onSuppressInSolverToggeled(bool);
  void onLinkTypeToggeled(int, bool);
  void onDoubleChanged(double);
  void onIntChanged(int);
  void onChangeViz();
  void onVizFileTyped(const std::string&);
  void update();

  bool IAmSensitive;
  bool ICanChange;
  bool ICanChangeViz;
  FFaDynCB0 valuesChangedCB;
  FFaDynCB0 changeLinkCB;
  FFaDynCB2<const std::string&,bool> changeGPViz;
};


/*!
  The Link Origin Sheet consists only of the FuiPositionData widget.
  Therefore, defining an alias directly to this class name
  instead of a separate widget class with a single member.
*/
using FuiLinkOriginSheet = FuiPositionData;


class FuiLinkNodeSheet : virtual public FFuComponentBase
{
public:
  FuiLinkNodeSheet() { myViewedObj = NULL; }

  void setViewedObj(FmIsPositionedBase* obj) { myViewedObj = obj; }
  void setValues(const FuiLinkValues& values);

protected:
  void initWidgets();

  Fui3DPoint*      myNodePosition;
  FFuLabel*        myElementLabel;
  FFuScrolledList* myElements;

private:
  void onPosRefChanged(bool toGlobal);

  FmIsPositionedBase* myViewedObj;
  FaVec3              myPos;
};


class FuiLinkRedOptSheet : virtual public FFuComponentBase
{
public:
  FuiLinkRedOptSheet() { IAmLocked = false; IAmSensitive = true; }

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuLabelField* singCriterionField;
  FFuLabelField* componentModesField;
  FFuLabelField* eigValToleranceField;
  FFuRadioButton* massFactBtn;
  FFuRadioButton* stiffFactBtn;
  FFuLogicButtonGroup eigValFactorizationGroup;
  FFuRadioButton* singlePrecisionBtn;
  FFuRadioButton* doublePrecisionBtn;
  FFuLogicButtonGroup recoveryMatrixPrecisionGroup;
  FFuToggleButton* consistentMassBtn;
  FFuToggleButton* ignoreCSBtn;
  FFuToggleButton* expandMSBtn;
  FuiReductionFrame* reductionFrame;

private:
  void onDoubleChanged(double);
  void onIntChanged(int);
  void onBtnToggeled(bool);
  void onBtnToggeled(int, bool);
  void onIgnoreCSBtnToggeled(bool);

  bool IAmLocked;
  bool IAmSensitive;
  FFaDynCB0 valuesChangedCB;
};


class FuiLinkLoadSheet : virtual public FFuComponentBase
{
public:
  FuiLinkLoadSheet() { IAmSensitive = true; }

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void buildDynamicWidgets(const FuiLinkValues& values) = 0;
  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets(const FuiLinkValues& values);

  virtual void setNoLoadCases(unsigned int nlc) = 0;

  std::vector<FFuLabel*>           loadCase;
  std::vector<FFuIOField*>         delay;
  std::vector<FuiQueryInputField*> loadFact;

private:
  void onDoubleChanged(double);
  void onIntDoubleChanged(int, double);

  bool IAmSensitive;
  FFaDynCB0 valuesChangedCB;
};


class FuiGenericPartCGSheet : virtual public FFuComponentBase
{
public:
  FuiGenericPartCGSheet() { IAmUsingCalculatedMass = IAmSensitive = true; }

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);
  void setEditedObjs(const std::vector<FmModelMemberBase*>& objs = {});

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FuiPositionData* posData;
  FFuToggleButton* condenseCGToggle;

private:
  void onCondenseCGToggeled(bool);
  void updateSensitivity();

  bool IAmUsingCalculatedMass;
  bool IAmSensitive;
  FFaDynCB0 valuesChangedCB;
};


class FuiGenericPartMassSheet : virtual public FFuComponentBase
{
public:
  FuiGenericPartMassSheet() { IAmSensitive = true; ICanCalculateMass = false; }

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuRadioButton* calculateMassPropExplicitBtn;
  FFuRadioButton* calculateMassPropFEBtn;
  FFuRadioButton* calculateMassPropGeoBtn;
  FFuLogicButtonGroup calculateMassPropGroup;

public:
  FuiQueryInputField* materialField;

protected:
  FFuOptionMenu* inertiaRefMenu;
  FFuLabelField* massField;

  enum {IXX, IYY, IZZ, IXY, IXZ, IYZ, NINERTIAS};

  std::array<FFuIOField*,NINERTIAS> inertias;

private:
  void onDoubleChanged(double);
  void onOptionSelected(int);
  void onCalculateMassPropToggeled(int, bool);
  void updateSensitivity();

  bool IAmSensitive;
  char ICanCalculateMass;
  FFaDynCB0 valuesChangedCB;
};


class FuiGenericPartStiffSheet : virtual public FFuComponentBase
{
public:
  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuLabelField* ktField;
  FFuLabelField* krField;

  FFuLabel* stiffDescrLabel;

  FFuLogicButtonGroup stiffTypeBtnGroup;
  FFuRadioButton* defaultStiffTypeBtn;
  FFuRadioButton* nodeStiffTypeBtn;

private:
  void onDoubleChanged(double);
  void onOptionSelected(int);
  void onStiffTypeBtnToggeled(int, bool);

  void update();

  FFaDynCB0 valuesChangedCB;
};


class FuiHydrodynamicsSheet : virtual public FFuComponentBase
{
public:
  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuToggleButton* buoyancyToggle;
  FFuLabel*        buoyancyLabel;

private:
  void onOptionToggled(bool);

  FFaDynCB0 valuesChangedCB;
};


class FuiMeshingSheet : virtual public FFuComponentBase
{
public:
  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);
  void setMeshLinkCB(const FFaDynCB1<bool>& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

public:
  FuiQueryInputField* materialField;

protected:
  FFuLabelField*      minsizeField;

  FFuRadioButton*     linearBtn;
  FFuRadioButton*     parabolicBtn;
  FFuLogicButtonGroup orderBtnGroup;

  FFuScale* qualityScale;

  FFuPushButton* meshBtn;

  FFuLabel* noElmsLabel;
  FFuLabel* noNodesLabel;

private:
  void onIntChanged(int);
  void onChanged();
  void onMeshLink();

  FFaDynCB0       valuesChangedCB;
  FFaDynCB1<bool> meshLinkCB;
};


class FuiAdvancedLinkOptsSheet : virtual public FFuComponentBase
{
public:
  FuiAdvancedLinkOptsSheet() { IAmSensitive = true; ICanRecover = false; }

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuOptionMenu* coordSysOptionMenu;
  FFuOptionMenu* centripOptionMenu;

  FFuToggleButton* recoverStressToggle;
  FFuToggleButton* recoverGageToggle;
  FFuToggleButton* ignoreRecoveryToggle;

  FFuToggleButton*    extResToggle;
  FFuFileBrowseField* extResField;

private:
  void onOptionSelected(int);
  void onOptionToggled(bool);
  void onExtResToggeled(bool);
  void onExtResFileChanged(const std::string&, int);

  FFaDynCB0 valuesChangedCB;

  bool IAmSensitive;
  char ICanRecover;
};


class FuiNonlinearLinkOptsSheet : virtual public FFuComponentBase
{
public:
  FuiNonlinearLinkOptsSheet() { IAmSensitive = true; }

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  void initWidgets();

  FFuToggleButton*    useNonlinearToggle;
  FFuLabelField*      numberOfSolutionsField;
  FFuFileBrowseField* nonlinearInputFileField;

private:
  void onOptionSelected(int);
  void onNonlinearToggeled(bool);
  void onNonlinearInputFileChanged(const std::string&, int);
  void onIntChanged(int);

  FFaDynCB0 valuesChangedCB;

  bool IAmSensitive;
};

#endif
