// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>

#include "FuiQtInputSelector.H"
#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtQueryInputField.H"
#include "FFuLib/FFuQtComponents/FFuQtOptionMenu.H"


FuiQtInputSelector::FuiQtInputSelector(QWidget* parent, int iArg)
  : FFuQtLabelFrame(parent), FuiInputSelector(iArg)
{
  if (iArg < 0)
    this->setObjectName("FuiQtInputSelector");
  else
    this->setObjectName(("FuiQtInputSelector" + std::to_string(iArg)).c_str());
  this->setLabel("Argument");

  myObjSelector = new FuiQtQueryInputField(NULL);
  myDofSelector = new FFuQtOptionMenu();
  myVarSelector = new FFuQtOptionMenu();

  this->initWidgets();

  QGridLayout* layout = new QGridLayout(this);
  layout->setContentsMargins(5,0,5,5);
  layout->setColumnStretch(1,1);
  layout->addWidget(static_cast<FuiQtQueryInputField*>(myObjSelector), 0,0,1,2);
  layout->addWidget(new QLabel("DOF:"), 1,0);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(myDofSelector), 1,1);
  layout->addWidget(new QLabel("Var:"), 2,0);
  layout->addWidget(static_cast<FFuQtOptionMenu*>(myVarSelector), 2,1);
}


class FuiQtInputSelectorSheet : public FFuQtWidget,
                                public FuiInputSelectors::Sheet
{
public:
  FuiQtInputSelectorSheet(QWidget* parent, int numArg, const char* name)
    : FFuQtWidget(NULL,name)
  {
    QScrollArea* mainView = new QScrollArea();
    mainView->setWidget(myMainWidget = new QWidget(mainView->viewport()));
    mainView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainView->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

    myArgs.reserve(numArg);
    for (int i = 0; i < numArg; i++)
      myArgs.push_back(new FuiQtInputSelector(myMainWidget,i));

    QLayout* layout = new QVBoxLayout(this);
    layout->addWidget(mainView);

    static_cast<QBoxLayout*>(parent->layout())->insertWidget(1,this,2);
  }

  virtual void setSensitivity(bool isSensitive)
  {
    this->Sheet::setSensitivity(isSensitive);
  }

protected:
  virtual void resizeEvent(QResizeEvent* e)
  {
    if (myArgs.empty()) return;

    int height = e->size().height();
    int width  = e->size().width();

    QWidget* mainView = this->layout()->itemAt(0)->widget();
    mainView->setGeometry(0,0,width,height);

    QScrollBar* bar = static_cast<QScrollArea*>(mainView)->verticalScrollBar();
    if (bar->isVisible()) width -= bar->width();

    int ypos = 0;
    int rowHeight = myArgs.front()->getHeightHint();
    for (size_t row = 0; row < myArgs.size(); row++, ypos += rowHeight+2)
      myArgs[row]->setSizeGeometry(2,ypos,width-8,rowHeight);
    myMainWidget->resize(width,ypos);
  }

private:
  QWidget* myMainWidget;
};


FuiInputSelectors::Sheet*
FuiQtInputSelectors::createSheet(int numArg, const char* name) const
{
  return new FuiQtInputSelectorSheet(myParent,numArg,name);
}
