// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdSelector.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdLink.H"
#include "vpmDisplay/FdSimpleJoint.H"
#include "vpmDisplay/FdLinJoint.H"
#include "vpmDisplay/FdCamJoint.H"
#include "vpmDisplay/FaDOF.H"

#include "vpmDB/FmSticker.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmCamJoint.H"

#include "vpmApp/FapEventManager.H"


namespace
{
  void selectMasterTriadsInJoint(FmJointBase* joint, bool deSelect = false)
  {
    std::vector<FmTriad*> mtriads;
    joint->getMasterTriads(mtriads);
    for (FmTriad* master : mtriads)
      if (FapEventManager::isPermSelected(master) == deSelect)
      {
        if (deSelect)
          FapEventManager::permUnselect(master);
        else
          FapEventManager::permSelect(master);
      }
  }


  void expandSelectLink(FmLink* link)
  {
    if (!link) return;

    FapEventManager::permSelect(link);

    std::vector<FmTriad*> triads;
    link->getTriads(triads);

    for (FmTriad* triad : triads)
    {
      if (!FapEventManager::isPermSelected(triad))
        FapEventManager::permSelect(triad);

      std::vector<FmJointBase*> joints;
      triad->getJointBinding(joints);
      for (FmJointBase* joint : joints)
        if (!joint->isAttachedToLink(link))
        {
          FapEventManager::permSelect(joint);
          if (!FapEventManager::isPermSelected(joint->getSlaveTriad()))
            FapEventManager::permSelect(joint->getSlaveTriad());
          selectMasterTriadsInJoint(joint);
        }
        else if (!FapEventManager::isPermSelected(joint))
        {
          if (triad->isMasterTriad())
          {
            if (FapEventManager::isPermSelected(joint->getSlaveTriad()))
              FapEventManager::permSelect(joint);
          }
          else // our triad is a slave
          {
            std::vector<FmTriad*> mtriads;
            joint->getMasterTriads(mtriads);
            if (!mtriads.empty())
              if (FapEventManager::isPermSelected(mtriads.front()))
                FapEventManager::permSelect(joint);
          }
        }
    }
  }


  void expandDeselectLink(FmLink* link)
  {
    if (!link) return;

    FapEventManager::permUnselect(link);

    std::vector<FmTriad*> triads;
    link->getTriads(triads);

    for (FmTriad* triad : triads)
    {
      if (FapEventManager::isPermSelected(triad))
        FapEventManager::permUnselect(triad);

      std::vector<FmJointBase*> joints;
      triad->getJointBinding(joints);
      for (FmJointBase* joint : joints)
        if (joint->isAttachedToLink(link))
        {
          if (FapEventManager::isPermSelected(joint))
            FapEventManager::permUnselect(joint);
        }
        else
        {
          FapEventManager::permUnselect(joint);
          if (FapEventManager::isPermSelected(joint->getSlaveTriad()))
            FapEventManager::permUnselect(joint->getSlaveTriad());
          selectMasterTriadsInJoint(joint,true);
        }
    }
  }
}


void FdSelector::getSelectedObjects(std::vector<FdObject*>& toBeFilled)
{
  toBeFilled.clear();
  std::vector<FmModelMemberBase*> fmSelection = FapEventManager::getPermMMBSelection();
  for (FmModelMemberBase* selection : fmSelection)
    if (selection && selection->isOfType(FmIsRenderedBase::getClassTypeID()))
      toBeFilled.push_back(((FmIsRenderedBase*)selection)->getFdPointer());
}


void FdSelector::selectExpanded(FdObject* object)
{
  if (!object)
    return;

  FmIsRenderedBase* fmobj = object->getFmOwner();
  if (FapEventManager::isPermSelected(fmobj))
    return;

  long selectionIndex = FapEventManager::getNumPermSelected();

  if (fmobj->isOfType(FmLink::getClassTypeID()))
    expandSelectLink(static_cast<FmLink*>(fmobj));

  else if (fmobj->isOfType(FmTriad::getClassTypeID()))
  {
    FapEventManager::permSelect(fmobj);
    FmTriad* triad = static_cast<FmTriad*>(fmobj);
    if (triad->isAttached())
      expandSelectLink(triad->getOwnerLink(0));
    else // Triad is not attached to a link:
    {
      std::vector<FmJointBase*> joints;
      triad->getJointBinding(joints);
      for (FmJointBase* joint : joints)
        if (!joint->isOfType(FmFreeJoint::getClassTypeID()) &&
            joint->isOfType(FmSMJointBase::getClassTypeID()))
        {
          if (joint->isAttachedToLink()) // If the part of the joint not clicked on is attached
            // Add that link to the selection
            // That means that the none attached part of the joint is rigidly
            // moved with the link the joint is attached to
            expandSelectLink(joint->getOtherLink(triad));
          else // the Joint is not attached to any links at all
          {
            // Select the whole joint
            // That means that the triad that vas hit is moved rigidly with
            // the rest of the joint
            FapEventManager::permSelect(joint);
            if (!FapEventManager::isPermSelected(joint->getSlaveTriad()))
              FapEventManager::permSelect(joint->getSlaveTriad());
            selectMasterTriadsInJoint(joint);
          }
        }
    }
  }
  else if (fmobj->isOfType(FmJointBase::getClassTypeID()))
  {
    FapEventManager::permSelect(fmobj);
    FmJointBase* joint = static_cast<FmJointBase*>(fmobj);
    if (!fmobj->isOfType(FmCamJoint::getClassTypeID()))
    {
      if (joint->isSlaveAttachedToLink())
        expandSelectLink(joint->getSlaveLink());
      else
        FapEventManager::permSelect(joint->getSlaveTriad());
    }
    if (joint->isMasterAttachedToLink())
      expandSelectLink(joint->getMasterLink());
    else
      selectMasterTriadsInJoint(joint);
  }

  FapEventManager::permUnselect(selectionIndex);
  FapEventManager::permSelect(fmobj);
}


void FdSelector::deselectExpandedLast()
{
  if (!FapEventManager::getNumPermSelected())
    return;

  FmModelMemberBase* fmobj = dynamic_cast<FmModelMemberBase*>(FapEventManager::getLastPermSelectedObject());
  if (!fmobj)
    return;

  if (!FapEventManager::isPermSelected(fmobj))
    return;

  if (fmobj->isOfType(FmLink::getClassTypeID()))
    expandDeselectLink(static_cast<FmLink*>(fmobj));

  else if (fmobj->isOfType(FmTriad::getClassTypeID()))
  {
    FapEventManager::permUnselect(fmobj);
    FmTriad* triad = static_cast<FmTriad*>(fmobj);
    if (triad->isAttached())
      expandDeselectLink(triad->getOwnerLink(0));
    else // Triad is not attached to a link
    {
      std::vector<FmJointBase*> joints;
      triad->getJointBinding(joints);
      for (FmJointBase* joint : joints)
        if (!joint->isOfType(FmFreeJoint::getClassTypeID()) &&
            joint->isOfType(FmSMJointBase::getClassTypeID()))
        {
          if (joint->isAttachedToLink())
            expandDeselectLink(joint->getOtherLink(triad));
          else
          {
            FapEventManager::permUnselect(joint);
            if (FapEventManager::isPermSelected(joint->getSlaveTriad()))
              FapEventManager::permUnselect(joint->getSlaveTriad());
            selectMasterTriadsInJoint(joint,true);
          }
        }
    }
  }
  else if (fmobj->isOfType(FmJointBase::getClassTypeID()))
  {
    FapEventManager::permUnselect(fmobj);
    FmJointBase* joint = static_cast<FmJointBase*>(fmobj);
    if (!fmobj->isOfType(FmCamJoint::getClassTypeID()))
    {
      if (joint->isSlaveAttachedToLink())
        expandDeselectLink(joint->getSlaveLink());
      else
        FapEventManager::permUnselect(joint->getSlaveTriad());
    }
    if (joint->isMasterAttachedToLink())
      expandDeselectLink(joint->getMasterLink());
    else
      selectMasterTriadsInJoint(joint,true);
  }
}


void FdSelector::smartMoveSelection(const FaVec3& firstPoint,
                                    const FaVec3& secondPoint,
                                    const FaDOF& dof)
{
  std::vector<FdObject*> selectedObjects;
  FdSelector::getSelectedObjects(selectedObjects);
  for (FdObject* obj : selectedObjects)
    if (obj->isOfType(FdLink::getClassTypeID()))
      obj->smartMove(firstPoint,secondPoint,dof);

    else if (obj->isOfType(FdSimpleJoint::getClassTypeID()))
    {
      if (!((FmJointBase*)(obj->getFmOwner()))->isAttachedToLink())
        obj->smartMove(firstPoint,secondPoint,dof);
    }

    else if (obj->isOfType(FdLinJoint::getClassTypeID()))
    {
      if (!((FmJointBase*)(obj->getFmOwner()))->isAttachedToLink())// ToDo: If LinJoint : move nomatterwhat ?
        obj->smartMove(firstPoint,secondPoint,dof);
    }

    else if (obj->isOfType(FdCamJoint::getClassTypeID()))
    {
      if (!((FmJointBase*)(obj->getFmOwner()))->isMasterAttachedToLink(true))
        obj->smartMove(firstPoint,secondPoint,dof);
    }

    else if (obj->isOfType(FdTriad::getClassTypeID()))
    {
      if (!((FmTriad*)(obj->getFmOwner()))->isAttached())
      {
        std::vector<FmJointBase*> joints;
        ((FmTriad*)(obj->getFmOwner()))->getJointBinding(joints);

        bool notInNonFreeJoints = true;
        for (FmJointBase* joint : joints)
          if (!joint->isOfType(FmFreeJoint::getClassTypeID()) &&
              !joint->isOfType(FmMMJointBase::getClassTypeID()))
            notInNonFreeJoints = false;
          else if (FapEventManager::isPermSelected(joint))
            notInNonFreeJoints = false;

        if (notInNonFreeJoints)
          obj->smartMove(firstPoint,secondPoint,dof);
      }
    }
}


FaDOF FdSelector::getDegOfFreedom()
{
#ifdef FD_DEBUG
  std::cout <<"\nFdSelector::getDegOfFreedom()"<< std::endl;
#endif
  FaDOF groupDOF;
  const double smTol = 1.0e-7;

  std::vector<FdObject*> selection;
  FdSelector::getSelectedObjects(selection);

  for (FdObject* dobj : selection)
  {
    FmIsRenderedBase* obj = dobj->getFmOwner();
#ifdef FD_DEBUG
    std::cout <<"  "<< obj->getIdString(true);
#endif
    if (obj->isOfType(FmIsPositionedBase::getClassTypeID()))
    {
      std::vector<FmSticker*> stickers;
      ((FmIsPositionedBase*)obj)->getStickers(stickers);
#ifdef FD_DEBUG
      std::cout <<" has "<< stickers.size() <<" stickers"<< std::endl;
#endif
      for (FmSticker* sticker : stickers)
        groupDOF.compound(sticker->getObjDegOfFreedom(),smTol);
    }
#ifdef FD_DEBUG
    else std::cout << std::endl;
#endif

    if (obj->isOfType(FmSticker::getClassTypeID()))
      groupDOF.compound(obj->getObjDegOfFreedom(),smTol);

    else if (obj->isOfType(FmTriad::getClassTypeID()))
    {
      std::vector<FmJointBase*> joints;
      ((FmTriad*)obj)->getJointBinding(joints);
#ifdef FD_DEBUG
      std::cout <<"  "<< obj->getIdString() <<" has "<< joints.size() <<" joints"<< std::endl;
#endif
      for (FmJointBase* joint : joints)
        if (!FapEventManager::isPermSelected(joint))
          groupDOF.compound(joint->getObjDegOfFreedom(),smTol);
    }
  }

#ifdef FD_DEBUG
  std::cout <<"FdSelector::getDegOfFreedom: "<< groupDOF;
#endif
  return groupDOF;
}
