// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <Qt3Support/Q3ScrollView>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"
#include "FFuLib/FFuQtBaseClasses/FFuQtMultUIComponent.H"

#include "vpmUI/vpmUIComponents/FuiParameterView.H"


class FuiQtParameterView : public virtual FFuQtMultUIComponent,
                           public FuiParameterView::ParameterSheet
{
public:
  FuiQtParameterView(QWidget* parent, const char* name)
    : FFuQtMultUIComponent(parent,name)
  {
    IAmSensitive = true;

    Q3ScrollView* mainView = new Q3ScrollView(this);
    QWidget* mainWidget = new QWidget(mainView->viewport());
    mainView->addChild(mainWidget);

    myLayout = new QGridLayout(mainWidget);
    myLayout->setContentsMargins(2,2,2,2);
    myLayout->setHorizontalSpacing(8);
    myLayout->setVerticalSpacing(1);

    QLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainView);
    this->setLayout(mainLayout);
  }

  virtual void setFields(const std::vector<std::string>& names,
                         const FFaDynCB1<double>& acceptCB)
  {
    int row = 0;
    QWidget* parent = myLayout->parentWidget();
    for (const std::string& name : names)
    {
      myLayout->addWidget(new QLabel(name.c_str(),parent),row,0);
      FFuQtIOField* field = new FFuQtIOField(parent);
      field->setInputCheckMode(FFuIOField::DOUBLECHECK);
      field->FFuIOField::setSensitivity(IAmSensitive);
      field->setAcceptedCB(acceptCB);
      myLayout->addWidget(field,row++,1);
    }
  }

  virtual void setValues(const std::vector<double>& values)
  {
    int row = 0;
    for (double value : values)
      if (row < myLayout->rowCount())
      {
        FFuQtIOField* fld = this->getField(row++);
	if (fld) fld->FFuIOField::setValue(value);
      }
      else
	break;
  }

  virtual void getValues(std::vector<double>& values) const
  {
    values.clear();
    values.reserve(myLayout->rowCount());
    for (int row = 0; row < myLayout->rowCount(); row++)
    {
      FFuQtIOField* fld = this->getField(row);
      values.push_back(fld ? fld->getDouble() : 0.0);
    }
  }

  virtual void setSensitivity(bool isSensitive)
  {
    IAmSensitive = isSensitive;

    for (int row = 0; row < myLayout->rowCount(); row++)
    {
      FFuQtIOField* fld = this->getField(row);
      if (fld) fld->FFuIOField::setSensitivity(isSensitive);
    }
  }

protected:
  FFuQtIOField* getField(int row) const
  {
    QLayoutItem* item = myLayout->itemAtPosition(row,1);
    return item ? dynamic_cast<FFuQtIOField*>(item->widget()) : NULL;
  }

private:
  bool IAmSensitive;

  QGridLayout* myLayout;
};


FuiParameterView::ParameterSheet*
FuiParameterView::createSheet(FFuMultUIComponent* parent, const char* name)
{
  FFuQtMultUIComponent* qparent = dynamic_cast<FFuQtMultUIComponent*>(parent);
  return new FuiQtParameterView(qparent,name);
}
