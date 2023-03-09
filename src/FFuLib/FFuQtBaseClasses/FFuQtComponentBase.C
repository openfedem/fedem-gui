// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \file FFuQtComponentBase.C
  \brief Implementation of the Fedem "Widget equivalent", using Qt.
  \author Jacob Storen
  \date 3 Feb 1999
*/

#include <QWidget>
#include <QFontMetrics>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"


/*!
  Method to be called from Subclass Constructor
  (Which must be subclass of QWidget) to initialise the widget pointer.
*/

void FFuQtComponentBase::setWidget(QWidget* widget)
{
  myWidget = widget;
  // Qt default is false, Fedem default is true.
  myWidget->setAutoFillBackground(true);
}

////////////////////////////////////////////////////////////////////////
//
// Geometry management :
//

void FFuQtComponentBase::setSizeGeometry(int leftX, int topY, int width, int height)
{
  if (width  < 0) width  = 0;
  if (height < 0) height = 0;

  myWidget->setGeometry(leftX,topY,width,height);
}


void FFuQtComponentBase::setWidth(int width)
{
  myWidget->resize(width,myWidget->height());
}


void FFuQtComponentBase::setHeight(int height)
{
  myWidget->resize(myWidget->width(),height);
}


void FFuQtComponentBase::setMinWidth(int width)
{
  myWidget->setMinimumWidth(width);
}


void FFuQtComponentBase::setMinHeight(int height)
{
  myWidget->setMinimumHeight(height);
}


void FFuQtComponentBase::setMaxWidth(int width)
{
  myWidget->setMaximumWidth(width);
}


void FFuQtComponentBase::setMaxHeight(int height)
{
  myWidget->setMaximumHeight(height);
}


////////////////////////////////////////////////////////////////////////
//
// Access size of widget :
//

int FFuQtComponentBase::getWidth()
{
  return myWidget->width();
}


int FFuQtComponentBase::getHeight()
{
  return myWidget->height();
}


////////////////////////////////////////////////////////////////////////
//
// Access Widget position :
// Including window frame of parent, if any.
//

int FFuQtComponentBase::getXPos()
{
  return myWidget->x();
}


int FFuQtComponentBase::getYPos()
{
  return myWidget->y();
}


////////////////////////////////////////////////////////////////////////
//
// Method to "Gray out" Widget :
//

void FFuQtComponentBase::setSensitivity(bool makeSensitive)
{
  myWidget->setEnabled(makeSensitive);
}


bool FFuQtComponentBase::getSensitivity()
{
  return myWidget->isEnabled();
}


////////////////////////////////////////////////////////////////////////
//
// Methods to show or hide widget :
//

void FFuQtComponentBase::popUp()
{
  myWidget->show();
}


void FFuQtComponentBase::popDown()
{
  myWidget->hide();
}


bool FFuQtComponentBase::isPoppedUp()
{
  return myWidget->isVisible();
}


////////////////////////////////////////////////////////////////////////
//
// Methods to edit the layering of the widgets :
//

void FFuQtComponentBase::toFront()
{
  myWidget->raise();
}


void FFuQtComponentBase::toBack()
{
  myWidget->lower();
}


////////////////////////////////////////////////////////////////////////
//
// Methods to set the look of the widget :
//

void FFuQtComponentBase::setToolTip(const char* tip)
{
  myWidget->setToolTip(tip ? tip : "");
}


void FFuQtComponentBase::setFonts(FFuaFontSet aFontSet)
{
  QFont stdFont;
  stdFont.setFamily   (aFontSet.StandardFont.Family.c_str());
  stdFont.setPointSize(aFontSet.StandardFont.Size    );
  stdFont.setBold     (aFontSet.StandardFont.IsBold  );
  stdFont.setItalic   (aFontSet.StandardFont.IsItalic);

  myWidget->setFont(stdFont);
}


void FFuQtComponentBase::setColors(FFuaPalette aPalette)
{
  int r, g, b;

  aPalette.getStdBackground(r, g, b);
  QColor StdBackground(r, g, b);

  aPalette.getFieldBackground(r, g, b);
  QColor FieldBackground(r, g, b);

  aPalette.getTextOnStdBackground(r, g, b);
  QColor TextOnStdBackground(r, g, b);

  aPalette.getTextOnFieldBackground(r, g, b);
  QColor TextOnFieldBackground(r, g, b);

  aPalette.getIconText(r, g, b);
  QColor IconText(r, g, b);

  aPalette.getDarkShadow(r, g, b);
  QColor DarkShadow (r, g, b);

  aPalette.getMidShadow(r, g, b);
  QColor MidShadow (r, g, b);

  aPalette.getLightShadow(r, g, b);
  QColor LightShadow(r, g, b);

  QPalette textFieldPalette;

  textFieldPalette.setColorGroup(QPalette::Active,
                                 TextOnStdBackground, FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground, TextOnStdBackground,
                                 FieldBackground, StdBackground);
  textFieldPalette.setColorGroup(QPalette::Disabled,
                                 TextOnStdBackground.darker(125),
                                 FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground.darker(125),
                                 TextOnStdBackground.darker(125),
                                 FieldBackground, StdBackground.darker(80));
  textFieldPalette.setColorGroup(QPalette::Inactive,
                                 TextOnStdBackground, FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground, TextOnStdBackground,
                                 FieldBackground, StdBackground);

  myWidget->setPalette(textFieldPalette);
}


void FFuQtComponentBase::getColors(FFuaPalette& aPalette)
{
  int r, g, b;

  QPalette colorPalette = myWidget->palette();
  QColor tmpColor;

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::WindowText);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setTextOnStdBackground(r, g, b);

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::Window);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setStdBackground(r, g, b);

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::Light);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setLightShadow(r, g, b);

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::Dark);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setDarkShadow(r, g, b);

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::Mid);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setMidShadow(r, g, b);

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::Text);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setTextOnFieldBackground(r, g, b);

  tmpColor = colorPalette.color(QPalette::Normal, QPalette::Base);
  tmpColor.getRgb(&r, &g, &b);
  aPalette.setFieldBackground(r, g, b);
}


////////////////////////////////////////////////////////////////////////
//
// Font information methods
//

int FFuQtComponentBase::getFontHeigth() const
{
  return myWidget->fontMetrics().height();
}


int FFuQtComponentBase::getFontMaxWidth() const
{
  return myWidget->fontMetrics().maxWidth();
}


int FFuQtComponentBase::getFontWidth(const char* aString, int len) const
{
  return myWidget->fontMetrics().width(aString,len);
}


const char* FFuQtComponentBase::getName() const
{
  static std::string name;
  name = myWidget->accessibleName().toStdString();
  return name.c_str();
}


FFuComponentBase* FFuQtComponentBase::getTopLevelWidget()
{
  return dynamic_cast<FFuComponentBase*>(myWidget->topLevelWidget());
}


void FFuQtComponentBase::blockLibSignals(bool block)
{
  myWidget->blockSignals(block);
}


bool FFuQtComponentBase::areLibSignalsBlocked()
{
  return myWidget->signalsBlocked();
}


void FFuQtComponentBase::giveKeyboardFocus()
{
  myWidget->setFocus();
}

////////////////////////////////////////////////////////////////////////
//
// Get hints about the size this widget needs.
// Useful for widgets using fonts, and text.
// Reimplemented from FFuComponentBase.

int FFuQtComponentBase::getBasicWidthHint()
{
  return myWidget->sizeHint().width();
}


int FFuQtComponentBase::getBasicHeightHint()
{
  return myWidget->sizeHint().height();
}
