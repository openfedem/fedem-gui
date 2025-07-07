// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QApplication>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"


FFuQtIOField::FFuQtIOField(QWidget* parent) : QLineEdit(parent), myTip(NULL)
{
  this->setWidget(this);

  QObject::connect(this,SIGNAL(textChanged(const QString&)),this,SLOT(fwdTextChanged(const QString&)));
  QObject::connect(this,SIGNAL(returnPressed()),this,SLOT(fwdReturnPressed()));
}


FFuQtIOField::~FFuQtIOField()
{
  if (myTip) delete[] myTip;
}


////////////////////////////////////////////////////////////////////////
//
//  Access methods :
//

void FFuQtIOField::setToolTip(const char* tip)
{
  this->FFuQtComponentBase::setToolTip(tip);

  if (myTip) delete[] myTip;
  if (tip)
  {
    myTip = new char[strlen(tip)+1];
    strcpy(myTip,tip);
  }
}


void FFuQtIOField::setValue(const char* value)
{
  this->blockLibSignals(true);

  this->setText(value);
  this->setCursorPosition(0);
  if (this->getWidth() < this->getFontWidth(value))
    this->FFuQtComponentBase::setToolTip(value);
  else
    this->FFuQtComponentBase::setToolTip(myTip);

  this->blockLibSignals(false);

  this->setBackgroundColor(255, 255, 255);
}


std::string FFuQtIOField::getValue() const
{
  return this->text().toStdString();
}


////////////////////////////////////////////////////////////////////////
//
//  Highlighting :
//

void FFuQtIOField::highlight()
{
  this->selectAll();
}

void FFuQtIOField::unHighlight()
{
  this->deselect();
}


////////////////////////////////////////////////////////////////////////
//
//  Slots to forward signals :
//

void FFuQtIOField::fwdTextChanged(const QString& txt)
{
  this->setBackgroundColor(255, 255, 208);

  this->tChanged(txt.toStdString());
}

void FFuQtIOField::fwdReturnPressed()
{
  this->setBackgroundColor(255, 255, 255);

  this->rtPressed();
}


////////////////////////////////////////////////////////////////////////
//
//  Reimplementation of Event handlers to control Focus In and Out :
//

void FFuQtIOField::focusInEvent(QFocusEvent* e)
{
  this->QLineEdit::focusInEvent(e);

  this->keybFocusIn();
}

void FFuQtIOField::focusOutEvent(QFocusEvent* e)
{
  this->QLineEdit::focusOutEvent(e);

  // Needs some smart code to distinguish a focus out from a "Real" focus out

  // if (qApp->focusWidget()->topLevelWiget() == this->topLevelWidget()) etc ..

  this->keybFocusOut(false);
}


////////////////////////////////////////////////////////////////////////
//
//  Colors and fonts. Reimplemented from FFuQtComponentBase,
//  to make special use of the palette and FontSet:
//

void FFuQtIOField::setColors(FFuaPalette aPalette)
{
  int r,g,b;

  aPalette.getStdBackground(r, g, b);
  QColor StdBackground(255,0,0);

  aPalette.getFieldBackground(r, g, b);
  QColor FieldBackground(r, g, b);

  aPalette.getTextOnStdBackground(r, g, b);
  QColor TextOnStdBackground(r, g, b);

  aPalette.getTextOnFieldBackground(r, g, b);
  QColor TextOnFieldBackground(r, g, b);

  aPalette.getDarkShadow(r, g, b);
  QColor DarkShadow (r, g, b);

  aPalette.getMidShadow(r, g, b);
  QColor MidShadow (r, g, b);

  aPalette.getLightShadow(r, g, b);
  QColor LightShadow(r, g, b);

  QPalette textFieldPalette;
  textFieldPalette.setColor(QPalette::WindowText, TextOnFieldBackground);
  textFieldPalette.setColor(QPalette::Window, StdBackground);
  textFieldPalette.setColor(QPalette::Light, LightShadow);
  textFieldPalette.setColor(QPalette::Dark, DarkShadow);
  textFieldPalette.setColor(QPalette::Mid, MidShadow);
  textFieldPalette.setColor(QPalette::Text, TextOnFieldBackground);
  textFieldPalette.setColor(QPalette::Base, FieldBackground);
  this->QLineEdit::setPalette(textFieldPalette);
  this->setAutoFillBackground(true);

}


void FFuQtIOField::setFonts(FFuaFontSet aFontSet)
{
  QFont textFieldFont;

  textFieldFont.setFamily   (aFontSet.TextFieldFont.Family.c_str());
  textFieldFont.setPointSize(aFontSet.TextFieldFont.Size    );
  textFieldFont.setBold     (aFontSet.TextFieldFont.IsBold  );
  textFieldFont.setItalic   (aFontSet.TextFieldFont.IsItalic);

  this->QLineEdit::setFont(textFieldFont);
}


/*!
  Sets the background color of the widget.
  This is used to assign a yellow background color to modified controls.
*/

void FFuQtIOField::setBackgroundColor(int r, int g, int b)
{
  if (useCustomBackgroundColor) return;

  bool bEnabled = !this->isReadOnly() && this->isEnabled();
  QPalette pal = this->palette();
  pal.setColor(this->backgroundRole(), bEnabled ? QColor(r,g,b) : QColor(240,240,240));
  this->setPalette(pal);
}


void FFuQtIOField::setSensitivity(bool makeSensitive)
{
  QPalette desktopP = QApplication::palette();

  if (!makeSensitive) {
    QColor background = desktopP.color(QPalette::Disabled, QPalette::Window);
    desktopP.setColor(QPalette::Disabled, QPalette::Base, background);

    QColor tmpColor;
    tmpColor = desktopP.color(QPalette::Disabled, QPalette::WindowText);
    desktopP.setColor(QPalette::Active, QPalette::WindowText, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::WindowText, tmpColor);

    tmpColor = desktopP.color(QPalette::Disabled, QPalette::Window);
    desktopP.setColor(QPalette::Active, QPalette::Window, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::Window, tmpColor);

    tmpColor = desktopP.color(QPalette::Disabled, QPalette::Light);
    desktopP.setColor(QPalette::Active, QPalette::Light, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::Light, tmpColor);

    tmpColor = desktopP.color(QPalette::Disabled, QPalette::Dark);
    desktopP.setColor(QPalette::Active, QPalette::Dark, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::Dark, tmpColor);

    tmpColor = desktopP.color(QPalette::Disabled, QPalette::Mid);
    desktopP.setColor(QPalette::Active, QPalette::Mid, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::Mid, tmpColor);

    tmpColor = desktopP.color(QPalette::Disabled, QPalette::Text);
    desktopP.setColor(QPalette::Active, QPalette::Text, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::Text, tmpColor);

    tmpColor = desktopP.color(QPalette::Disabled, QPalette::Base);
    desktopP.setColor(QPalette::Active, QPalette::Base, tmpColor);
    desktopP.setColor(QPalette::Inactive, QPalette::Base, tmpColor);
  }

  this->setPalette(desktopP);
  this->setReadOnly(!makeSensitive);
}


bool FFuQtIOField::getSensitivity()
{
  return !this->isReadOnly();
}
