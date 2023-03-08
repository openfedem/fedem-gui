/* SPDX-FileCopyrightText: 2023 SAP SE
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file is part of FEDEM - https://openfedem.org
 */
/*
 * HeaderDelegate.C
 *
 *  Created on: May 23, 2012
 *      Author: runarhr
 */

#include "FFuLib/FFuCustom/inputTables/delegates/HeaderDelegate.H"
#include <QApplication>
#include <QPushButton>
#include <QPainter>


void HeaderDelegate::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const
{
  painter->save();

  QStyleOptionViewItem opt(option);
  initStyleOption(&opt, index);

  // draw correct background
  opt.text = "";
  QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

  QRect rect = opt.rect;
  QPalette::ColorGroup cg = QPalette::Normal;
  if (!(QStyle::State_Enabled & QStyle::State_Enabled))
    cg = QPalette::Disabled;
  else if (!(opt.state & QStyle::State_Active))
    cg = QPalette::Inactive;

  // set pen color
  if (opt.state & QStyle::State_Selected)
    painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
  else
    painter->setPen(opt.palette.color(cg, QPalette::Text));

  QPushButton* btn = new QPushButton();
  btn->setFixedSize(rect.width(),rect.height());
  painter->drawPixmap(QRect(rect.left(), rect.top(), rect.width(), rect.height()), btn->grab());

  // draw text
  QString name = index.model()->data(index, Qt::DisplayRole).toString();
  painter->drawText(QRect(rect.left(), rect.top(), rect.width(), rect.height()),
  Qt::AlignCenter, name);

  painter->restore();
}

