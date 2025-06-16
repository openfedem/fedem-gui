// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_QUERY_INPUT_FIELD_H
#define FUI_QUERY_INPUT_FIELD_H

#include "FFuLib/FFuBase/FFuComponentBase.H"
#include "FFuLib/FFuBase/FFuUAExistenceHandler.H"
#include "FFuLib/FFuBase/FFuUADataHandler.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"
#include <vector>
#include <string>

class FuiQueryInputFieldValues;
class FmModelMemberBase;
class FFuaQueryMatch;
class FFuaQueryBase;
class FFuIOField;
class FFuOptionMenu;
class FFuPushButton;


class FuiQueryInputField : public virtual FFuComponentBase,
                           public FFuUAExistenceHandler,
                           public FFuUADataHandler
{
public:
  FuiQueryInputField();

  // Setting up

  enum Policy { REF_NUMBER, REF_TEXT, REF_NONE, REF };
  virtual void setBehaviour(Policy behaviour, bool noButton = false);

  enum ButtonMeaning { NONE, SELECT, EDIT, EDIT_CREATE };
  void setButtonMeaning(ButtonMeaning meaning) { myButtonMeaning = meaning; }

  virtual void setToolTip(const char* t = NULL) { myToolTip = t ? t : ""; }

  virtual void setSensitivity(bool makeSensitive);
  void setEditSensitivity(bool isSensitive); // true if allowed to edit the input field

  void setTextForNoRefSelected(const char* text) { myNoRefSelectedText = text; }

  bool useAddedConstValue; // true if the field adds the const value to the function value

  // Setting callbacks

  void setRefSelectedCB( const FFaDynCB1< int > &aDynCB );  // -1 if no engine
  void setRefSelectedCB( const FFaDynCB1< FmModelMemberBase *> &aDynCB );

  void setChangedCB    ( const FFaDynCB1< double > &aDynCB );
  void setChangedCB    ( const FFaDynCB1< const std::string& > &aDynCB );

  void setChangedCB    ( const FFaDynCB2< int, double > &aDynCB );  // If int == -1 => use double as const.
  void setChangedCB    ( const FFaDynCB2< int, const std::string& > &aDynCB );  // If int == -1 => use double as const.

  void setChangedCB    ( const FFaDynCB2< FmModelMemberBase *, double > &aDynCB );// If qmb == 0  => use double as const.
  void setChangedCB    ( const FFaDynCB2< FmModelMemberBase *, const std::string& > &aDynCB );// If qmb == 0  => use double as const.

  void setChangedCB    ( const FFaDynCB1< FuiQueryInputFieldValues & > &aDynCB );// If qmb == 0  => use double as const.

  void setButtonCB     ( const FFaDynCB0  &aDynCB );

  void setButtonCB     ( const FFaDynCB1< FmModelMemberBase *> &aDynCB);

  void setButtonCB     ( const FFaDynCB2< FmModelMemberBase *, double > &aDynCB);
  void setButtonCB     ( const FFaDynCB2< FmModelMemberBase *, const std::string& > &aDynCB);

  void setButtonCB     ( const FFaDynCB1< FuiQueryInputFieldValues &> &aDynCB);

  void setChangedCB    ( const FFaDynCB1< FuiQueryInputField * > & aDynCB);
  void setButtonCB     ( const FFaDynCB1< FuiQueryInputField * > & aDynCB);

  // Setting Values

  void setQuery(FFuaQueryBase* query);
  void setValue(double value);
  void setText(const std::string& val);
  void setSelectedRef(FmModelMemberBase* matchToLookupIndexFor);
  void accept();

  // Getting Values

  bool isAConstant() const { return IAmAConstant; }
  double getValue() const;
  std::string getText() const;
  FmModelMemberBase* getSelectedRef() const;

protected:

  void initWidgets();

private:
  // Internal setting of list contents :
  void setRefList(const std::vector<FFuaQueryMatch*>& list);

  void setSelectedRefIdx(int id);
  int  getSelectedRefIdx() const;
  void getValues(FuiQueryInputFieldValues& v);

  void updateSensitivity();
  void updateToolTip();

  FFuaQueryMatch* getSelectedQueryMatch() const;

  // Internal Callback Forwarding :

  void onFieldAccepted(double d);
  void onFieldAccepted(const std::string& s);
  void onOptionSelected(int choice);
  void onQueryChanged(int, double);
  void onButtonPressed();

protected:
  // Shadow pointers initiated from subclass
  FFuIOField*    myIOField;
  FFuOptionMenu* myOptions;
  FFuPushButton* myButton;

private:
  Policy        myBehaviour;
  ButtonMeaning myButtonMeaning;

  bool IAmAConstant;
  bool IAmSensitive;
  bool IAmEditSensitive;

  double myConstant;
  std::string myNoRefSelectedText;
  std::string myToolTip;

  // Callbacks

  FFaDynCB1< int >                          myRefIdxSelectedCB;
  FFaDynCB1< FmModelMemberBase *>          myRefSelectedCB;

  FFaDynCB1< double >                       myDoubleChangedCB;
  FFaDynCB1< const std::string& >                myStringChangedCB;

  FFaDynCB2< int, double >                         myIdxChangedCB;
  FFaDynCB2< int, const std::string& >                  myIdxStrChangedCB;
  FFaDynCB2< FmModelMemberBase *, double >         myChangedCB;
  FFaDynCB2< FmModelMemberBase *, const std::string& >  myChangedStrCB;
  FFaDynCB1< FuiQueryInputFieldValues & >          myStructChangedCB;

  FFaDynCB0                                 myButtonCB;
  FFaDynCB1< FmModelMemberBase *>          myButtonRefCB;
  FFaDynCB2< FmModelMemberBase *, double>  myButtonRefValueCB;
  FFaDynCB2< FmModelMemberBase *, const std::string& >  myButtonRefStringCB;
  FFaDynCB1< FuiQueryInputFieldValues &  >  myButtonStructCB;

  FFaDynCB1< FuiQueryInputField * > myThisChangedCB;
  FFaDynCB1< FuiQueryInputField * > myButtonThisCB;

  // Internal list of entries :

  std::vector<FFuaQueryMatch*> myRefList;
};


class FuiQueryInputFieldValues
{
public:
  double constant = 0.0;
  std::string constantText;
  int    selectedRefIdx = -1;
  FmModelMemberBase* selectedRef = NULL;
};


class FuaQueryInputFieldValues : public FFuaUIValues
{
public:
  FFuaQueryBase*               query = NULL;
  std::vector<FFuaQueryMatch*> matches;

  int                selectedIdx = -1;
  FmModelMemberBase* matchToLookupIndexFor = NULL;
};

#endif
