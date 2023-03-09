// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <Qt3Support/Q3Header>
#include <QMouseEvent>

#include "FFuLib/FFuQtComponents/FFuQtListViewItem.H"
#include "FFuLib/FFuQtComponents/FFuQtPopUpMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtListView.H"

//----------------------------------------------------------------------------

FFuQtListView::FFuQtListView(QWidget* parent, int nColumns, const char* name)
  : Q3ListView(parent)
{
  this->setObjectName(name);
  this->setWidget(this);

  for (int c = 0; c < nColumns; c++)
    this->addColumn("empty");

  this->popUpMenu = new FFuQtPopUpMenu(this);

#ifndef linux
  this->setSglSelectionMode(true);
  this->setListSorting(-1); // disable sorting
  this->setAllListColumnsShowSelection(true);
#endif

  QObject::connect(this,SIGNAL(selectionChanged()),
		   this,SLOT(fwdSelectionChanged()));
  QObject::connect(this,SIGNAL(expanded(Q3ListViewItem*)),
		   this,SLOT(fwdExpanded(Q3ListViewItem*)));
  QObject::connect(this,SIGNAL(collapsed(Q3ListViewItem*)),
		   this,SLOT(fwdCollapsed(Q3ListViewItem*)));
  QObject::connect(this,SIGNAL(returnPressed(Q3ListViewItem*)),
		   this,SLOT(fwdReturnPressed(Q3ListViewItem*)));
  QObject::connect(this,SIGNAL(doubleClicked(Q3ListViewItem*)),
		   this,SLOT(fwdDoubleClicked(Q3ListViewItem*)));
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdSelectionChanged()
{
  this->onPermSelectionChanged();
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdExpanded(Q3ListViewItem* item)
{
  this->onListItemOpened(dynamic_cast<FFuListViewItem*>(item),true);
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdCollapsed(Q3ListViewItem* item)
{
  this->onListItemOpened(dynamic_cast<FFuListViewItem*>(item),false);
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdReturnPressed(Q3ListViewItem* item)
{
  this->invokeReturnPressedCB(dynamic_cast<FFuListViewItem*>(item));
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdDoubleClicked(Q3ListViewItem* item)
{
  this->invokeDoubleClickedCB(dynamic_cast<FFuListViewItem*>(item));
}
//----------------------------------------------------------------------------

void FFuQtListView::setListSorting(int column,bool ascending)
{
  this->setSorting(column,ascending);
}
//----------------------------------------------------------------------------

void FFuQtListView::setHeaderClickEnabled(int column,bool enable)
{
  this->header()->setSectionsClickable(enable);
}
//----------------------------------------------------------------------------

void FFuQtListView::setListRootIsDecorated(bool enable)
{
  this->setRootIsDecorated(enable);
}
//----------------------------------------------------------------------------

void FFuQtListView::setSglSelectionMode(bool single)
{
  this->setSelectionMode(single ? Q3ListView::Single : Q3ListView::Extended);
}
//----------------------------------------------------------------------------

void FFuQtListView::setAllListColumnsShowSelection(bool enable)
{
  this->setAllColumnsShowFocus(enable);
}
//----------------------------------------------------------------------------

void FFuQtListView::setHeaderOff(bool Off)
{
  if (Off)
    this->header()->hide();
  else
    this->header()->show();
}
//----------------------------------------------------------------------------

void FFuQtListView::clearList()
{
  this->clear();
  this->lviMap.clear();
}
//----------------------------------------------------------------------------

void FFuQtListView::setListColumns(const std::vector<const char*>& labels)
{
  for (const char* label : labels)
    this->addColumn(label);
}
//----------------------------------------------------------------------------

void FFuQtListView::setListColumnWidth(int column, int width)
{
  this->setColumnWidth(column,width);
}
//----------------------------------------------------------------------------

void FFuQtListView::permSelectListItem(FFuListViewItem* item, bool select, bool notify)
{
  Q3ListViewItem* qitem = dynamic_cast<Q3ListViewItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  this->setSelected(qitem,select);

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

void FFuQtListView::clearListSelection(bool notify)
{
  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  this->clearSelection();

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

void FFuQtListView::openListItem(FFuListViewItem* item,bool open,bool notify)
{
  Q3ListViewItem* qitem = dynamic_cast<Q3ListViewItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  qitem->setOpen(open);

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

bool FFuQtListView::isExpanded(FFuListViewItem* item)
{
  Q3ListViewItem* qitem = dynamic_cast<Q3ListViewItem*>(item);
  return qitem ? this->isOpen(qitem) : false;
}

void FFuQtListView::ensureListItemVisible(FFuListViewItem* item,bool notify)
{
  Q3ListViewItem* qitem = dynamic_cast<Q3ListViewItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  this->ensureItemVisible(qitem);

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getSelectedListItemSglMode()
{
  return dynamic_cast<FFuListViewItem*>(this->selectedItem());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getCurrentListItem()
{
  return dynamic_cast<FFuListViewItem*>(this->currentItem());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getFirstChildItem()
{
  return dynamic_cast<FFuListViewItem*>(this->firstChild());
}
//----------------------------------------------------------------------------


bool FFuQtListView::isSglSelectionMode() const
{
  return this->selectionMode() == Q3ListView::Single;
}
//----------------------------------------------------------------------------

int FFuQtListView::getNColumns()
{
  return this->columns();
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::createListItem(FFuListViewItem* parent,FFuListViewItem* after,
					       FFuListViewItem* original,const char* label)
{
  FFuQtListViewItem* lvi = NULL;

  if (!parent)
    if (original)
      lvi = new FFuQtListViewItem(this,(FFuQtListViewItem*)(after),
				  (FFuQtListViewItem*)(original));
    else
      lvi = new FFuQtListViewItem(this,(FFuQtListViewItem*)(after),
				  label);
  else
    if (original)
      lvi = new FFuQtListViewItem((FFuQtListViewItem*)(parent),
				  (FFuQtListViewItem*)(after),
				  (FFuQtListViewItem*)(original));
    else
      lvi = new FFuQtListViewItem((FFuQtListViewItem*)(parent),
				  (FFuQtListViewItem*)(after),label);

  if (lvi){
    this->lviMap[lvi->getItemId()] = lvi;
    //lvi->setItemDragable(true);
    //lvi->setItemDropable(true);
  }
  return lvi;
}
//----------------------------------------------------------------------------

void FFuQtListView::setColors(FFuaPalette aPalette)
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
                                 TextOnStdBackground, TextOnFieldBackground,
                                 FieldBackground, StdBackground);
  textFieldPalette.setColorGroup(QPalette::Disabled,
                                 TextOnFieldBackground.darker(125),
                                 FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground.lighter(80),
                                 TextOnStdBackground.darker(125),
                                 FieldBackground, StdBackground.darker(80));
  textFieldPalette.setColorGroup(QPalette::Active,
                                 TextOnFieldBackground,
                                 FieldBackground,
                                 LightShadow, DarkShadow, MidShadow,
                                 TextOnStdBackground, TextOnFieldBackground,
                                 FieldBackground, StdBackground);

  this->setPalette(textFieldPalette);

  QPalette stdPalette;
  stdPalette.setColorGroup(QPalette::Active,
                           TextOnStdBackground, FieldBackground,
                           LightShadow, DarkShadow, MidShadow,
                           TextOnStdBackground, TextOnStdBackground,
                           FieldBackground, StdBackground);
  stdPalette.setColorGroup(QPalette::Disabled,
                           TextOnStdBackground.darker(125), FieldBackground,
                           LightShadow, DarkShadow, MidShadow,
                           TextOnStdBackground.lighter(80),
                           TextOnStdBackground.darker(125),
                           FieldBackground, StdBackground.darker(80));
  stdPalette.setColorGroup(QPalette::Active,
                           TextOnStdBackground, FieldBackground,
                           LightShadow, DarkShadow, MidShadow,
                           TextOnStdBackground, TextOnStdBackground,
                           FieldBackground, StdBackground);

  this->header()->setPalette(stdPalette);
}
//----------------------------------------------------------------------------

void FFuQtListView::setFonts (FFuaFontSet aFontSet)
{
  QFont listviewFont;
  listviewFont.setFamily   (aFontSet.TextFieldFont.Family.c_str());
  listviewFont.setPointSize(aFontSet.TextFieldFont.Size    );
  listviewFont.setBold     (aFontSet.TextFieldFont.IsBold  );
  listviewFont.setItalic   (aFontSet.TextFieldFont.IsItalic);

  this->Q3ListView::setFont(listviewFont);

  QFont stdFont;
  stdFont.setFamily   (aFontSet.StandardFont.Family.c_str());
  stdFont.setPointSize(aFontSet.StandardFont.Size    );
  stdFont.setBold     (aFontSet.StandardFont.IsBold  );
  stdFont.setItalic   (aFontSet.StandardFont.IsItalic);

  this->header()->setFont(stdFont);
}
//----------------------------------------------------------------------------

void FFuQtListView::viewportMousePressEvent(QMouseEvent* e)
{
  // every press not only on items
  FFuListViewItem* listItem = dynamic_cast<FFuListViewItem*>(this->itemAt(e->pos()));
  if (listItem && e->button() == Qt::LeftButton)
    {
      // manual toggling
      if (listItem->isItemToggleAble()) {
	int offset = this->rootIsDecorated() ? this->treeStepSize() : 0;
	FFuListViewItem* parent = listItem;
	while ((parent = parent->getParentItem()))
	  offset += this->treeStepSize();
	if (offset < e->pos().x() && e->pos().x() < offset+15) {
	  listItem->toggleItem(true);
	  return;
	}
      }

      // Changed 28.10.16 (kmo): Invoke call-back method before
      // the Q3ListView::viewportMousePressEvent call
      this->invokeLeftMouseBPressedCB(listItem);
      this->Q3ListView::viewportMousePressEvent(e);
    }
  else if (e->button() == Qt::RightButton)
    {
      this->invokeRightMouseBPressedCB(listItem);
      this->executePopUp(listItem);
    }
}
//----------------------------------------------------------------------------

void FFuQtListView::viewportMouseReleaseEvent(QMouseEvent* e)
{
  // every release not only on items
  if (e->button() == Qt::LeftButton)
    this->invokeLeftMouseBReleaseCB();

  this->Q3ListView::viewportMouseReleaseEvent(e);
}
//----------------------------------------------------------------------------
