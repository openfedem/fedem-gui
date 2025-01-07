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

#include "vpmDB/FmSticker.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmFreeJoint.H"
#include "vpmDB/FmCamJoint.H"

#include "vpmApp/FapEventManager.H"


static void selectMasterTriadsInJoint(FmJointBase* joint, bool deSelect = false)
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


void FdSelector::getSelectedObjects(std::vector<FdObject*>& toBeFilled)
{
  toBeFilled.clear();
  std::vector<FmModelMemberBase*> fmSelection = FapEventManager::getPermMMBSelection();
  for (FmModelMemberBase* selection : fmSelection)
    if (selection && selection->isOfType(FmIsRenderedBase::getClassTypeID()))
      toBeFilled.push_back(((FmIsRenderedBase*)selection)->getFdPointer());
}


void FdSelector::expandSelectLink(FdLink* link)
{
  if (!link) return;

  FmLink* fmlink = (FmLink*)(link->getFmOwner());
  FapEventManager::permSelect(fmlink);

  std::vector<FmTriad*> triads;
  fmlink->getTriads(triads);

  for (FmTriad* triad : triads)
  {
    if (!FapEventManager::isPermSelected(triad))
      FapEventManager::permSelect(triad);

    std::vector<FmJointBase*> joints;
    triad->getJointBinding(joints);
    for (FmJointBase* joint : joints)
      if (joint->isAttachedToLink(fmlink))
      {
        if (triad->isMasterTriad())
        {
          if (FapEventManager::isPermSelected(joint->getSlaveTriad()))
            if (!FapEventManager::isPermSelected(joint))
              FapEventManager::permSelect(joint);
        }
        else // our triad is a slave
        {
          if (joint->isOfType(FmSMJointBase::getClassTypeID()))
          {
            if (FapEventManager::isPermSelected(((FmSMJointBase*)joint)->getItsMasterTriad()))
              if (!FapEventManager::isPermSelected(joint))
                FapEventManager::permSelect(joint);
          }
          else if (joint->isOfType(FmMMJointBase::getClassTypeID()))
          {
            std::vector<FmTriad*> mtriads;
            ((FmMMJointBase*)joint)->getMasterTriads(mtriads);
            if (!mtriads.empty())
              if (FapEventManager::isPermSelected(mtriads.front()))
                if (!FapEventManager::isPermSelected(joint))
                  FapEventManager::permSelect(joint);
          }
        }
      }
      else
      {
        FapEventManager::permSelect(joint);
        if (!FapEventManager::isPermSelected(joint->getSlaveTriad()))
          FapEventManager::permSelect(joint->getSlaveTriad());
        selectMasterTriadsInJoint(joint);
      }
  }
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
    FdSelector::expandSelectLink((FdLink*)object);

  else if (fmobj->isOfType(FmTriad::getClassTypeID()))
  {
    FapEventManager::permSelect(fmobj);
    if (((FmTriad*)fmobj)->isAttached())
      FdSelector::expandSelectLink((FdLink*)(((FmTriad*)fmobj)->getOwnerLink(0)->getFdPointer()));
    else // Triad is not attached to a link:
    {
      std::vector<FmJointBase*> joints;
      ((FmTriad*)fmobj)->getJointBinding(joints);
      for (FmJointBase* joint : joints)
        if (joint->isOfType(FmFreeJoint::getClassTypeID()) ||
            joint->isOfType(FmCamJoint::getClassTypeID()))
        {
          // Nothing more than the hit triad is selected
        }
        else if (joint->isOfType(FmMMJointBase::getClassTypeID()))
        {
          /*
          if (joint->isAttachedToLink()) // If the part of the joint not clicked on is attached
          {
            if (((FmTriad*)fmobj)->isMasterTriad())
              selectMasterTriadsInJoint(joint);
          }
          else // the Joint is not attached to any links at all
          {
            // Select the whole joint
            // That means that the triad that vas hit is moved with
            // the rest of the joint
            FapEventManager::permSelect(joint);
            if (!(FapEventManager::isPermSelected(joint->getSlaveTriad())))
            FapEventManager::permSelect(joint->getSlaveTriad());
            selectMasterTriadsInJoint(joint);
          }
          */
        }
        else if (joint->isOfType(FmSMJointBase::getClassTypeID())) // But Not Free Joint
        {
          if (joint->isAttachedToLink()) // If the part of the joint not clicked on is attached
          {
            // Add that link to the selection
            // That means that the none attached part of the joint is rigidly moved
            // with the link the joint is attached to
            FmLink* otherLink = joint->getOtherLink((FmTriad*)fmobj);
            if (otherLink)
              FdSelector::expandSelectLink((FdLink*)(otherLink->getFdPointer()));
          }
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
  else if (fmobj->isOfType(FmCamJoint::getClassTypeID()))
  {
    FapEventManager::permSelect(fmobj);

    if (((FmJointBase*)fmobj)->isMasterAttachedToLink())
      FdSelector::expandSelectLink((FdLink*)(((FmJointBase*)fmobj)->getMasterLink()->getFdPointer()));
    else
      selectMasterTriadsInJoint((FmJointBase*)fmobj);
  }
  else if (fmobj->isOfType(FmJointBase::getClassTypeID()))
  {
    FapEventManager::permSelect(fmobj);

    if (((FmJointBase*)fmobj)->isSlaveAttachedToLink())
      FdSelector::expandSelectLink((FdLink*)(((FmJointBase*)fmobj)->getSlaveLink()->getFdPointer()));
    else
      FapEventManager::permSelect(((FmJointBase*)fmobj)->getSlaveTriad());

    if (((FmJointBase*)fmobj)->isMasterAttachedToLink())
      FdSelector::expandSelectLink((FdLink*)(((FmJointBase*)fmobj)->getMasterLink()->getFdPointer()));
    else
      selectMasterTriadsInJoint((FmJointBase*)fmobj);
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
    FdSelector::expandDeselectLink((FdLink*)(((FmLink*)fmobj)->getFdPointer()));

  else if (fmobj->isOfType(FmTriad::getClassTypeID()))
  {
    FapEventManager::permUnselect(fmobj);
    if (((FmTriad*)fmobj)->isAttached())
      FdSelector::expandDeselectLink((FdLink*)(((FmTriad*)fmobj)->getOwnerLink(0)->getFdPointer()));
    else // Triad is not attached to a link
    {
      std::vector<FmJointBase*> joints;
      ((FmTriad*)fmobj)->getJointBinding(joints);
      for (FmJointBase* joint : joints)
        if (joint->isOfType(FmFreeJoint::getClassTypeID()) ||
            joint->isOfType(FmCamJoint::getClassTypeID()))
        {
          // Nothing more than the hit triad is deselected
        }
        else if (joint->isOfType(FmMMJointBase::getClassTypeID()))
        {
          /*
          if (joint->isAttachedToLink()) // If the part of the joint not clicked on is attached
          {
            if (((FmTriad*)fmobj)->isMasterTriad())
              selectMasterTriadsInJoint(joint,true);
          }
          else // the Joint is not attached to any links at all
          {
            // Select the whole joint
            // That means that the triad that vas hit is moved with
            // the rest of the joint
            FapEventManager::permUnselect(joint);
            if ((FapEventManager::isPermSelected(joint->getSlaveTriad())))
              FapEventManager::permUnselect(joint->getSlaveTriad());
            selectMasterTriadsInJoint(joint,true);
          }
          */
        }
        else if (joint->isOfType(FmSMJointBase::getClassTypeID())) // But Not Free Joint
        {
          if (joint->isAttachedToLink())
          {
            FmLink* otherLink = joint->getOtherLink((FmTriad*)fmobj);
            if (otherLink)
              FdSelector::expandDeselectLink((FdLink*)(otherLink->getFdPointer()));
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
  else if (fmobj->isOfType(FmCamJoint::getClassTypeID()))
  {
    FapEventManager::permUnselect(fmobj);
    if (((FmJointBase*)fmobj)->isMasterAttachedToLink())
      FdSelector::expandDeselectLink((FdLink*)(((FmJointBase*)fmobj)->getMasterLink()->getFdPointer()));
    else
      selectMasterTriadsInJoint((FmJointBase*)fmobj,true);
  }
  else if (fmobj->isOfType(FmJointBase::getClassTypeID()))
  {
    FapEventManager::permUnselect(fmobj);
    if (((FmJointBase*)fmobj)->isSlaveAttachedToLink())
      FdSelector::expandDeselectLink((FdLink*)(((FmJointBase*)fmobj)->getSlaveLink()->getFdPointer()));
    else
      FapEventManager::permUnselect(((FmJointBase*)fmobj)->getSlaveTriad());

    if (((FmJointBase*)fmobj)->isMasterAttachedToLink())
      FdSelector::expandDeselectLink((FdLink*)(((FmJointBase*)fmobj)->getMasterLink()->getFdPointer()));
    else
      selectMasterTriadsInJoint((FmJointBase*)fmobj,true);
  }
}


void FdSelector::expandDeselectLink(FdLink* link)
{
  if (!link) return;

  FmLink* fmlink = (FmLink*)link->getFmOwner();
  FapEventManager::permUnselect(fmlink);

  std::vector<FmTriad*> triads;
  ((FmLink*)fmlink)->getTriads(triads);
  for (FmTriad* triad : triads)
  {
    if ((FapEventManager::isPermSelected(triad)))
      FapEventManager::permUnselect(triad);

    std::vector<FmJointBase*> joints;
    triad->getJointBinding(joints);
    for (FmJointBase* joint : joints)
      if (joint->isAttachedToLink(fmlink))
      {
        if ((FapEventManager::isPermSelected(joint)))
          FapEventManager::permUnselect(joint);
      }
      else
      {
        FapEventManager::permUnselect(joint);
        if ((FapEventManager::isPermSelected(joint->getSlaveTriad())))
          FapEventManager::permUnselect(joint->getSlaveTriad());
        selectMasterTriadsInJoint(joint,true);
      }
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
      if (!((FmJointBase*)(obj->getFmOwner()))->isMasterAttachedToLink(true))// ToDo: If LinJoint : move nomatterwhat ?
        obj->smartMove(firstPoint,secondPoint,dof);
    }

    else if (obj->isOfType(FdTriad::getClassTypeID()))
    {
      if (!((FmTriad*)(obj->getFmOwner()))->isAttached())
      {
        std::vector<FmJointBase*> joints;
        ((FmTriad*)(obj->getFmOwner()))->getJointBinding(joints);

        for (FmJointBase* joint : joints)
          if (!joint->isOfType(FmFreeJoint::getClassTypeID()) &&
              !joint->isOfType(FmMMJointBase::getClassTypeID()))
            continue;
          else if (FapEventManager::isPermSelected(joint))
            continue;

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
