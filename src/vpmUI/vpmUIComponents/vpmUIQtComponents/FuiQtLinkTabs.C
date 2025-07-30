// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "FFuLib/FFuQtComponents/FFuQtScale.H"
#include "FFuLib/FFuQtComponents/FFuQtTable.H"
#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelField.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuQtComponents/FFuQtLabelFrame.H"
#include "FFuLib/FFuQtComponents/FFuQtFileBrowseField.H"
#include "FFuLib/FFuQtComponents/FFuQtPushButton.H"
#include "FFuLib/FFuQtComponents/FFuQtRadioButton.H"
#include "FFuLib/FFuQtComponents/FFuQtToggleButton.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPositionData.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtLinkTabs.H"


FuiQtReductionFrame::FuiQtReductionFrame()
{
  this->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  myLabel = new QLabel();
  myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  myLabel->setMinimumWidth(60);

  QLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(myLabel);
}


void FuiQtReductionFrame::setReducedVersion(int redVer)
{
  if (redVer > 0)
  {
    std::string reduced = "Reduced\n[" + std::to_string(redVer) + "]";
    myLabel->setText(reduced.c_str());
  }
  else
    myLabel->setText("Needs\nreduction");
}


FuiQtLinkModelSheet::FuiQtLinkModelSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  feModelBtn = new FFuQtRadioButton();
  genPartBtn = new FFuQtRadioButton();

  suppressInSolverLabel = new FFuQtLabel();
  suppressInSolverToggle = new FFuQtToggleButton();

  feModelFrame = new FFuQtLabelFrame();
  structDampFrame = new FFuQtLabelFrame();
  dynPropFrame = new FFuQtLabelFrame();

  repositoryFileField = new FFuQtLabelField();
  changeLinkBtn = new FFuQtPushButton();
  importedFileField = new FFuQtLabelField();
  unitConversionLabel = new FFuQtLabel();
  reductionFrame = new FuiQtReductionFrame();

  vizFrame = new FFuQtLabelFrame();
  vizField = new FFuQtLabelField();
  vizChangeBtn = new FFuQtPushButton();
  vizLabel = new FFuQtLabel();

  massProportionalField = new FFuQtLabelField();
  stiffProportionalField = new FFuQtLabelField();

  stiffScaleField = new FFuQtLabelField();
  massScaleField = new FFuQtLabelField();

  this->initWidgets();

  QWidget* qButtons = new QWidget();
  QBoxLayout* layout = new QVBoxLayout(qButtons);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(feModelBtn->getQtWidget());
  layout->addWidget(genPartBtn->getQtWidget());
  layout->addWidget(suppressInSolverToggle->getQtWidget());

  QWidget* qRepoFld = new QWidget();
  layout = new QHBoxLayout(qRepoFld);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(repositoryFileField->getQtWidget());
  layout->addWidget(changeLinkBtn->getQtWidget());

  QWidget* qImportFld = new QWidget();
  layout = new QHBoxLayout(qImportFld);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(importedFileField->getQtWidget());
  layout->addWidget(unitConversionLabel->getQtWidget());

  layout = new QVBoxLayout(feModelFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(qRepoFld);
  layout->addWidget(qImportFld);

  QWidget* qVizFld = new QWidget();
  layout = new QHBoxLayout(qVizFld);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(vizField->getQtWidget());
  layout->addWidget(vizChangeBtn->getQtWidget());

  layout = new QVBoxLayout(vizFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(qVizFld);
  layout->addWidget(vizLabel->getQtWidget());

  QWidget* qUpper = new QWidget();
  layout = new QHBoxLayout(qUpper);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qButtons);
  layout->addWidget(feModelFrame->getQtWidget());
  layout->addWidget(vizFrame->getQtWidget());
  layout->addWidget(reductionFrame->getQtWidget());

  layout = new QVBoxLayout(structDampFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(massProportionalField->getQtWidget());
  layout->addWidget(stiffProportionalField->getQtWidget());

  layout = new QVBoxLayout(dynPropFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(stiffScaleField->getQtWidget());
  layout->addWidget(massScaleField->getQtWidget());

  QGridLayout* mainLayout = new QGridLayout(this);
  mainLayout->setContentsMargins(5,5,5,5);
  mainLayout->addWidget(qUpper, 0,0, 1,2);
  mainLayout->addWidget(structDampFrame->getQtWidget(), 1,0);
  mainLayout->addWidget(dynPropFrame->getQtWidget(), 1,1);
  mainLayout->addWidget(suppressInSolverLabel->getQtWidget(), 2,0,1,2);
}


FuiQtLinkNodeSheet::FuiQtLinkNodeSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myFENodeField = new FFuQtLabelField();
  myNodePosition = new FuiQt3DPoint();

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(myFENodeField->getQtWidget());
  layout->addWidget(myNodePosition->getQtWidget());
}


FuiQtLinkRedOptSheet::FuiQtLinkRedOptSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  singCriterionField = new FFuQtLabelField();
  componentModesField = new FFuQtLabelField();
  eigValToleranceField = new FFuQtLabelField();

  consistentMassBtn = new FFuQtToggleButton();
  ignoreCSBtn = new FFuQtToggleButton();
  expandMSBtn = new FFuQtToggleButton();

  massFactBtn = new FFuQtRadioButton();
  stiffFactBtn = new FFuQtRadioButton();

  singlePrecisionBtn = new FFuQtRadioButton();
  doublePrecisionBtn = new FFuQtRadioButton();

  reductionFrame = new FuiQtReductionFrame();

  this->initWidgets();

  QWidget* qLeft = new QWidget();
  QBoxLayout* layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(singCriterionField->getQtWidget());
  layout->addWidget(componentModesField->getQtWidget());
  layout->addWidget(eigValToleranceField->getQtWidget());
  layout->addWidget(consistentMassBtn->getQtWidget());
  layout->addWidget(ignoreCSBtn->getQtWidget());
  layout->addWidget(expandMSBtn->getQtWidget());

  QGroupBox* eigValFactFrame = new QGroupBox("Eigenvalue Factorization");
  layout = new QVBoxLayout(eigValFactFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(massFactBtn->getQtWidget());
  layout->addWidget(stiffFactBtn->getQtWidget());

  QGroupBox* recPrecFrame = new QGroupBox("Recovery matrix storage precision");
  layout = new QVBoxLayout(recPrecFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(singlePrecisionBtn->getQtWidget());
  layout->addWidget(doublePrecisionBtn->getQtWidget());

  QWidget* qTopRight = new QWidget();
  layout = new QHBoxLayout(qTopRight);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(eigValFactFrame,1);
  layout->addWidget(reductionFrame->getQtWidget());

  QWidget* qRight = new QWidget();
  layout = new QVBoxLayout(qRight);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qTopRight);
  layout->addWidget(recPrecFrame);

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->addWidget(qLeft);
  layout->addWidget(qRight,1);
}


FuiQtLinkLoadSheet::FuiQtLinkLoadSheet(QWidget* parent, const char* name)
  : FFuQtTable(parent,name)
{
  this->showRowHeader(false);
  this->showColumnHeader(true);
  this->FFuTable::setNumberColumns(3,true);
  this->FFuTable::setColumnLabel(0,"Load Case"     ,1.0);
  this->FFuTable::setColumnLabel(1,"Delay"         ,1.0);
  this->FFuTable::setColumnLabel(2,"Load Amplitude",6.0);
  this->setSelectionPolicy(FFuTable::NO_SELECTION);
}


void FuiQtLinkLoadSheet::buildDynamicWidgets(const FuiLinkValues& values)
{
  if (values.loadCases.empty()) return; // don't touch if no load cases at all

  int numCols = this->getNumberColumns();
  int numRows = this->getNumberRows();
  size_t numLC = values.loadCases.size();
  for (int row = numLC; row < numRows; row++)
    for (int col = 0; col < numCols; col++)
      this->clearCellContents(row, col);

  this->setNoLoadCases(numLC);
  this->setNumberRows(numLC);
  this->initWidgets(values);

  for (size_t i = numRows; i < numLC; i++)
  {
    this->insertWidget(i, 0, loadCase[i]);
    this->insertWidget(i, 1, delay[i]);
    this->insertWidget(i, 2, loadFact[i]);
    this->setRowHeight(i, 20);
  }
}


void FuiQtLinkLoadSheet::setNoLoadCases(unsigned int nlc)
{
  unsigned int olc = loadFact.size();
  loadCase.resize(nlc,NULL);
  loadFact.resize(nlc,NULL);
  delay.resize(nlc,NULL);

  for (unsigned int i = olc; i < nlc; i++)
  {
    loadCase[i] = new FFuQtLabel(this);
    loadFact[i] = new FuiQtQueryInputField(this);
    loadFact[i]->setBehaviour(FuiQueryInputField::REF_NUMBER);
    delay[i] = new FFuQtIOField(this);
  }
}


void FuiQtLinkLoadSheet::resizeEvent(QResizeEvent* e)
{
  this->QWidget::resizeEvent(e);
  this->updateColumnWidths();
}


FuiQtGenericPartCGSheet::FuiQtGenericPartCGSheet(QWidget* parent,
                                                 const char* name)
  : FFuQtWidget(parent,name)
{
  posData = new FuiQtPositionData();
  condenseCGToggle = new FFuQtToggleButton();

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(posData->getQtWidget());
  layout->addWidget(condenseCGToggle->getQtWidget(),1,
                    Qt::AlignVCenter | Qt::AlignHCenter);
}


FuiQtGenericPartMassSheet::FuiQtGenericPartMassSheet(QWidget* parent,
                                                     const char* name)
  : FFuQtWidget(parent,name)
{
  calculateMassPropExplicitBtn = new FFuQtRadioButton();
  calculateMassPropFEBtn = new FFuQtRadioButton();
  calculateMassPropGeoBtn = new FFuQtRadioButton();
  materialField = new FuiQtQueryInputField(NULL);
  inertiaRefMenu = new FFuQtOptionMenu();

  massField = new FFuQtLabelField();
  for (FFuIOField*& field : inertias)
    field = new FFuQtIOField();

  this->initWidgets();

  QWidget* qLeft = new QWidget();
  QBoxLayout* layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(1);
  layout->addWidget(calculateMassPropExplicitBtn->getQtWidget());
  layout->addWidget(calculateMassPropFEBtn->getQtWidget());
  layout->addWidget(calculateMassPropGeoBtn->getQtWidget());
  layout->addStretch(2);
  layout->addWidget(new QLabel("Material"));
  layout->addWidget(materialField->getQtWidget());
  layout->addStretch(1);
  layout->addWidget(new QLabel("Inertia reference"));
  layout->addWidget(inertiaRefMenu->getQtWidget());

  QGroupBox* qRight = new QGroupBox("Mass and inertias");
  QGridLayout* gl = new QGridLayout(qRight);
  gl->setContentsMargins(5,0,5,5);
  gl->setHorizontalSpacing(10);
  gl->addWidget(massField->getQtWidget(), 0,0,1,4);
  gl->addWidget(new QLabel("Ixx"), 1,0);
  gl->addWidget(new QLabel("Ixy"), 2,0);
  gl->addWidget(new QLabel("Ixz"), 3,0);
  gl->addWidget(new QLabel("Iyy"), 2,1);
  gl->addWidget(new QLabel("Iyz"), 3,1);
  gl->addWidget(new QLabel("Izz"), 3,2);
  gl->addWidget(inertias[IXX]->getQtWidget(), 1,3);
  gl->addWidget(inertias[IXY]->getQtWidget(), 2,3);
  gl->addWidget(inertias[IXZ]->getQtWidget(), 3,3);
  gl->addWidget(inertias[IYY]->getQtWidget(), 2,4);
  gl->addWidget(inertias[IYZ]->getQtWidget(), 3,4);
  gl->addWidget(inertias[IZZ]->getQtWidget(), 3,5);

  layout = new QHBoxLayout(this);
  layout->addWidget(qLeft,2);
  layout->addWidget(qRight,3);
}


FuiQtGenericPartStiffSheet::FuiQtGenericPartStiffSheet(QWidget* parent,
                                                       const char* name)
  : FFuQtWidget(parent,name)
{
  defaultStiffTypeBtn = new FFuQtRadioButton();
  nodeStiffTypeBtn    = new FFuQtRadioButton();

  ktField = new FFuQtLabelField();
  krField = new FFuQtLabelField();

  stiffDescrLabel = new FFuQtLabel();

  this->initWidgets();

  QGroupBox* stiffTypeFrame = new QGroupBox("Type");
  QBoxLayout* layout = new QVBoxLayout(stiffTypeFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(defaultStiffTypeBtn->getQtWidget());
  layout->addWidget(nodeStiffTypeBtn->getQtWidget());

  QGroupBox* stiffPropsFrame = new QGroupBox("Properties");
  layout = new QVBoxLayout(stiffPropsFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(ktField->getQtWidget());
  layout->addWidget(krField->getQtWidget());
  layout->addWidget(stiffDescrLabel->getQtWidget());

  layout = new QHBoxLayout(this);
  layout->addWidget(stiffTypeFrame,2);
  layout->addWidget(stiffPropsFrame,3);
}


FuiQtHydrodynamicsSheet::FuiQtHydrodynamicsSheet(QWidget* parent,
                                                 const char* name)
  : FFuQtWidget(parent,name)
{
  buoyancyToggle = new FFuQtToggleButton();
  buoyancyLabel = new FFuQtLabel();

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(buoyancyToggle->getQtWidget());
  layout->addWidget(buoyancyLabel->getQtWidget());
}


FuiQtMeshingSheet::FuiQtMeshingSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  materialField = new FuiQtQueryInputField(NULL);
  minsizeField = new FFuQtLabelField();

  linearBtn = new FFuQtRadioButton();
  parabolicBtn = new FFuQtRadioButton();

  FFuQtScale* qtScale;
  qualityScale = qtScale = new FFuQtScale();
  qtScale->setOrientation(Qt::Horizontal);

  meshBtn = new FFuQtPushButton();

  noElmsLabel = new FFuQtLabel();
  noNodesLabel = new FFuQtLabel();

  this->initWidgets();

  QWidget* qLeft = new QWidget();
  QBoxLayout* layout = new QVBoxLayout(qLeft);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Material"));
  layout->addWidget(materialField->getQtWidget());
  layout->addWidget(noElmsLabel->getQtWidget());
  layout->addWidget(noNodesLabel->getQtWidget());
  layout->addStretch(1);

  QWidget* qQuality = new QWidget();
  QGridLayout* gl = new QGridLayout(qQuality);
  gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(20);
  gl->setColumnStretch(1,1);
  gl->setColumnStretch(2,1);
  gl->addWidget(new QLabel("Loose"), 0,1);
  gl->addWidget(new QLabel("Strong"), 0,2, Qt::AlignRight);
  gl->addWidget(new QLabel("Angle control"), 1,0);
  gl->addWidget(qtScale, 1,1,1,2);

  QWidget* qGenerate = new QWidget();
  gl = new QGridLayout(qGenerate);
  gl->setContentsMargins(0,0,0,0);
  gl->setHorizontalSpacing(20);
  gl->addWidget(meshBtn->getQtWidget(), 0,0,2,1);
  gl->addWidget(linearBtn->getQtWidget(), 0,1);
  gl->addWidget(parabolicBtn->getQtWidget(), 1,1);

  QWidget* qRight = new QWidget();
  layout = new QVBoxLayout(qRight);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qQuality);
  layout->addWidget(minsizeField->getQtWidget());
  layout->addWidget(qGenerate);
  layout->addStretch(1);

  layout = new QHBoxLayout(this);
  layout->setSpacing(20);
  layout->addWidget(qLeft);
  layout->addWidget(qRight);
}


FuiQtAdvancedLinkOptsSheet::FuiQtAdvancedLinkOptsSheet(QWidget* parent,
                                                       const char* name)
  : FFuQtWidget(parent,name)
{
  coordSysOptionMenu = new FFuQtOptionMenu();
  centripOptionMenu = new FFuQtOptionMenu();

  recoverStressToggle = new FFuQtToggleButton();
  recoverGageToggle = new FFuQtToggleButton();

  extResToggle = new FFuQtToggleButton();
  extResField = new FFuQtFileBrowseField(NULL);

  this->initWidgets();

  QWidget* qCoordSysMenu = new QWidget();
  QLabel* qCoordSysLabel = new QLabel("Positioning algorithm for the co-\n"
                                      "rotated reference coordinate system:");
  coordSysOptionMenu->setMinHeight(qCoordSysLabel->sizeHint().height()-3);
  QBoxLayout* layout = new QHBoxLayout(qCoordSysMenu);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(qCoordSysLabel);
  layout->addWidget(coordSysOptionMenu->getQtWidget(),1);

  QWidget* qCentripMenu = new QWidget();
  layout = new QHBoxLayout(qCentripMenu);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(new QLabel("Centripital force correction:"));
  layout->addWidget(centripOptionMenu->getQtWidget(),1);

  QWidget* qExtRes = new QWidget();
  layout = new QHBoxLayout(qExtRes);
  layout->setContentsMargins(0,0,0,0);
  layout->addWidget(extResToggle->getQtWidget());
  layout->addWidget(extResField->getQtWidget());

  layout = new QVBoxLayout(this);
  layout->setSpacing(1);
  layout->addWidget(qCoordSysMenu);
  layout->addWidget(qCentripMenu);
  layout->addWidget(recoverStressToggle->getQtWidget());
  layout->addWidget(recoverGageToggle->getQtWidget());
  layout->addWidget(qExtRes);
}


FuiQtNonlinearLinkOptsSheet::FuiQtNonlinearLinkOptsSheet(QWidget* parent,
                                                         const char* name)
  : FFuQtWidget(parent,name)
{
  useNonlinearToggle = new FFuQtToggleButton();
  numberOfSolutionsField = new FFuQtLabelField();
  nonlinearInputFileField = new FFuQtFileBrowseField(NULL);

  this->initWidgets();

  QLayout* layout = new QVBoxLayout(this);
  layout->addWidget(new QLabel("Capturing nonlinear behaviour using CFEM"));
  layout->addWidget(useNonlinearToggle->getQtWidget());
  layout->addWidget(numberOfSolutionsField->getQtWidget());
  layout->addWidget(nonlinearInputFileField->getQtWidget());
}
