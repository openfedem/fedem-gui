// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_AIRFOIL_DEFINITION_H
#define FUI_QT_AIRFOIL_DEFINITION_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiAirfoilDefinition.H"

class AirfoilSelector;
class InputTable;
class QPushButton;
class QLineEdit;
class QSignalMapper;


class FuiQtAirfoilDefinition : public FFuQtTopLevelShell,
                               public FuiAirfoilDefinition
{
  Q_OBJECT

public:
  FuiQtAirfoilDefinition(int xpos, int ypos, int width, int height,
                         const char* title, const char* name);

private:
	// Widgets
	InputTable*      apAirfoilTable;
	AirfoilSelector* apAirfoilSelector;

	//Buttons
	QPushButton* apAddRowButton;
	QPushButton* apRemoveRowButton;

	QPushButton* apSaveButton;
	QPushButton* apCloseButton;
	QPushButton* apHelpButton;

	QPushButton* apCreateAirfoilButton;

	QSignalMapper* apLineEditMapper;

	//Line edits
	QLineEdit* apNumEdit;
	QLineEdit* apIDEdit;
	QLineEdit* apStallEdit;
	QLineEdit* apCnAngleEdit;
	QLineEdit* apCnSlopeEdit;
	QLineEdit* apCnExtrapolEdit;
	QLineEdit* apCnStallEdit;
	QLineEdit* apAttackEdit;
	QLineEdit* apMinCDEdit;

	bool currentAirfoilTouched;
	QString currentAirfoil;

	void connections();

  void setCurrentAirfoil(const QString& airfoilPath);
  bool saveAirfoil(const QString& airfoilPath);

  virtual void showEvent(QShowEvent*);
  virtual void closeEvent(QCloseEvent* event);
  virtual void setSensitivity(bool) {}

private slots:
	void insertSegment();
	void removeSegment();
	void acceptClicked();
	void createAirfoil();
	void airfoilSelected();
	void currentModelChanged();
	void lineEditChanged(int) { this->currentModelChanged(); }
	void help();
};

#endif
