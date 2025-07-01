// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiCtrlModeller.H"

#include "FFuLib/FFuBase/FFuTopLevelShell.H"
#include "vpmUI/FuiCtrlModes.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpPM.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdCtrlDB.H"
#endif


namespace
{
  int mode = 0;
  int state = 0;

  int neutralType = 0;
  int neutralState = 0;

  bool ctrlModellerOpen = false;
}


void FuiCtrlModes::setCtrlModellerState(bool isOpen)
{
  ctrlModellerOpen = isOpen;
}


bool FuiCtrlModes::isCtrlModellerOpen()
{
  return ctrlModellerOpen;
}


void FuiCtrlModes::setMode(int newMode)
{
  // Sets mode to NEUTRAL
  FuiCtrlModes::cancel();

  if (FpPM::getResultFlag())
    return;

  mode = newMode;
  state = 0;

#ifdef USE_INVENTOR
  FdCtrlDB::updateMode(newMode,NEUTRAL_MODE);
#endif

  FFuTopLevelShell* ui = FFuTopLevelShell::getInstanceByType(FuiCtrlModeller::getClassTypeID());
  if (ui) ui->updateMode();

  FuiCtrlModes::setTip();
}


int FuiCtrlModes::getMode()
{
  return mode;
}


void FuiCtrlModes::setState(int newState)
{
  state = newState;

  FuiCtrlModes::setTip();
}


int FuiCtrlModes::getState()
{
  return state;
}


void FuiCtrlModes::setNeutralType(int newType)
{
  if (FpPM::getResultFlag() || mode != NEUTRAL_MODE)
    return;

#ifdef USE_INVENTOR
  int oldNeutralType = neutralType;
#endif
  neutralType  = newType;
  neutralState = 0;

#ifdef USE_INVENTOR
  FdCtrlDB::updateNeutralType(newType,oldNeutralType);
#endif

  FFuTopLevelShell* ui = FFuTopLevelShell::getInstanceByType(FuiCtrlModeller::getClassTypeID());
  if (ui) ui->updateMode();
}


int FuiCtrlModes::getNeutralType()
{
  return neutralType;
}


void FuiCtrlModes::setNeutralState(int newState)
{
  neutralState = newState;

  FuiCtrlModes::setTip();
}


int FuiCtrlModes::getNeutralState()
{
  return neutralState;
}


void FuiCtrlModes::done()
{
  if (FuiModes::getMode() != FuiModes::EXAM_MODE)
    return;

  switch (mode)
    {
    case NEUTRAL_MODE:
      if (neutralType != EXAM_NEUTRAL)
        break;

    case CREATE_MODE:
    case ADDLINEPOINT_MODE:
      switch (state)
        {
        case 0:
          FuiCtrlModes::cancel();
          break;
        }
      break;

    case ROTATE_MODE:
      switch (state)
      {
        case 0:
          FuiCtrlModes::cancel();
          break;
        case 1:
          FuiCtrlModes::setState(2);
          break;
        default:
          FuiCtrlModes::cancel();
          break;
      }
      break;

    case DELETE_MODE:
      switch (state)
        {
        case 0:
          FuiCtrlModes::cancel();
          break;
        case 1:
          FuiCtrlModes::setState(2);
          break;
        default:
          FuiCtrlModes::cancel();
          break;
        }
      break;

    case REMOVELINEPOINT_MODE:
      switch (state)
        {
        case 0:
          FuiCtrlModes::cancel();
          break;
        case 1:
          FuiCtrlModes::setState(4);
          break;
        case 2:
          FuiCtrlModes::setState(0);
          break;
        case 3:
          FuiCtrlModes::setState(0);
          break;
        default:
          FuiCtrlModes::setState(0);
          break;
        }
      break;

    case MOVEGROUP_MODE:
      switch (state)
        {
        case 0:
          FuiCtrlModes::setState(1);
          break;
        case 1:
          FuiCtrlModes::setState(2);
          break;
        default:
          FuiCtrlModes::cancel();
          break;
        }
      break;
    }
}


void FuiCtrlModes::cancel()
{
  static bool isDoingCancelAlready = false;

  if (isDoingCancelAlready || !ctrlModellerOpen)
    return;

  if (FuiModes::getMode() == FuiModes::EXAM_MODE)
    isDoingCancelAlready = true;
  else
    return;

#ifdef USE_INVENTOR
  int oldNeutralType = neutralType;
  int oldMode = mode;
#endif

  mode = NEUTRAL_MODE;
  state = 0;

  neutralType = EXAM_NEUTRAL;
  neutralState = 0;

#ifdef USE_INVENTOR
  FdCtrlDB::updateMode(mode, oldMode);
  FdCtrlDB::updateNeutralType(neutralType, oldNeutralType);
#endif

  FuiCtrlModes::setTip();

  isDoingCancelAlready = false;
}


void FuiCtrlModes::setTip()
{
  if (FpPM::getResultFlag())
    return;

  switch (mode)
    {
    case NEUTRAL_MODE:
      switch (neutralType)
	{
	case EXAM_NEUTRAL:
	  Fui::tip(0);
	  break;
	case MOVEELEM_NEUTRAL:
	  Fui::tip("Move the mouse");
	  break;
	case DRAW_NEUTRAL:
	  switch (neutralState)
	    {
	    case 0:
	      Fui::tip("Drag the line to an element input port");
	      break;
	    case 1:
	      Fui::tip("Release the mouse button to connect the line");
	      break;
	    case 2:
	      Fui::tip("This port is occupied, delete the existing line first");
	      break;
	    }
	  break;
	case MOVELINE_NEUTRAL:
	  switch (neutralState)
	    {
	    case 0:
	      Fui::tip("Move the mouse");
	      break;
	    case 1:
	      Fui::tip("You can't move this part of the line");
	      break;
	    }
	}
      break;

    case CREATE_MODE:
      Fui::tip("Press the left mouse button to release the element");
      break;

    case ROTATE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick the element you want to rotate");
	  break;
	case 1:
	  Fui::tip("Rotate the selected element by pressing Done");
	  break;
	}
      break;

    case DELETE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick control object to erase");
	  break;
	case 1:
	  Fui::tip("Erase the element by pressing Done");
	  break;
	}
      break;

    case ADDLINEPOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick and drag a line segment to insert a new Breakpoint");
	  break;
	case 1:
	  Fui::tip("Drag the line segment to the position you want");
	  break;
	case 2:
	  break;
	}
      break;

    case REMOVELINEPOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick the Breakpoint you want to remove");
	  break;
	case 1:
	  Fui::tip("Press Done to remove the Breakpoint");
	  break;
	case 2:
	  Fui::tip("This Breakpoint can't be removed");
	  break;
	case 3:
	  Fui::tip("You didn't hit a Breakpoint, try again");
	  break;
	}
      break;

    case MOVEGROUP_MODE:
      switch (state)
	{
	case 0:
	case 1:
	case 2:
	  Fui::tip("Pick elements to move, then drag them to a new position. Keep the <Ctrl>-key pressed until you are finished.");
	  break;
	}
      break;
    }
}
