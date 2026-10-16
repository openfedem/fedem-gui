// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_MSG_H
#define FUI_MSG_H

#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include <cstdio>


class FuiMsg : public FFaMsg
{
public:
  //! \brief Default constructor initializing the members.
  FuiMsg() { listFile = NULL; tmpName = NULL; }
  //! \brief The destructor closes the list file, if opened.
  virtual ~FuiMsg() { this->closeListFile(); }

  //! \brief Opens a list file containing a copy of the output list window.
  virtual bool openListFile(const char* = NULL, bool = false);
  //! \brief Closes the list file.
  virtual void closeListFile();

private:
  //! \brief Prints time stamp to the given file.
  static void printCurrentTime(FILE* fd, const char* action);

protected:
  virtual int dialogVt(const std::string& message,
		       const FFaDialogType type,
		       const char** genericButtons);
  virtual int dialogVt(const std::string& message,
                       const FFaDialogType type,
                       const std::vector<std::string>& buttonTexts);
  virtual int dialogVt(int& selectionIdx,
                       const std::string& message,
                       const FFaDialogType type,
                       const std::vector<std::string>& buttonTexts,
                       const std::vector<std::string>& selectionList);

  virtual void listVt(const std::string& str, bool onScreen = false);
  virtual void tipVt(const std::string& str);

  virtual void setStatusVt(const std::string& statusText);
  virtual void changeStatusVt(const std::string& statusText);
  virtual void pushStatusVt(const std::string& statusText);
  virtual void popStatusVt();

  virtual void enableSubStepsVt(int steps);
  virtual void setSubStepVt(int step);
  virtual void disableSubStepsVt();

  virtual void displayTimeVt(int hour, int min, int sec);
  virtual void clearTimeVt();

  virtual void setSubTaskVt(const std::string& taskText);

  virtual void enableProgressVt(int nSteps);
  virtual void setProgressVt(int progressStep);
  virtual void disableProgressVt();

private:
  FILE* listFile; //!< Copy all Output List contents to this file
  char* tmpName;  //!< Name of temporary list file to be deleted when closed
};

#endif
