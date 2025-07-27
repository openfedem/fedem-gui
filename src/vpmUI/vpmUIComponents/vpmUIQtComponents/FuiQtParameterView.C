// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QScrollArea>
#include <QScrollBar>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "vpmUI/vpmUIComponents/FuiParameterView.H"


class FuiQtParameterView : public FFuQtWidget, public FuiParameterView::PrmSheet
{
public:
  FuiQtParameterView(QWidget* parent, const char* name)
    : FFuQtWidget(NULL,name), xpos(0)
  {
    QScrollArea* mainView = new QScrollArea();
    mainView->setWidget(myMainWidget = new QWidget(mainView->viewport()));
    mainView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainView->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

    QLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainView);

    layout = parent->layout();
    if (!layout)
    {
      layout = new QVBoxLayout(parent);
      layout->setContentsMargins(3,1,3,3);
    }
    layout->addWidget(this);
  }

  virtual void setFields(const std::vector<std::string>& names,
                         const FFaDynCB1<double>& acceptCB)
  {
    myLabels.reserve(names.size());
    myFields.reserve(names.size());
    for (const std::string& name : names)
    {
      FFuQtIOField* field = new FFuQtIOField(myMainWidget);
      field->setInputCheckMode(FFuIOField::DOUBLECHECK);
      field->FFuIOField::setSensitivity(IAmSensitive);
      field->setAcceptedCB(acceptCB);
      myFields.push_back(field);
      myLabels.push_back(new QLabel(name.c_str(),myMainWidget));
      int labelWidth = myLabels.back()->sizeHint().width();
      if (labelWidth+10 > xpos) xpos = labelWidth+10;
    }
  }

  virtual void setSensitivity(bool isSensitive)
  {
    this->PrmSheet::setSensitivity(isSensitive);
  }

protected:
  virtual void resizeEvent(QResizeEvent* e)
  {
    if (myFields.empty()) return;

    int height = e->size().height();
    int width  = e->size().width();

    QWidget* mainView = this->layout()->itemAt(0)->widget();
    mainView->setGeometry(0,0,width,height);

    QScrollBar* bar = static_cast<QScrollArea*>(mainView)->verticalScrollBar();
    if (bar->isVisible()) width -= bar->width();

    int ypos = 0;
    int fieldHeight = myFields.front()->getHeightHint();
    for (size_t row = 0; row < myLabels.size(); row++, ypos += fieldHeight+2)
    {
      myLabels[row]->setGeometry(2,ypos,xpos-10,fieldHeight);
      myFields[row]->setSizeGeometry(xpos,ypos,width-xpos-5,fieldHeight);
    }
    myMainWidget->resize(width,ypos);
  }

private:
  int xpos;

  QWidget*              myMainWidget;
  std::vector<QWidget*> myLabels;
};


FuiParameterView::PrmSheet*
FuiParameterView::createSheet(FFuComponentBase* parent, const char* name)
{
  return new FuiQtParameterView(parent->getQtWidget(),name);
}
