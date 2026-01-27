// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmPM/FpExtractor.H"
#include "FFrLib/FFrResultContainer.H"
#include "FFrLib/FFrSuperObjectGroup.H"
#include "FFrLib/FFrObjectGroup.H"


FpExtractor::FpExtractor(const char* xName) : FFrExtractor(xName)
{
  emitHeaderChanged = emitDataChanged = false;
}


bool FpExtractor::addFiles(const std::vector<std::string>& fileNames,
                           bool showProgress, bool mustExist, bool doMemPoll)
{
  emitHeaderChanged = emitDataChanged = false;

  if (!this->FFrExtractor::addFiles(fileNames,showProgress,mustExist,doMemPoll))
    return false;

  if (emitHeaderChanged) myHeaderChangedCB.invoke(this);
  if (emitDataChanged)   myDataChangedCB.invoke(this);

  return true;
}


bool FpExtractor::removeFiles(const std::set<std::string>& fileNames)
{
  if (this->FFrExtractor::removeFiles(fileNames))
    myHeaderChangedCB.invoke(this);

  return true;
}


void FpExtractor::getTopLevelVars(std::vector<FFaListViewItem*>& tlvars) const
{
  tlvars.reserve(tlvars.size()+myTopLevelVars.size());
  for (const std::pair<const std::string,FFrEntryBase*>& var : myTopLevelVars)
    tlvars.push_back(var.second);
}


void FpExtractor::getSuperObjectGroups(std::vector<FFaListViewItem*>& sogs) const
{
  sogs.reserve(sogs.size()+myTopLevelSOGs.size());
  for (const std::pair<const std::string,FFrSuperObjectGroup*>& sog : myTopLevelSOGs)
    sogs.push_back(sog.second);
}


void FpExtractor::getSuperObjectGroups(std::vector<FFaListViewItem*>& sogs,
                                       const std::set<int>& usedBaseId) const
{
  for (const std::pair<const std::string,FFrSuperObjectGroup*>& sog : myTopLevelSOGs)
    for (FFrEntryBase* og : sog.second->dataFields)
      if (usedBaseId.find(og->getBaseID()) == usedBaseId.end())
      {
#ifdef FP_DEBUG
        std::cout <<"FpExtractor::getSuperObjectGroups(): baseID "
                  << og->getBaseID() <<" not among\n";
        for (int bid : usedBaseId) std::cout <<" "<< bid;
        std::cout << std::endl;
#endif
        sogs.push_back(sog.second);
        break;
      }
}


bool FpExtractor::getObjectGroupFields(int baseId, const std::string& name,
                                       std::vector<FFaListViewItem*>& fields) const
{
  FFrObjectGroup* ogr = this->getObjectGroup(baseId);
  if (!ogr)
  {
    // Object without results. This is not an error condition.
    // Silently ignore, unless debug build.
#ifdef FP_DEBUG
    std::cout <<"FpExtractor::getObjectGroupFields: No object ("<< name
              <<") with baseId="<< baseId << std::endl;
#endif
    return false;
  }
  else if (ogr->getType() != name)
  {
    std::cerr <<" *** FpExtractor::getObjectGroupFields: Mismatching type name "
              << ogr->getType() <<" != "<< name << std::endl;
    return false;
  }

  fields.reserve(fields.size()+ogr->dataFields.size());
  for (FFrEntryBase* field : ogr->dataFields)
    fields.push_back(field);

  return true;
}


void FpExtractor::doResultFilesUpdate(bool doMemPoll)
{
  emitHeaderChanged = false;
  emitDataChanged = false;

  this->FFrExtractor::doResultFilesUpdate(doMemPoll);

  if (emitHeaderChanged) myHeaderChangedCB.invoke(this);
  if (emitDataChanged)   myDataChangedCB.invoke(this);
}


int FpExtractor::doResultFileUpdate(FFrResultContainer* container,
                                    bool doMemPoll)
{
#if FP_DEBUG > 2
  std::cout <<"FpExtractor::doResultFileUpdate()\n\tfilename: "
            << container->getFileName() << std::endl;
#endif

  int statusBefore = container->getContainerStatus();
  if (statusBefore == FFrResultContainer::FFR_DATA_CLOSED)
    return statusBefore;

  bool wasComplete = container->isHeaderComplete();
  int status = container->updateStatus(doMemPoll);

  if (!wasComplete && container->isHeaderComplete())
  {
    this->updateExtractorHeader(container,doMemPoll);
    emitHeaderChanged = true;
  }

  if (status == FFrResultContainer::FFR_NEW_DATA ||
      status == FFrResultContainer::FFR_NEW_TEXT)
  {
    emitDataChanged = true;
    if (statusBefore < FFrResultContainer::FFR_TEXT_FILE)
      emitHeaderChanged = true;
  }

  return status;
}
