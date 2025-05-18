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
#include "FFuLib/FFuQtComponents/FFuQtScrolledList.H"

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtPositionData.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQt3DPoint.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtDynamicProperties.H"
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

  repositoryFileField = new FFuQtLabelField();
  changeLinkBtn = new FFuQtPushButton();
  importedFileField = new FFuQtLabelField();
  unitConversionLabel = new FFuQtLabel();
  reductionFrame = new FuiQtReductionFrame();

  vizFrame = new FFuQtLabelFrame();
  vizField = new FFuQtLabelField();
  vizChangeBtn = new FFuQtPushButton();
  vizLabel = new FFuQtLabel();

  dynamicProps = new FuiQtDynamicProperties();

  this->initWidgets();

  QBoxLayout* layout1 = new QHBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->addWidget(repositoryFileField->getQtWidget());
  layout1->addWidget(changeLinkBtn->getQtWidget());

  QBoxLayout* layout2 = new QHBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(importedFileField->getQtWidget());
  layout2->addWidget(unitConversionLabel->getQtWidget());

  QBoxLayout* layout = new QVBoxLayout(feModelFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addLayout(layout1);
  layout->addLayout(layout2);

  layout1 = new QHBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->addWidget(vizField->getQtWidget());
  layout1->addWidget(vizChangeBtn->getQtWidget());

  layout = new QVBoxLayout(vizFrame->getQtWidget());
  layout->setContentsMargins(5,0,5,5);
  layout->addLayout(layout1);
  layout->addWidget(vizLabel->getQtWidget());

  layout2 = new QVBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addWidget(feModelBtn->getQtWidget());
  layout2->addWidget(genPartBtn->getQtWidget());
  layout2->addWidget(suppressInSolverToggle->getQtWidget());

  layout1 = new QHBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->addLayout(layout2);
  layout1->addWidget(feModelFrame->getQtWidget());
  layout1->addWidget(vizFrame->getQtWidget());
  layout1->addWidget(reductionFrame->getQtWidget());

  layout = new QVBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->addLayout(layout1);
  layout->addWidget(dynamicProps->getQtWidget());
  layout->addWidget(suppressInSolverLabel->getQtWidget());
}


FuiQtLinkNodeSheet::FuiQtLinkNodeSheet(QWidget* parent, const char* name)
  : FFuQtWidget(parent,name)
{
  myNodePosition = new FuiQt3DPoint(NULL,"FE node",true,true);
  myElementLabel = new FFuQtLabel();
  myElements     = new FFuQtScrolledList();

  this->initWidgets();

  QBoxLayout* layout1 = new QVBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->addWidget(myElementLabel->getQtWidget());
  layout1->addWidget(myElements->getQtWidget(),1);

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(myNodePosition->getQtWidget());
  layout->addSpacing(10);
  layout->addLayout(layout1,1);
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

  QLayout* layout1 = new QVBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->addWidget(singCriterionField->getQtWidget());
  layout1->addWidget(componentModesField->getQtWidget());
  layout1->addWidget(eigValToleranceField->getQtWidget());
  layout1->addWidget(consistentMassBtn->getQtWidget());
  layout1->addWidget(ignoreCSBtn->getQtWidget());
  layout1->addWidget(expandMSBtn->getQtWidget());

  QGroupBox* eigValFactFrame = new QGroupBox("Eigenvalue Factorization");
  QBoxLayout* layout = new QVBoxLayout(eigValFactFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(massFactBtn->getQtWidget());
  layout->addWidget(stiffFactBtn->getQtWidget());

  QGroupBox* recPrecFrame = new QGroupBox("Recovery matrix storage precision");
  layout = new QVBoxLayout(recPrecFrame);
  layout->setContentsMargins(5,0,5,5);
  layout->addWidget(singlePrecisionBtn->getQtWidget());
  layout->addWidget(doublePrecisionBtn->getQtWidget());

  QBoxLayout* layout3 = new QHBoxLayout();
  layout3->setContentsMargins(0,0,0,0);
  layout3->addWidget(eigValFactFrame,1);
  layout3->addWidget(reductionFrame->getQtWidget());

  QBoxLayout* layout2 = new QVBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addLayout(layout3);
  layout2->addWidget(recPrecFrame);

  layout = new QHBoxLayout(this);
  layout->setContentsMargins(5,5,5,5);
  layout->addLayout(layout1);
  layout->addLayout(layout2,1);
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

  QBoxLayout* layout1 = new QVBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->setSpacing(1);
  layout1->addWidget(calculateMassPropExplicitBtn->getQtWidget());
  layout1->addWidget(calculateMassPropFEBtn->getQtWidget());
  layout1->addWidget(calculateMassPropGeoBtn->getQtWidget());
  layout1->addStretch(2);
  layout1->addWidget(new QLabel("Material"));
  layout1->addWidget(materialField->getQtWidget());
  layout1->addStretch(1);
  layout1->addWidget(new QLabel("Inertia reference"));
  layout1->addWidget(inertiaRefMenu->getQtWidget());

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

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->addLayout(layout1,2);
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

  QBoxLayout* layout1 = new QVBoxLayout();
  layout1->setContentsMargins(0,0,0,0);
  layout1->addWidget(new QLabel("Material"));
  layout1->addWidget(materialField->getQtWidget());
  layout1->addWidget(noElmsLabel->getQtWidget());
  layout1->addWidget(noNodesLabel->getQtWidget());
  layout1->addStretch(1);

  QGridLayout* gl1 = new QGridLayout();
  gl1->setContentsMargins(0,0,0,0);
  gl1->setHorizontalSpacing(20);
  gl1->setColumnStretch(1,1);
  gl1->setColumnStretch(2,1);
  gl1->addWidget(new QLabel("Loose"), 0,1);
  gl1->addWidget(new QLabel("Strong"), 0,2, Qt::AlignRight);
  gl1->addWidget(new QLabel("Angle control"), 1,0);
  gl1->addWidget(qtScale, 1,1,1,2);

  QGridLayout* gl2 = new QGridLayout();
  gl2->setContentsMargins(0,0,0,0);
  gl2->setHorizontalSpacing(20);
  gl2->addWidget(meshBtn->getQtWidget(), 0,0,2,1);
  gl2->addWidget(linearBtn->getQtWidget(), 0,1);
  gl2->addWidget(parabolicBtn->getQtWidget(), 1,1);

  QBoxLayout* layout2 = new QVBoxLayout();
  layout2->setContentsMargins(0,0,0,0);
  layout2->addLayout(gl1);
  layout2->addWidget(minsizeField->getQtWidget());
  layout2->addLayout(gl2);
  layout2->addStretch(1);

  QBoxLayout* layout = new QHBoxLayout(this);
  layout->setSpacing(20);
  layout->addLayout(layout1);
  layout->addLayout(layout2);
}


FuiQtAdvancedLinkOptsSheet::FuiQtAdvancedLinkOptsSheet(QWidget* parent,
                                                       const char* name)
  : FFuQtWidget(parent,name)
{
  coordSysOptionMenu = new FFuQtOptionMenu();
  centripOptionMenu = new FFuQtOptionMenu();

  recoverStressToggle = new FFuQtToggleButton();
  recoverGageToggle = new FFuQtToggleButton();
  ignoreRecoveryToggle = new FFuQtToggleButton();

  extResToggle = new FFuQtToggleButton();
  extResField = new FFuQtFileBrowseField(NULL);

  this->initWidgets();

  QBoxLayout* layout = new QVBoxLayout(this);
  layout->setSpacing(1);
  std::array<QBoxLayout*,5> layouts;
  for (QBoxLayout*& hl : layouts)
  {
    hl = new QHBoxLayout();
    hl->setContentsMargins(0,0,0,0);
    hl->setSpacing(10);
    layout->addLayout(hl);
  }

  layouts[0]->addWidget(new QLabel("Positioning algorithm for the co-\n"
                                   "rotated reference coordinate system:"));
  layouts[1]->addWidget(new QLabel("Centripital force correction:"));
  layouts[0]->addWidget(coordSysOptionMenu->getQtWidget(),1);
  layouts[1]->addWidget(centripOptionMenu->getQtWidget(),1);
  layouts[2]->addWidget(recoverStressToggle->getQtWidget(),1);
  layouts[2]->addWidget(ignoreRecoveryToggle->getQtWidget());
  layouts[3]->addWidget(recoverGageToggle->getQtWidget());
  layouts[4]->addWidget(extResToggle->getQtWidget());
  layouts[4]->addWidget(extResField->getQtWidget());
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
