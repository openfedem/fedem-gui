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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSignalMapper>
#include <QTextStream>
#include <QFileDialog>
#include <QFile>
#include <QDate>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include <QCloseEvent>
#include <QComboBox>
#include <QSplitter>

#include "vpmUI/vpmUITopLevels/vpmUIQtTopLevels/FuiQtAirfoilDefinition.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFuLib/FFuQtComponents/FFuQtLabel.H"
#include "FFuLib/FFuCustom/mvcModels/AirfoilSelectionModel.H"
#include "FFuLib/FFuCustom/components/guiComponents/AirfoilSelector.H"
#include "FFuLib/FFuCustom/inputTables/InputTable.H"
#include "FFuLib/FFuCustom/inputTables/delegates/DoubleFieldDelegate.H"


FuiAirfoilDefinition* FuiAirfoilDefinition::create(int xpos, int ypos,
                                                   int width, int height,
                                                   const char* title,
                                                   const char* name)
{
  return new FuiQtAirfoilDefinition(xpos, ypos, width, height, title, name);
}


FuiQtAirfoilDefinition::FuiQtAirfoilDefinition(int xpos, int ypos,
                                               int width, int height,
                                               const char* title,
                                               const char* name)
  : FFuQtTopLevelShell(NULL, xpos, ypos, width, height, title, name)
{
	this->setFixedWidth(size().width());

	// ** Create widgets **
	QSplitter* apSplitter = new QSplitter(this);
	apSplitter->setOrientation(Qt::Vertical);
	apSplitter->setChildrenCollapsible(false);
	QWidget* apBottomContainer = new QWidget(this);

	QWidget* apNotes = new FFuQtNotes(this,
					  "Use this dialog to view and modify "
					  "airfoil files in the AeroDyn format.");

	apAddRowButton = new QPushButton("Add Row",this);
	apRemoveRowButton = new QPushButton("Remove Row",this);

	apSaveButton = new QPushButton("&Save",this);
	apCloseButton = new QPushButton("&Close",this);
	apHelpButton = new QPushButton("&Help",this);

	apCreateAirfoilButton = new QPushButton("&New Airfoil",this);

	//Line edits
	apNumEdit = new QLineEdit(this);
	apNumEdit->setFixedWidth(200);
	apIDEdit = new QLineEdit(this);
	apIDEdit->setFixedWidth(200);
	apStallEdit = new QLineEdit(this);
	apStallEdit->setFixedWidth(200);
	apCnAngleEdit = new QLineEdit(this);
	apCnAngleEdit->setFixedWidth(200);
	apCnSlopeEdit = new QLineEdit(this);
	apCnSlopeEdit->setFixedWidth(200);
	apCnExtrapolEdit = new QLineEdit(this);
	apCnExtrapolEdit->setFixedWidth(200);
	apCnStallEdit = new QLineEdit(this);
	apCnStallEdit->setFixedWidth(200);
	apAttackEdit = new QLineEdit(this);
	apAttackEdit->setFixedWidth(200);
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
	apAirfoilTable = new InputTable(2,4, ROW_DOMINANT, this);
	apAirfoilTable->setFixedWidth(420);

	apAirfoilTable->GetView()->verticalHeader()->setDefaultSectionSize(20);
	apAirfoilTable->GetView()->horizontalHeader()->setFixedHeight(20);

	//Setup vertical headers for tables
	QStringList headers({ "alpha", "CL", "CD", "CM" });
	apAirfoilTable->GetModel()->setHeaders(headers);

	//Add delegate
	apAirfoilTable->GetView()->setItemDelegate(new DoubleFieldDelegate(4,0.1,apAirfoilTable->GetView()));

	// ** Create layouts **
	QBoxLayout* apMainLayout = new QVBoxLayout(this);
	QBoxLayout* apContentLayout = new QHBoxLayout();
	QBoxLayout* apLineEditLayout = new QVBoxLayout();
	QBoxLayout* apTableButtonLayout = new QHBoxLayout();
	QBoxLayout* apTableLayout  = new QVBoxLayout();
	QBoxLayout* apDialogButtonLayout = new QHBoxLayout();

	// ** Initialize layouts **
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
	apTableLayout->addWidget(apNotes);

	apLineEditLayout->addWidget(new QLabel("Number of airfoil tables in this file:"));
	apLineEditLayout->addWidget(apNumEdit);
	apLineEditLayout->addWidget(new QLabel("Table ID parameter:"));
	apLineEditLayout->addWidget(apIDEdit);
	apLineEditLayout->addWidget(new QLabel("Stall angle[deg]:"));
	apLineEditLayout->addWidget(apStallEdit);
	apLineEditLayout->addWidget(new QLabel("Zero C<sub>n</sub> angle of attack[deg]:"));
	apLineEditLayout->addWidget(apCnAngleEdit);
	apLineEditLayout->addWidget(new QLabel("C<sub>n</sub> slope for zero lift (dimensionless):"));
	apLineEditLayout->addWidget(apCnSlopeEdit);
	apLineEditLayout->addWidget(new QLabel("C<sub>n</sub> extrapolated to value at positive stall angle of attack:"));
	apLineEditLayout->addWidget(apCnExtrapolEdit);
	apLineEditLayout->addWidget(new QLabel("C<sub>n</sub> at stall value for negative angle of attack:"));
	apLineEditLayout->addWidget(apCnStallEdit);
	apLineEditLayout->addWidget(new QLabel("Angle of attack for minimum C<sub>D</sub>[deg]:"));
	apLineEditLayout->addWidget(apAttackEdit);
	apLineEditLayout->addWidget(new QLabel("Minimum C<sub>D</sub> value:"));
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

	connections();

	currentAirfoilTouched = false;
}


void FuiQtAirfoilDefinition::showEvent(QShowEvent*)
{
  /* Note: Removing this refresh() call as it causes the GUI to hang after
   * the upgrade to Qt 6.8, when the Airfoil browser dialog is launched.
   * This does not happen on Linux with Qt 6.2.
   * Not sure why, but the call seems unnecessary anyway as the dialog opens
   * nicely without it. Therefore removing it as a temporary solution. Revist
   * this when/if further work on Airfoils editing requires it. KMO 15.09.25 */
#ifdef linux64
  apAirfoilSelector->refresh();
#endif
  apAirfoilSelector->selectItem("");
}


void FuiQtAirfoilDefinition::closeEvent(QCloseEvent* event)
{
  // Check for unsaved airfoil, and pop-up save-message
  if (currentAirfoilTouched)
    switch (QMessageBox::warning(this, "Closing", "Do you want to save changes to the current file?",
				 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
    case QMessageBox::Cancel:
      event->ignore();
      return;
    case QMessageBox::Yes:
      this->saveAirfoil(apAirfoilSelector->getCurrentItem());
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
	QObject::connect(apLineEditMapper, SIGNAL(mappedInt(int)), this, SLOT(lineEditChanged(int)));

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
	QObject::connect(apAirfoilSelector->getComboBox(), SIGNAL(currentTextChanged(const QString&)), this, SLOT(airfoilSelected()));
}


void FuiQtAirfoilDefinition::insertSegment()
{
  TableModel* model = apAirfoilTable->GetModel();
  if (model->getTableOrdering() == ROW_DOMINANT)
    model->insertRow(model->rowCount());
}


void FuiQtAirfoilDefinition::removeSegment()
{
  TableModel* model = apAirfoilTable->GetModel();
  if (model->getTableOrdering() == ROW_DOMINANT)
    model->removeRow(model->rowCount()-1);
}


void FuiQtAirfoilDefinition::acceptClicked()
{
  this->saveAirfoil(apAirfoilSelector->getCurrentItem());
}


void FuiQtAirfoilDefinition::airfoilSelected()
{
  this->setCurrentAirfoil(apAirfoilSelector->getCurrentItem());
}


void FuiQtAirfoilDefinition::setCurrentAirfoil(const QString& airfoilPath)
{
  // Check if the current airfoil is unsaved, and pop-up save-message, before changing to new airfoil
  if (currentAirfoilTouched && QMessageBox::warning(this, "Closing",
                                                    "Do you want to save changes to the previous file?",
                                                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    this->saveAirfoil(currentAirfoil);

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

	if (file.open(QIODeviceBase::WriteOnly)) {
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

	QMessageBox::warning(this, "Could not create file!",
                             "The program was unable to create a new airfoil-file in the specified directory. "
                             "Please try a different path", QMessageBox::Ok);
}


bool FuiQtAirfoilDefinition::saveAirfoil(const QString& airfoilPath)
{
  if (QFile::exists(airfoilPath))
  {
    // Don't save if airfoil is locked, or if it is unchanged
    bool readOnly = true;
    apAirfoilSelector->getModel()->itemIsReadOnly(readOnly,airfoilPath);
    if (readOnly) return false; // Airfoil is locked, so don't save
  }
  else
  {
    // Query for a new airfoil path
    QString path = QFileDialog::getSaveFileName(this, "Save airfoil",
	                                        airfoilPath, "Airfoil(*.dat)");
    if (path.isEmpty() || path == airfoilPath)
      return false; // Airfoil path was not changed, so don't save

    // Start over with the new path
    return this->saveAirfoil(path);
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

void FuiQtAirfoilDefinition::currentModelChanged(){
  apSaveButton->setEnabled(currentAirfoilTouched = true);
}

void FuiQtAirfoilDefinition::help(){
  // Display the topic in the help file
  Fui::showCHM("dialogbox/windpower/airfoil-definition.htm");
}
