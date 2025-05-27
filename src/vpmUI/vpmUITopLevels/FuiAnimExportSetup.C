// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUITopLevels/FuiAnimExportSetup.H"
#include "vpmUI/Fui.H"
#include "vpmPM/FpFileSys.H"
#include "FFuLib/FFuSpinBox.H"
#include "FFuLib/FFuRadioButton.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuPushButton.H"
#include "FFuLib/FFuDialogButtons.H"
#include "FFuLib/FFuFileDialog.H"
#include "FFaLib/FFaOS/FFaFilePath.H"


Fmd_SOURCE_INIT(FUI_ANIMEXPORTSETUP, FuiAnimExportSetup, FFuModalDialog);

enum { MPEG1, MPEG2, AVI };

enum {
  EXPORT = FFuDialogButtons::LEFTBUTTON,
  CANCEL = FFuDialogButtons::RIGHTBUTTON
};


FuiAnimExportSetup::FuiAnimExportSetup()
{
  Fmd_CONSTRUCTOR_INIT(FuiAnimExportSetup);

  selectedFilter = MPEG1;
}


void FuiAnimExportSetup::initWidgets()
{
  myRealBtn->setLabel("Real Time");
  myRealBtn->setValue(true);

  myAllBtn->setLabel("All Frames");
  myAllBtn->setValue(false);

  myOmitBtn->setLabel("Omit Every");
  myOmitBtn->setValue(false);

  mySomeBtn->setLabel("Export Only Every");
  mySomeBtn->setValue(false);

  myButtonGroup.insert(myRealBtn);
  myButtonGroup.insert(myAllBtn);
  myButtonGroup.insert(mySomeBtn);
  myButtonGroup.insert(myOmitBtn);
  myButtonGroup.setExclusive(true);
  myButtonGroup.setGroupToggleCB(FFaDynCB2M(FuiAnimExportSetup,this,
					    onToggleChanged,int,bool));

  omitNthSpinBox->setMinMax(2, 1000000);
  omitNthSpinBox->setStepSize(1);
  omitNthSpinBox->setWrapping(false);
  omitNthSpinBox->setSensitivity(false);

  everyNthSpinBox->setMinMax(2, 1000000);
  everyNthSpinBox->setStepSize(1);
  everyNthSpinBox->setWrapping(false);
  everyNthSpinBox->setSensitivity(false);

#if defined(win32) || defined(win64)
  myFileField->setValue("C:\\animation.mpeg");
#else
  myFileField->setValue("/tmp/animation.mpeg");
#endif

  myBrowseButton->setLabel("Browse...");
  myBrowseButton->setActivateCB(FFaDynCB0M(FuiAnimExportSetup,this,
					   onBrowseButtonClicked));

  dialogButtons->setButtonLabel(EXPORT, "Export");
  dialogButtons->setButtonLabel(CANCEL, "Cancel");
  dialogButtons->setButtonClickedCB(FFaDynCB1M(FuiAnimExportSetup,this,
					       onDialogButtonClicked,int));
}


/*!
  CB from dialog buttons.
  - If file extension is ok, invokes own cb.
    If file exists, asks user if overwrite is ok.
  - If unknown extension or avi on unix, asks user to correct the error.
*/

void FuiAnimExportSetup::onDialogButtonClicked(int button)
{
  std::string file = myFileField->getValue();
  std::string ext = FFaFilePath::getExtension(file);

  if (button == CANCEL)
    myClickedCB.invoke(button);
#if defined(win32) || defined(win64)
  else if (ext == "mpeg" || ext == "mpg" || ext == "avi")
#else
  else if (ext == "avi")
    Fui::dismissDialog("Avi export is available on Windows only.\nPlease correct before continuing.",FFuDialog::WARNING);
  else if (ext == "mpeg" || ext == "mpg")
#endif
  {
    if (!FpFileSys::isFile(file) || Fui::yesNoDialog(("The file " + file +
         " already exists.\nDo you wish to replace the existing file?").c_str()))
      myClickedCB.invoke(button);
  }
  else
    Fui::dismissDialog("The selected file extension is unknown.\nPlease correct before continuing.",FFuDialog::ERROR);
}


void FuiAnimExportSetup::onToggleChanged(int, bool)
{
  omitNthSpinBox->setSensitivity(myOmitBtn->getValue());
  everyNthSpinBox->setSensitivity(mySomeBtn->getValue());
}


void FuiAnimExportSetup::onBrowseButtonClicked()
{
  std::string defaultFile = myFileField->getValue();
  FFuFileDialog* fileD = FFuFileDialog::create(FFaFilePath::getPath(defaultFile),
					       "animation_export", FFuFileDialog::FFU_SAVE_FILE);
  fileD->setTitle("Save animation as");
  fileD->setDefaultName(FFaFilePath::getBaseName(defaultFile));
  fileD->remember("animationExport");
  fileD->addFilter("MPEG-1 Animation Export", "mpeg", true, MPEG1);
  fileD->addFilter("MPEG-2 Animation Export", "mpeg", false, MPEG2);
#if defined(win32) || defined(win64)
  fileD->addFilter("AVI Animation Export", "avi", false, AVI);
#endif

  std::vector<std::string> selectedFile = fileD->execute();
  selectedFilter = fileD->getSelectedFilter();
  delete fileD;

  if (!selectedFile.empty())
    myFileField->setValue(selectedFile.front());
}


void FuiAnimExportSetup::getSetupValues(bool& allFrames, bool& realTime, bool& omitSome,
					bool& onlySome, int& nthToOmit, int& nthToInclude)
{
  allFrames    = myAllBtn->getValue();
  realTime     = myRealBtn->getValue();
  omitSome     = myOmitBtn->getValue();
  onlySome     = mySomeBtn->getValue();
  nthToOmit    = omitNthSpinBox->getIntValue();
  nthToInclude = everyNthSpinBox->getIntValue();
}


void FuiAnimExportSetup::getFileValues(std::string& fileName, int& format)
{
  fileName = myFileField->getValue();
  format = selectedFilter;

  // To avoid conflict if user has manually typed in file name
  switch (selectedFilter) {
  case MPEG1:
  case MPEG2:
    if (FFaFilePath::isExtension(fileName,"avi"))
      format = AVI;
    break;
  case AVI:
    if (FFaFilePath::isExtension(fileName,"mpeg"))
      format = MPEG1;
    else if (FFaFilePath::isExtension(fileName,"mpg"))
      format = MPEG1;
    break;
  }
}


void FuiAnimExportSetup::setDefaultFileName(const std::string& fileName)
{
  myFileField->setValue(fileName);

  if (FFaFilePath::isExtension(fileName,"avi"))
    selectedFilter = AVI;
  else
    selectedFilter = MPEG1;
}


FuiAnimExportSetup* FuiAnimExportSetup::getUI(bool onScreen)
{
  FuiAnimExportSetup* setup = NULL;
  FFuTopLevelShell* uic = FFuTopLevelShell::getInstanceByType(FuiAnimExportSetup::getClassTypeID());
  if (!uic)
    uic = setup = FuiAnimExportSetup::create(100,100,370,280);
  else
    setup = dynamic_cast<FuiAnimExportSetup*>(uic);

  uic->manage(onScreen,true);

  return setup;
}
