// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_LINK_TABS_H
#define FUI_LINK_TABS_H

#include "FFuLib/FFuBase/FFuMultUIComponent.H"
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
class FFuFrame;
class FFuTable;
class FFuScale;
class FFuaQueryBase;
class FuiLinkValues;
class FuiPositionData;
class Fui3DPoint;
class FuiQueryInputField;
class FuiQueryInputFieldValues;
class FmIsPositionedBase;
class FmModelMemberBase;


class FuiLinkModelSheet : virtual public FFuMultUIComponent
{
public:
  FuiLinkModelSheet() { IAmSensitive = ICanChange = ICanChangeViz = true; }
  virtual ~FuiLinkModelSheet() {}

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);
  void setChangeLinkCB(const FFaDynCB0& aDynCB);
  void setUpdateLinkCB(const FFaDynCB0& aDynCB);

  void setChangeGPVizCB(const FFaDynCB2<const std::string&,bool>& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuRadioButton*     feModelBtn;
  FFuRadioButton*     genPartBtn;
  FFuLogicButtonGroup linkTypeBtnGroup;
  FFuToggleButton*    suppressInSolverToggle;
  FFuLabel*           suppressInSolverLabel;

  FFuLabelFrame* feModelFrame;
  FFuLabelFrame* structDampFrame;
  FFuLabelFrame* dynPropFrame;
  FFuLabelField* repositoryFileField;
  FFuLabelField* importedFileField;
  FFuLabelField* massProportionalField;
  FFuLabelField* stiffProportionalField;
  FFuLabelField* stiffScaleField;
  FFuLabelField* massScaleField;
  FFuLabel*      unitConversionLabel;
  FFuLabel*      needsReductionLabel;
  FFuFrame*      needsReductionFrame;
  FFuPushButton* changeLinkBtn;

  FFuLabelFrame* vizFrame;
  FFuLabelField* vizField;
  FFuPushButton* vizChangeBtn;
  FFuLabel*      vizLabel;

private:
  void onSuppressInSolverToggeled(bool);
  void onLinkTypeToggeled(int, bool);
  void onDoubleChanged(double);
  void onIntChanged(int);
  void onChangeLink();
  void onChangeViz();
  void onVizFileTyped(const std::string&);
  void update();

  bool IAmSensitive;
  bool ICanChange;
  bool ICanChangeViz;
  FFaDynCB0 valuesChangedCB;
  FFaDynCB0 changeLinkCB;
  FFaDynCB0 updateLinkCB;
  FFaDynCB2<const std::string&,bool> changeGPViz;
};


class FuiLinkOriginSheet : virtual public FFuMultUIComponent
{
public:
  FuiLinkOriginSheet() {}
  virtual ~FuiLinkOriginSheet() {}

  void setEditedObjs(const std::vector<FmModelMemberBase*>& objs = {});

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);

  FuiPositionData* posData;
};


class FuiLinkNodeSheet : virtual public FFuMultUIComponent
{
public:
  FuiLinkNodeSheet() {}
  virtual ~FuiLinkNodeSheet() {}

  void setViewedObj(FmIsPositionedBase* obj) { myViewedObj = obj; }
  void setValues(const FuiLinkValues& values);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuLabelField* myFENodeField;
  Fui3DPoint*    myNodePosition;

private:
  void onPosRefChanged(bool toGlobal);

  FmIsPositionedBase* myViewedObj;
  FaVec3              myPos;
};


class FuiLinkRedOptSheet : virtual public FFuMultUIComponent
{
public:
  FuiLinkRedOptSheet() { IAmLocked = false; IAmSensitive = true; }
  virtual ~FuiLinkRedOptSheet() {}

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuLabelFrame* eigValFactFrame;
  FFuLabelFrame* recoveryMatrixPrecisionFrame;
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
  FFuLabel* needsReductionLabel;
  FFuFrame* needsReductionFrame;

private:
  void onDoubleChanged(double);
  void onIntChanged(int);
  void onBtnToggeled(bool);
  void onBtnToggeled(int, bool);
  void onIgnoreCSBtnToggeled(bool);
  void updateSensitivity();

  bool IAmLocked;
  bool IAmSensitive;
  FFaDynCB0 valuesChangedCB;
};


class FuiLinkLoadSheet : virtual public FFuMultUIComponent
{
public:
  FuiLinkLoadSheet() { IAmSensitive = true; }
  virtual ~FuiLinkLoadSheet() {}

  void buildDynamicWidgets(const FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void setNoLoadCases(unsigned int nlc) = 0;

  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuLabel*                   label;
  FFuTable*                   table;
  std::vector<FuiQueryInputField*> loadFact;
  std::vector<FFuIOField*>         delay;

private:
  void onDoubleChanged(double);
  void onIntDoubleChanged(int, double);

  bool IAmSensitive;
  FFaDynCB0 valuesChangedCB;
};


class FuiGenericPartCGSheet : virtual public FFuMultUIComponent
{
public:
  FuiGenericPartCGSheet() { IAmUsingCalculatedMass = IAmSensitive = true; }
  virtual ~FuiGenericPartCGSheet() {}

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);
  void setEditedObjs(const std::vector<FmModelMemberBase*>& objs = {});

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
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


class FuiGenericPartMassSheet : virtual public FFuMultUIComponent
{
public:
  FuiGenericPartMassSheet() { IAmSensitive = true; ICanCalculateMass = false; }
  virtual ~FuiGenericPartMassSheet() {}

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuRadioButton* calculateMassPropExplicitBtn;
  FFuRadioButton* calculateMassPropFEBtn;
  FFuRadioButton* calculateMassPropGeoBtn;
  FFuLogicButtonGroup calculateMassPropGroup;

  FFuLabel*           materialLabel;
public:
  FuiQueryInputField* materialField;

protected:
  FFuLabel*      inertiaRefLabel;
  FFuOptionMenu* inertiaRefMenu;

  FFuLabelFrame* massInertiaFrame;
  FFuLabelField* massField;

  enum {IXX, IYY, IZZ, IXY, IXZ, IYZ, NINERTIAS};

  std::array<FFuIOField*,NINERTIAS> inertias;
  std::array<FFuLabel*,NINERTIAS> inertiaLabels;

private:
  void onDoubleChanged(double);
  void onOptionSelected(int);
  void onCalculateMassPropToggeled(int, bool);
  void updateSensitivity();

  bool IAmSensitive;
  char ICanCalculateMass;
  FFaDynCB0 valuesChangedCB;
};


class FuiGenericPartStiffSheet : virtual public FFuMultUIComponent
{
public:
  FuiGenericPartStiffSheet() {}
  virtual ~FuiGenericPartStiffSheet() {}

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuLabelFrame* stiffTypeFrame;
  FFuLabelFrame* stiffPropFrame;
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


class FuiHydrodynamicsSheet : virtual public FFuMultUIComponent
{
public:
  FuiHydrodynamicsSheet() {}
  virtual ~FuiHydrodynamicsSheet() {}

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuToggleButton* buoyancyToggle;
  FFuLabel*        buoyancyLabel;

private:
  void onOptionToggled(bool);

  FFaDynCB0 valuesChangedCB;
};


class FuiMeshingSheet : virtual public FFuMultUIComponent
{
public:
  FuiMeshingSheet() {}
  virtual ~FuiMeshingSheet() {}

  void getValues(FuiLinkValues& values);
  void setValues(const FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);
  void setMeshLinkCB(const FFaDynCB1<bool>& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

public:
  FuiQueryInputField* materialField;

protected:
  FFuLabel*           materialLabel;
  FFuLabelField*      minsizeField;

  FFuRadioButton*     linearBtn;
  FFuRadioButton*     parabolicBtn;
  FFuLogicButtonGroup orderBtnGroup;

  FFuScale*               qualityScale;
  std::array<FFuLabel*,3> qualityLabel;

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


class FuiAdvancedLinkOptsSheet : virtual public FFuMultUIComponent
{
public:
  FuiAdvancedLinkOptsSheet() { IAmSensitive = true; ICanRecover = showExtRes = false; }
  virtual ~FuiAdvancedLinkOptsSheet() {}

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuLabel*      coordSysLabel;
  FFuOptionMenu* coordSysOptionMenu;

  FFuLabel*      centripLabel;
  FFuOptionMenu* centripOptionMenu;

  FFuToggleButton* recoverStressToggle;
  FFuToggleButton* recoverGageToggle;

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
  bool showExtRes;
};


class FuiNonlinearLinkOptsSheet : virtual public FFuMultUIComponent
{
public:
  FuiNonlinearLinkOptsSheet() { IAmSensitive = true; }
  virtual ~FuiNonlinearLinkOptsSheet() {}

  void setValues(const FuiLinkValues& values);
  void getValues(FuiLinkValues& values);

  void setValuesChangedCB(const FFaDynCB0& aDynCB);

  virtual void setSensitivity(bool sensitivity);

protected:
  virtual void placeWidgets(int width, int height);
  void initWidgets();

  FFuLabel*           nonlinearDescrLabel;
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


class FuiLinkValues
{
public:
  FuiLinkValues() {
    locked = false;
    suppressInSolver = false;
    useGenericPart = false;
    usingFEModelViz = false;
    unitConversion = "No unit conversion";
    reducedVersionNumber = 0;
    massDamping = 0.0;
    stiffDamping = 0.0;
    stiffScale = 1.0;
    massScale = 1.0;
    allowChange = true;
    allowChangeViz = true;
    singularityCrit = 1e-12;
    compModes = 0;
    eigValTolerance = 1e-8;
    consistentMassMx = false;
    ignoreCheckSum = false;
    expandModeShapes = false;
    factorStiffMx = false;
    recoveryMxPrec = 2;
    loadEngineQuery = NULL;
    genericPartNoCGTriad = false;
    genericPartMass = 0.0;
    useCalculatedMass = 0;
    canCalculateMass = false;
    inertiaRef = 0;
    genericPartStiffType = 0;
    genericPartKT = 0.0;
    genericPartKR = 0.0;
    materialQuery = NULL;
    selectedMaterial = NULL;
    buoyancy = false;
    enableMeshing = false;
    quality = 0.0;
    minSize = 0;
    parabolic = false;
    noNodes = 0;
    noElms = 0;
    coordSysOption = 0;
    centripOption = 0;
    recoveryOption = 0;
    extResSwitch = false;
    numNonlinear = 0;
    useNonlinearSwitch = false;
    feNode = -1;
  }

  // model tab
  bool   locked;
  bool   suppressInSolver;
  bool   useGenericPart;
  std::string repositoryFile;
  std::string importedFile;
  std::string vizFile;
  bool   usingFEModelViz;
  std::string unitConversion;
  int    reducedVersionNumber;
  double massDamping;
  double stiffDamping;
  double stiffScale;
  double massScale;
  bool   allowChange;
  bool   allowChangeViz;

  // reduction options
  double singularityCrit;
  int    compModes;
  double eigValTolerance;
  bool   consistentMassMx;
  bool   ignoreCheckSum;
  bool   expandModeShapes;
  bool   factorStiffMx;
  int    recoveryMxPrec;

  // load options
  std::vector<int>                loadCases;
  std::vector<double>             loadFactors;
  std::vector<double>             loadDelays;
  std::vector<FmModelMemberBase*> selectedLoadEngines;
  FFuaQueryBase*                  loadEngineQuery;

  // generic part options
  bool       genericPartNoCGTriad;
  double     genericPartMass;
  int        useCalculatedMass;
  char       canCalculateMass;
  FFaTensor3 genericPartInertia;
  int        inertiaRef;
  int        genericPartStiffType;
  double     genericPartKT;
  double     genericPartKR;

  FFuaQueryBase*     materialQuery;
  FmModelMemberBase* selectedMaterial;

  // hydrodynamics options
  bool   buoyancy;

  // meshing options
  bool   enableMeshing;
  double quality;
  int    minSize;
  bool   parabolic;
  int    noNodes;
  int    noElms;

  // advanced options
  int    coordSysOption;
  int    centripOption;
  int    recoveryOption;
  char   extResSwitch;
  std::string extResFileName;
  std::string modelFilePath;

  // nonlinear link options
  int    numNonlinear;
  bool   useNonlinearSwitch;
  std::string nonlinearInputFile;

  // FE node data
  int    feNode;
  FaVec3 feNodePos;

  // callback functions
  FFaDynCB1<FuiQueryInputFieldValues&> editLoadEngineCB;
  FFaDynCB1<bool>                      linkMeshCB;
  FFaDynCB0                            linkChangeCB;
  FFaDynCB2<const std::string&,bool>   linkChangeVizCB;
};

#endif
