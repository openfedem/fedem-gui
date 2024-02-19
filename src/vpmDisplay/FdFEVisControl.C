// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdFEVisControl.H"
#include "vpmDisplay/FdFEGroupPart.H"
#include "FFdCadModel/FFdUtils/FFdLook.H"

#include <algorithm>
#include <iterator>


FdColor FdFEVisControl::ourBackgroundColor = { 0.0f, 0.0f, 0.0f };


FdFEVisControl::FdFEVisControl()
{
  IAmHidden = true;

  IAmShowingResults = false;
  IAmShowingColorResults = false;
  IAmShowingVertexResults = false;

  myLineDetailLevel = SURFACE;
  myDetailLevel = SURFACE;
  myDrawStyle = SOLID_LINES;

  IAmHighlighted = false;
  myHighlightType = OUTLINE;

  IHaveOverrideLook = false;
}


void FdFEVisControl::setLineWidth(GroupPartType type, double newWidth)
{
  for (FdFEGroupPart* part : myLists[type])
    part->setLineWidth(newWidth);
}


void FdFEVisControl::setGroupPartList(GroupPartType type, const GroupPartVec& gpl)
{
  myLists[type] = gpl;
}


void FdFEVisControl::addGroupPart(GroupPartType type, FdFEGroupPart* gp)
{
  myLists[type].push_back(gp);

  if (myReffedGroupPartTypes.count(type) > 0)
    gp->addRenderRef();

  if (myHighlightOverriddenGPTypes.count(type) > 0)
    gp->pushHighlightOverrideLook(FFdLook(myHighlightColor));

  if (IAmShowingResults && IAmShowingColorResults)
    return;
  else if (myContrastOverriddenGPTypes.count(type) == 0)
    return;

  FFdLook meshLook, specLinesLook, backgroundLook;
  this->getLooks(meshLook, specLinesLook, backgroundLook);

  // Find out what face types are shown :

  if (myDrawStyle == HIDDEN_LINES)
    gp->pushContrastOverrideLook(FFdLook());
  else switch (type) {
  case FdFEGroupPartSet::RED_INTERNAL_LINES:
  case FdFEGroupPartSet::INTERNAL_LINES:
    if (myReffedGroupPartTypes.count(FdFEGroupPartSet::RED_INTERNAL_FACES) > 0 ||
        myReffedGroupPartTypes.count(FdFEGroupPartSet::INTERNAL_FACES) > 0)
      gp->pushContrastOverrideLook(meshLook);
    else
      gp->pushContrastOverrideLook(FFdLook());
    break;

  default:
    if (myReffedGroupPartTypes.count(FdFEGroupPartSet::RED_SURFACE_FACES) > 0 ||
        myReffedGroupPartTypes.count(FdFEGroupPartSet::SURFACE_FACES) > 0)
      gp->pushContrastOverrideLook(specLinesLook);
    else
      gp->pushContrastOverrideLook(FFdLook());
    break;
  }
}


void FdFEVisControl::clearGroupParts()
{
  for (GroupPartVec& gpList : myLists)
    gpList.clear();

  myReffedGroupPartTypes.clear();
  myHighlightedGroupPartTypes.clear();
  myHighlightOverriddenGPTypes.clear();
  myContrastOverriddenGPTypes.clear();
}


void FdFEVisControl::highlight(bool isToHighlight, DetailType type,
                               const FdColor& color)
{
  myHighlightType = type;
  IAmHighlighted = isToHighlight;
  myHighlightColor = color;
  this->update();
}


void FdFEVisControl::setLook(const FFdLook& aLook)
{
  for (const GroupPartVec& gpList : myLists)
    for (FdFEGroupPart* gp : gpList)
      gp->updateOverallLook(this,aLook);

  myDiffuseColor = aLook.diffuseColor;
  this->updateContrast();
}


void FdFEVisControl::setOverrideLook(const FFdLook& aLook)
{
  for (const GroupPartVec& gpList : myLists)
    for (FdFEGroupPart* gp : gpList) {
      if (IHaveOverrideLook)
        gp->popOverallOverrideLook();
      gp->pushOverallOverrideLook(aLook);
    }

  IHaveOverrideLook = true;
}

void FdFEVisControl::unsetOverrideLook()
{
  if (IHaveOverrideLook)
    for (const GroupPartVec& gpList : myLists)
      for (FdFEGroupPart* gp : gpList)
        gp->popOverallOverrideLook();

  IHaveOverrideLook = false;
}


void FdFEVisControl::showResults(bool doShow)
{
  for (const GroupPartVec& gpList : myLists)
    for (FdFEGroupPart* gp : gpList)
      gp->showResults(doShow);

  IAmShowingResults = doShow;
  this->update();
}


void FdFEVisControl::showColorResults(bool doShow)
{
  for (size_t i = 0; i < myLists.size(); i++)
    switch (i) {
    case FdFEGroupPartSet::OUTLINE_LINES:
    case FdFEGroupPartSet::SPECIAL_LINES:
    case FdFEGroupPartSet::SURFACE_LINES:
    case FdFEGroupPartSet::INTERNAL_LINES:
    case FdFEGroupPartSet::SURFACE_FACES:
    case FdFEGroupPartSet::INTERNAL_FACES:
      for (FdFEGroupPart* gp : myLists[i])
        gp->showResultLook(doShow);
    default:
      break;
    }

  IAmShowingColorResults = doShow;
  this->update();
}


/*!
  Sync. visualization with internal state.
*/

void FdFEVisControl::update()
{
  this->updateGroupPartsShown();
  this->updateContrast();
}


void FdFEVisControl::getLooks(FFdLook& meshLook, FFdLook& specLinesLook, FFdLook& backgroundLook)
{
  enum ContrastStat {DARK, MED, LIGHT};

  // Lambda function returning DARK if black lines will not have proper contrast,
  // returns LIGHT if white lines will not have proper contrast, and MED if both will work
  auto&& getColorContrastStatus = [](const FdColor& color) -> ContrastStat
  {
    if (color[0] < 0.4f && color[1] < 0.35f && color[2] < 0.55f)
      return DARK; // black lines will be too dark

    // If white lines work
    if ((1.0f-color[0])/0.75f + (1.0f-color[1])/0.37f + (1.0f-color[2]) < 1.0f)
      return LIGHT; // white lines will be too light

    return MED;
  };

  ContrastStat colStatus = getColorContrastStatus(myDiffuseColor);
  ContrastStat bgStatus  = getColorContrastStatus(ourBackgroundColor);

  FdColor specialLinesColor = {0,0,0};
  if ((bgStatus == DARK || colStatus == DARK) && bgStatus != LIGHT)
    specialLinesColor = {1,1,1};

  FdColor lineColor = {0,0,0};
  if (colStatus == DARK)
    lineColor = {1,1,1};

  meshLook.diffuseColor = lineColor;
  meshLook.shininess    = 1;
  meshLook.specularColor= lineColor;
  meshLook.ambientColor = lineColor;

  specLinesLook.diffuseColor = specialLinesColor;
  specLinesLook.shininess    = 1;
  specLinesLook.specularColor= specialLinesColor;
  specLinesLook.ambientColor = specialLinesColor;

  backgroundLook.diffuseColor = ourBackgroundColor;
  backgroundLook.shininess    = 1;
  backgroundLook.specularColor= ourBackgroundColor;
  backgroundLook.ambientColor = ourBackgroundColor;
}


/*!
  This is an internal method to make sure the lines of the FE
  model has a proper contrast against the faces if present.
  Taking into account hidden line style if applyed
*/

void FdFEVisControl::updateContrast()
{
  FFdLook meshLook, specLinesLook, backgroundLook;
  this->getLooks(meshLook, specLinesLook, backgroundLook);

  std::set<GroupPartType> faceTypes;
  faceTypes.insert(FdFEGroupPartSet::RED_SURFACE_FACES);
  faceTypes.insert(FdFEGroupPartSet::SURFACE_FACES);
  faceTypes.insert(FdFEGroupPartSet::RED_INTERNAL_FACES);
  faceTypes.insert(FdFEGroupPartSet::INTERNAL_FACES);
  faceTypes.insert(FdFEGroupPartSet::BBOX_FACES);

  std::set<GroupPartType> lineTypes;
  lineTypes.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
  lineTypes.insert(FdFEGroupPartSet::OUTLINE_LINES);
  lineTypes.insert(FdFEGroupPartSet::RED_SURFACE_LINES);
  lineTypes.insert(FdFEGroupPartSet::SURFACE_LINES);
  lineTypes.insert(FdFEGroupPartSet::RED_INTERNAL_LINES);
  lineTypes.insert(FdFEGroupPartSet::INTERNAL_LINES);
  lineTypes.insert(FdFEGroupPartSet::BBOX_LINES);
  lineTypes.insert(FdFEGroupPartSet::SPECIAL_LINES);
  lineTypes.insert(FdFEGroupPartSet::RED_SPECIAL_LINES);

  // Get a set of all line grouppart types that is turned on.

  std::set<GroupPartType> linesOn;
  std::set_intersection(myReffedGroupPartTypes.begin(),
			myReffedGroupPartTypes.end(),
			lineTypes.begin(),
			lineTypes.end(),
			std::inserter(linesOn,linesOn.begin()));

  // Get a set of all face grouppart types that is turned on.

  std::set<GroupPartType> facesOn;
  std::set_intersection(myReffedGroupPartTypes.begin(),
			myReffedGroupPartTypes.end(),
			faceTypes.begin(),
			faceTypes.end(),
			std::inserter(facesOn,facesOn.begin()));

  // Find out what facetypes is shown :

  bool isSurfaceFacesOn =
    myReffedGroupPartTypes.count(FdFEGroupPartSet::RED_SURFACE_FACES)
    || myReffedGroupPartTypes.count(FdFEGroupPartSet::SURFACE_FACES);

  bool isInternalFacesOn =
    myReffedGroupPartTypes.count(FdFEGroupPartSet::RED_INTERNAL_FACES)
    || myReffedGroupPartTypes.count(FdFEGroupPartSet::INTERNAL_FACES);

  bool isBBoxFacesOn =
    myReffedGroupPartTypes.count(FdFEGroupPartSet::BBOX_FACES) > 0;

  // Loop over the actual line groupParts shown,
  // make them dark if they have faces among them

  if (!(IAmShowingResults && IAmShowingColorResults))
    for (GroupPartType gpType : linesOn)
      switch (gpType) {
      case FdFEGroupPartSet::RED_INTERNAL_LINES:
      case FdFEGroupPartSet::INTERNAL_LINES:
        if (isInternalFacesOn && myDrawStyle != HIDDEN_LINES)
          this->pushContrastGPTOverrideLook(meshLook,gpType);
        else
          this->popContrastGPTOverrideLook(gpType);
        break;

      case FdFEGroupPartSet::BBOX_LINES:
        if (isBBoxFacesOn && myDrawStyle != HIDDEN_LINES)
          this->pushContrastGPTOverrideLook(meshLook,gpType);
        else
          this->popContrastGPTOverrideLook(gpType);
        break;

      case FdFEGroupPartSet::SPECIAL_LINES:
        if (isSurfaceFacesOn && myDrawStyle != HIDDEN_LINES)
          this->pushContrastGPTOverrideLook(specLinesLook,gpType);
        else
          this->popContrastGPTOverrideLook(gpType);
        break;

      default:
        if (isSurfaceFacesOn && myDrawStyle != HIDDEN_LINES)
          this->pushContrastGPTOverrideLook(meshLook,gpType);
        else
          this->popContrastGPTOverrideLook(gpType);
      }

  // Loop over the actual face groupParts shown,
  // make them background like if shiowing as hidden line

  if (!(IAmShowingResults && IAmShowingColorResults))
  {
    for (GroupPartType gpType : facesOn)
      if (myDrawStyle == HIDDEN_LINES)
        this->pushContrastGPTOverrideLook(backgroundLook,gpType);
      else
        this->popContrastGPTOverrideLook(gpType);
  }

  // Highlight :

  std::set<GroupPartType> shouldHL;
  if (IAmHighlighted)
    getWhatShouldBeHighlighted(shouldHL,myHighlightType,
                               IAmShowingVertexResults,IAmShowingResults);

  // Get group part types that must be turned on

  std::set<GroupPartType> toTurnOn;
  std::set_difference(shouldHL.begin(), shouldHL.end(),
		      myHighlightOverriddenGPTypes.begin(), myHighlightOverriddenGPTypes.end(),
		      std::inserter(toTurnOn,toTurnOn.begin()));

  // Get group part types that is on but should not be

  std::set<GroupPartType> toTurnOff;
  std::set_difference(myHighlightOverriddenGPTypes.begin(),myHighlightOverriddenGPTypes.end(),
		      shouldHL.begin(), shouldHL.end(),
		      std::inserter(toTurnOff,toTurnOff.begin()) );

  // Do the actual on off switching :

  FFdLook highlightLook(myHighlightColor);
  for (GroupPartType gpType : toTurnOn)
    this->pushHighlightGPTOverrideLook(highlightLook,gpType);

  for (GroupPartType gpType : toTurnOff)
    this->popHighlightGPTOverrideLook(gpType);
}


void FdFEVisControl::pushContrastGPTOverrideLook(const FFdLook& aLook, GroupPartType type)
{
  bool hasPushed = myContrastOverriddenGPTypes.count(type) > 0;
  for (FdFEGroupPart* groupPart : myLists[type])
  {
    if (hasPushed)
      groupPart->popContrastOverrideLook();
    groupPart->pushContrastOverrideLook(aLook);
    myContrastOverriddenGPTypes.insert(type);
  }
}

void FdFEVisControl::popContrastGPTOverrideLook(GroupPartType type)
{
  if (myContrastOverriddenGPTypes.count(type))
    for (FdFEGroupPart* groupPart : myLists[type])
    {
      groupPart->popContrastOverrideLook();
      myContrastOverriddenGPTypes.erase(type);
    }
}


void FdFEVisControl::pushHighlightGPTOverrideLook(const FFdLook& aLook, GroupPartType type)
{
  if (!myHighlightOverriddenGPTypes.count(type))
    for (FdFEGroupPart* groupPart : myLists[type])
    {
      groupPart->pushHighlightOverrideLook(aLook);
      myHighlightOverriddenGPTypes.insert(type);
    }
}

void FdFEVisControl::popHighlightGPTOverrideLook(GroupPartType type)
{
  if (myHighlightOverriddenGPTypes.count(type))
    for (FdFEGroupPart* groupPart : myLists[type])
    {
      groupPart->popHighlightOverrideLook();
      myHighlightOverriddenGPTypes.erase(type);
    }
}


void FdFEVisControl::updateGroupPartsShown()
{
  std::set<GroupPartType> shouldBeOn, toTurnOff, toTurnOn;
  if (!IAmHidden)
  {
    getWhatShouldBeOn(shouldBeOn,myDetailLevel,myLineDetailLevel,myDrawStyle,
                      IAmShowingResults,IAmShowingColorResults,
                      IAmShowingVertexResults);

    if (IAmHighlighted)
      getWhatShouldBeHighlighted(shouldBeOn,myHighlightType,
                                 IAmShowingVertexResults,IAmShowingResults);
  }

  // Get group part types that should be on, that is not.

  std::set_difference(shouldBeOn.begin()            , shouldBeOn.end(),
		      myReffedGroupPartTypes.begin(), myReffedGroupPartTypes.end(),
		      std::inserter(toTurnOn,toTurnOn.begin()));

  // Get group part types that is on but should not be

  std::set_difference(myReffedGroupPartTypes.begin(), myReffedGroupPartTypes.end(),
		      shouldBeOn.begin()            , shouldBeOn.end(),
		      std::inserter(toTurnOff,toTurnOff.begin()));

  myReffedGroupPartTypes = shouldBeOn;

  // Turning off:
  for (GroupPartType grpType : toTurnOff)
    for (FdFEGroupPart* grpPart : myLists[grpType])
      grpPart->removeRenderRef();

  // Turning on:
  for (GroupPartType grpType : toTurnOn)
    for (FdFEGroupPart* grpPart : myLists[grpType])
      grpPart->addRenderRef();

#ifdef FD_DEBUG
  static int nWrites = 0;
  std::cout <<"\n#"<< ++nWrites <<"\tShould be ON:";
  for (GroupPartType gpt : shouldBeOn) std::cout <<" "<< gpt;
  std::cout <<"\n\t\tTurned ON  :";
  for (GroupPartType gpt : toTurnOn) std::cout <<" "<< gpt;
  std::cout <<"\n\t\tTurned OFF :";
  for (GroupPartType gpt : toTurnOff) std::cout <<" "<< gpt;
  std::cout << std::endl;
#endif
}


void FdFEVisControl::getWhatShouldBeOn(std::set<GroupPartType>& result,
                                       DetailType detailLevel, DetailType lineDetailLevel,
                                       DrawStyleType drawStyle,
                                       bool isResultsVisible, bool isColorResultsVisible,
                                       bool isVxResultsVisible)
{
  if (detailLevel == BBOX)
    switch (drawStyle) // Bounding box
      {
      case HIDDEN_LINES:
      case SOLID_LINES:
        result.insert(FdFEGroupPartSet::BBOX_LINES);
      case SOLID:
        result.insert(FdFEGroupPartSet::BBOX_FACES);
        break;
      case LINES:
        result.insert(FdFEGroupPartSet::BBOX_LINES);
        break;
      default:
        break;
      }

  else if (isResultsVisible && (isVxResultsVisible || isColorResultsVisible))
  {
    switch (detailLevel) // Controls faces, Results is on
      {
      case RED_FULL:
      case FULL:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case SOLID:
            result.insert(FdFEGroupPartSet::SURFACE_FACES);
            result.insert(FdFEGroupPartSet::INTERNAL_FACES);
            break;
          default:
            break;
          }
        break;

      case SURFACE:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case SOLID:
            result.insert(FdFEGroupPartSet::SURFACE_FACES);
            break;
          default:
            break;
          }
        break;

      case OUTLINE:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case SOLID:
            result.insert(FdFEGroupPartSet::SURFACE_FACES);
            break;
          default:
            break;
          }

      default:
        break;
      }

    switch (lineDetailLevel) // Controls lines, Results is on
      {
      case RED_FULL:
      case FULL:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case LINES:
            result.insert(FdFEGroupPartSet::OUTLINE_LINES);
            result.insert(FdFEGroupPartSet::SURFACE_LINES);
            result.insert(FdFEGroupPartSet::INTERNAL_LINES);
            result.insert(FdFEGroupPartSet::SPECIAL_LINES);
            break;
          default:
            break;
          }
        break;

      case SURFACE:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case LINES:
            result.insert(FdFEGroupPartSet::OUTLINE_LINES);
            result.insert(FdFEGroupPartSet::SURFACE_LINES);
            result.insert(FdFEGroupPartSet::SPECIAL_LINES);
            break;
          default:
            break;
          }
        break;

      case OUTLINE:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case LINES:
            result.insert(FdFEGroupPartSet::OUTLINE_LINES);
            result.insert(FdFEGroupPartSet::SPECIAL_LINES);
            break;
          default:
            break;
          }
        break;

      case OUTLINE_NO1D:
        switch (drawStyle)
          {
          case HIDDEN_LINES:
          case SOLID_LINES:
          case LINES:
            result.insert(FdFEGroupPartSet::OUTLINE_LINES);
            break;
          default:
            break;
          }

      default:
        break;
      }
    }

    else // Results is off
    {
      switch (detailLevel) // Controls faces, Results is off
        {
        case FULL:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case SOLID:
              result.insert(FdFEGroupPartSet::SURFACE_FACES);
              result.insert(FdFEGroupPartSet::INTERNAL_FACES);
              break;
            default:
              break;
            }
          break;

        case RED_FULL:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case SOLID:
              result.insert(FdFEGroupPartSet::RED_SURFACE_FACES);
              result.insert(FdFEGroupPartSet::RED_INTERNAL_FACES);
              break;
            default:
              break;
            }
          break;

        case SURFACE:
          switch (drawStyle)
            {
            case SOLID_LINES:
            case HIDDEN_LINES:
            case SOLID:
              result.insert(FdFEGroupPartSet::SURFACE_FACES);
              break;
            default:
              break;
            }
          break;

        case OUTLINE:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case SOLID:
              result.insert(FdFEGroupPartSet::RED_SURFACE_FACES);
              break;
            default:
              break;
            }

        default:
          break;
        }

      switch (lineDetailLevel) // Controls lines, Results is off
        {
        case RED_FULL:
        case FULL:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case LINES:
              result.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
              result.insert(FdFEGroupPartSet::RED_SURFACE_LINES);
              result.insert(FdFEGroupPartSet::RED_INTERNAL_LINES);
              result.insert(FdFEGroupPartSet::SPECIAL_LINES);
              break;
            default:
              break;
            }
          break;

        case SURFACE:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case LINES:
              result.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
              result.insert(FdFEGroupPartSet::RED_SURFACE_LINES);
              result.insert(FdFEGroupPartSet::SPECIAL_LINES);
              break;
            default:
              break;
            }
          break;

        case OUTLINE:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case LINES:
              result.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
              result.insert(FdFEGroupPartSet::SPECIAL_LINES);
              break;
            default:
              break;
            }
          break;

        case OUTLINE_NO1D:
          switch (drawStyle)
            {
            case HIDDEN_LINES:
            case SOLID_LINES:
            case LINES:
              result.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
              break;
            default:
              break;
            }

        default:
          break;
        }
    }
}


void FdFEVisControl::getWhatShouldBeHighlighted(std::set<GroupPartType>& result,
                                                DetailType highlightType,
                                                bool isVxResultsVisible,
                                                bool isResultsVisible)
{
  if (highlightType == BBOX)
    result.insert(FdFEGroupPartSet::BBOX_LINES);

  else if (isResultsVisible && isVxResultsVisible)
    switch (highlightType)
      {
      case RED_FULL:
      case FULL:
      case SURFACE:
        result.insert(FdFEGroupPartSet::SURFACE_LINES);
      case OUTLINE:
        result.insert(FdFEGroupPartSet::OUTLINE_LINES);
        result.insert(FdFEGroupPartSet::SPECIAL_LINES);
        break;
      case OUTLINE_NO1D:
        result.insert(FdFEGroupPartSet::OUTLINE_LINES);
        break;
      default:
        break;
      }

  else
    switch (highlightType)
      {
      case RED_FULL:
      case FULL:
      case SURFACE:
        result.insert(FdFEGroupPartSet::RED_SURFACE_LINES);
      case OUTLINE:
        result.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
        result.insert(FdFEGroupPartSet::SPECIAL_LINES);
        break;
      case OUTLINE_NO1D:
        result.insert(FdFEGroupPartSet::RED_OUTLINE_LINES);
        break;
      default:
        break;
      }
}
