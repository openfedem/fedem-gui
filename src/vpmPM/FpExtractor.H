// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FP_EXTRACTOR_H
#define FP_EXTRACTOR_H

#include "FFrLib/FFrExtractor.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


/*!
  \brief Front-end for the result extraction module.

  \details This class extends the FFrExtractor class with added functionality
  needed for the result list view handling.
*/

class FpExtractor : public FFrExtractor
{
public:
  //! \brief Default constructor.
  FpExtractor(const char* xname = NULL);

  // Result file opened/closed signals:
  //! \brief Invoked on each addFiles() and removeFiles().
  void setHeaderChangedCB(const FFaDynCB1<const FFrExtractor*>& dynCB)
  { myHeaderChangedCB = dynCB; }
  //! \brief Invoked on new data read into some of the containers.
  void setDataChangedCB(const FFaDynCB1<const FFrExtractor*>& dynCB)
  { myDataChangedCB = dynCB; }

  using FFrExtractor::addFiles;
  //! \brief Adds a list of files to the RDB.
  virtual bool addFiles(const std::vector<std::string>& fileNames,
                        bool showProgress = false, bool mustExist = false,
                        bool doMemPoll = false);
  //! \brief Removes the specified files from the RDB.
  virtual bool removeFiles(const std::set<std::string>& fileNames);

  //! \brief Checks if there is new data on disk.
  virtual void doResultFilesUpdate(bool doMemPoll = true);

  //! \brief Returns a hierarchy of top-level objects sorted by object type.
  void getSuperObjectGroups(std::vector<FFaListViewItem*>& sogs) const;
  //! \brief Returns the super objects groups containing unused objects.
  void getSuperObjectGroups(std::vector<FFaListViewItem*>& sogs,
                            const std::set<int>& usedBaseId) const;
  //! \brief Returns the children of the specified object group.
  bool getObjectGroupFields(int baseId, const std::string& name,
                            std::vector<FFaListViewItem*>& fields) const;
  //! \brief Returns all top level variables and item groups.
  void getTopLevelVars(std::vector<FFaListViewItem*>& tlvars) const;

protected:
  //! \brief Checks if there is new data on disk for the given \a container.
  virtual int doResultFileUpdate(FFrResultContainer* container, bool doMemPoll);

private:
  bool emitHeaderChanged; //!< Temporary variable used in open/close
  bool emitDataChanged;   //!< Temporary variable used in update

  //! Call-back invoked when a container header is changed
  FFaDynCB1<const FFrExtractor*> myHeaderChangedCB;
  //! Call-back invoked when there is new data
  FFaDynCB1<const FFrExtractor*> myDataChangedCB;
};

#endif
