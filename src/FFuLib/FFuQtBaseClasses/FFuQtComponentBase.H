// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \file FFuQtComponentBase.H
  \brief Implementation of the Fedem "Widget equivalent", using Qt.
  \author Jacob Storen
  \date 2 Feb 1999
*/

#ifndef FFU_QT_COMPONENT_BASE_H
#define FFU_QT_COMPONENT_BASE_H

#include <QWidget>

#include "FFuLib/FFuBase/FFuComponentBase.H"


/*!
  \brief Implementation of the Fedem "Widget equivalent", using Qt.
  \details This is the Qt-implementation of FFuComponentBase.
  It has the QWidget as aggregate because Qt does not use virtual inheritance.
*/

class FFuQtComponentBase : virtual public FFuComponentBase
{
protected:
  FFuQtComponentBase() : myWidget(NULL) {}

public:
  virtual QWidget* getQtWidget() { return myWidget; }

  // Geometry management :

  virtual void setSizeGeometry(int leftX, int topY, int width, int height);

  virtual void setWidth(int width);
  virtual void setHeight(int height);

  virtual void setMinWidth(int width);
  virtual void setMinHeight(int height);

  virtual void setMaxWidth(int width);
  virtual void setMaxHeight(int height);

  virtual int  getWidthHint() const;
  virtual int  getHeightHint() const;

  // Access size of widget :

  virtual int  getWidth();
  virtual int  getHeight();

  // Access Widget position :

  virtual int  getXPos();
  virtual int  getYPos();

  // Method to "Gray out" Widget :

  virtual void setSensitivity(bool makeSensitive);
  virtual bool getSensitivity();

  // Methods to show or hide widget :

  virtual void popUp();
  virtual void popDown();
  virtual bool isPoppedUp();

  // Methods to edit the layering of the widgets :

  virtual void toFront();
  virtual void toBack();

  // Methods to set the look of the widget :

  virtual void setFonts (FFuaFontSet aFontSet);
  virtual void setColors(FFuaPalette aPalette);
  virtual void getColors(FFuaPalette& aPalette);
  virtual void setToolTip(const char* tip);

  // Font information methods:

  virtual int getFontHeigth() const;
  virtual int getFontMaxWidth() const;
  virtual int getFontWidth(const char* aString) const;

  //! \brief Returns the top-level widget for this widget.
  //! \details If this widget is a top-level, the widget itself is returned.
  virtual FFuComponentBase* getTopLevelWidget();

  //! \brief Returns the assigned name of this widget.
  virtual const char* getName() const;

  //! Gives this widget the keyboard focus.
  virtual void giveKeyboardFocus();

protected:
  // Method to be called from subclass constructor
  // (which must be a QWidget subclass) to initialise the widget pointer.
  void setWidget(QWidget* widget);

  virtual void blockLibSignals(bool block);
  virtual bool areLibSignalsBlocked();

private:
  QWidget* myWidget;
};


class FFuQtWidget : public QWidget, public FFuQtComponentBase
{
protected:
  FFuQtWidget(QWidget* parent, const char* name);
};

#endif
