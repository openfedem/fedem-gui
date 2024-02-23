// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFU_QT_MEMO_H
#define FFU_QT_MEMO_H

#include <QTextEdit>

#include "FFuLib/FFuQtBaseClasses/FFuQtComponentBase.H"
#include "FFuLib/FFuMemo.H"


class FFuQtMemo : public QTextEdit, public FFuMemo, public FFuQtComponentBase
{
  Q_OBJECT

public:
  FFuQtMemo(QWidget* parent = NULL, bool withClearCmd = false);
  virtual ~FFuQtMemo() {}

  virtual void displayTextFile(const char* fileName);
  virtual void setAllText(const char* text);
  virtual void addText(const char* text);
  virtual void clearText();

  virtual void setNoWordWrap();
  virtual void copyToClipboard();
  virtual void cutToClipboard();
  virtual void pasteFromClipboard();
  virtual void selectAllText();
  virtual void zoomInText();
  virtual void zoomOutText();
  virtual void unDo();
  virtual void reDo();
  virtual void deleteSelectedText();
  virtual void setCursorPos(int actor, bool select);
  virtual void ensureCursorIsVisible();
  virtual bool isViewingEnd() const ;
  virtual bool isDraggingVScroll() ;

  virtual void scrollToEnd();
  virtual void scrollToTop();

  virtual void insertText(const char* text, bool noScroll);

  virtual std::string getText() const;
  virtual void getText(std::string& txt) const;

  virtual int getNumChar() const;
  virtual bool hasSelection() const;

  virtual void enableUndoRedo(bool enable);
  virtual bool undoRedoEnabled() const;

  virtual bool readOnly() const;
  virtual void setEditable(bool yesOrNo);

  virtual void setTextFont(const FFuaFont& aFont);

private slots:
  void fwdSelectionChanged() { mySelectionChangedCB.invoke(); }

protected:

  // Colors and fonts. Reimplemented from FFuQtComponentBase,
  // to make special use of the palette and FontSet:

  virtual void setColors(FFuaPalette aPalette);
  virtual void setFonts (FFuaFontSet aFontSet);

  // Reimplement virtual methods from QTextEdit
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void contextMenuEvent(QContextMenuEvent* event);

private:
  bool haveClearCmd;
};

#endif
