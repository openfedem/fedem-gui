// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_DUTY_CYCLE_OPTIONS_H
#define FUI_DUTY_CYCLE_OPTIONS_H

#include <vector>
#include <string>
#include <set>

#include "vpmUI/vpmUITopLevels/FuiTopLevelDialog.H"

class FFuTable;
class FFuPushButton;
class FFuLabelFrame;
class FFuIOField;
class FFuListView;
class FFuLabel;
class FuiEventData;


class FuiDutyCycleOptions : public FuiTopLevelDialog
{
  Fmd_HEADER_INIT();

public:
  FuiDutyCycleOptions();
  virtual ~FuiDutyCycleOptions() {}

  static FuiDutyCycleOptions* create(int xpos = 100, int ypos  = 100,
				     int width= 100, int height= 100,
				     const char* title= "Duty Cycle Setup",
				     const char* name = "FuiDutyCycleOptions");

  // protected/private?
  virtual void setUIValues(const FFuaUIValues* values);
  virtual void getUIValues(FFuaUIValues* values);

  void setAddEventCB(const FFaDynCB0& dynCB);

  void setLinks(const std::vector< std::pair<std::string,bool> >& links);
  void setEvents(const std::vector<FuiEventData>& events);
  void addEvent(const FuiEventData& event, int row = -1); // -1 for append
  bool eventPresent(const std::string& file);

protected:
  virtual void initWidgets();

private:
  FFuaUIValues* createValuesObject();
  virtual void placeWidgets(int width,int height);

  void toggleAllLinks(bool on);

  // cbs
  void onAddEventButtonClicked();
  void onDelEventButtonClicked();
  void onCheckAllButtonClicked();
  void onCheckNoneButtonClicked();
  void onLinkCellClicked(int row, int col);
  void onEventCellClicked(int row, int col);

protected:
  FFuTable* eventTable;
  FFuTable* linkTable;

  FFuPushButton* addEventButton;
  FFuPushButton* delEventButton;
  FFuPushButton* checkAllButton;
  FFuPushButton* checkNoneButton;

  FFuIOField* equivUnitField;
  FFuIOField* equivUnitScaleField;

  FFuLabel *equivUnitLabel;
  FFuLabel* equivUnitScaleLabel;

  FFuLabelFrame* eventFrame;
  FFuLabelFrame* linkFrame;

  std::set<std::string> myEvents;

  FFaDynCB0 addEventCB;
};


class FuiEventData
{
public:
  FuiEventData() {}
  FuiEventData(const std::string& path, const std::string& eventName,
	       double numRepeats, bool masterEvent) {
    this->fmmPath = path;
    this->name = eventName;
    this->repeats = numRepeats;
    this->master = masterEvent;
  }

  std::string fmmPath;
  std::string name;
  double repeats;
  bool master;
};


class FuaDutyCycleOptionsValues : public FFuaUIValues
{
public:
  FuaDutyCycleOptionsValues() {}
  virtual ~FuaDutyCycleOptionsValues() {}

  std::vector< std::pair<std::string,bool> > links;
  std::vector<FuiEventData> events;
  std::pair<double,std::string> equivUnit;
  std::string modelFilePath;
};

#endif
