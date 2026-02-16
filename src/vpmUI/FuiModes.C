// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppCmds/FapAllignCSCmds.H"
#include "vpmApp/vpmAppCmds/FapStrainRosetteCmds.H"
#include "vpmApp/vpmAppCmds/FapGeneralSpiderCmds.H"
#include "vpmApp/vpmAppCmds/FapCylinderSpiderCmds.H"
#include "vpmApp/vpmAppCmds/FapCreateSensorCmd.H"
#include "vpmApp/vpmAppCmds/FapAddResProbeCmd.H"
#include "vpmApp/vpmAppCmds/FapMoveToCenterCmds.H"
#include "vpmApp/vpmAppUAMap/FapUAModeller.H"
#include "vpmUI/vpmUIComponents/FuiPositionData.H"
#include "vpmUI/FuiModes.H"
#include "vpmUI/Fui.H"
#ifdef USE_INVENTOR
#include "vpmDisplay/FdDB.H"
#endif
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFuLib/FFuUserDialog.H"


// Initializing static variables

namespace
{
  FuiModes::ModeType mode = FuiModes::EXAM_MODE;
  int state = 0;
  bool createPositionIsChangedByGUI = false;
}

std::string FuiModes::tipComPicking;


void FuiModes::setMode(ModeType newMode)
{
  FuiModes::cancel();

  switch ((mode = newMode))
    {
    case ALLIGNCS_MODE:
    case ALLIGNROT_MODE:
      FapAllignCSCmds::enterMode();
      break;
    case MOVE_TO_CENTER_MODE:
      FapMoveToCenterCmds::enterMode();
      break;
    case MAKESTRAINROSETTE_MODE:
      FapStrainRosetteCmds::enterMode();
      break;
    case EDITSTRAINROSETTENODES_MODE:
      FapEditStrainRosetteNodesCmd::enterMode();
      break;
    case EDITSTRAINROSETTEDIR_MODE:
      FapEditStrainRosetteDirCmd::enterMode();
      break;
    case ADD_RESULTSPROBE_MODE:
      FapAddResProbeCmd::enterMode();
      break;
    case CREATE_SENSOR_MODE:
      FapCreateSensorCmd::enterMode();
      break;
    case SELECTREFCS_MODE:
      Fui::tip("Select reference CS by picking in the 3D view or selecting from the Objects browser");
      break;
    case MAKEGENERALSPIDER_MODE:
      FapGeneralSpiderCmds::enterMode();
      break;
    case MAKECYLINDERSPIDER_MODE:
      FapCylinderSpiderCmds::enterMode();
      break;
    default:
      break;
    }

#ifdef USE_INVENTOR
  FdDB::updateMode();
#endif
  Fui::updateMode();
  FapUAModeller::updateMode();

  FuiModes::setTip();

  // To make the default create position work
  // when we enter a mode for the first time
  createPositionIsChangedByGUI = true;
}


FuiModes::ModeType FuiModes::getMode()
{
  return mode;
}


void FuiModes::setState(int newState)
{
#ifdef USE_INVENTOR
  FdDB::updateState(newState);
#endif
  Fui::updateState(newState);
  FapUAModeller::updateState(newState);

  state = newState;

  FuiModes::setTip();

  createPositionIsChangedByGUI = false;
}


int FuiModes::getState()
{
  return state;
}


void FuiModes::notifyCreatePointChange()
{
  createPositionIsChangedByGUI = true;
}


void FuiModes::cancel()
{
  switch (mode)
    {
    case ALLIGNCS_MODE:
    case ALLIGNROT_MODE:
      FapAllignCSCmds::cancelMode();
      break;
    case MOVE_TO_CENTER_MODE:
      FapMoveToCenterCmds::cancelMode();
      break;
    case MAKESTRAINROSETTE_MODE:
      FapStrainRosetteCmds::cancelMode();
      break;
    case EDITSTRAINROSETTENODES_MODE:
      FapEditStrainRosetteNodesCmd::cancelMode();
      break;
    case EDITSTRAINROSETTEDIR_MODE:
      FapEditStrainRosetteDirCmd::cancelMode();
      break;
    case ADD_RESULTSPROBE_MODE:
      FapAddResProbeCmd::cancelMode();
      break;
    case CREATE_SENSOR_MODE:
      FapCreateSensorCmd::cancelMode();
      break;
    case SELECTREFCS_MODE:
      FuiPositionData::cancelActiveUI();
      break;
    case MAKEGENERALSPIDER_MODE:
      FapGeneralSpiderCmds::cancelMode();
      break;
    case MAKECYLINDERSPIDER_MODE:
      FapCylinderSpiderCmds::cancelMode();
      break;
    default:
      break;
    }

#ifdef USE_INVENTOR
  FdDB::cancel();
#endif
  Fui::cancel();
  FapUAModeller::cancel();

  mode = EXAM_MODE;
  state = 0;

  FuiModes::setTip();
}


void FuiModes::done()
{
  switch (mode)
    {
    case EXAM_MODE:
    case ERASE_MODE:
    case APPEARANCE_MODE:
    case DETACH_MODE:
      if (state == 1)
        FuiModes::setState(0);
      else
        FuiModes::cancel();
      break;

    case ATTACH_MODE:
    case PTPMOVE_MODE:
      if (state == 1)
        FuiModes::setState(2);
      else if (state == 2 || state == 3)
        FuiModes::setState(0);
      else
        FuiModes::cancel();
      break;

    case ALLIGNCS_MODE:
    case ALLIGNROT_MODE:
      FapAllignCSCmds::allignCSDone();
      break;
    case MOVE_TO_CENTER_MODE:
      FapMoveToCenterCmds::done();
      break;
    case MAKESTRAINROSETTE_MODE:
      FapStrainRosetteCmds::done();
      break;
    case EDITSTRAINROSETTENODES_MODE:
      FapEditStrainRosetteNodesCmd::done();
      break;
    case EDITSTRAINROSETTEDIR_MODE:
      FapEditStrainRosetteDirCmd::done();
      break;
    case ADD_RESULTSPROBE_MODE:
      FapAddResProbeCmd::done();
      break;
    case MAKEGENERALSPIDER_MODE:
      FapGeneralSpiderCmds::done();
      break;
    case MAKECYLINDERSPIDER_MODE:
      FapCylinderSpiderCmds::done();
      break;

    case MAKETRIAD_MODE:
    case MAKEFORCE_MODE:
    case MAKETORQUE_MODE:
    case MAKEREVJOINT_MODE:
    case MAKEBALLJOINT_MODE:
    case MAKERIGIDJOINT_MODE:
      // One-pick create done
      switch (state) {
      case 0:
        if (!createPositionIsChangedByGUI) {
          FuiModes::cancel();
          break;
        }
      case 1:
        FuiModes::setState(2);
        FuiModes::setState(0); // For continous creating
        break;
      default:
        FuiModes::cancel();
      }
      break;

    case MAKESPRING_MODE:
    case MAKEDAMPER_MODE:
    case MAKEFREEJOINT_MODE:
    case MAKECYLJOINT_MODE:
    case MAKEPRISMJOINT_MODE:
      // Two-pick create done
      switch (state) {
      case 0:
        if (createPositionIsChangedByGUI)
          FuiModes::setState(2);
        else
          FuiModes::cancel();
        break;
      case 1:
        FuiModes::setState(2);
        break;
      case 2: // Pick slave
      case 3:
        FuiModes::setState(4);
        FuiModes::setState(0); // For continous creating
        break;
      default:
        FuiModes::cancel();
      }
      break;

    case MAKEPRISMJOINTBETWEENTRIADS_MODE:
    case MAKECYLJOINTBETWEENTRIADS_MODE:
      // Three-pick create done
      switch (state) {
      case 0: // Pick first master
        if (createPositionIsChangedByGUI)
          FuiModes::setState(3);
        else
          FuiModes::cancel();
        break;
      case 1: // Triad picked
        FuiModes::setState(3);
        break;
      case 3: // Pick second master
        FuiModes::setState(4);
        break;
      case 4: // Triad picked
        FuiModes::setState(6);
        break;
      case 6:
      case 7:
        FuiModes::setState(10);
        FuiModes::setState(0); // For continous creating
        break;
      case 2: // Triad is already slave in another joint
      case 5: // Triad is already slave
      case 8:
      case 9:
        break;
      default:
        FuiModes::cancel();
      }
      break;

    case MAKEFREEJOINTBETWEENTRIADS_MODE:
      switch (state) {
      case 0:
        if (createPositionIsChangedByGUI)
          FuiModes::setState(4);
        else
          FuiModes::cancel();
        break;
      case 1: // Triad picked
        FuiModes::setState(4);
        break;
      case 2: // Reference planed picked
        FuiModes::setState(4);
        break;
      case 3: // Other object picked
      case 4: // Pick slave
      case 5: // Triad picked
        FuiModes::setState(8);
        FuiModes::setState(0); // For continous creating
        break;
      case 6: // Triad is slave in another joint
      case 7: // No triad selected
      case 8: // Triad is attached to reference plane
        break;
      default:
        FuiModes::cancel();
      }
      break;

    case MAKECAMJOINT_MODE:
      switch (state) {
      case 0:
        if (createPositionIsChangedByGUI)
          FuiModes::setState(2);
        else
          FuiModes::cancel();
        break;
      case 1:
        FuiModes::setState(2);
        break;
      case 2:
        if (createPositionIsChangedByGUI) {
          FuiModes::setState(4);
          FuiModes::setState(2);
        }
        else
          FuiModes::setState(0);
        break;
      case 3:
        FuiModes::setState(4);
        FuiModes::setState(2);
        break;
      case 5:
        FuiModes::setState(0);
        break;
      default:
        FuiModes::cancel();
      }
      break;

    case MAKEGEAR_MODE:
    case MAKERACKPIN_MODE:
      if (state == 1)
        FuiModes::setState(2);
      else if (state == 3)
      {
        FuiModes::setState(4);
        FuiModes::setState(0); // For continous creating
      }
      else if (state != 2)
        FuiModes::cancel();
      break;

    case CREATE_SENSOR_MODE:
      FapCreateSensorCmd::done();
      break;

    case MAKERELATIVESENSOR_MODE:
      if (state == 1 || state == 2)
        FuiModes::setState(2);
      else if (state == 3)
      {
        FuiModes::setState(4);
        FuiModes::setState(0); // For continous creating
      }
      else
        FuiModes::cancel();
      break;

    case PICKLOADFROMPOINT_MODE:
    case PICKLOADTOPOINT_MODE:
    case PICKLOADATTACKPOINT_MODE:
    case COMPICKPOINT_MODE:
      if (state == 1)
        FuiModes::setState(2);
      FuiModes::cancel();
      break;

    case MAKESTICKER_MODE:
    case MAKESIMPLESENSOR_MODE:
    case MAKETIRE_MODE:
    case ADDMASTERINLINJOINT_MODE:
      if (state == 1)
      {
        FuiModes::setState(2);
        FuiModes::setState(0); // For continous creating
      }
      else
        FuiModes::cancel();
      break;

    case SELECTREFCS_MODE:
    case MEASURE_DISTANCE_MODE:
    case MEASURE_ANGLE_MODE:
      FuiModes::cancel();
      break;

    default:
      break;
    }
}


void FuiModes::setTip()
{
  switch (mode)
    {
    case EXAM_MODE:
      Fui::tip(NULL);
      FFaMsg::setStatus("Ready");
      break;

    case APPEARANCE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select a Part or the Reference Plane");
	  break;
	case 1:
	  Fui::tip("Select again to edit appearance on another Part or the Reference Plane");
	  break;
	}
      break;

    case PTPMOVE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick point to move from, on object to move");
	  break;
	case 1:
	  Fui::tip("Pick point to move from, on object to move. "
		   "Press and hold the <Ctrl>-key to add objects to the selection (Done when ready)");
	  break;
	case 2:
	case 3:
	  Fui::tip("Pick point to move to (Done when ready)");
	  break;
	}
      break;

    case ERASE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select an object to erase");
	  break;
	case 1:
	  Fui::tip("Select another object, or press and hold the <Ctrl>-key to add objects to the selection (Done when ready)");
	  break;
	}
      break;

    case ATTACH_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select object to attach");
	  break;
	case 1:
	  Fui::tip("Select another object to attach (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Select Part to attach to");
	  break;
	case 3:
	  Fui::tip("Select Part to attach to (Done when ready)");
	  break;
	}
      break;

    case DETACH_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select object to detach");
	  break;
	case 1:
	  Fui::tip("Select object to detach (Done when ready)");
	  break;
	}
      break;

    case MAKEFORCE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to apply Force (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to apply Force (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Force ...");
	  break;
	}
      break;

    case MAKETORQUE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to apply Torque (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to apply Torque (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Torque ...");
	  break;
	}
      break;

    case MAKETRIAD_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put Triad (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put Triad (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Triad ...");
	  break;
	}
      break;

    case MAKEREVJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put Revolute Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put Revolute Joint (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Revolute Joint ...");
	  break;
	}
      break;

    case MAKEBALLJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put Ball Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put Ball Joint (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Ball Joint ...");
	  break;
	}
      break;

    case MAKERIGIDJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put Rigid Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put Rigid Joint (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Rigid Joint ...");
	  break;
	}
      break;

    case MAKESPRING_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put start of Axial Spring (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put start of Axial Spring (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Pick or write where to put end of Axial Spring (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Pick again, or write to change where to put end of Axial Spring (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Axial Spring ...");
	  break;
	}
      break;

    case MAKEDAMPER_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put start of Axial Damper (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put start of Axial Damper (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Pick or write where to put end of Axial Damper (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Pick again, or write to change where to put end of Axial Damper (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Axial Damper ...");
	  break;
	}
      break;

    case MAKEFREEJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put the Master triad of Free Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put the Master triad of Free Joint (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Pick or write where to put the Slave triad of Free Joint (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Pick again, or write to change where to put the Slave triad of Free Joint (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Free Joint ...");
	  break;
	}
      break;

    case MAKEFREEJOINTBETWEENTRIADS_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick a Triad to use as Master in the Free Joint, or the Reference Plane to create a grounded Free Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Triad selected. Pick again to change (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Reference Plane selected. Pick again to change (Done when ready)");
	  break;
	case 3:
	  Fui::tip("No Triad selected. Pick again (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Pick a Triad to use as Slave in the Free Joint (Done when ready)");
	  break;
	case 5:
	  Fui::tip("Triad selected. Pick again to change (Done when ready)");
	  break;
	case 6:
	  Fui::tip("Selected Triad is already a Slave in another Joint. Pick again (Done when ready)");
	  break;
	case 7:
	  Fui::tip("No Triad selected. Pick again (Done when ready)");
	  break;
	case 8:
	  Fui::tip("Selected Triad is attached to ground and can therefore not be a Slave. Pick again (Done when ready)");
	  break;
	case 9:
	  Fui::tip("Creating Free Joint ...");
	  break;
	}
      break;

    case MAKECYLJOINT_MODE:
    case MAKEPRISMJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write where to put start of glider (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to put start of glider (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Pick or write where to put end of glider (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Pick again, or write to change where to put end of glider (Done when ready)");
	  break;
	case 4:
	  if (mode == MAKECYLJOINT_MODE)
	    Fui::tip("Creating Cylindric Joint ...");
	  else
	    Fui::tip("Creating Prismatic Joint ...");
	  break;
	}
      break;

    case MAKECYLJOINTBETWEENTRIADS_MODE:
    case MAKEPRISMJOINTBETWEENTRIADS_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick the first Triad of the glider (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Pick the end Triad of the glider (Done when ready)");
	  break;
	case 1:
	case 4:
	case 7:
	  Fui::tip("Triad selected. Pick again to change (Done when ready)");
	  break;
	case 2:
	case 5:
	  Fui::tip("Selected Triad is already a Slave, and can therefore not be used as Master. Pick again (Done when ready)");
	  break;
	case 6:
	  Fui::tip("Pick the Slave triad of the Joint, or press Done to create a new Triad between the two Master triads.");
	  break;
	case 8:
	  Fui::tip("Selected Triad is already a Slave in another Joint. Pick again (Done when ready)");
	  break;
	case 9:
	  Fui::tip("Selected Triad is not along the line through the two Master triads. Pick again (Done when ready)");
	  break;
	case 10:
	  if (mode == MAKECYLJOINTBETWEENTRIADS_MODE)
	    Fui::tip("Creating Cylindric Joint ...");
	  else
	    Fui::tip("Creating Prismatic Joint ...");
	  break;
	}
      break;

    case MAKECAMJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick or write the position for the follower Triad (Done when ready)");
	  break;
	case 1:
	  Fui::tip("To change the position for the follower Triad, pick again or change the numbers in the odometre (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Pick or write a position, pick a Triad along the Cam curve, or pick an existing Cam curve (Done when ready)");
	  break;
	case 3:
	  Fui::tip("To change the selected position, pick again or change the numbers in the odometre (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Cam Joint ...");
	  break;
        case 5:
	  Fui::tip("To select a different Cam curve, or rather a position along a new curve, pick again (Done when ready)");
	  break;
	}
      break;

    case MAKESTICKER_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick what to apply Sticker to (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick again, or write to change where to apply Sticker (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Sticker ...");
	  break;
	}
      break;

    case MAKEGEAR_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select input Revolute Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Select another input Revolute Joint (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Select output Revolute Joint (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Select another output Revolute Joint (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Gear ...");
	  break;
	}
      break;

    case MAKERACKPIN_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select input Revolute Joint (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Select another input Revolute Joint (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Select output Prismatic Joint (Done when ready)");
	  break;
	case 3:
	  Fui::tip("Select another output Prismatic Joint (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Rack-and-Pinion ...");
	  break;
	}
      break;

    case MAKESIMPLESENSOR_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select object to attach Sensor to");
	  break;
	case 1:
	  Fui::tip("Select another object to attach Sensor to (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Sensor ...");
	  break;
	}
      break;

    case MAKETIRE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select Revolute Joint to attach Tire to");
	  break;
	case 1:
	  Fui::tip("Select another Revolute Joint to attach Tire to (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Creating Tire ...");
	  break;
	}
      break;

    case MAKERELATIVESENSOR_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Select first Triad");
	  break;
	case 1:
	  Fui::tip("Select another Triad (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Select second Triad");
	  break;
	case 3:
	  Fui::tip("Select another Triad (Done when ready)");
	  break;
	case 4:
	  Fui::tip("Creating Sensor ...");
	  break;
	}
      break;

    case PICKLOADFROMPOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick first point of Load direction (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick to change first point of Load direction (Done when ready)");
	  break;
	}
      break;

    case PICKLOADTOPOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick second point of Load direction (Done when ready)");
	  break;
	case 1:
	  Fui::tip("Pick to change second point of Load direction (Done when ready)");
	  break;
	}
      break;

    case PICKLOADATTACKPOINT_MODE:
      switch (state)
	{
	case 0:
	case 1:
	  Fui::tip("Pick to change where to apply Load (Done when ready)");
	  break;
	}
      break;

    case ADDMASTERINLINJOINT_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick where to add a Master to the Linear Joint");
	  break;
	case 1:
	  Fui::tip("Pick to change where to add the Master (Done when ready)");
	  break;
	case 2:
	  Fui::tip("Trying to Add master ...");
	  break;
	}
      break;

    case COMPICKPOINT_MODE:
      if (tipComPicking.empty())
        Fui::tip("Pick or write where to set application point (Done when ready)");
      else
        Fui::tip(tipComPicking.c_str());
      break;

    case MEASURE_DISTANCE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick first point to measure distance between");
	  break;
	case 1:
	  Fui::tip("Pick second point to measure distance betwem");
	  break;
	}
      break;

    case MEASURE_ANGLE_MODE:
      switch (state)
	{
	case 0:
	  Fui::tip("Pick first point to measure angle between");
	  break;
	case 1:
	  Fui::tip("Pick second point to measure angle between");
	  break;
	case 2:
	  Fui::tip("Pick point of rotation");
	  break;
	}
      break;

    default: // All other modes handle the tip setting themselves
      break;
    }
}
