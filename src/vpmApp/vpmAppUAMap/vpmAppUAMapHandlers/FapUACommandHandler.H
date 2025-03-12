// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_COMMAND_HANDLER_H
#define FAP_UA_COMMAND_HANDLER_H

#include <vector>

#include "vpmApp/FapEventManager.H"
#include "FFuLib/FFuAuxClasses/FFuaCmdItem.H"
#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "vpmPM/FpProcessManager.H"
#include "vpmPM/FpRDBExtractorManager.H"

class FFuUACommandHandler;
class FFuaUICommands;
class FFuMDIWindow;
class FmAnimation;
class FFrExtractor;
class FFaViewItem;


/*!
  \brief Plugin class and receiver for FFuUACommandHandler.

  The task of this class is to handle command execution
  and settings between UI and UA.
  Plugin whenever needing that functionality.
*/

class FapUACommandHandler
{
public:
  FapUACommandHandler(FFuUACommandHandler* ui);
  virtual ~FapUACommandHandler() {}

  virtual const char* getTypeIDName() const { return "FapUACommandHandler"; }

  void updateUICommands() { this->updateUICommands(true); }
  void updateUICommands(bool setNew, bool sens = false, bool toggle = false);

  static void updateAllUICommands(bool sens = false, bool toggle = false);
  static void updateAllUICommandsSensitivity() { updateAllUICommands(true); }
  static void updateAllUICommandsToggle() { updateAllUICommands(false,true); }

private:
  // slots from EventManager
  void onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
			      const std::vector<FFaViewItem*>& selectedSinceLast,
			      const std::vector<FFaViewItem*>& unselectedSinceLast);
  void onActiveWindowChanged(FFuMDIWindow* newActive, FFuMDIWindow* prevActive);
  void onActiveAnimationChanged(FmAnimation* newActive, FmAnimation* prevActive);
  void onRDBHeaderChanged(FFrExtractor*);
  void onProcessGroupChanged(int groupId);

  // Methods to reimplement if you want this functionality in your model view.
  // This method should be re-implemented and is supposed to heap-allocate
  // a FFuaUICommands object. Remember to delete it.
  virtual FFuaUICommands* getCommands() { return NULL; }
  // shouldn't be necessary for ordinary appl commands
  virtual void executeCommand(FFuaCmdItem*) {}

protected:
  FFuaCmdSeparatorItem separator;

private:
  FFuUACommandHandler* ui;

  // Signal Receivers
  FapPermSelChangedReceiver<FapUACommandHandler> permSelReceiver;
  FapActiveWinChangedReceiver<FapUACommandHandler> activeWinReceiver;
  FapActiveAnimationChangedReceiver<FapUACommandHandler> activeAnimReceiver;
  FpRDBHeaderChangedReceiver<FapUACommandHandler> headerChangedReciever;
  FpProcessGroupChangedReceiver<FapUACommandHandler> processChanged;

  friend class FapActiveWinChangedReceiver<FapUACommandHandler>;
  friend class FapPermSelChangedReceiver<FapUACommandHandler>;
  friend class FapActiveAnimationChangedReceiver<FapUACommandHandler>;
  friend class FpRDBHeaderChangedReceiver<FapUACommandHandler>;
  friend class FpProcessGroupChangedReceiver<FapUACommandHandler>;
};

#endif
