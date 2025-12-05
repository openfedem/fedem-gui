// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSignalMapper>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QTreeView>
#include <QCloseEvent>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtBladeDefinition.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuCustom/inputTables/InputTable.H"
#include "FFuLib/FFuCustom/components/DataNode.H"
#include "FFuLib/FFuCustom/components/Blade.H"
#include "FFuLib/FFuCustom/components/BladeDrawer.H"
#include "FFuLib/FFuCustom/components/guiComponents/BladeSelector.H"
#include "FFuLib/FFuCustom/mvcModels/BladeSelectionModel.H"
#include "FFuLib/FFuCustom/components/renderers/BladeView.H"
#include "FFuLib/FFuCustom/components/renderers/AirfoilView.H"
#include "FFuLib/FFuCustom/inputTables/delegates/FileFieldDelegate.H"
#include "FFuLib/FFuCustom/inputTables/delegates/DoubleFieldDelegate.H"
#include "vpmDB/FmBladeProperty.H"
#include "vpmDB/FmTurbine.H"
#include "vpmDB/FmMechanism.H"
#include "vpmDB/FmDB.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "FFaLib/FFaOS/FFaFilePath.H"


FuiBladeDefinition* FuiBladeDefinition::create(int xpos, int ypos, int width, int height, const char* title, const char* name)
{
  return new FuiQtBladeDefinition(xpos, ypos, width, height, title, name);
}


FuiQtBladeDefinition::FuiQtBladeDefinition(int xpos, int ypos, int width, int height,
                                           const char* title, const char* name)
  : FFuQtTopLevelShell(NULL, xpos, ypos, width, height, title, name)
{
	std::string fedemFolderPath = FpPM::getFullFedemPath("Properties");

	//**********Create Widgets**********
	apTabWidget = new QTabWidget(this);
	apBladeDrawer = new BladeDrawer(this);
	apNameEdit = new QLineEdit("Insert description");
	apBladeSelector = new BladeSelector(fedemFolderPath,this);
	apSaveButton = new QPushButton("&Save");
	apCloseButton = new QPushButton("&Close");
	apHelpButton = new QPushButton("&Help");
	apAddSegmentButton = new QPushButton("&Add Segment");
	apRemoveSegmentButton = new QPushButton("&Remove Segment");
	apCreateBladeButton = new QPushButton("&New Blade");
	QWidget* apNotes = new FFuQtNotes(this,
					  "Use this dialog to edit blade-definition files. "
					  "You can create new blades, or browse folders containing blade-definitions. "
					  "Selected airfoils will be copied to the folder [blade name]_airfoils.",5);

	apStructureTab = new QWidget(this);

	apBendingStiffnesCheck = new QCheckBox("Bending stiffness", this);
	apBendingStiffnesCheck->setChecked(false);
	apAxialStiffnesCheck = new QCheckBox("Axial stiffness", this);
	apAxialStiffnesCheck->setChecked(false);
	apTorsionStiffnesCheck = new QCheckBox("Torsion stiffness", this);
	apTorsionStiffnesCheck->setChecked(false);
	apShearStiffnesCheck = new QCheckBox("Shear stiffness", this);
	apShearStiffnesCheck->setChecked(false);
	apCheckboxMapper = new QSignalMapper(this);
	apCheckboxMapper->setMapping(apAxialStiffnesCheck, 0);
	apCheckboxMapper->setMapping(apTorsionStiffnesCheck, 1);
	apCheckboxMapper->setMapping(apBendingStiffnesCheck, 2);
	apCheckboxMapper->setMapping(apShearStiffnesCheck, 3);

	//**********Setup Tables**********
	apAerodynamicTable = new InputTable(8, 0, COLUMN_DOMINANT, this);
	apStructureTable = new InputTable(15, 0, COLUMN_DOMINANT, this);

	apAerodynamicTable->GetView()->verticalHeader()->setDefaultSectionSize(20);
	apAerodynamicTable->GetView()->horizontalHeader()->setFixedHeight(20);
	apStructureTable->GetView()->verticalHeader()->setDefaultSectionSize(20);
	apStructureTable->GetView()->horizontalHeader()->setFixedHeight(20);

	//**********Setup vertical headers for tables**********
	QStringList headers;
	headers << "Segment length [m]" << "Chord length [m]" << "Aerodynamic twist angle [deg]" << "Pitch center parallel"<< "Pitch center normal" <<
			"Aero center parallel" << "Airfoil type" << "Thickness ratio [0-1]";
	apAerodynamicTable->GetModel()->setHeaders(headers);
	headers.clear();
	headers << "Elastic center parallel" << "Elastic center normal" << "Elastic axes relative rotation [deg]"
		<< "Mass center offset-x, parallel" << "Mass center offset-y, normal"
		<< "Mass per unit length [kg/m]" << "Torsional inertia per unit length [kg m]"
		<< "Bending stiffness, flapwise [Nm^2]" << "Bending stiffness, edgewise [Nm^2]" << "Torsion stiffness [Nm^2]"
		<< "Axial stiffness [N]" << "Shear stiffness, flapwise [N]" << "Shear stiffness, edgewise [N]"
		<< "Shear center, parallel" << "Shear center, normal";
	apStructureTable->GetModel()->setHeaders(headers);

	//**********Tooltips**********
	headers.clear();
	headers << "Length of the blade segment, along the blade" << "Chord length of the blade segment. Distance between the two edges of the segment"
			<< "The twist angle, relative to the rotor-plane"
			<< "Position of the pitch center along the chord line, given as a fraction of the chord length"
			<< "Position of the pitch center along the axis perpendicular to the chord line, given as a fraction of the chord length"
			<< "Position of the aerodynamic center along the chord line, given as a fraction of the chord length"
			<< "Points to the file defining the aerodynamic properties of the airfoil. RED: not a valid path, ORANGE: valid path, GREEN: airfoil is in the blade's airfoil folder."
			<< "Thickness of blade, given as a fraction of the Chord length. Only used for visualization and calculation of effective ice mass";
	apAerodynamicTable->GetModel()->setHeadersToolTips(headers);
	headers.clear();
	headers << "Position of the elastic center along the chord line, given as a fraction of the chord length" <<
			"Position of the elastic center along the axis perpendicular to the chord line, given as a fraction of the chord length" <<
			"Relative rotation around the elastic center" <<
			"Offset of the mass center from the elastic center, along the rotated elastic x-axis, given as a fraction of the chord length" <<
			"Offset of the mass center from the elastic center, along the rotated elastic y-axis, given as a fraction of the chord length" <<
			"Density of the blade segment given as mass per unit length"<<
			"Torsional inertia of the blade segment per unit length" <<
			"Flapwise bending stiffness. Direction along the blade" <<
			"Edgewise bending stiffness. Direction along the chord" << "Torsional stiffness" << "Axial stiffness" <<
			"Flapwise shear stiffness. Direction along the blade" << "Edgewise shear stiffness. Direction along the chord" <<
			"Position of the shear center along the chord line, given as a fraction of the chord length" <<
			"Position of the shear center along the axis perpendicular to the chord line, given as a fraction of the chord length";
	apStructureTable->GetModel()->setHeadersToolTips(headers);

	//**********Validators**********
	//apAerodynamicTable->GetModel()->addValidator(7,QString("[0]([.][0-9]+)?"));

	//**********Default values**********
	apAerodynamicTable->GetModel()->addDefaultValue(5, QVariant(0.25));
	apAerodynamicTable->GetModel()->addDefaultValue(3, QVariant(0.25));
	apAerodynamicTable->GetModel()->addDefaultValue(6, QVariant("None"));
	apAerodynamicTable->GetModel()->addDefaultValue(7, QVariant(0.3));

	apStructureTable->GetModel()->addDefaultValue(0, QVariant(0.25));

	//**********Add delegates**********
	FFaFilePath::appendToPath(fedemFolderPath,"AeroData");
	pSelectorDelegate = new FileFieldDelegate(fedemFolderPath,apAerodynamicTable->GetView());
	DoubleFieldDelegate* pDoubleDelegate = new DoubleFieldDelegate(4, 0.1, apAerodynamicTable->GetView());

	apAerodynamicTable->GetView()->setItemDelegate(pDoubleDelegate);
	apAerodynamicTable->GetView()->setItemDelegateForRow(6, pSelectorDelegate);
	apStructureTable->GetView()->setItemDelegate(pDoubleDelegate);

	apAerodynamicTable->ResizeToContents();
	apStructureTable->ResizeToContents();

	//**********Create layouts**********
	QBoxLayout* apMainLayout = new QVBoxLayout(this);
	QBoxLayout* apViewLayout = new QHBoxLayout();
	QBoxLayout* apBladeSelectorLayout = new QHBoxLayout();
	QBoxLayout* apDialogButtonLayout = new QHBoxLayout();
	QBoxLayout* apCheckBoxLayout = new QHBoxLayout();
	QBoxLayout* apStiffnessLayout = new QVBoxLayout();

	//**********Initialize layouts**********
	apViewLayout->addWidget(apBladeDrawer);
	apViewLayout->setContentsMargins(0,0,0,0);

	apBladeSelectorLayout->addWidget(new QLabel("Description:"));
	apBladeSelectorLayout->addWidget(apNameEdit);
	apBladeSelectorLayout->insertSpacing(-1,30);
	apBladeSelectorLayout->addWidget(apAddSegmentButton);
	apBladeSelectorLayout->addWidget(apRemoveSegmentButton);
	apBladeSelectorLayout->insertSpacing(-1,30);
	apBladeSelectorLayout->addWidget(apCreateBladeButton);
	apBladeSelectorLayout->insertStretch(-1);

	apDialogButtonLayout->addWidget(apSaveButton);
	apDialogButtonLayout->addWidget(apCloseButton);
	apDialogButtonLayout->addWidget(apHelpButton);
	apDialogButtonLayout->addStretch(1);
	apDialogButtonLayout->setContentsMargins(5, 0, 5, 5);

	apCheckBoxLayout->addWidget(apBendingStiffnesCheck);
	apCheckBoxLayout->addWidget(apAxialStiffnesCheck);
	apCheckBoxLayout->addWidget(apTorsionStiffnesCheck);
	apCheckBoxLayout->addWidget(apShearStiffnesCheck);
	apCheckBoxLayout->insertStretch(-1);

	apStiffnessLayout->addLayout(apCheckBoxLayout);
	apStiffnessLayout->addWidget(apStructureTable);
	apStiffnessLayout->setContentsMargins(0,0,0,0);
	apStructureTab->setLayout(apStiffnessLayout);

	apMainLayout->addLayout(apViewLayout);
	apMainLayout->addWidget(apBladeSelector);
	apMainLayout->addLayout(apBladeSelectorLayout);
	apMainLayout->addWidget(apTabWidget);
	apMainLayout->addWidget(apNotes);
	apMainLayout->addLayout(apDialogButtonLayout);
	apMainLayout->setContentsMargins(2, 0, 2, 0);

	setLayout(apMainLayout);

	//**********Setup tabs**********
	apTabWidget->addTab(apAerodynamicTable,"Aerodynamic Properties");
	apTabWidget->addTab(apStructureTab,"Structural Properties");

	bladeChanged = false;

	connections();
}

FuiQtBladeDefinition::~FuiQtBladeDefinition()
{
  for (Blade* blade : apBlades) delete blade;
}


void FuiQtBladeDefinition::initializeBlades()
{
  std::vector<std::string> bladePaths;
  apBladeSelector->getModel()->getAllItems(bladePaths);
  if (bladePaths.empty())
    this->createBlade();
  else for (const std::string& path : bladePaths)
    if (QString(path.c_str()).split(QDir::separator()).first() != "Unsaved Blades")
    {
      bool readOnly = true;
      apBladeSelector->getModel()->itemIsReadOnly(readOnly,path.c_str());
      FpPM::dontTouchModel();
      FmBladeDesign* pDesign = FmBladeDesign::readFromFMM(path,readOnly);
      FpPM::resetTouchedFlag();
      if (pDesign)
      {
        if (!apBladeSelector->itemExist(pDesign->myModelFile.getValue().c_str()))
        {
          std::cerr <<" *** "<< pDesign->myModelFile.getValue()
                    <<" does not exist in the Blade selector dialog."<< std::endl;
          FpPM::dontTouchModel();
          pDesign->erase();
          FpPM::resetTouchedFlag();
          continue;
        }

        QVector<QVector<QVariant>*>* pAeroData   = new QVector<QVector<QVariant>*>();
        QVector<QVector<QVariant>*>* pStructData = new QVector<QVector<QVariant>*>();

        std::vector<FmBladeProperty*> properties;
        pDesign->getBladeSegments(properties);
        for (FmBladeProperty* property : properties)
        {
          // Get aerodynamic data
          QVector<QVariant>* pAeroProperties = new QVector<QVariant>;
          pAeroProperties->push_back(QVariant(property->Length.getValue()));
          pAeroProperties->push_back(QVariant(property->Chord.getValue()));
          pAeroProperties->push_back(QVariant(property->Twist.getValue()));
          pAeroProperties->push_back(QVariant(property->PitchCentre.getValue().first));
          pAeroProperties->push_back(QVariant(property->PitchCentre.getValue().second));
          pAeroProperties->push_back(QVariant(property->AeroCentre.getValue()));
          pAeroProperties->push_back(QVariant(property->AirFoil.getValue().c_str()));
          pAeroProperties->push_back(QVariant(property->Thick.getValue()));
          pAeroData->push_back(pAeroProperties);

          // Get structural data
          QVector<QVariant>* pStructuralProperties = new QVector<QVariant>;
          pStructuralProperties->push_back(QVariant(property->ElCentre.getValue().first));
          pStructuralProperties->push_back(QVariant(property->ElCentre.getValue().second));
          pStructuralProperties->push_back(QVariant(property->ElAxisRot.getValue()));
          pStructuralProperties->push_back(QVariant(property->MassCentre.getValue().first));
          pStructuralProperties->push_back(QVariant(property->MassCentre.getValue().second));
          pStructuralProperties->push_back(QVariant(property->Mass.getValue()));
          pStructuralProperties->push_back(QVariant(property->Tinertia.getValue()));
          pStructuralProperties->push_back(QVariant(property->EI.getValue().second));
          pStructuralProperties->push_back(QVariant(property->EI.getValue().first));
          pStructuralProperties->push_back(QVariant(property->GIt.getValue()));
          pStructuralProperties->push_back(QVariant(property->EA.getValue()));
          pStructuralProperties->push_back(QVariant(property->GAs.getValue().second));
          pStructuralProperties->push_back(QVariant(property->GAs.getValue().first));
          pStructuralProperties->push_back(QVariant(property->ShrCentre.getValue().first));
          pStructuralProperties->push_back(QVariant(property->ShrCentre.getValue().second));
          pStructData->push_back(pStructuralProperties);
        }

        Blade* blade = new Blade(pDesign->getUserDescription(),pDesign->getUserDescription(),"");

        blade->SetIsInUse(false);
        blade->SetPath(pDesign->myModelFile.getValue());
        blade->SetAerodynamicData(pAeroData);
        blade->SetStructureData(pStructData);

        blade->SetAxialStiffnessState(pDesign->withAstiff.getValue());
        blade->SetBendingStiffnessState(pDesign->withBstiff.getValue());
        blade->SetTorsionStiffnessState(pDesign->withTstiff.getValue());
        blade->SetShearStiffnessState(pDesign->withSstiff.getValue());

        blade->SetFileExists(true);
        blade->SetDesign(pDesign);
        blade->SetIsTemplate(pDesign->readOnly.getValue());

        apBlades.push_back(blade);
      }
    }

  apBladeSelector->selectItem(apBlades.at(0)->GetPath().c_str());

  bladeChanged = true;
  for (int i = 0; i < 4; i++)
    this->checkBoxChanged(i);
  bladeChanged = false;
}


bool FuiQtBladeDefinition::setCurrentBlade(Blade* pBlade) {
	if(!pBlade)
		return false;
	apCurrentBlade = pBlade;

	// Updates model data for the three tables. bladeChanged must be set to true between each time.
	// SetModelData triggers the dataChanged()-signal in the model, which will cause currentModelChanged() to
	// "touch" the blade if bladeChanged is not true.
	bladeChanged = true;
	updateCheckBoxes();

	//If the blade we are switching to is completely empty, we must first fill it with dummy data, and then remove the data.
	// If not the model will be filled improperly
	if(!apCurrentBlade->GetAerodynamicData()->size())
	{
		QVector<QVector<QVariant>*>* pAeroData =  new QVector<QVector<QVariant>*>();
		QVector<QVariant>* pAeroSegment = new QVector<QVariant>(8);
		pAeroData->push_back(pAeroSegment);

		QVector<QVector<QVariant>*>* pStiffnessData =  new QVector<QVector<QVariant>*>();
		QVector<QVariant>* pStiffnessSegment = new QVector<QVariant>(15);
		pStiffnessData->push_back(pStiffnessSegment);

		apCurrentBlade->SetAerodynamicData(pAeroData);
		apCurrentBlade->SetStructureData(pStiffnessData);

		apAerodynamicTable->GetModel()->SetModelData(apCurrentBlade->GetAerodynamicData());
		bladeChanged = true;
		apStructureTable->GetModel()->SetModelData(apCurrentBlade->GetStructureData());
		switch(apAerodynamicTable->GetModel()->getTableOrdering()){
			case ROW_DOMINANT:
				apAerodynamicTable->GetModel()->removeRow(apAerodynamicTable->GetModel()->rowCount()-1);
				apStructureTable->GetModel()->removeRow(apStructureTable->GetModel()->rowCount()-1);
			case COLUMN_DOMINANT:
				apAerodynamicTable->GetModel()->removeColumn(apAerodynamicTable->GetModel()->columnCount()-1);
				apStructureTable->GetModel()->removeColumn(apStructureTable->GetModel()->columnCount()-1);
		}
	}

	if(apCurrentBlade->GetAerodynamicData()->size()){
		apAerodynamicTable->GetModel()->SetModelData(apCurrentBlade->GetAerodynamicData());
		bladeChanged = true;
		apStructureTable->GetModel()->SetModelData(apCurrentBlade->GetStructureData());
	}

	if(apCurrentBlade->IsTemplate() ){
		apAerodynamicTable->GetModel()->SetEditable(false);
		apStructureTable->GetModel()->SetEditable(false);
		apNameEdit->setEnabled(false);
		apAddSegmentButton->setEnabled(false);
		apRemoveSegmentButton->setEnabled(false);
		apSaveButton->setEnabled(false);
		apAxialStiffnesCheck->setEnabled(false);
		apBendingStiffnesCheck->setEnabled(false);
		apShearStiffnesCheck->setEnabled(false);
		apTorsionStiffnesCheck->setEnabled(false);
	}
	else{
		apAerodynamicTable->GetModel()->SetEditable(true);
		apStructureTable->GetModel()->SetEditable(true);
		apNameEdit->setEnabled(true);
		apAddSegmentButton->setEnabled(true);
		apRemoveSegmentButton->setEnabled(true);
		apSaveButton->setEnabled(pBlade->Touched());
		apAxialStiffnesCheck->setEnabled(true);
		apBendingStiffnesCheck->setEnabled(true);
		apShearStiffnesCheck->setEnabled(true);
		apTorsionStiffnesCheck->setEnabled(true);
		pSelectorDelegate->setBladePath(apCurrentBlade->GetPath());
	}

	apBladeDrawer->setBlade(apCurrentBlade);

	bladeChanged = true;
	emit(currentBladeChanged());
	bladeChanged = false;
	return true;
}

void FuiQtBladeDefinition::connections() {
	// Buttons
	QObject::connect( apSaveButton, SIGNAL(clicked()), this, SLOT( acceptClicked() ) );
	QObject::connect( apCloseButton, SIGNAL(clicked()), this, SLOT( close() ) );
	QObject::connect( apHelpButton, SIGNAL(clicked()), this, SLOT( help() ) );

	QObject::connect(apAddSegmentButton, SIGNAL(clicked()), this, SLOT(insertSegment()));
	QObject::connect(apRemoveSegmentButton, SIGNAL(clicked()), this, SLOT(removeSegment()));

	QObject::connect(apCreateBladeButton, SIGNAL(clicked()), this, SLOT(createBlade()));

	// Fields and selectors
	QObject::connect(this, SIGNAL(currentBladeChanged()), this, SLOT(updateNameField()));
	QObject::connect(apNameEdit, SIGNAL(textChanged(const QString)), this, SLOT(nameChanged()));

	//Model change
	QObject::connect(apAerodynamicTable->GetModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentModelChanged()));
	QObject::connect(apStructureTable->GetModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentModelChanged()));

	//Checkboxes
	QObject::connect(apCheckboxMapper, SIGNAL(mappedInt(int)), this, SLOT(checkBoxChanged(int)));
	QObject::connect(apShearStiffnesCheck, SIGNAL(stateChanged(int)), apCheckboxMapper, SLOT(map()));
	QObject::connect(apTorsionStiffnesCheck, SIGNAL(stateChanged(int)), apCheckboxMapper, SLOT(map()));
	QObject::connect(apBendingStiffnesCheck, SIGNAL(stateChanged(int)), apCheckboxMapper, SLOT(map()));
	QObject::connect(apAxialStiffnesCheck, SIGNAL(stateChanged(int)), apCheckboxMapper, SLOT(map()));

	// Tabs
	QObject::connect( apTabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeAirfoil() ));
	QObject::connect( this, SIGNAL(currentBladeChanged()), this, SLOT(changeAirfoil() ));

	// Connection to update airfoil figure based on which column is currently selected
	QObject::connect( apAerodynamicTable->GetView()->selectionModel(),SIGNAL(currentColumnChanged(const QModelIndex &, const QModelIndex &)),
			apBladeDrawer->apAirfoilView, SLOT( setSegment(const QModelIndex &, const QModelIndex &)));
	QObject::connect( apStructureTable->GetView()->selectionModel(),SIGNAL(currentColumnChanged(const QModelIndex &, const QModelIndex &)),
				apBladeDrawer->apAirfoilView, SLOT( setSegment(const QModelIndex &, const QModelIndex &)));
	// Connection to update blade figure based on which column is currently selected
	QObject::connect( apAerodynamicTable->GetView()->selectionModel(),SIGNAL(currentColumnChanged(const QModelIndex &, const QModelIndex &)),
			apBladeDrawer->apBladeView, SLOT( setSegment(const QModelIndex &, const QModelIndex &)));
	QObject::connect( apStructureTable->GetView()->selectionModel(),SIGNAL(currentColumnChanged(const QModelIndex &, const QModelIndex &)),
				apBladeDrawer->apBladeView, SLOT( setSegment(const QModelIndex &, const QModelIndex &)));

	QObject::connect( apAerodynamicTable->GetView(),SIGNAL(clicked(const QModelIndex &)), this, SLOT(itemClicked(const QModelIndex &)));

	QObject::connect( apBladeSelector->getView()->selectionModel(), SIGNAL(currentChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(bladeSelected()));
	QObject::connect( apBladeSelector, SIGNAL(selectionModelChanged()), this, SLOT(initializeBlades()));
}


bool FuiQtBladeDefinition::saveBlade(Blade* blade)
{
  // Don't save if blade is a locked blade, or if it is not changed
  if (blade->IsTemplate() || !blade->Touched())
    return false;

  QDir path;
  FmBladeDesign* bd;
  if (blade->FileExists())
  {
    bd = blade->GetDesign();
    path.setPath(blade->GetPath().c_str());;

    // Remove all blade properties
    std::vector<FmBladeProperty*> empty, oldProperties;
    bd->getBladeSegments(oldProperties);
    bd->setBladeSegments(empty);
    for (FmBladeProperty* bladeProp : oldProperties)
      bladeProp->erase();
  }
  else
  {
    QString filePath = apBladeSelector->getCurrentFolder() + QString(blade->GetName().c_str());
    filePath = QFileDialog::getSaveFileName(this, "Save blade", filePath, QString("Blade(*.fmm)"));
    if (filePath == "") return false;

    bd = new FmBladeDesign();
    bd->connect();
    blade->SetPath(QDir::toNativeSeparators(QDir(filePath).absolutePath()).toStdString());
    path.setPath(filePath);
  }

  this->resolveAirfoils(blade);

  for (QVector<QVariant>* segment : *(blade->GetAerodynamicData()))
  {
    FmBladeProperty* bp = new FmBladeProperty();

    // Save aerodynamic data
    bp->Length = segment->at(0).toDouble();
    bp->Chord = segment->at(1).toDouble();
    bp->Twist = segment->at(2).toDouble();
    bp->PitchCentre.setValue(std::make_pair(segment->at(3).toDouble(),segment->at(4).toDouble()));
    bp->AeroCentre = segment->at(5).toDouble();
    bp->AirFoil = segment->at(6).toString().toStdString();
    bp->Thick = segment->at(7).toDouble();
    bp->connect();

    bd->addBladeProperty(bp);
  }

  int segNr = 0;
  for (QVector<QVariant>* segment : *(blade->GetStructureData()))
  {
    FmBladeProperty* bp = bd->getBladeProperty(segNr++);

    // Save structural data
    bp->ElCentre.setValue(std::make_pair(segment->at(0).toDouble(),segment->at(1).toDouble()));
    bp->ElAxisRot = segment->at(2).toDouble();
    bp->MassCentre.setValue(std::make_pair(segment->at(3).toDouble(),segment->at(4).toDouble()));
    bp->Mass = segment->at(5).toDouble();
    bp->Tinertia = segment->at(6).toDouble();
    bp->EI.setValue(std::make_pair(segment->at(7).toDouble(),segment->at(8).toDouble()));
    bp->GIt = segment->at(9).toDouble();
    bp->EA = segment->at(10).toDouble();
    bp->GAs.setValue(std::make_pair(segment->at(11).toDouble(),segment->at(12).toDouble()));
    bp->ShrCentre.setValue(std::make_pair(segment->at(13).toDouble(),segment->at(14).toDouble()));
  }

  bd->withAstiff.setValue(blade->AxialStiffness());
  bd->withBstiff.setValue(blade->BendingStiffness());
  bd->withTstiff.setValue(blade->TorsionStiffness());
  bd->withSstiff.setValue(blade->ShearStiffness());

  bd->setUserDescription(blade->GetName());

  // Save to file, or create new file
  if (blade->InUse())
    bd->onChanged();
  else if (blade->FileExists())
    bd->writeToFMM(QDir::toNativeSeparators(path.absolutePath()).toStdString());
  else
  {
    bd->myModelFile.setValue(QDir::toNativeSeparators(path.absolutePath()).toStdString());
    bd->onChanged();
    bd->writeToFMM(QDir::toNativeSeparators(path.absolutePath()).toStdString());

    blade->SetInfo(blade->GetName());
    blade->SetPath(bd->myModelFile.getValue());
    blade->SetFileExists(true);
    blade->SetDesign(bd);
  }
  blade->UnTouch();

  return true;
}


// ************SLOTS**********
void FuiQtBladeDefinition::createBlade(){
	// Check if there already exists a new, unsaved blade
	for (Blade* blade : apBlades)
		if (!blade->FileExists())
			return;

	QVector<QVector<QVariant>*>* pAeroData   = new QVector<QVector<QVariant>*>();
	QVector<QVector<QVariant>*>* pStructData = new QVector<QVector<QVariant>*>();

	pAeroData->push_back(new QVector<QVariant>(8));
	pStructData->push_back(new QVector<QVariant>(15));

	Blade* aBlade = new Blade("New Blade","New Blade",QString(QString("Unsaved Blades") + QDir::separator() + QString("New Blade")).toStdString());
	aBlade->SetAerodynamicData(pAeroData);
	aBlade->SetStructureData(pStructData);
	aBlade->SetIsTemplate(false);
	aBlade->SetFileExists(false);

	aBlade->SetAxialStiffnessState(false);
	aBlade->SetTorsionStiffnessState(false);
	aBlade->SetBendingStiffnessState(false);
	aBlade->SetShearStiffnessState(false);

	apBlades.push_back(aBlade);

	apBladeSelector->addUnsavedItem(QString("Unsaved Blades")+QDir::separator(), QString(aBlade->GetName().c_str()));
	apBladeSelector->selectItem(aBlade->GetPath().c_str());
	apBladeSelector->bladeSelected();

	switch(apAerodynamicTable->GetModel()->getTableOrdering()){
		case ROW_DOMINANT:
			apAerodynamicTable->GetModel()->removeRow(apAerodynamicTable->GetModel()->rowCount()-1);
			apAerodynamicTable->GetModel()->insertRow(apAerodynamicTable->GetModel()->rowCount());
			apStructureTable->GetModel()->removeRow(apStructureTable->GetModel()->rowCount()-1);
			apStructureTable->GetModel()->insertRow(apStructureTable->GetModel()->rowCount());
		case COLUMN_DOMINANT:
			apAerodynamicTable->GetModel()->removeColumn(apAerodynamicTable->GetModel()->columnCount()-1);
			apAerodynamicTable->GetModel()->insertColumn(apAerodynamicTable->GetModel()->columnCount());
			apStructureTable->GetModel()->removeColumn(apStructureTable->GetModel()->columnCount()-1);
			apStructureTable->GetModel()->insertColumn(apStructureTable->GetModel()->columnCount());
	}
}


void FuiQtBladeDefinition::acceptClicked()
{
  Blade* bladeToSave = apCurrentBlade;
  std::string oldPath = bladeToSave->GetPath(); //Used to delete unsaved entry
  if (!this->saveBlade(bladeToSave))
    return;

  if (oldPath != bladeToSave->GetPath())
  {
    // Create new entry in bladeSelector
    apBladeSelector->addEntry(bladeToSave->GetPath().c_str());
    // Remove unsaved entry
    apBladeSelector->removeEntry(oldPath.c_str());
  }

  apBladeSelector->selectItem(bladeToSave->GetPath().c_str());
  apBladeSelector->bladeSelected();
  apSaveButton->setEnabled(false);
}


void FuiQtBladeDefinition::bladeSelected()
{
  if (!apBladeSelector->getView()->currentIndex().isValid())
    return;

  std::string bladeInfo = (static_cast<DataNode*>(apBladeSelector->getView()->currentIndex().internalPointer())->getParentNode()->getData(0).toString() +
                           apBladeSelector->getView()->model()->data(apBladeSelector->getView()->currentIndex(),Qt::DisplayRole).toString()).toStdString();

  // Set the currently active blade based on the selection from the drop-down
  for (Blade* blade : apBlades)
    if (blade->GetPath() == bladeInfo)
    {
      this->setCurrentBlade(blade);
      return;
    }
}


void FuiQtBladeDefinition::insertSegment(){
	switch(apAerodynamicTable->GetModel()->getTableOrdering()){
	case ROW_DOMINANT:
		apAerodynamicTable->GetModel()->insertRow(apAerodynamicTable->GetModel()->rowCount());
		apStructureTable->GetModel()->insertRow(apStructureTable->GetModel()->rowCount());

		apAerodynamicTable->GetView()->resizeRowToContents(apAerodynamicTable->GetModel()->rowCount()-1);
		apStructureTable->GetView()->resizeRowToContents(apStructureTable->GetModel()->rowCount()-1);
		break;
	case COLUMN_DOMINANT:
		apAerodynamicTable->GetModel()->insertColumn(apAerodynamicTable->GetModel()->columnCount());
		apStructureTable->GetModel()->insertColumn(apStructureTable->GetModel()->columnCount());

		apAerodynamicTable->GetView()->resizeColumnToContents(apAerodynamicTable->GetModel()->columnCount()-1);
		apStructureTable->GetView()->resizeColumnToContents(apStructureTable->GetModel()->columnCount()-1);
	}
}

void FuiQtBladeDefinition::removeSegment(){
	switch(apAerodynamicTable->GetModel()->getTableOrdering()){
	case ROW_DOMINANT:
		apAerodynamicTable->GetModel()->removeRow(apAerodynamicTable->GetModel()->rowCount()-1);
		apStructureTable->GetModel()->removeRow(apStructureTable->GetModel()->rowCount()-1);
	case COLUMN_DOMINANT:
		apAerodynamicTable->GetModel()->removeColumn(apAerodynamicTable->GetModel()->columnCount()-1);
		apStructureTable->GetModel()->removeColumn(apStructureTable->GetModel()->columnCount()-1);
	}
}

void FuiQtBladeDefinition::updateNameField(){
	apNameEdit->setText(QString(apCurrentBlade->GetName().c_str()) );
}

void FuiQtBladeDefinition::currentModelChanged(){
	if(!apCurrentBlade->IsTemplate() && !bladeChanged){
		apCurrentBlade->Touch();
		apSaveButton->setEnabled(true);
	}
	bladeChanged = false;
	apBladeDrawer->setBlade(apCurrentBlade);
}

void FuiQtBladeDefinition::nameChanged(){
	apCurrentBlade->SetName( apNameEdit->text().toStdString() );

	if(!apCurrentBlade->IsTemplate() && !bladeChanged){
		apCurrentBlade->Touch();
		apSaveButton->setEnabled(true);
	}
	bladeChanged = false;
}


void FuiQtBladeDefinition::showEvent(QShowEvent*)
{
  // Remove all blade-designs, except the one connected to the model
  FmBladeDesign* usedBladeDesign = NULL;
  if (FmDB::getTurbineObject())
    usedBladeDesign = dynamic_cast<FmBladeDesign*>(FmDB::getTurbineObject()->bladeDef.getPointer());

  std::vector<FmBladeDesign*> pBladeDesigns;
  FmDB::getAllBladeDesigns(pBladeDesigns);
  for (FmBladeDesign* design : pBladeDesigns)
    if (design != usedBladeDesign)
    {
      FpPM::dontTouchModel();
      design->erase();
      FpPM::resetTouchedFlag();
    }

  // If a blade is associated with the current model, add its directory
  if (usedBladeDesign)
    BladeSelectionModel::instance()->addDirectory(0,FmDB::getMechanismObject()->getAbsBladeFolderPath().c_str(),false);

  // Refresh blade-selector
  apBladeSelector->refresh();

  this->initializeBlades();

  // If no blade is in use, select the blade on the top of the list.
  apBladeSelector->selectItem("");
  this->bladeSelected();
}


void FuiQtBladeDefinition::closeEvent(QCloseEvent* event)
{
  // Check for unsaved blades, and pop-up save-dialog
  for (Blade* blade : apBlades)
    if (blade->Touched())
      switch (QMessageBox::warning(this, "Closing",
                                   ("Do you want to save changes to "+ blade->GetName() +"?").c_str(),
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
      case QMessageBox::Cancel:
        event->ignore();
        return;
      case QMessageBox::Yes:
        this->saveBlade(blade);
      default:
        break;
      }

  event->accept();

  apBladeSelector->removeEntry(QString("Unsaved Blades")+QDir::separator()+QString("New Blade"));

  while (!apBlades.empty())
    delete apBlades.back(), apBlades.pop_back();
}


void FuiQtBladeDefinition::checkBoxChanged(int checkBox){
	switch(checkBox){
		case 0:
			if(apAxialStiffnesCheck->isChecked()){
				apStructureTable->GetView()->setRowHidden(10,false);
				apCurrentBlade->SetAxialStiffnessState(true);
			}else{
				apStructureTable->GetView()->setRowHidden(10,true);
				apCurrentBlade->SetAxialStiffnessState(false);
			}
			break;
		case 1:
			if(apTorsionStiffnesCheck->isChecked()){
				apStructureTable->GetView()->setRowHidden(9,false);
				apCurrentBlade->SetTorsionStiffnessState(true);
			}else{
				apStructureTable->GetView()->setRowHidden(9,true);
				apCurrentBlade->SetTorsionStiffnessState(false);
			}
			break;
		case 2:
			if(apBendingStiffnesCheck->isChecked()){
				apStructureTable->GetView()->setRowHidden(7,false);
				apStructureTable->GetView()->setRowHidden(8,false);
				apCurrentBlade->SetBendingStiffnessState(true);
			}else{
				apStructureTable->GetView()->setRowHidden(7,true);
				apStructureTable->GetView()->setRowHidden(8,true);
				apCurrentBlade->SetBendingStiffnessState(false);
			}
			break;
		case 3:
			if(apShearStiffnesCheck->isChecked()){
				apStructureTable->GetView()->setRowHidden(11,false);
				apStructureTable->GetView()->setRowHidden(12,false);
				apStructureTable->GetView()->setRowHidden(13,false);
				apStructureTable->GetView()->setRowHidden(14,false);
				apCurrentBlade->SetShearStiffnessState(true);
			}else{
				apStructureTable->GetView()->setRowHidden(11,true);
				apStructureTable->GetView()->setRowHidden(12,true);
				apStructureTable->GetView()->setRowHidden(13,true);
				apStructureTable->GetView()->setRowHidden(14,true);
				apCurrentBlade->SetShearStiffnessState(false);
			}
			break;
	}
	if(!bladeChanged){
		apCurrentBlade->Touch();
		apSaveButton->setEnabled(true);
	}
}


void FuiQtBladeDefinition::updateCheckBoxes()
{
  apAxialStiffnesCheck->setChecked(apCurrentBlade->AxialStiffness());
  apBendingStiffnesCheck->setChecked(apCurrentBlade->BendingStiffness());
  apShearStiffnesCheck->setChecked(apCurrentBlade->ShearStiffness());
  apTorsionStiffnesCheck->setChecked(apCurrentBlade->TorsionStiffness());
}


void FuiQtBladeDefinition::changeAirfoil()
{
  InputTable* pTable;
  switch (apTabWidget->currentIndex()){
  case 0:
    pTable = dynamic_cast<InputTable*>(apAerodynamicTable);
    break;
  case 1:
    pTable = dynamic_cast<InputTable*>(apStructureTable);
    break;
  default:
    return;
  }
  apBladeDrawer->apAirfoilView->setTab(apTabWidget->currentIndex());
  apBladeDrawer->apAirfoilView->setSegment(pTable->GetView()->currentIndex(),pTable->GetView()->currentIndex());
  apBladeDrawer->apBladeView->setSegment(pTable->GetView()->currentIndex(),pTable->GetView()->currentIndex());
}


void FuiQtBladeDefinition::help()
{
  // Display the topic in the help file
  Fui::showCHM("dialogbox/windpower/blade-definition.htm");
}


void FuiQtBladeDefinition::itemClicked(const QModelIndex& index)
{
  if (index.row() == 6)
    apAerodynamicTable->GetView()->edit(index);
}


void FuiQtBladeDefinition::resolveAirfoils(Blade* pBlade)
{
  QString path(pBlade->GetPath().c_str());
  QDir folder(path);
  folder.cdUp();

  // Create and move to airfoils-folder
  QString dirName = path.split(QDir::separator()).last().split(".").first() + QString("_airfoils");
  folder.mkdir(dirName);
  folder.cd(dirName);

  // For all columns in airfoil-row
  for (QVector<QVariant>* segment : *(pBlade->GetAerodynamicData()))
  {
    QString oldPath = QDir::toNativeSeparators(segment->at(6).toString());
    // Check if path exists and it is an absolute path (and not relative to blades local folder)
    if (folder.exists(oldPath) && oldPath.split(QDir::separator()).size() > 1) {
      // We have existing airfoil with absolute path. Move it to blades airfoil-folder:
      QString oldFile = oldPath.split(QDir::separator()).last();
      QFile(oldPath).copy(folder.path() + QDir::separator() + oldFile);
      segment->replace(6,oldFile);
      apAerodynamicTable->repaint();
    }
  }
}
