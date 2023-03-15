// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

/*!
  \class FFuQtMemo FFuQtMemo.H

  \brief Qt implementation of a scrolled window with editable text.

  \author Jacob Storen & Sven-Kaare Evenseth

  \date \b 13.08.99 Created (\e JL)
  \date \b 21.03.03 Extended. Inherits QTextEdit instead of QMultiLineEdit.
*/

#include <QScrollBar>
#include <QTextStream>
#include <QMenu>
#include <QFile>
#include <QFont>
#include <QKeyEvent>

#include "FFuLib/FFuQtComponents/FFuQtPopUpMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtMemo.H"


//! Constructor
FFuQtMemo::FFuQtMemo(QWidget* parent) : QTextEdit(parent)
{
  this->setWidget(this);
}


//! Clears text and displays text
void FFuQtMemo::setAllText(const char* text)
{
  this->setText(text);
}


//! Parses file and displays it
void FFuQtMemo::displayTextFile(const char* fileName)
{
  QFile qFile(fileName);
  if (qFile.exists() && qFile.open(QIODevice::ReadOnly)) {
    QTextStream ts(&qFile);
    this->setText(ts.readAll());
  }
}


/*!
  Adds text to the end of the document
  \note Will start on a new line!
*/

void FFuQtMemo::addText(const char* text)
{
  this->append(text);
}


//! Clears the text
void FFuQtMemo::clearText()
{
  this->clear();
}


//! Sets whether the text should be editable by the user or not
void FFuQtMemo::setEditable(bool yesOrNo)
{
  this->setReadOnly(!yesOrNo);
}


//! Sets font
void FFuQtMemo::setTextFont(const FFuaFont& aFont)
{
  int weight = aFont.IsBold ? 75 : 50;
  QFont theQFont(aFont.Family.c_str(),aFont.Size,weight,aFont.IsItalic);
  this->setFont(theQFont);
}


//! Positions the cursor according to a QTextCursor::MoveOperation
void FFuQtMemo::setCursorPos(int action, bool select)
{
  QTextCursor::MoveMode mode = select ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor;
  switch (action) {
  case MOVE_BACK:
    this->moveCursor(QTextCursor::PreviousCharacter,mode);
    break;
  case MOVE_FORWARD:
    this->moveCursor(QTextCursor::NextCharacter,mode);
    break;
  case MOVE_BACK_WORD:
    this->moveCursor(QTextCursor::PreviousWord,mode);
    break;
  case MOVE_FORWARD_WORD:
    this->moveCursor(QTextCursor::NextWord,mode);
    break;
  case MOVE_UP:
    this->moveCursor(QTextCursor::Up,mode);
    break;
  case MOVE_DOWN:
    this->moveCursor(QTextCursor::Down,mode);
    break;
  case MOVE_LINE_START:
    this->moveCursor(QTextCursor::StartOfLine,mode);
    break;
  case MOVE_LINE_END:
    this->moveCursor(QTextCursor::EndOfLine,mode);
    break;
  case MOVE_HOME:
    this->moveCursor(QTextCursor::Start,mode);
    break;
  case MOVE_END:
    this->moveCursor(QTextCursor::End,mode);
    break;
  case MOVE_PG_UP:
    this->moveCursor(QTextCursor::PreviousBlock,mode);
    break;
  case MOVE_PG_DOWN:
    this->moveCursor(QTextCursor::NextBlock,mode);
    break;
  }
}


//! Returns the displayed text
void FFuQtMemo::getText(std::string& txt) const
{
  txt = this->toPlainText().toStdString();
}

std::string FFuQtMemo::getText() const
{
  return this->toPlainText().toStdString();
}


////////////////////////////////////////////////////////////////////////
//
//  Colors and fonts. Reimplemented from FFuQtComponentBase,
//  to make special use of the palette and FontSet:
//

//! Sets colors used to display text
void FFuQtMemo::setColors(FFuaPalette aPalette)
{
  int r,g,b;

  aPalette.getStdBackground(r, g, b);
  QColor StdBackground(r, g, b);

  aPalette.getFieldBackground(r, g, b);
  QColor FieldBackground(r, g, b);

  aPalette.getTextOnStdBackground(r, g, b);
  QColor TextOnStdBackground(r, g, b);

  aPalette.getTextOnFieldBackground(r, g, b);
  QColor TextOnFieldBackground(r, g, b);

  aPalette.getIconText(r, g, b);
  QColor IconText(r, g, b);

  aPalette.getDarkShadow(r, g, b);
  QColor DarkShadow(r, g, b);

  aPalette.getMidShadow(r, g, b);
  QColor MidShadow(r, g, b);

  aPalette.getLightShadow(r, g, b);
  QColor LightShadow(r, g, b);

  QColorGroup textFieldNormal  (TextOnFieldBackground,
				StdBackground,
				LightShadow,
				DarkShadow,
				MidShadow,
				TextOnFieldBackground,
				FieldBackground);

  QColorGroup textFieldDisabled(textFieldNormal.foreground().dark(125),
				textFieldNormal.background(),
				textFieldNormal.light(),
				textFieldNormal.dark(),
				textFieldNormal.mid(),
				textFieldNormal.text().light(80),
				textFieldNormal.base().dark(80));

  QPalette textFieldPalette(textFieldNormal, textFieldNormal, textFieldNormal);

  this->setPalette(textFieldPalette);
}


//! Sets fonts
void FFuQtMemo::setFonts(FFuaFontSet aFontSet)
{
  QFont textFieldFont;

  textFieldFont.setFamily   (aFontSet.TextFieldFont.Family.c_str());
  textFieldFont.setPointSize(aFontSet.TextFieldFont.Size    );
  textFieldFont.setBold     (aFontSet.TextFieldFont.IsBold  );
  textFieldFont.setItalic   (aFontSet.TextFieldFont.IsItalic);

  this->setFont(textFieldFont);
}


//! Sets how to wrap words at right side of widget
void FFuQtMemo::setNoWordWrap()
{
  this->setWordWrapMode(QTextOption::NoWrap);
}


//! Copies the selected text to clipboard
void FFuQtMemo::copyToClipboard()
{
  this->copy();
}

//! Cuts the displayed text to clipboard
void FFuQtMemo::cutToClipboard()
{
  this->cut();
}

//! Pastes text from clipboard at cursor pos
void FFuQtMemo::pasteFromClipboard()
{
  this->paste();
}

//! Selects all text
void FFuQtMemo::selectAllText()
{
  this->selectAll();
}

//! Increases font size by 1
void FFuQtMemo::zoomInText()
{
  this->zoomIn();
}

//! Decreases font size by 1
void FFuQtMemo::zoomOutText()
{
  this->zoomOut();
}

//! Undo last change (not accessible when using appendText())
void FFuQtMemo::unDo()
{
  this->undo();
}

//! Redo last undo (not accessible when using appendText())
void FFuQtMemo::reDo()
{
  this->redo();
}

//! Deletes selected text
void FFuQtMemo::deleteSelectedText()
{
  this->cut();
}

//! Makes sure the cursor is visible
void FFuQtMemo::ensureCursorIsVisible()
{
  this->ensureCursorVisible();
}


//! Returns whether the user is seeing the end of the memo contents.
bool FFuQtMemo::isViewingEnd() const
{
  int vpos = this->verticalScrollBar()->value();
  int vmax = this->verticalScrollBar()->maximum();
  int step = this->verticalScrollBar()->pageStep();
  return vpos >= vmax-step;
}


//! Returns true if the user is dragging the vertical scrollbar.
bool FFuQtMemo::isDraggingVScroll()
{
  return this->verticalScrollBar()->isSliderDown();
}


//! Scrolls to the end of the document
void FFuQtMemo::scrollToEnd()
{
  this->moveCursor(QTextCursor::End);
}

//! Scrolls to the top of the document
void FFuQtMemo::scrollToTop()
{
  this->moveCursor(QTextCursor::Start);
}


//! Insert text
void FFuQtMemo::insertText(const char* text, bool noScroll)
{
  if (noScroll)
  {
    QTextCursor cursor = this->textCursor();
    int pos = cursor.position();
    this->moveCursor(QTextCursor::End);
    this->insertPlainText(text);
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
  }
  else
    this->insertPlainText(text);
}

//! Enables or disables undo and redo
void FFuQtMemo::enableUndoRedo(bool enable)
{
  this->setUndoRedoEnabled(enable);
}

//! Returns number of charachters in document
int FFuQtMemo::getNumChar() const
{
  return this->toPlainText().length();
}

//! Returns true if there is a selection in the document
bool FFuQtMemo::hasSelection() const
{
  return this->textCursor().hasSelection();
}

//! Returns true if undo and redo is enabled
bool FFuQtMemo::undoRedoEnabled() const
{
  return isUndoRedoEnabled();
}

//! Returns true if document is read only
bool FFuQtMemo::readOnly() const
{
  return this->isReadOnly();
}


//! Invokes the text-changed call-back when the Return key is pressed.
void FFuQtMemo::keyPressEvent(QKeyEvent* event)
{
  this->QTextEdit::keyPressEvent(event);
  if (!myTextChangedCB.empty() && event->key() == Qt::Key_Return)
  {
    QTextCursor cursor = this->textCursor();
    int pos = cursor.position();
    myTextChangedCB.invoke();
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
  }
}


/*!
  Creates a pop up menu, accoring to what user have set
*/

QMenu* FFuQtMemo::createPopupMenu(const QPoint& pos)
{
  // This was a shortcut to custom pop up
  // quite usable, and you will avoid
  // CBs here and there. Works directly on the QTextEdit
  // This is where it ends up anyway...
  // oh well...
//   if (popUpCmds == -1)
//     return 0;

//   if (popUpCmds == 0)
//     return QTextEdit::createPopupMenu(pos);

//   QPopupMenu* p = new QPopupMenu(this);
//   int id = -1;

//   // Inserting items
//   if (popUpCmds & FFuMemo::CUT) {
//     id = p->insertItem("Cut", this, SLOT(cut), CTRL+Key_X);
//     p->setItemEnabled(id , this->hasSelection() && !this->readOnly());
//   }
//   if (popUpCmds & FFuMemo::COPY) {
//     id = p->insertItem("Copy", this, SLOT(copy()), CTRL+Key_C);
//     p->setItemEnabled(id, this->hasSelection());
//   }
//   if (popUpCmds & FFuMemo::PASTE) {
//     id = p->insertItem("Paste", this, SLOT(paste()), CTRL+Key_V);
//     p->setItemEnabled(id, !this->readOnly());
//   }
//   if (popUpCmds & FFuMemo::SELECT_ALL) {
//     id = p->insertItem("Select All", this, SLOT(selectAll()), CTRL+Key_A);
//     p->setItemEnabled(id, !this->getText().empty());
//   }
//   if (popUpCmds & FFuMemo::CLEAR) {
//     if (id > -1) p->insertSeparator();
//     id = p->insertItem("Clear", this, SLOT(clear()));
//     p->setItemEnabled(id, !this->getText().empty());
//   }

//   return p;

  /*
    NOTE from SKE may-2003:
    Sometime in the future, combine the above commented out with
    FFuaCmdItems, so that the commands that can be handled directly in
    this widget will be, without anymore hassle, and commands that
    need to be processed elsewhere can also be included.
  */

  if (commands.empty())
    return this->createStandardContextMenu(pos);

  FFuQtPopUpMenu* popUp = new FFuQtPopUpMenu(this);
  for (FFuaCmdItem* cmd : commands)
    popUp->insertCmdItem(cmd);

  return popUp;
}
