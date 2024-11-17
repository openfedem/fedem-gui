// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmPM/FpRDBExtractorManager.H"
#include "vpmPM/FpPM.H"
#include "vpmPM/FpExtractor.H"
#include "FFaLib/FFaOS/FFaFilePath.H"
#include "FFaLib/FFaCmdLineArg/FFaCmdLineArg.H"

#include "vpmDB/FmRingStart.H"
#include "vpmDB/FmSticker.H"
#include "vpmDB/FmRefPlane.H"
#include "vpmDB/FmRigidJoint.H"
#include "vpmDB/FmGear.H"
#include "vpmDB/FmRackPinion.H"
#include "vpmDB/FmSimpleSensor.H"
#include "vpmDB/FmRelativeSensor.H"
#include "vpmDB/FmRoad.H"
#include "vpmDB/FmFileReference.H"
#include "vpmDB/FmGenericDBObject.H"
#include "vpmDB/FmElementGroupProxy.H"
#ifdef FT_HAS_EXTCTRL
#include "vpmDB/FmExternalCtrlSys.H"
#endif
#include "vpmDB/FmSpringChar.H"
#include "vpmDB/FmEngine.H"
#include "vpmDB/FmMathFuncBase.H"
#include "vpmDB/FmFrictionBase.H"
#include "vpmDB/FmCtrlLine.H"
#include "vpmDB/Fm1DMaster.H"
#include "vpmDB/FmSimulationEvent.H"

//----------------------------------------------------------------------------

FpRDBExtractorManager::FpRDBExtractorManager()
  : FFaSwitchBoardConnector("FpRDBExtractorManager"), lvFilter("Solver Filter")
{
  modelExtr = posExtr = NULL;

  lvFilter.verifyItemCB = FFaDynCB2M(FpRDBExtractorManager,this,
				     verifySolverItem,FFaListViewItem*,bool&);
}
//----------------------------------------------------------------------------

FpRDBExtractorManager::~FpRDBExtractorManager()
{
  delete this->modelExtr;
  delete this->posExtr;
}
//----------------------------------------------------------------------------

void FpRDBExtractorManager::createModelExtractor()
{
  if (!this->modelExtr)
    this->modelExtr = new FpExtractor("model extractor");
}
//----------------------------------------------------------------------------


FFrExtractor* FpRDBExtractorManager::getModelExtractor()
{
  return this->modelExtr;
}
//----------------------------------------------------------------------------


FFrExtractor* FpRDBExtractorManager::getPossibilityExtractor()
{
  return this->posExtr;
}
//----------------------------------------------------------------------------

void FpRDBExtractorManager::clearExtractors()
{
  if (this->modelExtr)
  {
#if FP_DEBUG > 2
    std::cout <<"\nFFaSwitchBoardCall: delete model extractor"<< std::endl;
#endif
    FFaSwitchBoardCall(this,MODELEXTRACTOR_ABOUT_TO_DELETE,this->getModelExtractor());
    delete this->modelExtr;
    this->modelExtr = NULL;
#if FP_DEBUG > 2
    std::cout <<"\nFFaSwitchBoardCall: model extractor deleted"<< std::endl;
#endif
    FFaSwitchBoardCall(this,MODELEXTRACTOR_DELETED);
  }

  if (this->posExtr)
  {
#if FP_DEBUG > 2
    std::cout <<"\nFFaSwitchBoardCall: delete possibility extractor"<< std::endl;
#endif
    FFaSwitchBoardCall(this,POSEXTRACTOR_ABOUT_TO_DELETE,this->getPossibilityExtractor());
    delete this->posExtr;
    this->posExtr = NULL;
#if FP_DEBUG > 2
    std::cout <<"\nFFaSwitchBoardCall: possibility extractor deleted"<< std::endl;
#endif
    FFaSwitchBoardCall(this,POSEXTRACTOR_DELETED);
  }

  FFrExtractor::releaseMemoryBlocks();
}
//----------------------------------------------------------------------------

void FpRDBExtractorManager::renewExtractors(const std::set<std::string>& keep)
{
  this->clearExtractors();

  this->modelExtr = new FpExtractor("model extractor");
  this->modelExtr->setHeaderChangedCB(FFaDynCB1M(FpRDBExtractorManager,this,
						 onModelExtractorHeaderChanged,
						 const FFrExtractor*));
  this->modelExtr->setDataChangedCB(FFaDynCB1M(FpRDBExtractorManager,this,
					       onModelExtractorDataChanged,
					       const FFrExtractor*));

#if FP_DEBUG > 2
  std::cout <<"\nFFaSwitchBoardCall: new model extractor"<< std::endl;
#endif
  FFaSwitchBoardCall(this,NEW_MODELEXTRACTOR,this->getModelExtractor());

  this->modelExtr->addFiles(keep);

  this->posExtr = new FpExtractor("pos extractor");
  this->posExtr->setHeaderChangedCB(FFaDynCB1M(FpRDBExtractorManager,this,
					       onPosExtractorHeaderChanged,
					       const FFrExtractor*));
  this->posExtr->addFiles(this->getPredefPosFiles());
}
//----------------------------------------------------------------------------

bool FpRDBExtractorManager::hasResults(FmModelMemberBase* obj) const
{
  if (!modelExtr) return false;

  return modelExtr->getObjectGroup(obj->getBaseID()) != NULL;
}
//----------------------------------------------------------------------------


void FpRDBExtractorManager::onModelExtractorHeaderChanged(const FFrExtractor*)
{
  FpPM::setResultFlag();
#if FP_DEBUG > 2
  std::cout <<"\nFFaSwitchBoardCall: model extractor header change"<< std::endl;
#endif
  FFaSwitchBoardCall(this,MODELEXTRACTOR_HEADER_CHANGED,this->getModelExtractor());
}
//----------------------------------------------------------------------------

void FpRDBExtractorManager::onModelExtractorDataChanged(const FFrExtractor*)
{
#if FP_DEBUG > 2
  std::cout <<"\nFFaSwitchBoardCall: model extractor data change"<< std::endl;
#endif
  FFaSwitchBoardCall(this,MODELEXTRACTOR_DATA_CHANGED,this->getModelExtractor());
}
//----------------------------------------------------------------------------

void FpRDBExtractorManager::onPosExtractorHeaderChanged(const FFrExtractor*)
{
#if FP_DEBUG > 2
  std::cout <<"\nFFaSwitchBoardCall: possibility extractor header change"<< std::endl;
#endif
  FFaSwitchBoardCall(this,POSEXTRACTOR_HEADER_CHANGED,this->getPossibilityExtractor());
}
//----------------------------------------------------------------------------

std::vector<std::string> FpRDBExtractorManager::getPredefPosFiles()
{
  std::string response = FpPM::getFullFedemPath("resources/response_pos.frs",'e');
  std::string stress   = FpPM::getFullFedemPath("resources/stress_pos.frs",'e');

  if (!response.empty() && !stress.empty())
    return { response, stress };
  else if (!response.empty())
    return { response };
  else if (!stress.empty())
    return { stress };

  return std::vector<std::string>();
}
//----------------------------------------------------------------------------

void FpRDBExtractorManager::verifySolverItem(FFaListViewItem* item, bool& valid)
{
  valid = item != NULL;
  if (!valid) return;

  FmModelMemberBase* mmb = dynamic_cast<FmModelMemberBase*>(item);
  if (mmb)
  {
    FmRingStart* rs = dynamic_cast<FmRingStart*>(item);
    int classTypeID = rs ? rs->getRingMemberType() : mmb->getTypeID();
    if (classTypeID == FmSticker::getClassTypeID() ||
	classTypeID == FmRefPlane::getClassTypeID() ||
	classTypeID == FmRigidJoint::getClassTypeID() ||
	classTypeID == FmGear::getClassTypeID() ||
	classTypeID == FmRackPinion::getClassTypeID() ||
	classTypeID == FmSimpleSensor::getClassTypeID() ||
	classTypeID == FmRelativeSensor::getClassTypeID() ||
	classTypeID == FmRoad::getClassTypeID() ||
	classTypeID == FmFileReference::getClassTypeID() ||
	classTypeID == FmGenericDBObject::getClassTypeID() ||
	classTypeID == FmElementGroupProxy::getClassTypeID() ||
#ifdef FT_HAS_EXTCTRL
	classTypeID == FmExternalCtrlSys::getClassTypeID() ||
#endif
	classTypeID == FmSpringChar::getClassTypeID() ||
	classTypeID == FmEngine::getClassTypeID() ||
	classTypeID == FmSimulationEvent::getClassTypeID())
      valid = false;
    else if (!rs)
    {
      // Check the parent class type
      if (mmb->isOfType(FmParamObjectBase::getClassTypeID()) ||
	  mmb->isOfType(Fm1DMaster::getClassTypeID()) ||
	  mmb->isOfType(FmCtrlElementBase::getClassTypeID()))
	valid = false;
    }
    else if (rs->getParent())
    {
      // Check class type associated with the parent ring start object
      int parentTypeID = rs->getParent()->getRingMemberType();
      if (parentTypeID == FmMathFuncBase::getClassTypeID() ||
	  parentTypeID == Fm1DMaster::getClassTypeID() ||
	  parentTypeID == FmFrictionBase::getClassTypeID())
	valid = false;
      else if (parentTypeID == FmCtrlElementBase::getClassTypeID() &&
	       classTypeID  != FmCtrlLine::getClassTypeID())
	valid = false;
    }
    return;
  }

  FFrEntryBase* ffr = dynamic_cast<FFrEntryBase*>(item);
  if (!ffr)
    valid = false;
  else if (ffr->getDescription() == "Nodes")
    FFaCmdLineArg::instance()->getValue("plotNodes",valid);
  else if (ffr->getDescription() == "Elements")
    FFaCmdLineArg::instance()->getValue("plotElements",valid);
  else if (ffr->getDescription() == "Vectors")
    valid = false;
}
