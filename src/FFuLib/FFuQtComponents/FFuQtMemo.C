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
#include <QContextMenuEvent>
#include <QMenu>
#include <QFile>
#include <QFont>
#include <QKeyEvent>

#include "FFuLib/FFuQtComponents/FFuQtMemo.H"


//! Constructor
FFuQtMemo::FFuQtMemo(QWidget* parent, bool withClearCmd) : QTextEdit(parent)
{
  this->setWidget(this);

  QObject::connect(this,SIGNAL(selectionChanged()),this,SLOT(fwdSelectionChanged()));
  haveClearCmd = withClearCmd;
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
  int r, g, b;

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

  QPalette textFieldPalette;

  textFieldPalette.setColorGroup(QPalette::Active,
				 TextOnFieldBackground, FieldBackground,
				 LightShadow, DarkShadow, MidShadow,
				 TextOnFieldBackground,
				 TextOnFieldBackground,
				 FieldBackground, StdBackground);
  textFieldPalette.setColorGroup(QPalette::Disabled,
				 TextOnFieldBackground, FieldBackground,
				 LightShadow, DarkShadow, MidShadow,
				 TextOnFieldBackground,
				 TextOnFieldBackground,
				 FieldBackground, StdBackground);
  textFieldPalette.setColorGroup(QPalette::Inactive,
				 TextOnFieldBackground, FieldBackground,
				 LightShadow, DarkShadow, MidShadow,
				 TextOnFieldBackground,
				 TextOnFieldBackground,
				 FieldBackground, StdBackground);

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

  this->QTextEdit::setFont(textFieldFont);
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
  /* Old Qt3 implementation
  int y = this->contentsY();
  int height = this->contentsHeight();
  int visibHeight = this->visibleHeight();
  return (y >= (height - visibHeight - 3));
  TODO: Find replacement for the above with Qt4 and beyond */
  return true;
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
    this->moveCursor(QTextCursor::End);
    this->insertPlainText(text);
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


//! Customize the right-click menu adding the clear command if wanted
void FFuQtMemo::contextMenuEvent(QContextMenuEvent* event)
{
  QMenu* menu = this->createStandardContextMenu();
  if (haveClearCmd)
  {
    menu->addSeparator();
    QAction* action = menu->addAction("Clear");
    action->setEnabled(!this->document()->isEmpty());
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(clear()));
  }
  menu->exec(event->globalPos());
  delete menu;
}
