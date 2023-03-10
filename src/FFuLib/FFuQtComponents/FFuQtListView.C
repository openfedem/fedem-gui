// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <QHeaderView>
#include <QMouseEvent>

#include "FFuLib/FFuQtComponents/FFuQtListViewItem.H"
#include "FFuLib/FFuQtComponents/FFuQtPopUpMenu.H"
#include "FFuLib/FFuQtComponents/FFuQtListView.H"

//----------------------------------------------------------------------------

FFuQtListView::FFuQtListView(QWidget* parent, int nColumns, const char* name)
  : QTreeWidget(parent)
{
  this->setObjectName(name);
  this->setWidget(this);
  this->setColumnCount(nColumns);
  this->setSglSelectionMode(true);
  this->setListSorting(-1,true); // disable sorting
  this->setAllListColumnsShowSelection(true);

  this->popUpMenu = new FFuQtPopUpMenu(this);

  QObject::connect(this, SIGNAL(itemSelectionChanged()),
                   this, SLOT(fwdSelectionChanged()));
  QObject::connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                   this, SLOT(fwdExpanded(QTreeWidgetItem*)));
  QObject::connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                   this, SLOT(fwdCollapsed(QTreeWidgetItem*)));
  QObject::connect(this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
                   this, SLOT(fwdReturnPressed(QTreeWidgetItem*,int)));
  QObject::connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                   this, SLOT(fwdDoubleClicked(QTreeWidgetItem*,int)));
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdSelectionChanged()
{
  this->onPermSelectionChanged();
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdExpanded(QTreeWidgetItem* item)
{
  this->onListItemOpened(dynamic_cast<FFuListViewItem*>(item),true);
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdCollapsed(QTreeWidgetItem* item)
{
  this->onListItemOpened(dynamic_cast<FFuListViewItem*>(item),false);
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdReturnPressed(QTreeWidgetItem* item, int)
{
  this->invokeReturnPressedCB(dynamic_cast<FFuListViewItem*>(item));
}
//----------------------------------------------------------------------------

void FFuQtListView::fwdDoubleClicked(QTreeWidgetItem* item, int)
{
  this->invokeDoubleClickedCB(dynamic_cast<FFuListViewItem*>(item));
}
//----------------------------------------------------------------------------

void FFuQtListView::setListSorting(int column, bool ascending)
{
  this->sortByColumn(column, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
}
//----------------------------------------------------------------------------

void FFuQtListView::setHeaderClickEnabled(int, bool enable)
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
  this->setSelectionMode(single ? SingleSelection : ExtendedSelection);
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
  QStringList qLabels;
  qLabels.reserve(labels.size());
  for (const char* label : labels)
    qLabels.push_back(label);
  this->setHeaderLabels(qLabels);
}
//----------------------------------------------------------------------------

void FFuQtListView::setListColumnWidth(int column, int width)
{
  this->setColumnWidth(column,width);
}
//----------------------------------------------------------------------------

void FFuQtListView::resizeListColumnsToContents()
{
  for (int col = 0; col < this->columnCount(); col++)
    this->resizeColumnToContents(col);
}
//----------------------------------------------------------------------------

void FFuQtListView::permSelectListItem(FFuListViewItem* item, bool select, bool notify)
{
  QTreeWidgetItem* qitem = dynamic_cast<QTreeWidgetItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  qitem->setSelected(select);

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

void FFuQtListView::openListItem(FFuListViewItem* item, bool open, bool notify)
{
  QTreeWidgetItem* qitem = dynamic_cast<QTreeWidgetItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  qitem->setExpanded(open);

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

/* TODO Move this to FFuQtListViewItem since it does not involve FFuQtListView */
bool FFuQtListView::isExpanded(FFuListViewItem* item)
{
  QTreeWidgetItem* qitem = dynamic_cast<QTreeWidgetItem*>(item);
  return qitem ? qitem->isExpanded() : false;
}

void FFuQtListView::ensureListItemVisible(FFuListViewItem* item,bool notify)
{
  QTreeWidgetItem* qitem = dynamic_cast<QTreeWidgetItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  this->scrollTo(this->indexFromItem(qitem));
  //this->ensureItemVisible(qitem); // Qt3 only

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getSelectedListItemSglMode()
{
  QList<QTreeWidgetItem*> selected = this->selectedItems();
  return selected.isEmpty() ? NULL : dynamic_cast<FFuListViewItem*>(selected.front());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getCurrentListItem()
{
  return dynamic_cast<FFuListViewItem*>(this->currentItem());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getFirstChildItem()
{
  return dynamic_cast<FFuListViewItem*>(this->topLevelItem(0));
}
//----------------------------------------------------------------------------


bool FFuQtListView::isSglSelectionMode() const
{
  return this->selectionMode() == SingleSelection;
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

  this->QTreeView::setFont(listviewFont);

  QFont stdFont;
  stdFont.setFamily   (aFontSet.StandardFont.Family.c_str());
  stdFont.setPointSize(aFontSet.StandardFont.Size    );
  stdFont.setBold     (aFontSet.StandardFont.IsBold  );
  stdFont.setItalic   (aFontSet.StandardFont.IsItalic);

  this->header()->setFont(stdFont);
}
//----------------------------------------------------------------------------

void FFuQtListView::mousePressEvent(QMouseEvent* e)
{
  // every press not only on items
  FFuListViewItem* listItem = dynamic_cast<FFuListViewItem*>(this->itemAt(e->pos()));
  if (listItem && e->button() == Qt::LeftButton)
    {
      // manual toggling
      if (listItem->isItemToggleAble()) {
	int treeStepSize = 20; // QTreeWidget::treeStepSize() does not exist
	int offset = this->rootIsDecorated() ? treeStepSize : 0;
	FFuListViewItem* parent = listItem;
	while ((parent = parent->getParentItem()))
	  offset += treeStepSize;
	if (offset < e->pos().x() && e->pos().x() < offset+15) {
	  listItem->toggleItem(true);
	  return;
	}
      }

      // Changed 28.10.16 (kmo): Invoke call-back before QTreeWidget::mousePressEvent()
      this->invokeLeftMouseBPressedCB(listItem);
      this->QTreeWidget::mousePressEvent(e);
    }
  else if (e->button() == Qt::RightButton)
    {
      this->invokeRightMouseBPressedCB(listItem);
      this->executePopUp(listItem);
    }
}
//----------------------------------------------------------------------------

void FFuQtListView::mouseReleaseEvent(QMouseEvent* e)
{
  // every release not only on items
  if (e->button() == Qt::LeftButton)
    this->invokeLeftMouseBReleaseCB();

  this->QTreeWidget::mouseReleaseEvent(e);
}
//----------------------------------------------------------------------------
