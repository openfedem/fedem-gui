// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <cstring>

#include "FapSelectionCmds.H"
#include "vpmApp/FapEventManager.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmEngine.H"
#include "vpmUI/vpmUITopLevels/FuiProperties.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "vpmUI/Fui.H"

FapSelectionCmds::SignalConnector FapSelectionCmds::signalConnector;
FapSelectionCmds::FFaViewItemList FapSelectionCmds::ourSelectionHistory;
FapSelectionCmds::FFaViewItemList::iterator FapSelectionCmds::ourSelectionPt;
bool FapSelectionCmds::ourIAmSelecting = false;
bool FapSelectionCmds::ourSelectionIsEmpty = true;


void FapSelectionCmds::init()
{
  FFuaCmdItem* cmdItem;

  cmdItem = new FFuaCmdItem("cmdId_backward_select");
  cmdItem->setSmallIcon(backwardSelect_xpm);
  cmdItem->setText("Previous Selection");
  cmdItem->setToolTip("Go to your previous selection");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSelectionCmds::backward));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapSelectionCmds::getBwdSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_forward_select");
  cmdItem->setSmallIcon(forwardSelect_xpm);
  cmdItem->setText("Next Selection");
  cmdItem->setToolTip("Go to your next selection");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSelectionCmds::forward));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapSelectionCmds::getFwdSensitivity,bool&));

  cmdItem = new FFuaCmdItem("cmdId_contextHelp_select");
  cmdItem->setSmallIcon(help_xpm);
  cmdItem->setText("Help Reference");
  cmdItem->setToolTip("Shows help for the currently selected object");
  cmdItem->setActivatedCB(FFaDynCB0S(FapSelectionCmds::help));
  cmdItem->setGetSensitivityCB(FFaDynCB1S(FapSelectionCmds::getHlpSensitivity,bool&));
}


void FapSelectionCmds::getFwdSensitivity(bool& sensitivity)
{
  if (ourSelectionHistory.size() <= 1)
    sensitivity = false;
  else
    sensitivity = ourSelectionPt != ourSelectionHistory.begin();
}


void FapSelectionCmds::getBwdSensitivity(bool& sensitivity)
{
  if (ourSelectionHistory.empty())
    sensitivity = false;
  else if (ourSelectionPt == ourSelectionHistory.end())
    sensitivity = false;
  else if (ourSelectionIsEmpty)
    sensitivity = true;
  else
  {
    FFaViewItemList::iterator last = ourSelectionHistory.end();
    sensitivity = ourSelectionPt != --last;
  }
}


void FapSelectionCmds::backward()
{
  FFaViewItemList::iterator endIt = ourSelectionHistory.end();
  if (endIt != ourSelectionHistory.begin())
    --endIt; 
  if (!ourSelectionIsEmpty && ourSelectionPt != ourSelectionHistory.end() && ourSelectionPt != endIt)
    ++ourSelectionPt;

  FapSelectionCmds::setSelection();
}


void FapSelectionCmds::forward()
{
  if (!ourSelectionIsEmpty && ourSelectionPt != ourSelectionHistory.begin())
    --ourSelectionPt;

  FapSelectionCmds::setSelection();
}


void FapSelectionCmds::getHlpSensitivity(bool& sensitivity)
{
  FFaViewItems permSelection;
  FFaViewItem* tmpSelection;
  FapEventManager::getSelection(permSelection,tmpSelection);
  if (!tmpSelection && !permSelection.empty())
    tmpSelection = permSelection.front();

  sensitivity = dynamic_cast<FmRingStart*>(tmpSelection) == NULL;
}


void FapSelectionCmds::help()
{
  // Get selected object
  FFaViewItems permSelection;
  FFaViewItem* mySelection = NULL;
  FapEventManager::getSelection(permSelection,mySelection);
  if (!mySelection) {
    if (permSelection.empty()) {
      // Nothing selected, show welcome page
      Fui::showCHM("introduction/default.htm");
      return;
    }
    else
      mySelection = permSelection.front();
  }

  static std::string basicObjs("Animation, Beam, Gear, Graph, Material, Reference plane, Rack and pinion");
  static std::string tabbedObjs("Ball joint, Beam cross section, Cam joint, Curve, Cylindric joint, Free joint, Part, Prismatic joint, Revolute joint, Rigid joint, Triad");
  static std::string marineObjs("Vessel motion, Wave function, Current function, Beamstring, Jacket, Soil pile");
  static std::string windpowerObjs("Blade, Gearbox, Generator, Nacelle, Rotor, Shaft, Tower, Turbine");

  // Get topic name from selected object
  const char* topic = mySelection->getItemName();
  if (tabbedObjs.find(topic) != std::string::npos) {
    const char* tab = Fui::getProperties()->getSelectedTab(topic);
    Fui::showCHM((std::string("properties/") + topic).c_str(),tab);
  }
  else if (basicObjs.find(topic) != std::string::npos)
    Fui::showCHM((std::string("properties/") + topic).c_str());
  else if (marineObjs.find(topic) != std::string::npos)
    Fui::showCHM((std::string("properties/marine/") + topic).c_str());
  else if (windpowerObjs.find(topic) != std::string::npos)
    Fui::showCHM((std::string("properties/windpower/") + topic).c_str());
  else if (!strcmp(topic,"Axial spring") || !strcmp(topic,"Joint spring"))
    Fui::showCHM("properties/spring.htm");
  else if (!strcmp(topic,"Axial damper") || !strcmp(topic,"Joint damper"))
    Fui::showCHM("properties/damper.htm");
  else if (!strcmp(topic,"Force") || !strcmp(topic,"Torque"))
    Fui::showCHM("properties/load.htm");
  else if (!strcmp(topic,"Structure Assembly"))
    Fui::showCHM("properties/sub-assembly.htm");
  else if (!strcmp(topic,"Function")) {
    const char* ftyp = NULL;
    if (FmEngine* engine = dynamic_cast<FmEngine*>(mySelection); engine) {
      if (FmMathFuncBase* func = engine->getFunction(); func)
        ftyp = func->getItemName();
      else if (engine->isFunctionLinked())
        ftyp = "refer-to-other-function";
      else if (engine->myOutput.getValue())
        ftyp = "threshold";
      else
        ftyp = "one-to-one";
    }
    Fui::showCHM("properties/function.htm",ftyp);
  }
  else
    Fui::showCHM("properties/other-elements.htm");
}


void FapSelectionCmds::setSelection()
{
  if (!ourSelectionHistory.empty() && ourSelectionPt != ourSelectionHistory.end())
  {
    ourIAmSelecting = true;
    FapEventManager::permTotalSelect(*ourSelectionPt);
    ourIAmSelecting = false;
    ourSelectionIsEmpty = false;
  }
}


void FapSelectionCmds::onModelMemberDisconnected(FmModelMemberBase* item)
{
  FFaViewItem* obj = dynamic_cast<FFaViewItem*>(item);
  if (!obj) return;

  // Loop over all the selections in the history to clean away
  // the item that is disconnected

  FFaViewItemList::iterator selIt = ourSelectionHistory.begin();
  while (selIt != ourSelectionHistory.end())
    {
      // Loop over all objects in the "current" selection set
      int i, nSelItems = selIt->size();
      for (i = 0; i < nSelItems; i++)
        if ((selIt->begin())[i] == obj){
          selIt->erase(selIt->begin() + i);
          nSelItems = selIt->size();
          i--;
        }

      // If the "current" selection is empty, then delete it
      if (selIt->empty())
	{
	  FFaViewItemList::iterator tmp = selIt++;
	  bool currentSelectionRemoved = ourSelectionPt == tmp;
	  ourSelectionHistory.erase(tmp);
	  if (currentSelectionRemoved){
	    ourSelectionPt = ourSelectionHistory.begin();
	    ourSelectionIsEmpty = true;
	  }
	}
      else
        ++selIt;
    }
}


void FapSelectionCmds::onPermSelectionChanged(FFaViewItems selection,
					      FFaViewItems, FFaViewItems)
{
  if (ourIAmSelecting)
    return;

  if (!ourSelectionHistory.empty() && ourSelectionPt != ourSelectionHistory.begin())
  {
    ourSelectionHistory.erase(ourSelectionHistory.begin(),ourSelectionPt);
    ourSelectionPt = ourSelectionHistory.begin();
  }

  if (!(ourSelectionIsEmpty = selection.empty()))
  {
    ourSelectionHistory.push_front(selection);
    ourSelectionPt = ourSelectionHistory.begin();
    if (ourSelectionHistory.size() > 50)
      ourSelectionHistory.pop_back();
  }
}


FapSelectionCmds::SignalConnector::SignalConnector() : FFaSwitchBoardConnector("FapSelectionCmds")
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1S(FapSelectionCmds,onModelMemberDisconnected,
				    FmModelMemberBase*));

  FFaSwitchBoard::connect(FapEventManager::getSignalConnector(),
			  FapEventManager::PERMSELECTIONCHANGED,
			  FFaSlot3S(FapSelectionCmds,onPermSelectionChanged,
				    FFaViewItems,FFaViewItems,FFaViewItems));
}
