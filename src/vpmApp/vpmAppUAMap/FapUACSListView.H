// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FAP_UA_CS_LISTVIEW_H
#define FAP_UA_CS_LISTVIEW_H

#include "vpmApp/vpmAppUAMap/FapUAItemsListView.H"
#include "FFaLib/FFaDefinitions/FFaListViewItem.H"


class FapUACSListView : virtual public FapUAItemsListView
{
  //! \brief Class representing one item of the beam cross section database.
  class PropertyItem : public FFaListViewItem
  {
  public:
    //! \brief Default constructor.
    PropertyItem(const std::string& name = "", int id = -1, size_t n = 0);
    //! \brief Copy constructor.
    PropertyItem(const PropertyItem& prp) : FFaListViewItem() { *this = prp; }
    //! \brief Empty destructor.
    virtual ~PropertyItem() {}

    //! \brief Assignment operator.
    PropertyItem& operator=(const PropertyItem& prp)
    {
      if (this != &prp)
      {
        myName = prp.myName;
        myID   = prp.myID;
        myData = prp.myData;
      }
      return *this;
    }

    //! \brief Returns the name of this item.
    virtual char const* getItemName() const { return myName.c_str(); }
    //! \brief Returns the description of this item.
    virtual std::string getItemDescr() const;
    //! \brief Returns the user ID of this item.
    virtual int getItemID() const { return myID; }
    //! \brief Returns the base ID of this item.
    virtual int getItemBaseID() const { return -1; }

    //! \brief Adds a property value to the property container.
    void push_back(double v) { myData.push_back(v); }
    //! \brief Returns \e true if this is a top-level item.
    bool isTopLevel() const { return myID <= 0 && myData.empty(); }

    //! \brief Indexing operator.
    double operator[](size_t i) const { return i < myData.size() ? myData[i] : 0.0; }
    //! \brief Returns the number of property values.
    size_t size() const { return myData.size(); }

    //! \brief Prints out the property item.
    std::ostream& print(std::ostream& os) const;

  private:
    std::string         myName; //!< Cross section name
    int                 myID;   //!< Cross section ID
    std::vector<double> myData; //!< Cross section properties
  };

public:
  FapUACSListView(FuiItemsListView* ui) : FapUAItemsListView(ui) { hasApplIndependentSelection = true;  }
  virtual ~FapUACSListView() {}

  virtual bool singleSelectionMode() const;

  bool loadDataBaseFile(const std::string& filename);
  void clearDataBase() { myDataBase.clear(); }

  static bool importCS(FFaViewItem* csItem, FFaViewItem* matItem);

protected:
  virtual void getChildren(FFaListViewItem* parent,
                           std::vector<FFaListViewItem*>& children) const;

  virtual std::string getItemText(FFaListViewItem* item);

private:
  typedef std::vector<PropertyItem>         PropertyVec;
  typedef std::map<std::string,PropertyVec> PropertyMap;

  PropertyMap myDataBase;
};

#endif
