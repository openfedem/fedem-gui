// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QGridLayout>
#include <QLabel>

#include "FFuLib/FFuQtComponents/FFuQtIOField.H"

#include "vpmUI/vpmUIComponents/vpmUIQtComponents/FuiQtParameterView.H"


FuiQtParameterView::FuiQtParameterView(QWidget* parent, const char* name)
  : FFuQtMultUIComponent(parent,name)
{
  IAmSensitive = true;

  myLayout = new QGridLayout(this);
  myLayout->setContentsMargins(2,2,2,2);
  myLayout->setHorizontalSpacing(8);
  myLayout->setVerticalSpacing(1);
}


void FuiQtParameterView::clear()
{
  QLayoutItem* item;
  while ((item = myLayout->takeAt(0)))
  {
    delete item->widget();
    delete item;
  }
}


void FuiQtParameterView::setFields(const std::vector<std::string>& names)
{
  int row = 0;
  QWidget* parent = myLayout->parentWidget();
  for (const std::string& name : names)
  {
    myLayout->addWidget(new QLabel(name.c_str(),parent),row,0);
    FFuQtIOField* field = new FFuQtIOField(parent);
    field->setInputCheckMode(FFuIOField::DOUBLECHECK);
    field->FFuIOField::setSensitivity(IAmSensitive);
    field->setAcceptedCB(myAcceptedCB);
    myLayout->addWidget(field,row++,1);
  }
}


void FuiQtParameterView::setValues(const std::vector<double>& values)
{
  int row = 0;
  for (double value : values)
    if (row < myLayout->rowCount())
    {
      QLayoutItem* item = myLayout->itemAtPosition(row++,1);
      FFuQtIOField* fld = item ? dynamic_cast<FFuQtIOField*>(item->widget()) : NULL;
      if (fld) fld->FFuIOField::setValue(value);
    }
    else
      break;
}


void FuiQtParameterView::getValues(std::vector<double>& values) const
{
  values.clear();
  values.reserve(myLayout->rowCount());
  for (int row = 0; row < myLayout->rowCount(); row++)
  {
    QLayoutItem* item = myLayout->itemAtPosition(row,1);
    FFuQtIOField* fld = item ? dynamic_cast<FFuQtIOField*>(item->widget()) : NULL;
    values.push_back(fld ? fld->getDouble() : 0.0);
  }
}


void FuiQtParameterView::setSensitivity(bool isSensitive)
{
  IAmSensitive = isSensitive;

  for (int row = 0; row < myLayout->rowCount(); row++)
  {
    QLayoutItem* item = myLayout->itemAtPosition(row,1);
    FFuQtIOField* fld = item ? dynamic_cast<FFuQtIOField*>(item->widget()) : NULL;
    if (fld) fld->FFuIOField::setSensitivity(isSensitive);
  }
}
