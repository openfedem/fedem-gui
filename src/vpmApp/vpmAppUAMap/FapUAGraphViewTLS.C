// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmApp/vpmAppUAMap/FapUAGraphViewTLS.H"
#include "vpmApp/vpmAppUAMap/FapUAGraphView.H"
#include "vpmUI/vpmUITopLevels/FuiGraphView.H"
#include "vpmUI/vpmUIComponents/FuiItemsListView.H"
#include "vpmDB/FmGraph.H"
#include "vpmDB/FmCurveSet.H"
#include "FFaLib/FFaDynCalls/FFaDynCB.H"


Fmd_SOURCE_INIT(FAPUAGRAPHVIEWTLS, FapUAGraphViewTLS, FapUAExistenceHandler);

//----------------------------------------------------------------------------

FapUAGraphViewTLS::FapUAGraphViewTLS(FuiGraphViewTLS* uic)
  : FapUAExistenceHandler(uic), FapUAFinishHandler(uic),
    selSignalConnector(this), signalConnector(this)
{
  Fmd_CONSTRUCTOR_INIT(FapUAGraphViewTLS);

  this->ui = uic;
}
//----------------------------------------------------------------------------

void FapUAGraphViewTLS::finishUI()
{
  // Pop down before we delete so that onPoppedDown events are being given
  this->ui->popDown();
  this->ui->removeFromTopLevel();
  delete this->ui;
}
//----------------------------------------------------------------------------

void FapUAGraphViewTLS::onModelMemberDisconnected(FmModelMemberBase* item)
{
  FmGraph* dbgraph = ((FapUAGraphView*)this->ui->getGraphViewComp()->getUA())->getDBPointer();
  if (!dbgraph) return;

  if (item->isOfType(FmGraph::getClassTypeID()))
    if (item == dbgraph)
      this->ui->onClose();
}
//----------------------------------------------------------------------------

void FapUAGraphViewTLS::onModelMemberChanged(FmModelMemberBase* item)
{
  FmGraph* dbgraph = ((FapUAGraphView*)this->ui->getGraphViewComp()->getUA())->getDBPointer();
  if (!dbgraph) return;

  if (item->isOfType(FmGraph::getClassTypeID()))
    if (item == dbgraph)
      this->ui->setTitle(dbgraph->getUserDescription().c_str());
}
//----------------------------------------------------------------------------

void FapUAGraphViewTLS::onPermSelectionChanged(const std::vector<FFaViewItem*>& totalSelection,
					       const std::vector<FFaViewItem*>&,
					       const std::vector<FFaViewItem*>&)
{
  // Bugfix #334: Don't change view focus when selecting using the arrow keys
  if (!FuiItemsListView::mouseSelected)
    return;

  // Only bother if there is one and only one selection
  if (totalSelection.size() != 1)
    return;

  FmGraph* dbgraph = ((FapUAGraphView*)this->ui->getGraphViewComp()->getUA())->getDBPointer();
  if (!dbgraph) return;

  if (dbgraph == dynamic_cast<FmGraph*>(totalSelection.front()))
    this->ui->popUp();
  else if (dbgraph->hasCurve(dynamic_cast<FmCurveSet*>(totalSelection.front())))
    this->ui->popUp();
}
//----------------------------------------------------------------------------

FapUAGraphViewTLS::SignalConnector::SignalConnector(FapUAGraphViewTLS* ua) : owner(ua)
{
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_DISCONNECTED,
			  FFaSlot1M(SignalConnector,this,onModelMemberDisconnected,FmModelMemberBase*));
  FFaSwitchBoard::connect(FmModelMemberBase::getSignalConnector(),
			  FmModelMemberBase::MODEL_MEMBER_CHANGED,
			  FFaSlot1M(SignalConnector,this,onModelMemberChanged,FmModelMemberBase*));
}
//----------------------------------------------------------------------------

FapUAGraphViewTLS::SignalConnector::~SignalConnector()
{
  FFaSwitchBoard::removeAllOwnerConnections(this);
}
//----------------------------------------------------------------------------
