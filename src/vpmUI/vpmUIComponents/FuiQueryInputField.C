// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmUI/vpmUIComponents/FuiQueryInputField.H"
#include "vpmUI/Icons/FuiIconPixmaps.H"
#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFuLib/FFuIOField.H"
#include "FFuLib/FFuOptionMenu.H"
#include "FFuLib/FFuPushButton.H"
#include "vpmApp/vpmAppUAMap/FapUAQueryMatch.H"


FuiQueryInputField::FuiQueryInputField()
{
  myButton = NULL;
  myOptions = NULL;
  myIOField = NULL;

  IAmAConstant = true;
  IAmSensitive = true;
  IAmEditSensitive = true;
  myBehaviour = REF_NONE;
  myButtonMeaning = EDIT;
  myConstant = 0.0;
  myNoRefSelectedText = "None";
  useAddedConstValue = false;
}


void FuiQueryInputField::initWidgets()
{
  myOptions->setOptionSelectedCB(FFaDynCB1M(FuiQueryInputField,this,
					    onOptionSelected,int));

  myButton->setActivateCB(FFaDynCB0M(FuiQueryInputField,this,
				     onButtonPressed));

  this->setChangedCB(FFaDynCB2M(FuiQueryInputField,this,
				onQueryChanged,int,double));

  FFuUAExistenceHandler::invokeCreateUACB(this);
}


//
// Setting up
//
////////////////////////////

void FuiQueryInputField::setBehaviour(Policy behaviour, bool noButton)
{
  switch (behaviour)
    {
    case REF_NUMBER:
      myIOField->setInputCheckMode(FFuIOField::DOUBLECHECK);
      myIOField->setAcceptedCB(FFaDynCB1M(FuiQueryInputField,this,
					  onFieldAccepted,double));
      myIOField->setValue(0.0);
      myConstant = 0.0;
      IAmAConstant = true;
      break;

    case REF_TEXT:
      myIOField->setInputCheckMode(FFuIOField::NOCHECK);
      myIOField->setAcceptedCB(FFaDynCB1M(FuiQueryInputField,this,
					  onFieldAccepted,const std::string&));
      myIOField->setValue(std::string());
      IAmAConstant = true;
      break;

    case REF_NONE:
    case REF:
      IAmAConstant = false;
      break;
    }

  if (noButton)
  {
    myButtonMeaning = NONE;
    myButton->popDown();
  }

  myBehaviour = behaviour;
  this->updateSensitivity();
}


void FuiQueryInputField::setSensitivity(bool makeSensitive)
{
  IAmSensitive = makeSensitive;

  myOptions->setSensitivity(makeSensitive);
  this->updateSensitivity();
}


void FuiQueryInputField::setEditSensitivity(bool makeSensitive)
{
  IAmEditSensitive = makeSensitive;

  this->updateSensitivity();
}


void FuiQueryInputField::updateSensitivity()
{
  switch (myButtonMeaning) {
  case SELECT:
    myButton->setSensitivity(IAmSensitive);
    break;
  case EDIT:
    myButton->setSensitivity(this->getSelectedRefIdx() >= 0);
    break;
  case EDIT_CREATE:
    myButton->setSensitivity(this->getSelectedRefIdx() >= 0 || IAmSensitive);
    break;
  default:
    break;
  }

  if (myIOField)
    myIOField->setSensitivity(IAmSensitive && IAmEditSensitive);
}


void FuiQueryInputField::updateToolTip()
{
  const char* tip = NULL;
  FFuaQueryMatch* item = this->getSelectedQueryMatch();
  if (myBehaviour >= REF_TEXT && item &&
      this->getWidth()-25 < this->getFontWidth(item->matchDescription.c_str()))
    tip = item->matchDescription.c_str();
  else
    tip = myToolTip.c_str();

  if (myIOField)
    myIOField->setToolTip(tip);
  else
    myOptions->setToolTip(tip);
}


//
// Setting Values
//
////////////////////////////

void FuiQueryInputField::setRefList(const std::vector<FFuaQueryMatch*>& matchList)
{
  std::string appendConstValue;
  std::vector<std::string> texts;
  texts.reserve(1+matchList.size());

  if (myBehaviour == REF_NUMBER && myConstant != 0.0)
  {
    // TT #2935: Display the previous constant value instead of "None".
    // NOTE: For this to work properly it is essential to invoke setValue
    // before setQuery when a selection was changed
    texts.push_back(FFaNumStr(myConstant,1,8));
    // Append the constant value so that users know that this
    // value will be added to the selected function output value
    if (useAddedConstValue)
      appendConstValue = " + " + texts.back();
  }
  else if (myBehaviour < REF)
    texts.push_back(myNoRefSelectedText);

  for (FFuaQueryMatch* match : matchList)
    if (match) texts.push_back(match->matchDescription + appendConstValue);

  myOptions->setOptions(texts);
  myRefList = matchList;
}


void FuiQueryInputField::setQuery(FFuaQueryBase* query)
{
  FuaQueryInputFieldValues v;
  v.query = query;
  this->invokeSetAndGetDBValuesCB(&v);
  this->setRefList(v.matches);
  this->setSelectedRefIdx(v.selectedIdx);
}


void FuiQueryInputField::setValue(double value)
{
  if (myBehaviour != REF_NUMBER)
    return;

  myConstant = value; // TT #2934
  if (IAmAConstant)
  {
    myIOField->setValue(value);
    this->setSelectedRefIdx(-1);
  }

  if (!useAddedConstValue)
    myOptions->addOption(FFaNumStr(myConstant,1,8).c_str(),0,true);
}


void FuiQueryInputField::setText(const std::string& val)
{
  if (myBehaviour == REF_TEXT && IAmAConstant)
  {
    myIOField->setValue(val);
    this->setSelectedRefIdx(-1);
  }
}


void FuiQueryInputField::setSelectedRef(FmModelMemberBase* matchToLookupIndexFor)
{
  if (matchToLookupIndexFor)
    {
      FuaQueryInputFieldValues v;
      v.matchToLookupIndexFor = matchToLookupIndexFor;
      this->invokeSetAndGetDBValuesCB(&v);
      this->setSelectedRefIdx(v.selectedIdx);
      this->updateSensitivity();
    }
  else
    this->setSelectedRefIdx(-1);
}


void FuiQueryInputField::setSelectedRefIdx(int id)
{
  switch (myBehaviour)
    {
    case REF_NONE:
      // This if-test gave wrong result without the (int)-casting when id == -1
      if (id >= -1 && id < (int)myRefList.size()) // TT #2668
	myOptions->selectOption(id+1);
      break;

    case REF_NUMBER:
    case REF_TEXT:
      if (id >= 0 && id < (int)myRefList.size())
      {
        IAmAConstant = false;
        if (myConstant != 0.0 && useAddedConstValue)
          myIOField->setValue(myRefList[id]->matchDescription +
                              FFaNumStr(" + %.8g",myConstant));
        else
          myIOField->setValue(myRefList[id]->matchDescription);
        myOptions->selectOption(id+1);
      }
      else
      {
        IAmAConstant = true;
        if (myBehaviour == REF_NUMBER)
          myIOField->setValue(myConstant); // TT #2934
      }
      break;

    case REF:
      if (id >= 0 && id < (int)myRefList.size())
        myOptions->selectOption(id);
      break;

    default:
      break;
    }

  switch (myButtonMeaning) {
  case SELECT:
    myButton->setPixMap(mouseArrow_xpm);
    break;
  case EDIT:
    myButton->setPixMap(edit_xpm);
    break;
  case EDIT_CREATE:
    if (id == -1)
      myButton->setLabel("+");
    else
      myButton->setPixMap(edit_xpm);
    break;
  default:
    break;
  }

  this->updateSensitivity();
  this->updateToolTip();
}


/*!
  This method is needed when the QI-field is used in a dialog with apply/ok
  buttons, and the user use these buttons without Enter in the IO-field first.
*/

void FuiQueryInputField::accept()
{
  if (!IAmAConstant && myIOField && myIOField->isEdited())
    switch (myBehaviour) {
    case REF_NUMBER:
      this->onFieldAccepted(myIOField->getDouble());
      break;
    case REF_TEXT:
      this->onFieldAccepted(myIOField->getValue());
      break;
    default:
      break;
    }
}


//
// Getting Values
//
////////////////////////////

double FuiQueryInputField::getValue() const
{
  if (IAmAConstant && myBehaviour == REF_NUMBER)
    return myIOField->getDouble();

  return myConstant;
}


std::string FuiQueryInputField::getText() const
{
  if (IAmAConstant && myBehaviour == REF_TEXT)
    return myIOField->getValue();

  return std::string();
}


FmModelMemberBase* FuiQueryInputField::getSelectedRef() const
{
  FFuaQueryMatch* item = this->getSelectedQueryMatch();
  return item ? item->matchItem : NULL;
}


FFuaQueryMatch* FuiQueryInputField::getSelectedQueryMatch() const
{
  int choice = this->getSelectedRefIdx();
  if (choice >= 0 && choice < (int)myRefList.size())
    return myRefList[choice];

  return NULL;
}


int FuiQueryInputField::getSelectedRefIdx() const
{
  switch (myBehaviour) {
  case REF_NUMBER:
  case REF_TEXT:
    if (IAmAConstant) return -1; // TT #717
  case REF_NONE:
    return myOptions->getSelectedOption()-1;
  default:
    return myOptions->getSelectedOption();
  }
}


void FuiQueryInputField::getValues(FuiQueryInputFieldValues& v)
{
  v.constant       = this->getValue();
  v.constantText   = this->getText();
  v.selectedRefIdx = this->getSelectedRefIdx();
  v.selectedRef    = this->getSelectedRef();
}


//
// Internal callback forwarding
//
//////////////////////////////////

void FuiQueryInputField::onFieldAccepted(double d)
{
  IAmAConstant = true;
  this->setValue(d);

  myDoubleChangedCB.invoke(d);
  myIdxChangedCB.invoke(-1,d);
  myChangedCB.invoke(0,d);

  FuiQueryInputFieldValues v;
  this->getValues(v);

  myStructChangedCB.invoke(v);
  myThisChangedCB.invoke(this);

  if (myIOField)
    myOptions->addOption(myIOField->getValue().c_str(), 0, true); // TT #2935
}


void FuiQueryInputField::onFieldAccepted(const std::string& t)
{
  IAmAConstant = true;
  this->setText(t);

  myStringChangedCB.invoke(t);
  myIdxStrChangedCB.invoke(-1,t);
  myChangedStrCB.invoke(0,t);

  FuiQueryInputFieldValues v;
  this->getValues(v);

  myStructChangedCB.invoke(v);
  myThisChangedCB.invoke(this);
}


void FuiQueryInputField::onOptionSelected(int choice)
{
  switch (myBehaviour) {
  case REF_NUMBER:
  case REF_TEXT:
    IAmAConstant = choice == 0; // TT #2934
  default:
    break;
  }

  this->updateToolTip();

  switch (myBehaviour) {
  case REF_NUMBER:
    if (IAmAConstant)
      myIOField->setValue(myConstant);
    else
      myIOField->setValue(this->getSelectedQueryMatch()->matchDescription);
    break;

  case REF_TEXT:
    if (IAmAConstant)
      myIOField->setValue(std::string());
    else
      myIOField->setValue(this->getSelectedQueryMatch()->matchDescription);
    break;

  default:
    break;
  }

  FuiQueryInputFieldValues v;
  this->getValues(v);

  myIdxStrChangedCB.invoke(v.selectedRefIdx,"");
  myChangedStrCB.invoke(v.selectedRef,"");
  myIdxChangedCB.invoke(v.selectedRefIdx,myConstant);
  myChangedCB.invoke(v.selectedRef,myConstant);

  myRefIdxSelectedCB.invoke(v.selectedRefIdx);
  myRefSelectedCB.invoke(v.selectedRef);

  myStructChangedCB.invoke(v);
  myThisChangedCB.invoke(this);
}


void FuiQueryInputField::onButtonPressed()
{
  myButtonCB.invoke();
  myButtonRefCB.invoke(this->getSelectedRef());

  FuiQueryInputFieldValues v;
  this->getValues(v);

  switch (myBehaviour) {
  case REF_NUMBER:
    myButtonRefValueCB.invoke(v.selectedRef, v.constant);
    break;
  case REF_TEXT:
    myButtonRefStringCB.invoke(v.selectedRef, v.constantText);
    break;
  default:
    break;
  }

  myButtonStructCB.invoke(v);
  myButtonThisCB.invoke(this);
}


void FuiQueryInputField::onQueryChanged(int, double)
{
  this->setSelectedRef(this->getSelectedRef());
}


//
// Setting callbacks
//
////////////////////////////

void FuiQueryInputField::setRefSelectedCB(const FFaDynCB1<int>& aDynCB)
{
  myRefIdxSelectedCB = aDynCB;
}

void FuiQueryInputField::setRefSelectedCB(const FFaDynCB1<FmModelMemberBase*>& aDynCB)
{
  myRefSelectedCB = aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB1<double>& aDynCB)
{
  myDoubleChangedCB = aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB1<const std::string&>& aDynCB)
{
  myStringChangedCB = aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB2<int,double>& aDynCB)
{
  myIdxChangedCB =  aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB2<int,const std::string&>& aDynCB)
{
  myIdxStrChangedCB =  aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB2<FmModelMemberBase*,const std::string&>& aDynCB)
{
  myChangedStrCB = aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB1<FuiQueryInputFieldValues&>& aDynCB)
{
  myStructChangedCB = aDynCB;
}

void FuiQueryInputField::setButtonCB(const FFaDynCB0& aDynCB)
{
  myButtonCB = aDynCB;
}

void FuiQueryInputField::setButtonCB(const FFaDynCB1<FmModelMemberBase*>& aDynCB)
{
  myButtonRefCB = aDynCB;
}

void FuiQueryInputField::setButtonCB(const FFaDynCB2<FmModelMemberBase*,double>& aDynCB)
{
  myButtonRefValueCB = aDynCB;
}

void FuiQueryInputField::setButtonCB(const FFaDynCB2<FmModelMemberBase*,const std::string&>& aDynCB)
{
  myButtonRefStringCB = aDynCB;
}

void FuiQueryInputField::setButtonCB(const FFaDynCB1<FuiQueryInputFieldValues&>& aDynCB)
{
  myButtonStructCB = aDynCB;
}

void FuiQueryInputField::setChangedCB(const FFaDynCB1<FuiQueryInputField*>& aDynCB)
{
  myThisChangedCB = aDynCB;
}

void FuiQueryInputField::setButtonCB(const FFaDynCB1<FuiQueryInputField*>& aDynCB)
{
  myButtonThisCB = aDynCB;
}
