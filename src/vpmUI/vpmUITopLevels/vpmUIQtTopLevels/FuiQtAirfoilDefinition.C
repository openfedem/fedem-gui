/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * FuiQtAirfoilDefinition.C
 *
 *  Created on: Apr 30, 2012
 *      Author: runarhr
 */

#include <QTextStream>
#include <QFileDialog>
#include <QFile>
#include <QDate>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include <QCloseEvent>
#include <QComboBox>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtAirfoilDefinition.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFuLib/FFuCustom/mvcModels/AirfoilSelectionModel.H"
#include "FFuLib/FFuCustom/inputTables/delegates/DoubleFieldDelegate.H"

extern const char* info_xpm[];


FuiAirfoilDefinition* FuiAirfoilDefinition::create(int xpos, int ypos,
		int width, int height, const char* title, const char* name)
{
  return new FuiQtAirfoilDefinition(0, xpos, ypos, width, height, title, name);
}

FuiQtAirfoilDefinition::FuiQtAirfoilDefinition(QWidget* parent,
		int xpos, int ypos, int width, int height, const char* title,
		const char* name) :
		FFuQtTopLevelShell(parent, xpos, ypos, width, height, title, name)
{
	this->setFixedWidth(size().width());

	// ** Create widgets **
	QSplitter* apSplitter = new QSplitter(this);
	apSplitter->setOrientation(Qt::Vertical);
	apSplitter->setChildrenCollapsible(false);
	QWidget* apBottomContainer = new QWidget(this);

	apNotesLabel = new QLabel("<b>Notes<\b>",this);
	apNotesLabelImage = new QLabel(this);
	apNotesLabelImage->setPixmap(QPixmap(info_xpm));
	apNotesText = new QLabel("Use this dialog to view and modify airfoil files in the AeroDyn format.");
	apNotesText->setWordWrap(true);

	apAddRowButton = new QPushButton("Add Row",this);
	apRemoveRowButton = new QPushButton("Remove Row",this);

	apSaveButton = new QPushButton("&Save",this);
	apCloseButton = new QPushButton("&Close",this);
	apHelpButton = new QPushButton("&Help",this);

	apCreateAirfoilButton = new QPushButton("&New Airfoil",this);

	//Line edits
	apNumLabel = new QLabel("Number of airfoil tables in this file:", this);
	apNumEdit = new QLineEdit(this);
	apNumEdit->setFixedWidth(200);
	//apNumEdit->setEnabled(false);
	apIDLabel = new QLabel("Table ID parameter:", this);
	apIDEdit = new QLineEdit(this);
	apIDEdit->setFixedWidth(200);
	apStallLabel = new QLabel("Stall angle[deg]:", this);
	apStallEdit = new QLineEdit(this);
	apStallEdit->setFixedWidth(200);
	apCnAnglekLabel = new QLabel("Zero C<sub>n</sub> angle of attack[deg]:", this);
	apCnAngleEdit = new QLineEdit(this);
	apCnAngleEdit->setFixedWidth(200);
	apCnSlopeLabel = new QLabel("C<sub>n</sub> slope for zero lift (dimensionless):", this);
	apCnSlopeEdit = new QLineEdit(this);
	apCnSlopeEdit->setFixedWidth(200);
	apCnExtrapolLabel = new QLabel("C<sub>n</sub> extrapolated to value at positive stall angle of attack:", this);
	apCnExtrapolEdit = new QLineEdit(this);
	apCnExtrapolEdit->setFixedWidth(200);
	apCnStallLabel = new QLabel("C<sub>n</sub> at stall value for negative angle of attack:", this);
	apCnStallEdit = new QLineEdit(this);
	apCnStallEdit->setFixedWidth(200);
	apAttackLabel = new QLabel("Angle of attack for minimum C<sub>D</sub>[deg]:", this);
	apAttackEdit = new QLineEdit(this);
	apAttackEdit->setFixedWidth(200);
	apMinCDLabel = new QLabel("Minimum C<sub>D</sub> value:", this);
	apMinCDEdit = new QLineEdit(this);
	apMinCDEdit->setFixedWidth(200);

	// Map line edits
	apLineEditMapper = new QSignalMapper(this);
	apLineEditMapper->setMapping(apNumEdit, 0);
	apLineEditMapper->setMapping(apIDEdit, 1);
	apLineEditMapper->setMapping(apStallEdit, 2);
	apLineEditMapper->setMapping(apCnAngleEdit, 3);
	apLineEditMapper->setMapping(apCnSlopeEdit, 4);
	apLineEditMapper->setMapping(apCnExtrapolEdit, 5);
	apLineEditMapper->setMapping(apCnStallEdit, 6);
	apLineEditMapper->setMapping(apAttackEdit, 7);
	apLineEditMapper->setMapping(apMinCDEdit, 8);

	//Selector widget
	std::string instPath = FpPM::getFullFedemPath("Properties");
	FFaFilePath::appendToPath(instPath,"AeroData");
	apAirfoilSelector = new AirfoilSelector(instPath,this);

	// Setup Tables
	apAirfoilTable = new TableT(2,4, ROW_DOMINANT, this);
	apAirfoilTable->setFixedWidth(420);

	apAirfoilTable->GetView()->verticalHeader()->setDefaultSectionSize(20);
	apAirfoilTable->GetView()->horizontalHeader()->setFixedHeight(20);

	//Setup vertical headers for tables
	QStringList headers;
	headers << "alpha" << "CL" << "CD" << "CM";
	apAirfoilTable->GetModel()->setHeaders(headers);

	//Add delegate
	DoubleFieldDelegate* pDoubleDelegate = new DoubleFieldDelegate(4, 0.1,apAirfoilTable->GetView());
	apAirfoilTable->GetView()->setItemDelegate(pDoubleDelegate);

	// ** Create layouts **
	apMainLayout = new QVBoxLayout;
	apContentLayout = new QHBoxLayout;
	apLineEditLayout = new QVBoxLayout;
	apTableButtonLayout = new QHBoxLayout;
	apTableLayout  = new QVBoxLayout;
	apDialogButtonLayout = new QHBoxLayout;
	apNotesLabelLayout = new QHBoxLayout;
	apNotesLayout = new QVBoxLayout;

	// ** Initialize layouts **
	apNotesLabel->setAlignment(Qt::AlignTop);
	apNotesLabelImage->setAlignment(Qt::AlignTop);
	apNotesLabelLayout->addWidget(apNotesLabelImage);
	apNotesLabelLayout->addWidget(apNotesLabel);
	apNotesLabelLayout->addStretch(-1);
	apNotesLayout->addLayout(apNotesLabelLayout);
	apNotesLayout->addWidget(apNotesText);

	apDialogButtonLayout->addWidget(apSaveButton);
	apDialogButtonLayout->addWidget(apCloseButton);
	apDialogButtonLayout->addWidget(apHelpButton);
	apDialogButtonLayout->addSpacing(15);
	apDialogButtonLayout->addWidget(apCreateAirfoilButton);
	apDialogButtonLayout->addStretch(-1);

	apTableButtonLayout->addWidget(apAddRowButton);
	apTableButtonLayout->addWidget(apRemoveRowButton);
	apTableButtonLayout->addStretch(-1);

	apTableLayout->addLayout(apTableButtonLayout);
	apTableLayout->addWidget(apAirfoilTable);
	apTableLayout->addLayout(apNotesLayout);

	apLineEditLayout->addWidget(apNumLabel);
	apLineEditLayout->addWidget(apNumEdit);
	apLineEditLayout->addWidget(apIDLabel);
	apLineEditLayout->addWidget(apIDEdit);
	apLineEditLayout->addWidget(apStallLabel);
	apLineEditLayout->addWidget(apStallEdit);
	apLineEditLayout->addWidget(apCnAnglekLabel);
	apLineEditLayout->addWidget(apCnAngleEdit);
	apLineEditLayout->addWidget(apCnSlopeLabel);
	apLineEditLayout->addWidget(apCnSlopeEdit);
	apLineEditLayout->addWidget(apCnExtrapolLabel);
	apLineEditLayout->addWidget(apCnExtrapolEdit);
	apLineEditLayout->addWidget(apCnStallLabel);
	apLineEditLayout->addWidget(apCnStallEdit);
	apLineEditLayout->addWidget(apAttackLabel);
	apLineEditLayout->addWidget(apAttackEdit);
	apLineEditLayout->addWidget(apMinCDLabel);
	apLineEditLayout->addWidget(apMinCDEdit);
	apLineEditLayout->addLayout(apDialogButtonLayout);

	apContentLayout->addSpacing(6);
	apContentLayout->addLayout(apLineEditLayout);
	apContentLayout->addLayout(apTableLayout);
	apContentLayout->addSpacing(6);

	apBottomContainer->setLayout(apContentLayout);
	apSplitter->addWidget(apAirfoilSelector);
	apSplitter->addWidget(apBottomContainer);

	apMainLayout->addWidget(apSplitter);
	apMainLayout->setContentsMargins(0,0,0,10);
	setLayout(apMainLayout);

	connections();

	currentAirfoilTouched = false;
}


void FuiQtAirfoilDefinition::showEvent(QShowEvent*)
{
  apAirfoilSelector->refresh();
  apAirfoilSelector->selectItem("");
}


void FuiQtAirfoilDefinition::closeEvent(QCloseEvent* event)
{
  // Check for unsaved airfoil, and pop-up save-message
  if (currentAirfoilTouched)
    switch (QMessageBox::warning(this, tr("Closing"), tr("Do you want to save changes to the current file?"),
				 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
    case QMessageBox::Cancel:
      event->ignore();
      return;
    case QMessageBox::Yes:
      this->save(this->apAirfoilSelector->getCurrentItem());
    default:
      break;
    }

  currentAirfoilTouched = false;
  event->accept();
}


void FuiQtAirfoilDefinition::connections(){
	// Buttons
	QObject::connect( apSaveButton, SIGNAL(clicked()), this, SLOT( acceptClicked() ) );
	QObject::connect( apCloseButton, SIGNAL(clicked()), this, SLOT( close() ) );
	QObject::connect( apHelpButton, SIGNAL(clicked()), this, SLOT( help() ) );

	QObject::connect(apAddRowButton, SIGNAL(clicked()), this, SLOT(insertSegment()));
	QObject::connect(apRemoveRowButton, SIGNAL(clicked()), this, SLOT(removeSegment()));

	QObject::connect(apCreateAirfoilButton, SIGNAL(clicked()), this, SLOT(createAirfoil()));

	// Line edits
	QObject::connect(apLineEditMapper, SIGNAL(mapped(int)), this, SLOT(lineEditChanged(int)));

	QObject::connect(apNumEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apIDEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apStallEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apCnAngleEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apCnSlopeEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apCnExtrapolEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apCnStallEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apAttackEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));
	QObject::connect(apMinCDEdit, SIGNAL(textEdited(const QString &)), apLineEditMapper, SLOT(map()));

	QObject::connect(apAirfoilTable->GetModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentModelChanged()));

	// Selection model
	QObject::connect( apAirfoilSelector->getComboBox(), SIGNAL(currentIndexChanged(const QString &)), this, SLOT(airfoilSelected()));
}

void FuiQtAirfoilDefinition::insertSegment(){
	switch(apAirfoilTable->GetModel()->getTableOrdering()){
	case ROW_DOMINANT:
		apAirfoilTable->GetModel()->insertRow(apAirfoilTable->GetModel()->rowCount());
		break;
	default:
		break;
	}
}

void FuiQtAirfoilDefinition::removeSegment(){
	switch(apAirfoilTable->GetModel()->getTableOrdering()){
	case ROW_DOMINANT:
		apAirfoilTable->GetModel()->removeRow(apAirfoilTable->GetModel()->rowCount()-1);
		break;
	default:
		break;
	}
}


void FuiQtAirfoilDefinition::acceptClicked()
{
  this->save(apAirfoilSelector->getCurrentItem());
}


void FuiQtAirfoilDefinition::airfoilSelected()
{
  this->setCurrentAirfoil(apAirfoilSelector->getCurrentItem());
}


void FuiQtAirfoilDefinition::setCurrentAirfoil(QString airfoilPath){
	// Check if the current airfoil is unsaved, and pop-up save-message, before changing to new airfoil
	if(currentAirfoilTouched){
		int ret = QMessageBox::warning(this, tr("Closing"), tr("Do you want to save changes to the previous file?"),
			QMessageBox::Yes | QMessageBox::No );

		if(ret == QMessageBox::Yes){
			save(currentAirfoil);
		}
	}

	QFile file(airfoilPath);

	if (file.open(QFile::ReadOnly)){
		// Parse file
		QTextStream ts(&file);
		// Lambda function reading a parameter value from the the file
		auto&& readValue = [&ts]() -> QString
		{
		  QString line = ts.readLine().trimmed();
		  return line.left(line.indexOf(" "));
		};
		// Skip header
		ts.readLine();
		ts.readLine();
		// number of tables
		apNumEdit->setText(readValue());
		// Table-id
		apIDEdit->setText(readValue());
		// Stall
		apStallEdit->setText(readValue());
		// Skip three unused parameters
		ts.readLine();
		ts.readLine();
		ts.readLine();
		// Zero Cn angle of attack
		apCnAngleEdit->setText(readValue());
		// Cn slope
		apCnSlopeEdit->setText(readValue());
		// Cn extrapolated
		apCnExtrapolEdit->setText(readValue());
		// Cn at stall
		apCnStallEdit->setText(readValue());
		// angle of attack
		apAttackEdit->setText(readValue());
		// minimum CD
		apMinCDEdit->setText(readValue());

		// Get table
		QVector<QVector<QVariant>*>* pData = new QVector<QVector<QVariant>*>;
		while (!ts.atEnd()) {
			QVector<QVariant>* pRow = new QVector<QVariant>;
			QString line = ts.readLine().trimmed();
			QStringList rowList = line.split(" ",Qt::SkipEmptyParts);
			if (rowList.size() == 4) {
				pRow->push_back(rowList.at(0).toDouble());
				pRow->push_back(rowList.at(1).toDouble());
				pRow->push_back(rowList.at(2).toDouble());
				pRow->push_back(rowList.at(3).toDouble());
				pData->push_back(pRow);
			}
		}

		if (pData->isEmpty()) {
			// If no data in file, initialize table with some data, then remove it
			pData->push_back(new QVector<QVariant>(4, 0.0));
			apAirfoilTable->GetModel()->SetModelData(pData);
			this->removeSegment();
		}
		else
			apAirfoilTable->GetModel()->SetModelData(pData);

		file.close();
	}
	else{
		apNumEdit->setText("Not Found!");
		apIDEdit->setText("Not Found!");
		apStallEdit->setText("Not Found!");
		apCnAngleEdit->setText("Not Found!");
		apCnSlopeEdit->setText("Not Found!");
		apCnExtrapolEdit->setText("Not Found!");
		apCnStallEdit->setText("Not Found!");
		apAttackEdit->setText("Not Found!");
		apMinCDEdit->setText("Not Found!");
		apAirfoilTable->GetModel()->SetModelData(new QVector<QVector<QVariant>*>);
	}

	//Set readOnly states
	bool readOnly = false;
	apAirfoilSelector->getModel()->itemIsReadOnly(readOnly,apAirfoilSelector->getCurrentItem());

	apAirfoilTable->GetModel()->SetEditable(!readOnly);

	apNumEdit->setDisabled(readOnly);
	apIDEdit->setDisabled(readOnly);
	apStallEdit->setDisabled(readOnly);
	apCnAngleEdit->setDisabled(readOnly);
	apCnSlopeEdit->setDisabled(readOnly);
	apCnExtrapolEdit->setDisabled(readOnly);
	apCnStallEdit->setDisabled(readOnly);
	apAttackEdit->setDisabled(readOnly);
	apMinCDEdit->setDisabled(readOnly);

	apAddRowButton->setDisabled(readOnly);
	apRemoveRowButton->setDisabled(readOnly);
	apSaveButton->setDisabled(readOnly);
	apCreateAirfoilButton->setDisabled(readOnly);

	currentAirfoilTouched = false;
	apSaveButton->setEnabled(false);
	currentAirfoil = airfoilPath;
}

void FuiQtAirfoilDefinition::createAirfoil(){
	QString folder = apAirfoilSelector->getCurrentFolder();
	QString filePath = QFileDialog::getSaveFileName(this, "Create airfoil", folder, "Airfoil(*.dat)");
	QFile file(filePath);

	if(file.open(QIODevice::WriteOnly)) {
		QString newContent = "Airfoil definition file\n";
		newContent+= "Generated by FEDEM Windpower. " + QDate::currentDate().toString() + "\n";
		newContent+= "1    Number of airfoil tables in this file\n";
		newContent+= "0    Table ID parameter\n";
		newContent+= "0.0    Stall angle (deg)\n";
		newContent+= "0.0    No longer used, enter zero\n";
		newContent+= "0.0    No longer used, enter zero\n";
		newContent+= "0.0    No longer used, enter zero\n";
		newContent+= "0.0    Zero Cn angle of attack (deg)\n";
		newContent+= "0.0    Cn slope for zero lift (dimensionless)\n";
		newContent+= "0.0    Cn extrapolated to value at positive stall angle of attack\n";
		newContent+= "0.0    Cn at stall value for negative angle of attack\n";
		newContent+= "0.0    Angle of attack for minimum CD (deg)\n";
		newContent+= "0.0    Minimum CD value\n";

		file.write(newContent.toLocal8Bit());

		file.close();
		apAirfoilSelector->addEntry(filePath);
		apAirfoilSelector->setLibrary(folder, false); //necessary to update qcombobox
		apAirfoilSelector->selectItem(filePath);

		return;
	}

	QMessageBox::warning(this,tr("Could not create file!"), tr("The program was unable to create a new airfoil-file in the specified directory. Please try a different path"), QMessageBox::Ok);
}

bool FuiQtAirfoilDefinition::save(QString airfoilPath){
	// Don't save if airfoil is a locked airfoil, or if it is not changed
    bool readOnly = true;
    apAirfoilSelector->getModel()->itemIsReadOnly(readOnly,airfoilPath);
	if (readOnly) return false; // Airfoil is locked, so don't save

	if (!QFile::exists(airfoilPath)) {
		QString filePath = QFileDialog::getSaveFileName(this, "Save airfoil", airfoilPath, "Airfoil(*.dat)");
		if (filePath.isEmpty() || filePath == airfoilPath) return false; // Airfoil path was not changed, so don't save
		if (!QFile::exists(filePath)) return false;
		airfoilPath = filePath;
	}

	// Save to file
	QFile file(airfoilPath);
	if (!file.open(QFile::ReadWrite)) return false;
	QTextStream ts(&file);

	// Lambda function reading a parameter value from the the file
	auto&& readValue = [&ts](const QString& text) -> QString
	{
	  QString line = ts.readLine();
	  QStringList splitList = line.trimmed().split(QRegularExpression("\\s+"));
	  line.replace(splitList.at(0), text);
	  return line + "\n";
	};

	// header
	QString newContent = ts.readLine() + "\n";
	newContent += ts.readLine() + "\n";
	// number of tables
	newContent += readValue(apNumEdit->text());
	// Table-id
	newContent += readValue(apIDEdit->text());
	// Stall angle
	newContent += readValue(apStallEdit->text());
	// unused parameters
	newContent += ts.readLine() + "\n";
	newContent += ts.readLine() + "\n";
	newContent += ts.readLine() + "\n";
	// Zero Cn angle of attack
	newContent += readValue(apCnAngleEdit->text());
	// Cn slope
	newContent += readValue(apCnSlopeEdit->text());
	// Cn extrapolated
	newContent += readValue(apCnExtrapolEdit->text());
	// Cn at stall
	newContent += readValue(apCnStallEdit->text());
	// angle of attack
	newContent += readValue(apAttackEdit->text());
	// minimum CD
	newContent += readValue(apMinCDEdit->text());
	// table
	for (QVector<QVariant>* seg : *apAirfoilTable->GetModel()->GetModelData())
	  newContent += seg->at(0).toString() + "  "
		         +  seg->at(1).toString() + "  "
	             +  seg->at(2).toString() + "  "
		         +  seg->at(3).toString() + "\n";

	file.close();
	// Open, truncate and write new content
	file.open(QFile::ReadWrite | QFile::Truncate);
	file.write(newContent.toLocal8Bit());
	file.close();

	currentAirfoilTouched = false;
	apSaveButton->setEnabled(false);

	return true;
}

void FuiQtAirfoilDefinition::lineEditChanged(int){
	currentAirfoilTouched = true;
	apSaveButton->setEnabled(true);
}

void FuiQtAirfoilDefinition::currentModelChanged(){
	currentAirfoilTouched = true;
	apSaveButton->setEnabled(true);
}

void FuiQtAirfoilDefinition::help(){
  // Display the topic in the help file
  Fui::showCHM("dialogbox/windpower/airfoil-definition.htm");
}
