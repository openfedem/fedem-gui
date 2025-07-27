// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_COMPONENT_BASE_H
#define FFU_COMPONENT_BASE_H

#include "FFuLib/FFuAuxClasses/FFuaFontSet.H"
#include "FFuLib/FFuAuxClasses/FFuaPalette.H"

class QWidget;


/*!
  \brief Base class for GUI components, equivalent to a Widget.
  \details Mostly a virtual interface class to methods reimplemented
  in UI-lib (Qt) dependent subclasses.

  \author Jacob Storen
  \date 20 Jan 1999
*/

class FFuComponentBase
{
public:
  FFuComponentBase() {}
  virtual ~FFuComponentBase() {}

  virtual QWidget* getQtWidget() = 0;

  // Geometry management:

  void setEdgeGeometry(int left,    int right,   int top,   int bottom);

  void setCenterGeometry(int centerX, int centerY, int width, int height);
  void setCenterGeometrySizeHint(int centerX, int centerY)
    {this->setCenterGeometry(centerX,centerY,this->getWidthHint(),this->getHeightHint());}

  void setCenterYGeometry(int leftX, int centerY, int width, int height);
  void setCenterYGeometrySizeHint(int leftX, int centerY)
    {this->setCenterYGeometry(leftX,centerY,this->getWidthHint(),this->getHeightHint());}
  void setCenterYGeometryWidthHint(int leftX, int centerY,int height)
    {this->setCenterYGeometry(leftX,centerY,this->getWidthHint(),height);}

  virtual void setSizeGeometry(int leftX, int topY, int width, int height) = 0;
  void setSizeGeometrySizeHint(int leftX, int topY)
    {this->setSizeGeometry(leftX,topY,this->getWidthHint(),this->getHeightHint());}

  virtual void setWidth(int){}
  virtual void setHeight(int){}

  virtual void setMinWidth(int){}
  virtual void setMinHeight(int){}

  virtual void setMaxWidth(int){}
  virtual void setMaxHeight(int){}

  // Access size of widget :

  virtual int getHeight(){return 0;}
  virtual int getWidth() {return 0;}

  // Access Widget position :

  virtual int getXPos(){return 0;}
  virtual int getYPos(){return 0;}

  int getXRightPos() {return this->getXPos()+this->getWidth();}

  // Get hints abut the size this widget needs.
  // Useful for widget using fonts, and text.
  // Reimplement them in MultipleComponents to get compound
  // size hints :

  virtual int getWidthHint(){return this->getBasicWidthHint();}
  virtual int getHeightHint(){return this->getBasicHeightHint();}

  // Method to "Gray out" Widget :

  void setDisabled(bool disable){this->setSensitivity(!disable);}
  virtual void setSensitivity(bool){}
  virtual bool getSensitivity(){return false;}

  // Methods to show or hide widget :

  virtual void popUp(){}
  virtual void popDown(){}
  virtual bool isPoppedUp(){return false;}
  virtual void removeFromTopLevel(){}

  // Methods to edit the layering of the widgets :

  virtual void toFront(){}
  virtual void toBack(){}

  // Methods to set the look of the widget :

  virtual void setFonts (FFuaFontSet){}
  virtual void setColors(FFuaPalette){}
  virtual void getColors(FFuaPalette&){}
  virtual void setToolTip(const char* = NULL){}

  // Font information methods:

  virtual int getFontHeigth() const  {return 0;}
  virtual int getFontMaxWidth() const  {return 0;}
  virtual int getFontWidth(const char*) const {return 0;}

  virtual const char* getName() const { return NULL; }

  // Returns the top-level widget for this widget, i.e. the next ancestor
  // widget that has a window-system frame (or at least may have one).
  // If the widget is a top-level, the widget itself is returned.
  virtual FFuComponentBase* getTopLevelWidget() { return NULL; }

  // This default impl blocks the signal, must be reimpl in cases where
  // that isnt enough to stop the notification
  virtual void blockNotification(bool block) { this->blockLibSignals(block); }

  // Give this widget the keyboard focus
  virtual void giveKeyboardFocus() {}

protected:
  virtual void blockLibSignals(bool) {}
  virtual bool areLibSignalsBlocked() {return false;}

private:
  // Methods to be reimplemented in GUILib dependent code
  // in basic GUI components
  virtual int getBasicWidthHint(){return 0;}
  virtual int getBasicHeightHint(){return 0;}
};

#endif
