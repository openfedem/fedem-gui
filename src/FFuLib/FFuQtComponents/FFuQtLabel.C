// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>

#include "FFuLib/FFuQtComponents/FFuQtLabel.H"

extern const char* info_xpm[];


FFuQtLabel::FFuQtLabel(QWidget* parent) : QLabel(parent)
{
  this->setWidget(this);
  this->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

  QObject::connect(this,SIGNAL(linkActivated(const QString&)),
                   this,SLOT(fwdLinkActivated(const QString&)));
}


void FFuQtLabel::setPixMap(const char** pixmap, bool stretch)
{
  if (pixmap)
  {
    this->setScaledContents(stretch);
    this->setPixmap(QPixmap(pixmap));
  }
  else
    this->clear();
}


void FFuQtLabel::setLabel(const char* label)
{
  this->setText(label);
}


std::string FFuQtLabel::getLabel() const
{
  return this->text().toStdString();
}


void FFuQtLabel::fwdLinkActivated(const QString& txt)
{
  myLinkActivatedCB.invoke(txt.toStdString());
}


FFuQtNotesLabel::FFuQtNotesLabel(QWidget* parent) : QWidget(parent)
{
  this->setWidget(this);

  QLabel*  qImage = new QLabel();
  QLayout* layout = new QHBoxLayout(this);
  qImage->setPixmap(QPixmap(info_xpm));
  layout->setContentsMargins(0,0,0,0);
  layout->setAlignment(Qt::AlignLeft);
  layout->addWidget(qImage);
  layout->addWidget(new QLabel("<b>Notes</b>"));
}


FFuQtNotes::FFuQtNotes(QWidget* parent, const char* text,
                       int margin, int spacing) : QWidget(parent)
{
  this->setWidget(this);

  myLabel = new QLabel(text);
  myLabel->setWordWrap(true);

  QLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(margin,0,margin,0);
  layout->setSpacing(spacing);
  layout->addWidget(new FFuQtNotesLabel());
  layout->addWidget(myLabel);
}
