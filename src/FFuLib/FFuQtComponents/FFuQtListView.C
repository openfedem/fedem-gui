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
  this->setWidget(this);
  this->setColumnCount(nColumns);
  this->setObjectName(name);
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

void FFuQtListView::setListSorting(int column, bool ascnd)
{
  this->sortByColumn(column, ascnd ? Qt::AscendingOrder : Qt::DescendingOrder);
}
//----------------------------------------------------------------------------

void FFuQtListView::setHeaderClickEnabled(int, bool enable)
{
  this->header()->setClickable(enable);
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

void FFuQtListView::permSelectListItem(FFuListViewItem* item,
                                       bool select, bool notify)
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

void FFuQtListView::openListItem(FFuListViewItem* item,
                                 bool open, bool notify)
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

void FFuQtListView::ensureListItemVisible(FFuListViewItem* item, bool notify)
{
  QTreeWidgetItem* qitem = dynamic_cast<QTreeWidgetItem*>(item);
  if (!qitem) return;

  bool wasblocked = this->areLibSignalsBlocked();

  if (!notify)
    this->blockLibSignals(true);
  this->scrollTo(this->indexFromItem(qitem));

  this->blockLibSignals(wasblocked);
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getSelectedListItemSglMode() const
{
  QList<QTreeWidgetItem*> selected = this->selectedItems();
  if (selected.isEmpty()) return NULL;

  return dynamic_cast<FFuListViewItem*>(selected.front());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getCurrentListItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->currentItem());
}
//----------------------------------------------------------------------------

FFuListViewItem* FFuQtListView::getFirstChildItem() const
{
  return dynamic_cast<FFuListViewItem*>(this->topLevelItem(0));
}
//----------------------------------------------------------------------------

bool FFuQtListView::isSglSelectionMode() const
{
  return this->selectionMode() == SingleSelection;
}
//----------------------------------------------------------------------------

const char* FFuQtListView::getName() const
{
  static std::string name;
  name = this->objectName().toStdString();
  return name.c_str();
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

  QColorGroup textFieldNormal   (TextOnFieldBackground,
				 StdBackground,
				 LightShadow,
				 DarkShadow,
				 MidShadow,
				 TextOnFieldBackground,
				 FieldBackground);

  QColorGroup textFieldDisabled (textFieldNormal.foreground().dark(125),
				 textFieldNormal.background(),
				 textFieldNormal.light     (),
				 textFieldNormal.dark      (),
				 textFieldNormal.mid       (),
				 textFieldNormal.text      ().light(80),
				 textFieldNormal.base      ().dark(80));

  QPalette textFieldPalette(textFieldNormal, textFieldDisabled, textFieldNormal);

  this->setPalette(textFieldPalette);

  QColorGroup stdNormal   (TextOnStdBackground,
			   StdBackground,
			   LightShadow,
			   DarkShadow,
			   MidShadow,
			   TextOnStdBackground,
			   FieldBackground);

  QColorGroup stdDisabled (stdNormal.foreground().dark(125),
			   stdNormal.background(),
			   stdNormal.light     (),
			   stdNormal.dark      (),
			   stdNormal.mid       (),
			   stdNormal.text      ().dark(125),
			   stdNormal.base      ().dark(80));

  QPalette stdPalette(stdNormal, stdDisabled, stdNormal);

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

  this->setFont(listviewFont);

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
  FFuListViewItem* item = dynamic_cast<FFuListViewItem*>(this->itemAt(e->pos()));
  if (e->button() & Qt::LeftButton)
  {
    if (item && item->isItemToggleAble()) // Manual toggling
    {
      int levels = item->getDepth() + (this->rootIsDecorated() ? 1 : 0);
      int offset = e->pos().x() - this->indentation()*levels;
      if (offset > 0 && offset < 15)
      {
        item->toggleItem(true);
        return;
      }
    }

    this->invokeLeftMouseBPressedCB(item);
  }
  else if (e->button() & Qt::RightButton)
  {
    this->invokeRightMouseBPressedCB(item);
    this->executePopUp(item);
  }

  this->QTreeWidget::mousePressEvent(e);
}
//----------------------------------------------------------------------------

void FFuQtListView::mouseReleaseEvent(QMouseEvent* e)
{
  if (e->button() & Qt::LeftButton)
    this->invokeLeftMouseBReleaseCB();

  this->QTreeWidget::mouseReleaseEvent(e);
}
//----------------------------------------------------------------------------
