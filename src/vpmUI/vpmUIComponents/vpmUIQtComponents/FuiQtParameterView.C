// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <Qt3Support/Q3ScrollView>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtMultUIComponent.H"

#include "vpmUI/vpmUIComponents/FuiParameterView.H"


class FuiQtParameterView : public virtual FFuQtMultUIComponent,
                           public FuiParameterView::PrmSheet
{
public:
  FuiQtParameterView(QWidget* parent, const char* name)
    : FFuQtMultUIComponent(parent,name), xpos(0)
  {
    Q3ScrollView* mainView = new Q3ScrollView(this);
    mainView->addChild(myMainWidget = new QWidget(mainView->viewport()));
    mainView->setVScrollBarMode(Q3ScrollView::Auto);
    mainView->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    mainView->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

    QLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainView);
    this->setLayout(mainLayout);
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

    QScrollBar* bar = static_cast<Q3ScrollView*>(mainView)->verticalScrollBar();
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
FuiParameterView::createSheet(FFuMultUIComponent* parent, const char* name)
{
  FFuQtMultUIComponent* qparent = dynamic_cast<FFuQtMultUIComponent*>(parent);
  return new FuiQtParameterView(qparent,name);
}
