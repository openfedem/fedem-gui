// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_MEMO_H
#define FFU_MEMO_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <string>

class FFuaFont;


/*!
  \brief A scrolled window with editable text.

  \description
  This is a widget used to display and edit text in a scrolled window

  \author Jacob Storen & Sven-Kaare Evenseth

  \date \b 13.08.99 Created (\e JL)
  \date \b 24.03.03 Re-written (\e SKE)

  \warning
      There is no management of the allocated colors yet.
      The color table will run full!

  \note All methods are documented in sub-class FFuQtMemo
*/

class FFuMemo : virtual public FFuComponentBase
{
public:
  //! Cursor actions (use with setCursorPos())
  enum {MOVE_BACK,         //!< Move one char backward
	MOVE_FORWARD,      //!< Move one char forward
	MOVE_BACK_WORD,    //!< Move one word backward
	MOVE_FORWARD_WORD, //!< Move one word forward
	MOVE_UP,           //!< Move up one line
	MOVE_DOWN,         //!< Move down one line
	MOVE_LINE_START,   //!< Move to beginning of line
	MOVE_LINE_END,     //!< Move to end of line
	MOVE_HOME,         //!< Move to beginning of document
	MOVE_END,          //!< Move to end of document
	MOVE_PG_UP,        //!< Move one page up
	MOVE_PG_DOWN       //!< Move one page down
  };

  virtual void displayTextFile(const char* fileName) = 0;
  virtual void setAllText(const char* text) = 0;
  virtual void addText(const char* text) = 0;
  virtual void clearText() = 0;
  virtual void setNoWordWrap() = 0;
  virtual void copyToClipboard() = 0;
  virtual void cutToClipboard() = 0;
  virtual void pasteFromClipboard() = 0;
  virtual void selectAllText() = 0;
  virtual void zoomInText() = 0;
  virtual void zoomOutText() = 0;
  virtual void unDo() = 0;
  virtual void reDo() = 0;
  virtual void deleteSelectedText() = 0;
  virtual void setCursorPos(int action, bool select) = 0;
  virtual void ensureCursorIsVisible() = 0;

  virtual void scrollToEnd() = 0;
  virtual void scrollToTop() = 0;
  virtual void insertText(const char* text, bool noScroll = false) = 0;
  virtual void enableUndoRedo(bool enable) = 0;

  virtual std::string getText() const = 0;
  virtual void getText(std::string& txt) const = 0;

  virtual int getNumChar() const = 0;
  virtual bool hasSelection() const = 0;

  virtual bool undoRedoEnabled() const = 0;
  virtual bool readOnly() const = 0;
  virtual bool isViewingEnd() const = 0;
  virtual bool isDraggingVScroll() = 0;

  virtual void setEditable(bool yesOrNo) = 0;
  virtual void setTextFont(const FFuaFont& aFont) = 0;

  void setTextChangedCB(const FFaDynCB0& cb) { myTextChangedCB = cb; }

protected:
  FFaDynCB0 myTextChangedCB;
};

#endif
