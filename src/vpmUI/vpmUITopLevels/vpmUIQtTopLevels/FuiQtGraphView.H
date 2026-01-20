// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QT_GRAPH_VIEW_H
#define FUI_QT_GRAPH_VIEW_H

#include "FFuLib/FFuQtComponents/FFuQt2DPlotter.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtMDIWindow.H"
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"


class FuiQtGraphView : public FFuQt2DPlotter, public FuiGraphView
{
public:
  FuiQtGraphView(QWidget* parent, const char* name = "FuiQtGraphView");
  virtual ~FuiQtGraphView() { this->onPoppedDownToMem(); /*hack*/ }

  virtual void setVisible(bool visible);

protected:
  virtual void showEvent(QShowEvent* e);
  virtual void hideEvent(QHideEvent* e);
};


class FuiQtGraphViewTLS : public FFuQtMDIWindow, public FuiGraphViewTLS
{
public:
  FuiQtGraphViewTLS(QWidget* parent,
                    int xpos, int ypos, int width,int height,
                    const char* title, const char* name);

protected:
  virtual void resizeEvent(QResizeEvent* e);
};

#endif
