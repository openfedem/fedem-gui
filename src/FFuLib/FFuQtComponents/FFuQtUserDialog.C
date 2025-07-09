// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QLabel>

#include "FFuLib/Icons/infoDialog.xpm"
#include "FFuLib/Icons/errorDialog.xpm"
#include "FFuLib/Icons/warningDialog.xpm"
#include "FFuLib/Icons/questionDialog.xpm"
#include "FFuLib/Icons/LetterLogoColorTranspAbout.xpm"
#include "FFuLib/Icons/SAPlogo.xpm"

#include "FFuLib/FFuDialogType.H"
#include "FFuLib/FFuAuxClasses/FFuaApplication.H"
#include "FFuLib/FFuAuxClasses/FFuQtAuxClasses/FFuaQtPixmapCache.H"
#include "FFuLib/FFuQtComponents/FFuQtUserDialog.H"


//----------------------------------------------------------------------------

FFuUserDialog* FFuUserDialog::create(const char*  msgText,
				     int          dialogType,
				     const char** buttonTexts,
				     unsigned int nButtons,
				     bool         isModal)
{
  return new FFuQtUserDialog(msgText,dialogType,buttonTexts,nButtons,isModal);
}
//----------------------------------------------------------------------------

FFuQtUserDialog::FFuQtUserDialog(const char* msgText, int dialogType,
                                 const char** buttonTexts, unsigned int nButton,
                                 bool isModal)
  : QMessageBox(QMessageBox::NoIcon,"Fedem",msgText)
{
  this->setWidget(this);
  this->setModal(isModal);

  // Add buttons
  if (nButton > 0) this->addButton(buttonTexts[0], QMessageBox::YesRole);
  if (nButton > 1) this->addButton(buttonTexts[1], QMessageBox::NoRole);
  if (nButton > 2) this->addButton(buttonTexts[2], QMessageBox::RejectRole);
  if (nButton > 3) this->setEscapeButton(QMessageBox::Cancel);

  // Set icon
  if (dialogType == FFuDialog::INFO)
    this->setIconPixmap(FFuaQtPixmapCache::getPixmap(infoDialog));
  else if (dialogType == FFuDialog::ERROR)
    this->setIconPixmap(FFuaQtPixmapCache::getPixmap(errorDialog));
  else if (dialogType == FFuDialog::WARNING)
    this->setIconPixmap(FFuaQtPixmapCache::getPixmap(warningDialog));
  else if (dialogType == FFuDialog::QUESTION)
    this->setIconPixmap(FFuaQtPixmapCache::getPixmap(questionDialog));
  else if (dialogType == FFuDialog::FT_LOGO)
    this->setIconPixmap(FFuaQtPixmapCache::getPixmap(LetterLogoColorTranspAbout_xpm));
  else if (dialogType == FFuDialog::SAP_LOGO)
    this->setIconPixmap(FFuaQtPixmapCache::getPixmap(SAPlogo_xpm,128));

  if (dialogType >= FFuDialog::FT_LOGO)
    this->findChild<QLabel*>("qt_msgbox_label")->setFixedWidth(400);

  if (!isModal)
  {
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    this->show();
  }
}
//----------------------------------------------------------------------------

void FFuQtUserDialog::setTitle(const char* title)
{
  this->setWindowTitle(title);
}
//----------------------------------------------------------------------------

int FFuQtUserDialog::execute()
{
  FFuaApplication::breakUserEventBlock(true);
  this->exec();
  FFuaApplication::breakUserEventBlock(false);

  switch (this->buttonRole(this->clickedButton())) {
  case QMessageBox::YesRole:
    return 0;
  case QMessageBox::NoRole:
    return 1;
  case QMessageBox::RejectRole:
    return 2;
  default:
    return -1; // if accidentally called on a modeless dialog
  }
}
