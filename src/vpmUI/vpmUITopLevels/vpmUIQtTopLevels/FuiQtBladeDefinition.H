// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_BLADE_DEFINITION_H
#define FUI_QT_BLADE_DEFINITION_H

#include "FFuLib/FFuQtBaseClasses/FFuQtTopLevelShell.H"
#include "vpmUI/vpmUITopLevels/FuiBladeDefinition.H"

class InputTable;
class Blade;
class BladeDrawer;
class BladeSelector;
class FileFieldDelegate;
class QPushButton;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QSignalMapper;
class QTabWidget;


class FuiQtBladeDefinition : public FFuQtTopLevelShell,
                             public FuiBladeDefinition
{
  Q_OBJECT

public:
  FuiQtBladeDefinition(int xpos, int ypos, int width, int height,
                       const char* title, const char* name);
  ~FuiQtBladeDefinition();

private:
	//**********Widgets**********
	QSignalMapper* apCheckboxMapper;
	BladeDrawer* apBladeDrawer;
	QTabWidget* apTabWidget;

	QLineEdit* apNameEdit;

	BladeSelector* apBladeSelector;

	QPushButton* apSaveButton;
	QPushButton* apCloseButton;
	QPushButton* apHelpButton;

	QPushButton* apAddSegmentButton;
	QPushButton* apRemoveSegmentButton;
	QPushButton* apCreateBladeButton;

	QCheckBox* apBendingStiffnesCheck;
	QCheckBox* apAxialStiffnesCheck;
	QCheckBox* apTorsionStiffnesCheck;
	QCheckBox* apShearStiffnesCheck;

	QWidget* apStructureTab;

	InputTable* apAerodynamicTable;
	InputTable* apStructureTable;

	FileFieldDelegate* pSelectorDelegate;

	//**********members**********
	std::vector<Blade*> apBlades;
	Blade* apCurrentBlade;
	bool bladeChanged;

	void connections();
	bool setCurrentBlade(Blade* pBlade);
	bool saveBlade(Blade* blade);
	void resolveAirfoils(Blade* pBlade);

  virtual void showEvent(QShowEvent*);
  virtual void closeEvent(QCloseEvent* event);
  virtual void setSensitivity(bool) {}

public slots:
	void initializeBlades();
	void acceptClicked();
	void bladeSelected();
	void insertSegment();
	void removeSegment();
	void updateNameField();
	void nameChanged();
	void currentModelChanged();
	void createBlade();
	void checkBoxChanged(int checkBox);
	void updateCheckBoxes();
	void changeAirfoil();
	void help();
	void itemClicked(const QModelIndex& index); //Used to display comboBox for airfoil selection on single-click

signals:
	void currentBladeChanged();
};

#endif
