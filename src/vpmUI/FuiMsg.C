// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stack>

#include "vpmUI/vpmUITopLevels/FuiMainWindow.H"
#include "vpmUI/FuiMsg.H"
#include "vpmUI/Fui.H"
#include "Admin/FedemAdmin.H"
#include <time.h>
#if defined(win32) || defined(win64)
#define unlink _unlink
#else
#include <unistd.h>
#endif


namespace
{
  std::stack<std::string> ourStatuses; //! Status message stack

  //! \brief Prints the content of \ref ourStatuses.
  void showStatuses(bool newline = false)
  {
    std::cout <<"Status: ";
    if (ourStatuses.empty())
      std::cout <<"EMPTY";
    else if (ourStatuses.size() == 1)
      std::cout << ourStatuses.top();
    else
    {
      std::stack<std::string> myCopy(ourStatuses);
      std::vector<std::string> tmp;
      for (tmp.reserve(myCopy.size()); !myCopy.empty(); myCopy.pop())
        tmp.push_back(myCopy.top());
      // Print in reverse order to get the top at the end
      std::cout <<" "<< tmp.back();
      for (int i = tmp.size()-2; i >= 0; i--)
        std::cout <<", "<< tmp[i];
    }
    if (newline) std::cout << std::endl;
  }
}


void FuiMsg::printCurrentTime(FILE* fd, const char* action)
{
  const time_t currentTime = time(NULL);
  const char*  myTime = ctime(&currentTime);
  const char*  myVer  = FedemAdmin::getVersion();
  fprintf(fd,"\n###> Model %sed by Fedem %s at %s\n",action,myVer,myTime);
}


bool FuiMsg::openListFile(const char* fname, bool copyOldOne)
{
  char* newName = NULL;
  FILE* newFile = NULL;

  if (fname)
  {
    // Open a permanent list file for append and read
    newName = (char*)fname;
    newFile = fopen(newName,"a+");
  }
  else
  {
    // Open a new temporary file, delete when closed
#if defined(win32) || defined(win64)
    newName = _tempnam("C:","fedem_");
#else
    newName = tempnam(NULL,"fedem_");
#endif
    newFile = fopen(newName,"w+");
  }
  if (!newFile)
  {
    fprintf(stderr,"FuiMsg: Error opening Fedem list file\n");
    perror(newName);
    if (!fname) free(newName);
    return false;
  }
  else if (!copyOldOne)
    printCurrentTime(newFile,"open");

  if (listFile)
  {
    if (copyOldOne)
    {
      // Copy contents of the existing list file over to the new one
      char buf[1024];
      size_t nRead = 0;
      rewind(listFile);
      while ((nRead = fread(buf,1,sizeof(buf),listFile)) > 0)
	if (fwrite(buf,1,nRead,newFile) != nRead)
	{
          fprintf(stderr,"FuiMsg: Failed to copy output list to %s\n",newName);
	  if (!fname) free(newName);
	  return false;
	}
      fflush(newFile);
    }
    if (tmpName)
    {
      // Close and delete the temporary file
      fclose(listFile);
      unlink(tmpName);
      free(tmpName);
    }
    else
    {
      // Close the permanent file
      printCurrentTime(listFile,"clos");
      fclose(listFile);
    }
  }

  if (!fname)
    tmpName = newName; // The new file is a temporary file, delete when closed
  else
    tmpName = NULL;    // The new file is a permanent file, keep it when closed

  listFile = newFile;
  return true;
}


void FuiMsg::closeListFile()
{
  if (!listFile) return;
  printCurrentTime(listFile,"clos");
  fclose(listFile);
  listFile = NULL;

  if (!tmpName) return;
  unlink(tmpName);
  free(tmpName);
  tmpName = NULL;
}


int FuiMsg::dialogVt(const std::string& str, const FFaDialogType dialogType,
		     const char** genericButtons)
{
  if (Fui::hasGUI())
    switch (dialogType)
      {
      case FFaMsg::DISMISS_WARNING:
	Fui::dismissDialog(str.c_str(),FFuDialog::WARNING);
	return 0;

      case FFaMsg::DISMISS_ERROR:
	Fui::dismissDialog(str.c_str(),FFuDialog::ERROR);
	return 0;

      case FFaMsg::DISMISS_INFO:
	Fui::dismissDialog(str.c_str(),FFuDialog::INFO);
	return 0;

      case FFaMsg::WARNING:
	Fui::okDialog(str.c_str(),FFuDialog::WARNING);
	return 0;

      case FFaMsg::ERROR:
	Fui::okDialog(str.c_str(),FFuDialog::ERROR);
	return 0;

      case FFaMsg::OK:
	Fui::okDialog(str.c_str(),FFuDialog::INFO);
	return 0;

      case FFaMsg::OK_CANCEL:
	return Fui::okCancelDialog(str.c_str());

      case FFaMsg::YES_NO:
	return Fui::yesNoDialog(str.c_str());

      case FFaMsg::YES_NO_CANCEL: {
	const char* texts[3] = {"Yes","No","Cancel"};
	int answer = Fui::genericDialog(str.c_str(),texts,3);
	// We want 0=No, 1=Yes and 2=Cancel to comply with the yes-no dialog
	return answer < 2 ? 1-answer : answer;
      }

      case FFaMsg::GENERIC:
	if (!genericButtons) return -1; // logic error
	{
	  int n = 0; while (genericButtons[n]) n++;
	  return Fui::genericDialog(str.c_str(),genericButtons,n);
	}

      default:
	break;
      }

  return FFaMsg::dialogVt(str,dialogType,genericButtons);
}


void FuiMsg::listVt(const std::string& str, const bool onScreen)
{
  if (listFile)
  {
    fprintf(listFile,"%s",str.c_str());
    fflush(listFile);
  }

  if (onScreen) Fui::outputListUI();

  if (Fui::hasGUI())
    Fui::list(str.c_str());
  else
    FFaMsg::listVt(str,onScreen);
}


void FuiMsg::tipVt(const std::string& str)
{
  if (Fui::hasGUI()) Fui::tip(str.c_str());
}


void FuiMsg::setStatusVt(const std::string& statusText)
{
#ifdef FUIMSG_DEBUG
  showStatuses(false);
  std::cout <<", "<< statusText <<" [set]"<< std::endl;
#endif

  // Only set if no stack is active to avoid disturbance.
  // If we have non-sync push and pop, this will _not_ clean things.
  if (!ourStatuses.empty()) return;

  ourStatuses.push(statusText);
  if (Fui::getMainWindow())
    Fui::getMainWindow()->setStatus(statusText.c_str());
#ifndef FUIMSG_DEBUG
  else showStatuses();
#endif
}

void FuiMsg::changeStatusVt(const std::string& statusText)
{
#ifdef FUIMSG_DEBUG
  showStatuses(false);
  std::cout <<", "<< statusText <<" [change]"<< std::endl;
#endif

  ourStatuses.pop();
  ourStatuses.push(statusText);

  if (Fui::getMainWindow())
    Fui::getMainWindow()->setStatus(ourStatuses.top().c_str());
#ifndef FUIMSG_DEBUG
  else showStatuses();
#endif
}

void FuiMsg::pushStatusVt(const std::string& statusText)
{
#ifdef FUIMSG_DEBUG
  showStatuses(false);
  std::cout <<", "<< statusText <<" [push]"<< std::endl;
#endif

  ourStatuses.push(statusText);

  if (Fui::getMainWindow())
    Fui::getMainWindow()->setStatus(ourStatuses.top().c_str());
#ifndef FUIMSG_DEBUG
  else showStatuses();
#endif
}

void FuiMsg::popStatusVt()
{
#ifdef FUIMSG_DEBUG
  showStatuses(false);
  std::cout <<" [pop]"<< std::endl;
#endif

  ourStatuses.pop();

  if (Fui::getMainWindow())
  {
    if (!ourStatuses.empty())
      Fui::getMainWindow()->setStatus(ourStatuses.top().c_str());
  }
#ifndef FUIMSG_DEBUG
  else showStatuses();
#endif
}


void FuiMsg::enableSubStepsVt(int steps)
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->enableSubSteps(steps);
}

void FuiMsg::setSubStepVt(int step)
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->setSubStep(step);
}

void FuiMsg::disableSubStepsVt()
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->disableSubSteps();
}


void FuiMsg::displayTimeVt(int hour, int min, int sec)
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->displayTime(hour, min, sec);
}

void FuiMsg::clearTimeVt()
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->clearTime();
}


void FuiMsg::setSubTaskVt(const std::string& taskText)
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->setSubTask(taskText.c_str());
}


void FuiMsg::enableProgressVt(int nSteps)
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->enableProgress(nSteps);
}

void FuiMsg::setProgressVt(int progressStep)
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->setProgress(progressStep);
}

void FuiMsg::disableProgressVt()
{
  if (Fui::getMainWindow()) Fui::getMainWindow()->disableProgress();
}
