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

private:
  // Reimplemented to handler show & hide events
  virtual bool event(QEvent* e);
  virtual void setVisible(bool visible);
};


class FuiQtGraphViewTLS : public FFuQtMDIWindow, public FuiGraphViewTLS
{
public:
  FuiQtGraphViewTLS(QWidget* parent=0,
                    int xpos=100,int ypos=100,
                    int width=640,int height=250,
                    const char* title= "Graph View",
                    const char* name = "FuiQtGraphViewTLS");

protected:
  virtual void resizeEvent(QResizeEvent* e);
};

#endif
