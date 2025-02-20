// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUACSListView.H"
#include "vpmApp/vpmAppCmds/FapDBCreateCmds.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "vpmDB/FmBeamProperty.H"

#include "FFaLib/FFaString/FFaStringExt.H"
#include "FFaLib/FFaDefinitions/FFaMsg.H"
#include "FFaLib/FFaOS/FFaFilePath.H"

#include <iostream>
#include <fstream>
#include <cstring>


//----------------------------------------------------------------------------

bool FapUACSListView::loadDataBaseFile(const std::string& fileName)
{
  // Read the first line which should be the heading
  char buf[512];
  std::ifstream is(fileName);
  if (!is.getline(buf,512))
  {
    FFaMsg::dialog("Failed to read cross section database file\n" + fileName,
		   FFaMsg::ERROR);
    return false;
  }

  // Count the number of (tab-separated) columns
  std::vector<std::string> heading;
  for (char* p = strtok(buf,"\t"); p; p = strtok(NULL,"\t"))
    heading.push_back(p);
  if (heading.size() < 3)
  {
    FFaMsg::dialog("Invalid cross section database file\n" + fileName +
                   "\nShould be tab-separated and with at least 3 columns.",
                   FFaMsg::ERROR);
    return false;
  }

  std::cout <<"   * Loading "<< fileName;
#ifdef LV_DEBUG
  std::cout <<"\n    ";
  for (const std::string& label : heading) std::cout <<" "<< label;
#endif
  std::cout << std::endl;

  std::string name = FFaFilePath::getBaseName(fileName,true);
  myDataBase["root"].push_back(PropertyItem(name));
  PropertyVec& csGroup = myDataBase[name];

  int ID = -1;
  PropertyItem* current = NULL;
  size_t ncol = heading.size();
  for (size_t icol = 1; is.getline(buf, 512, icol < ncol ? '\t' : '\n'); icol++)
    if (icol == 1)
      name = buf;
    else if (icol == 2)
      ID = atoi(buf);
    else if (icol == 3)
    {
      csGroup.push_back(PropertyItem(name,ID,ncol-2));
      current = &csGroup.back();
      current->push_back(atof(buf));
    }
    else if (icol < ncol)
      current->push_back(atof(buf));
    else
    {
      icol = 0;
      current->push_back(atof(buf));
#ifdef LV_DEBUG
      std::cout <<"     ";
      current->print(std::cout) << std::endl;
#endif
    }

  return true;
}
//----------------------------------------------------------------------------

bool FapUACSListView::importCS(FFaViewItem* csItem, FFaViewItem* matItem)
{
  PropertyItem* bcs = dynamic_cast<PropertyItem*>(csItem);
  if (!bcs || bcs->size() < 7)
  {
    std::cerr <<"ERROR: Missing/invalid cross section data."<< std::endl;
    return false;
  }

  PropertyItem* mat = dynamic_cast<PropertyItem*>(matItem);
  if (!mat || mat->size() < 3)
  {
    std::cerr <<"ERROR: Missing/invalid material data"<< std::endl;
    return false;
  }

  FmBeamProperty* bp = new FmBeamProperty();
  bp->setParentAssembly(FapDBCreateCmds::getSelectedAssembly());
  bp->setID(bcs->getItemID());
  if (!bp->connect())
    return !bp->erase();

  // Get material parameters
  double rho = (*mat)[0];
  double nu  = (*mat)[1];
  double E   = (*mat)[2];
  double G   = 0.5*E/(1.0+nu);

  // Assuming the cross section parameters are organized as follows:
  // A  I_YY  I_ZZ  As_y  As_z  It  Ip  Mass  Shear_center_y  Shear_center_z
  bp->EA.setValue(E*(*bcs)[0]);
  bp->EI.setValue(std::make_pair(E*(*bcs)[1],E*(*bcs)[2]));
  bp->GAs.setValue(std::make_pair(G*(*bcs)[3],G*(*bcs)[4]));
  bp->GIt.setValue(G*(*bcs)[5]);
  bp->RoIp.setValue(rho*(*bcs)[6]);
  bp->Mass.setValue(rho*(*bcs)[0]); // Note: Mass=(*bcs)[7] is not used
  if (bcs->size() > 9)
    bp->ShrCentre.setValue(std::make_pair((*bcs)[8],(*bcs)[9]));

  bp->crossSectionType.setValue(FmBeamProperty::GENERIC);
  bp->setUserDescription(bcs->getItemName() + std::string(", ") +
                         mat->getItemName());
  bp->onChanged();

  ListUI <<"  -> Imported "<< bp->getIdString(true) <<"\n";
  return true;
}
//----------------------------------------------------------------------------

bool FapUACSListView::singleSelectionMode() const
{
  return strcmp(ui->getName(),"Material") == 0;
}
//----------------------------------------------------------------------------

std::string FapUACSListView::getItemText(FFaListViewItem* item)
{
  return item->getItemDescr();
}
//----------------------------------------------------------------------------

void FapUACSListView::getChildren(FFaListViewItem* parent,
                                  std::vector<FFaListViewItem*>& children) const
{
  children.clear();
  const char* parentName = parent ? parent->getItemName() : "root";
  PropertyMap::const_iterator pit = myDataBase.find(parentName);
  if (pit == myDataBase.end()) return;

  children.reserve(pit->second.size());
  for (const PropertyItem& item : pit->second)
    children.push_back(const_cast<PropertyItem*>(&item));
}
//----------------------------------------------------------------------------

FapUACSListView::PropertyItem::PropertyItem(const std::string& name,
                                            int id, size_t n)
{
  myName = name;
  myID = id;
  myData.reserve(n);
}
//----------------------------------------------------------------------------

std::string FapUACSListView::PropertyItem::getItemDescr() const
{
  if (this->isTopLevel())
    return myName;

  std::string descr = FFaNumStr("[%d] ",myID) + myName + " :";
  for (double v : myData) descr += " " + FFaNumStr(v);
  return descr;
}
//----------------------------------------------------------------------------

std::ostream& FapUACSListView::PropertyItem::print(std::ostream& os) const
{
  os << myName;
  if (this->isTopLevel())
    return os;

  os <<"\t["<< myID <<"]:";
  for (double v : myData) os <<" "<< v;
  return os;
}
