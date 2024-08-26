// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/Fui.H"

#include <QApplication>
#include <QProcess>
#include <QString>
#include <QFile>


/*!
  This function opens the Fedem.CHM file (the Fedem Reference Guide)
  in the specified topic URL.
*/

void Fui::showCHM(const char* topic, const char* subtopic)
{
  // Get CHM parameters
  QString chmTopic(topic);
  if (chmTopic.right(4).compare(".htm") != 0)
    chmTopic += ".htm";
  if (subtopic)
    chmTopic += QString("#") + QString(subtopic);
  chmTopic.replace(' ','-');

  // Show CHM.
  // Example: "HH.EXE ms-its:C:/path/Fedem.chm::/htm/properties/animation.htm"
  QProcess* myProcess = new QProcess();
  QStringList arguments;
  arguments << "ms-its:" + QApplication::applicationDirPath() + "/Doc/Fedem.chm::/htm/" + chmTopic;
  myProcess->start("hh.exe", arguments);
}
