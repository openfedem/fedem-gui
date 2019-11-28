// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_EVENT_MANAGER_H
#define FAP_EVENT_MANAGER_H

#include "FFaLib/FFaDynCalls/FFaSwitchBoard.H"
#include <vector>
#include <list>

class FFaViewItem;
class FFaListViewItem;
class FFuMDIWindow;
class FmModelMemberBase;
class FmAnimation;
class FmGraph;


class FapEventManager
{
public:

  // Signals
  enum {PERMSELECTIONCHANGED,
        TMPSELECTIONCHANGED,
        PERMSELECTIONSTACKCHANGED,
        ACTIVEWINDOWCHANGED,
        ACTIVE_ANIMATION_CHANGED};

  static FFaSwitchBoardConnector* getSignalConnector() { return &signalConnector; }

  // Selection stacking
  // Pushing and poping of selections gives always a selection changed signal
  // with the new selection but with empty selected/unselected since last
  static void pushPermSelection();
  static void popPermSelection();
  static bool isInSelectionStack(FFaViewItem* item); // below current level
  static bool hasStackedSelections();

  typedef std::vector<FFaViewItem*> FFaViewItems;

  // Selection
  // An empty vector or a 0-pointer means "mechanism" in all perm select/unselect methods
  // Cannot guarantee that permSelection will get same order as total.
  // If that is important use permSelect(..)
  static void permTotalSelect(const FFaViewItems& total);
  static void permTotalSelect(FFaViewItem* total);

  // Select is appended to permSelection, if it's not already selected
  static void permSelect(const FFaViewItems& select);
  static void permSelect(FFaViewItem* select);

  // Select is appended to permSelection, whatsoever
  static void permAddSelect(const FFaViewItems& select);
  static void permAddSelect(FFaViewItem* select);

  static void permSelect(FFaViewItem* object, int index, bool replace = true);
  static void permSelectInsert(FFaViewItem* object, int index);

  static void permUnselectAll() { permTotalSelect(NULL); }
  static void permUnselect(const FFaViewItems& unselect);
  static void permUnselect(FFaViewItem* unselect);
  static void permUnselect(int index);
  static void permUnselectLast(); // most meaningful when used in comp with permSelect

  static void tmpSelect(FFaViewItem* tmp);

  static void getSelection(FFaViewItems& permSelection,
			   FFaViewItem*& tmpSelection);
  static void getLVSelection(std::vector<FFaListViewItem*>& permSelection,
			     FFaListViewItem*& tmpSelection);
  static void getMMBSelection(std::vector<FmModelMemberBase*>& permSelection,
			      FmModelMemberBase*& tmpSelection);

  static FFaViewItems getPermSelection();
  static std::vector<FFaListViewItem*> getPermLVSelection();
  static std::vector<FmModelMemberBase*> getPermMMBSelection();

  static FFaViewItem* getLastPermSelectedObject();
  static FFaViewItem* getPermSelectedObject(int index);
  static FFaViewItem* getFirstPermSelectedObject();

  static int getNumPermSelected() { return permSelectedItems->back().size(); }

  static FFaViewItem* getTmpSelection();
  static FFaListViewItem* getTmpLVSelection();
  static FmModelMemberBase* getTmpMMBSelection();

  static bool isPermSelected(FFaViewItem* item);
  static bool isTmpSelected(FFaViewItem* item);
  static bool isObjectOfTypeSelected(int typeID);
  static bool isOnlyObjectsOfTypeSelected(int typeID);

  static bool hasPermSelection() { return !FapEventManager::permSelectedItems->empty(); }
  static bool hasTmpSelection() { return FapEventManager::tmpSelectedItem != NULL; }

  // views
  static void setActiveWindow(FFuMDIWindow* active);
  static FFuMDIWindow* getActiveWindow() { return FapEventManager::activeWindow; }
  bool isActiveWindow(FFuMDIWindow* view) { return view == FapEventManager::activeWindow; }

  // fm pointers
  static void setLoadingGraph(FmGraph* graph);
  static FmGraph* getLoadingGraph() { return FapEventManager::loadingGraph; }

  static void setActiveAnimation(FmAnimation* animation);
  static FmAnimation* getActiveAnimation() { return FapEventManager::activeAnimation; }

private:
  // signals
  static void sendPermSelectionChanged(const FFaViewItems& permSelected,
				       const FFaViewItems& permSelectedSinceLast,
				       const FFaViewItems& permUnselectedSinceLast);
  static void sendTmpSelectionChanged(FFaViewItem* tmpSelected, FFaViewItem* tmpUnselected,
				      const FFaViewItems& permSelected);

  static void sendPermSelectionStackChanged(bool pushed);//pushed = false -> popped

  static void sendActiveWindowChanged(FFuMDIWindow* newActive, FFuMDIWindow* prevActive);

  // Selection
  static void addPermSelectedItems(const FFaViewItems& add, FFaViewItems& added);
  static void removePermSelectedItems(const FFaViewItems& remove, FFaViewItems& removed);
  // slots from db
  static void onModelMemberDisconnected(FmModelMemberBase* item);

  // misc
  static void filterNull(const FFaViewItems& in, FFaViewItems& out);
  static void filterLVI(const FFaViewItems& in, std::vector<FFaListViewItem*>& out);
  static void filterMMB(const FFaViewItems& in, std::vector<FmModelMemberBase*>& out);

  static void init();
  static void highlightCurrentLayer(bool highlight);
  static void highlightRendered(FFaViewItem* item, bool onOff);

  // selection
  static std::list<FFaViewItems>* permSelectedItems; // behaves as a stack
  static FFaViewItem*             tmpSelectedItem;

  // views
  static FFuMDIWindow* activeWindow;

  // fm pointers
  static FmGraph* loadingGraph;
  static FmAnimation* activeAnimation;

  static bool isDisconnectingItems;

  // Signal
  class SignalConnector : public FFaSwitchBoardConnector
  {
  public:
    SignalConnector();
    virtual ~SignalConnector() {}
  };
  static SignalConnector signalConnector;

  // Initializer
  friend class FFaInitialisation<FapEventManager>;
};

static FFaInitialisation<FapEventManager> FapEventManager_Init;


////////////////////////////////////////////////////////////////////////
//
// Template based signal receivers

template <class T>
class FapPermSelChangedReceiver : public FFaSwitchBoardConnector
{
  typedef std::vector<FFaViewItem*> FFaViewItems;
public:
  FapPermSelChangedReceiver(T* owner) : myOwner(owner)
  {
    FFaSwitchBoard::connect(FapEventManager::getSignalConnector(),
                            FapEventManager::PERMSELECTIONCHANGED,
                            new FFaSlot3<FapPermSelChangedReceiver<T>,FFaViewItems,FFaViewItems,FFaViewItems>
                            (this,&FapPermSelChangedReceiver<T>::onChanged));
  }
  virtual ~FapPermSelChangedReceiver()
  {
    FFaSwitchBoard::removeAllOwnerConnections(this);
  }
private:
  void onChanged(FFaViewItems totalSelection, FFaViewItems selectedSinceLast, FFaViewItems unselectedSinceLast)
  { myOwner->onPermSelectionChanged(totalSelection,selectedSinceLast,unselectedSinceLast); }
private:
  T* myOwner;
};


template <class T>
class FapTmpSelChangedReceiver : public FFaSwitchBoardConnector
{
  typedef std::vector<FFaViewItem*> FFaViewItems;
public:
  FapTmpSelChangedReceiver(T* owner) : myOwner(owner)
  {
    FFaSwitchBoard::connect(FapEventManager::getSignalConnector(),
                            FapEventManager::TMPSELECTIONCHANGED,
                            new FFaSlot3<FapTmpSelChangedReceiver<T>,FFaViewItem*,FFaViewItem*,FFaViewItems>
                            (this,&FapTmpSelChangedReceiver<T>::onChanged));
  }
  virtual ~FapTmpSelChangedReceiver()
  {
    FFaSwitchBoard::removeAllOwnerConnections(this);
  }
private:
  void onChanged(FFaViewItem* tmpSelected, FFaViewItem* tmpUnselected, FFaViewItems totalSelection)
  { myOwner->onTmpSelectionChanged(tmpSelected,tmpUnselected,totalSelection); }
private:
  T* myOwner;
};


template <class T>
class FapPermSelStackChangedReceiver : public FFaSwitchBoardConnector
{
public:
  FapPermSelStackChangedReceiver(T* owner) : myOwner(owner)
  {
    FFaSwitchBoard::connect(FapEventManager::getSignalConnector(),
                            FapEventManager::PERMSELECTIONSTACKCHANGED,
                            new FFaSlot1<FapPermSelStackChangedReceiver<T>,bool>
                            (this,&FapPermSelStackChangedReceiver<T>::onChanged));
  }
  virtual ~FapPermSelStackChangedReceiver()
  {
    FFaSwitchBoard::removeAllOwnerConnections(this);
  }
private:
  void onChanged(bool pushed) { myOwner->onPermSelectionStackChanged(pushed); }
private:
  T* myOwner;
};


template <class T>
class FapActiveWinChangedReceiver : public FFaSwitchBoardConnector
{
public:
  FapActiveWinChangedReceiver(T* owner) : myOwner(owner)
  {
    FFaSwitchBoard::connect(FapEventManager::getSignalConnector(),
                            FapEventManager::ACTIVEWINDOWCHANGED,
                            new FFaSlot2<FapActiveWinChangedReceiver<T>,FFuMDIWindow*,FFuMDIWindow*>
                            (this,&FapActiveWinChangedReceiver<T>::onChanged));
  }
  virtual ~FapActiveWinChangedReceiver()
  {
    FFaSwitchBoard::removeAllOwnerConnections(this);
  }
private:
  void onChanged(FFuMDIWindow* newActive, FFuMDIWindow* prevActive)
  { myOwner->onActiveWindowChanged(newActive,prevActive); }
private:
  T* myOwner;
};


template <class T>
class FapActiveAnimationChangedReceiver : public FFaSwitchBoardConnector
{
public:
  FapActiveAnimationChangedReceiver(T* owner) : myOwner(owner)
  {
    FFaSwitchBoard::connect(FapEventManager::getSignalConnector(),
                            FapEventManager::ACTIVE_ANIMATION_CHANGED,
                            new FFaSlot2<FapActiveAnimationChangedReceiver<T>,FmAnimation*,FmAnimation*>
                            (this,&FapActiveAnimationChangedReceiver<T>::onChanged));
  }
  virtual ~FapActiveAnimationChangedReceiver()
  {
    FFaSwitchBoard::removeAllOwnerConnections(this);
  }
private:
  void onChanged(FmAnimation* newActive, FmAnimation* prevActive)
  { myOwner->onActiveAnimationChanged(newActive,prevActive); }
private:
  T* myOwner;
};

#endif
