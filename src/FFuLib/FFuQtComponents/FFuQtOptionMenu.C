// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QLineEdit>

#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FFuQtOptionMenu::FFuQtOptionMenu(QWidget* parent) : QComboBox(parent)
{
  this->setWidget(this);
  this->setInsertPolicy(QComboBox::NoInsert);

  QObject::connect(this,SIGNAL(activated(int)),
                   this,SLOT(activatedFwd(int)));
  QObject::connect(this,SIGNAL(textActivated(const QString&)),
                   this,SLOT(activatedFwd(const QString&)));
  QObject::connect(this,SIGNAL(highlighted(int)),
                   this,SLOT(highlightedFwd(int)));
}


void FFuQtOptionMenu::setOptions(const std::vector<std::string>& options)
{
  this->clear();
  this->shadowDoubles.clear();
  this->shadowInts.clear();

  for (const std::string& option : options)
    this->insertItem(options.size(), option.c_str());
}


void FFuQtOptionMenu::setOptions(const std::set<std::string>& options)
{
  this->clear();
  this->shadowDoubles.clear();
  this->shadowInts.clear();

  for (const std::string& option : options)
    this->insertItem(options.size(), option.c_str());
}


void FFuQtOptionMenu::setOptions(const std::vector<double>& options)
{
  this->clear();
  this->shadowDoubles = options;
  this->shadowInts.clear();

  for (double option : options)
    this->insertItem(options.size(), FFaNumStr(option,1).c_str());
}


void FFuQtOptionMenu::setOptions(const std::vector<int>& options)
{
  this->clear();
  this->shadowInts = options;
  this->shadowDoubles.clear();

  for (int option : options)
    this->insertItem(options.size(), FFaNumStr(option).c_str());
}


bool FFuQtOptionMenu::selectDoubleOption(double opt)
{
  bool wasblocked = this->areLibSignalsBlocked();
  this->blockLibSignals(true);

  int index = 0;
  for (double value : this->shadowDoubles)
    if (value == opt) {
      this->setCurrentIndex(index);
      this->blockLibSignals(wasblocked);
      return true;
    }
    else
      ++index;

  if (this->isEditable()) {
    this->setEditText(FFaNumStr(opt,1).c_str());
    this->blockLibSignals(wasblocked);
    return true;
  }

  this->blockLibSignals(wasblocked);
  return false;
}


bool FFuQtOptionMenu::selectIntOption(int opt)
{
  bool wasblocked = this->areLibSignalsBlocked();
  this->blockLibSignals(true);

  int index = 0;
  for (int value : this->shadowInts)
    if (value == opt) {
      this->setCurrentIndex(index);
      this->blockLibSignals(wasblocked);
      return true;
    }
    else
      ++index;

  if (this->isEditable()) {
    this->setEditText(FFaNumStr(opt).c_str());
    this->blockLibSignals(wasblocked);
    return true;
  }

  this->blockLibSignals(wasblocked);
  return false;
}


void FFuQtOptionMenu::addOption(const char* aText, int index, bool replace)
{
  this->shadowDoubles.clear();
  this->shadowInts.clear();

  if (replace && index >= 0 && index < this->count())
    this->setItemText(index, aText);
  else
    this->insertItem(this->count(), aText);
}

void FFuQtOptionMenu::clearOptions()
{
  this->clear();
  this->shadowDoubles.clear();
  this->shadowInts.clear();
}

void FFuQtOptionMenu::selectOption(int optionIndex)
{
  this->setCurrentIndex(optionIndex);
}

bool FFuQtOptionMenu::selectOption(const std::string& text, bool notify)
{
  bool wasblocked = this->areLibSignalsBlocked();
  if (!notify) this->blockLibSignals(true);

  for (int i = 0; i < this->count(); i++)
    if (this->itemText(i).toStdString() == text) {
      this->setCurrentIndex(i);
      this->blockLibSignals(wasblocked);
      return true;
    }

  this->setEditText(text.c_str());
  this->blockLibSignals(wasblocked);
  return false;
}

void FFuQtOptionMenu::disableOption(int)
{
}

void FFuQtOptionMenu::removeOption(int optionIndex)
{
  this->removeItem(optionIndex);
}

int FFuQtOptionMenu::getSelectedOption() const
{
  return this->currentIndex();
}

std::string FFuQtOptionMenu::getSelectedOptionStr() const
{
  return this->currentText().toStdString();
}

int FFuQtOptionMenu::getOptionCount() const
{
  return this->count();
}

void FFuQtOptionMenu::wheelEvent(QWheelEvent*)
{
  // TT #2879: disable wheel event completely
}

void FFuQtOptionMenu::activatedFwd(int selectedOption)
{
  myOptionSelectedCB.invoke(selectedOption);
}

void FFuQtOptionMenu::activatedFwd(const QString& text)
{
  myOptionChangedCB.invoke(text.toStdString());
}

void FFuQtOptionMenu::highlightedFwd(int idx)
{
  myOptionHighlightedCB.invoke(idx);
}

int FFuQtOptionMenu::getBasicWidthHint()
{
  QFontMetrics fm = this->fontMetrics();
  int w = 0, newW = 0;

  for (int i = 0; i < this->count(); i++)
    if ((newW = fm.horizontalAdvance(this->itemText(i))) > w)
      w = newW;

  w += 30; //down arrow

  return w < 50 ? 50 : w;
}


void FFuQtOptionMenu::setColors(FFuaPalette aPalette)
{
  int r, g, b;

  aPalette.getStdBackground(r, g, b);
  QColor   StdBackground   (r, g, b);

  aPalette.getFieldBackground(r, g, b);
  QColor   FieldBackground   (r, g, b);

  aPalette.getTextOnStdBackground(r, g, b);
  QColor   TextOnStdBackground   (r, g, b);

  aPalette.getTextOnFieldBackground(r, g, b);
  QColor   TextOnFieldBackground   (r, g, b);

  aPalette.getIconText   (r, g, b);
  QColor   IconText      (r, g, b);

  aPalette.getDarkShadow (r, g, b);
  QColor   DarkShadow    (r, g, b);

  aPalette.getMidShadow  (r, g, b);
  QColor   MidShadow     (r, g, b);

  aPalette.getLightShadow(r, g, b);
  QColor   LightShadow   (r, g, b);

  QPalette textFieldPalette;

  textFieldPalette.setColorGroup(QPalette::Active,
                                 TextOnStdBackground, FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground, TextOnStdBackground,
                                 FieldBackground, StdBackground);
  textFieldPalette.setColorGroup(QPalette::Disabled,
                                 TextOnStdBackground.darker(125),
                                 FieldBackground.darker(80),
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground.darker(125),
                                 TextOnStdBackground.darker(125),
                                 FieldBackground.darker(125), StdBackground);
  textFieldPalette.setColorGroup(QPalette::Inactive,
                                 TextOnStdBackground, FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground, TextOnStdBackground,
                                 FieldBackground, StdBackground);

  this->setPalette(textFieldPalette);
}
