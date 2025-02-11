// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QPixmap>
#include <QCloseEvent>
#include <Qlabel>

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


inline QMessageBox::StandardButtons StdButtons(int nB)
{
  if (nB < 1) return QMessageBox::NoButton;
  if (nB < 2) return QMessageBox::Yes;
  if (nB < 3) return QMessageBox::Yes | QMessageBox::No;
  return QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
}

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
				 const char** buttonTexts,
				 unsigned int nButtons, bool isModal)
  : QMessageBox(QMessageBox::NoIcon,"Fedem",msgText,StdButtons(nButtons))
{
  this->setWidget(this);
  this->iAmModal = isModal;

  //set buttontext
  if (nButtons>0) this->setButtonText(QMessageBox::Yes   ,buttonTexts[0]);
  if (nButtons>1) this->setButtonText(QMessageBox::No    ,buttonTexts[1]);
  if (nButtons>2) this->setButtonText(QMessageBox::Cancel,buttonTexts[2]);
  if (nButtons>2) this->setEscapeButton(QMessageBox::Cancel);

  //set icon
  QPixmap pixmap;
  if (dialogType == FFuDialog::INFO)
    pixmap = FFuaQtPixmapCache::getPixmap(infoDialog);
  else if (dialogType == FFuDialog::ERROR)
    pixmap = FFuaQtPixmapCache::getPixmap(errorDialog);
  else if (dialogType == FFuDialog::WARNING)
    pixmap = FFuaQtPixmapCache::getPixmap(warningDialog);
  else if (dialogType == FFuDialog::QUESTION)
    pixmap = FFuaQtPixmapCache::getPixmap(questionDialog);
  else if (dialogType == FFuDialog::FT_LOGO)
    pixmap = FFuaQtPixmapCache::getPixmap(LetterLogoColorTranspAbout_xpm);
  else if (dialogType == FFuDialog::SAP_LOGO)
    pixmap = FFuaQtPixmapCache::getPixmap(SAPlogo_xpm,128);

  this->setIconPixmap(pixmap);

  if (dialogType >= FFuDialog::FT_LOGO)
    this->findChild<QLabel*>("qt_msgbox_label")->setFixedWidth(400);

  if (!this->iAmModal){
    this->setAttribute (Qt::WA_DeleteOnClose, true);
    this->show();
  }
}
//----------------------------------------------------------------------------

int FFuQtUserDialog::execute()
{
  if (this->iAmModal) FFuaApplication::breakUserEventBlock(true);
  int ret = this->exec();
  if (this->iAmModal) FFuaApplication::breakUserEventBlock(false);

  switch (ret) {
  case QMessageBox::Yes:
    return 0;
  case QMessageBox::No:
    return 1;
  case QMessageBox::Cancel:
    return 2;
  }
  return -1; //if accidentally called on a modeless dialog
}
//----------------------------------------------------------------------------

void FFuQtUserDialog::closeEvent(QCloseEvent* e)
{
  e->accept();
}
//----------------------------------------------------------------------------

void FFuQtUserDialog::done(int r)
{
  this->QMessageBox::done(r);
}
