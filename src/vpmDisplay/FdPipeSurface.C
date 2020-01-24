// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include "vpmDisplay/FdPipeSurface.H"
#include "vpmDisplay/FdSymbolDefs.H"
#include "vpmDisplay/FdBackPointer.H"
#include "vpmDisplay/FdPipeSurfaceKit.H"
#include "vpmDisplay/FdCurveKit.H"
#include "vpmDisplay/FdTransformKit.H"
#include "vpmDisplay/FdMechanismKit.H"
#include "vpmDisplay/FdAppearanceKit.H"
#include "vpmDisplay/FdDB.H"
#include "vpmDisplay/FdConverter.H"
#include "vpmDisplay/FdTriad.H"
#include "vpmDisplay/FdLink.H"
#include "vpmDB/FmLink.H"
#include "vpmDB/FmTriad.H"
#include "vpmDB/FmPipeSurface.H"
#include "FFaLib/FFaAlgebra/FFa3PArc.H"

#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>


 /**********************************************************************
 *
 * CLASS FdPipeSurface
 *
 **********************************************************************/


Fmd_SOURCE_INIT(FDPIPESURFACE,FdPipeSurface,FdObject);


FdPipeSurface::FdPipeSurface(FmPipeSurface* pt) : FdObject()
{
  Fmd_CONSTRUCTOR_INIT(FdPipeSurface);

  itsFmOwner = pt;

  itsKit = new FdPipeSurfaceKit;
  itsKit->ref();

  // Set up back pointer
  FdBackPointer* bp_pointer = SO_GET_PART(itsKit,"backPt",FdBackPointer);
  bp_pointer->setPointer(this);

  myLineMaterial = new SoMaterial;
  myFaceMaterial = new SoMaterial;

  myLineMaterial->ref();
  myFaceMaterial->ref();
}


FdPipeSurface::~FdPipeSurface()
{
  this->fdDisconnect();
  itsKit->unref();
  myLineMaterial->unref();
  myFaceMaterial->unref();
}


SoNodeKitListPart* FdPipeSurface::getListSw() const
{
  return SO_GET_PART(FdDB::getMechanismKit(),"pipeSurfaceListSw",SoNodeKitListPart);
}


bool FdPipeSurface::updateFdTopology(bool updateChildrenDisplay)
{
  if (updateChildrenDisplay)
    itsFmOwner->updateChildrenDisplayTopology();

  return true;
}


bool FdPipeSurface::updateFdApperance()
{
  // This test makes sure we do not unHighlight something
  // when it is supposed to be highlighted
  if (this->highlightRefCount > 0) return true;

  SoMaterial* mat = myFaceMaterial;
  mat->diffuseColor.setValue(SbColor(92.0/256, 90.0/256, 192.0/256));
  mat->ambientColor.setValue(SbColor(60.0/256, 60.0/256, 150.0/256));
  mat->specularColor.setValue(SbColor(160.0/256, 160.0/256, 217.0/256));
  mat->emissiveColor.setValue(SbColor(0,0,0));
  mat->transparency.setValue(0.9);

  if (((FmPipeSurface*)itsFmOwner)->isFacesOn()) // check whether the faces are on
    myLineMaterial->diffuseColor.setValue(SbColor(0, 0, 0));
  else
    myLineMaterial->diffuseColor.setValue(SbColor(92.0/256, 90.0/256, 192.0/256));

  return true;
}


bool FdPipeSurface::updateFdDetails()
{
  SoNodeKitListPart* list = SO_GET_PART(itsKit,"pipeList",SoNodeKitListPart);
  while (list->getNumChildren())
    list->removeChild(list->getNumChildren()-1);

  SoTransform* transLink = NULL;
  FmLink* ownerLink = ((Fm1DMaster*)itsFmOwner)->getOwnerLink();
  if (ownerLink) {
    FdLink*    linkDisplay = (FdLink*)(ownerLink->getFdPointer());
    SoBaseKit* linkKit     = (SoBaseKit*)(linkDisplay->getKit());
    transLink = SO_GET_PART(linkKit,"transform",SoTransform);
  }

  std::vector<CurveSegment> curves;
  ((FmPipeSurface*)itsFmOwner)->getCurveSegments(curves);
  double r = ((FmPipeSurface*)itsFmOwner)->getPipeRadius();

  for (const CurveSegment& segment : curves)
    {
      FFa3PArc curve(segment[0],segment[1],segment[2]);
      double tessLength = curve.getArcLength(0.1*r);
      double arcLength  = curve.getArcLength();

      // Check the validity of the curve
      if (tessLength > 1.0e-10 && arcLength > 1.0e-10)
        {
          SoSeparator* arc = new SoSeparator;

          // Set up connection
          if (transLink)
            arc->addChild(transLink);

          SoLevelOfDetail* lod = new SoLevelOfDetail;
          arc->addChild(lod);
          lod->screenArea.set1Value(0, 1000);
          lod->screenArea.set1Value(1, 100);

          // Centerline visualization

          SoSeparator* cLineSep = new SoSeparator;
          SoVertexProperty* vxp = new SoVertexProperty;
          SoLineSet*     clines = new SoLineSet;

          double part, numTess;
          int j, k, n;

          tessLength = curve.getArcLength(r);
          modf(arcLength/tessLength, &numTess);
          numTess += 1.0;
          tessLength = arcLength/numTess;

          for (j = 0, part = 0.0; part <= arcLength; part += tessLength, j++)
            vxp->vertex.set1Value(j, FdConverter::toSbVec3f(curve.getPointOnArc(part)));

          clines->vertexProperty.setValue(vxp);
          cLineSep->addChild(myLineMaterial);
          cLineSep->addChild(clines);

          // Simple visualization

          double maxError = r/50.0;
          int nVxPrCircle = 20;
          int nLinesPrCircle = 4;
          FaVec3 up = segment[3];
          up.normalize();

          for (int dl = 0; dl < 2; ++dl, maxError = r/5, nVxPrCircle = 8)
            {
              SoSeparator* sShapeSep = new SoSeparator;
              SoSeparator* sLineSep = new SoSeparator;
              SoSeparator* sFaceSep = new SoSeparator;
              SoPolygonOffset* po = new SoPolygonOffset;

              sShapeSep->addChild(sLineSep);
              sShapeSep->addChild(sFaceSep);

              vxp = new SoVertexProperty;
              SoIndexedLineSet* slines = new SoIndexedLineSet;
              SoIndexedFaceSet* sfaces = new SoIndexedFaceSet;

              sLineSep->addChild(myLineMaterial);
              sLineSep->addChild(slines);

              sFaceSep->addChild(myFaceMaterial);
              sFaceSep->addChild(po);
              sFaceSep->addChild(sfaces);

              tessLength = curve.getArcLength(maxError);
              modf(arcLength/tessLength, &numTess);
              numTess += 2.0;
              tessLength = arcLength/(numTess-1.0);

              int numCircles = (int)numTess;
              int numVxes = (int)(nVxPrCircle*numTess);

              vxp->vertex.setNum(numVxes);

              // Simple vertexes

              SbVec3f* vxes = vxp->vertex.startEditing();
              for (j = 0, part = 0.0; part <= arcLength + 0.1*tessLength; part += tessLength)
                {
                  FaVec3 midpoint = curve.getPointOnArc(part);
                  FaVec3 tangent = curve.getTangent(part);
                  tangent.normalize();
                  FaVec3 side = tangent ^ up;
                  double angleInc = 2.0*M_PI/nVxPrCircle;
                  for (k = 0; k < nVxPrCircle; k++)
                    vxes[j++] = FdConverter::toSbVec3f(midpoint + r*cos(angleInc*k)*side + r*sin(angleInc*k)*up);
                }
              vxp->vertex.finishEditing();

              // Indexes :

              int idxesPrCircle = nVxPrCircle+1 + nLinesPrCircle*3;
              slines->coordIndex.setNum(nVxPrCircle+1 + (numCircles-1)*idxesPrCircle);
              sfaces->coordIndex.setNum((numCircles-1) * 5*nVxPrCircle);

              // Simple line indexes

              int32_t* slIdx = slines->coordIndex.startEditing();

              for (j = 0; j < numCircles-1; ++j)
                {
                  // Circle

                  for (n = 0; n < nVxPrCircle; ++n)
                    slIdx[idxesPrCircle*j + n] = j*nVxPrCircle + n;
                  slIdx[idxesPrCircle*j + nVxPrCircle] = -1;

                  // Axial lines

                  for (n = 0; n < nLinesPrCircle; ++n) {
                    int circleVxIdx = (nVxPrCircle*n)/nLinesPrCircle;
                    slIdx[idxesPrCircle*j + nVxPrCircle+1 + n*3  ] = j*nVxPrCircle + circleVxIdx;
                    slIdx[idxesPrCircle*j + nVxPrCircle+1 + n*3+1] = j*nVxPrCircle + circleVxIdx + nVxPrCircle;
                    slIdx[idxesPrCircle*j + nVxPrCircle+1 + n*3+2] = -1;
                  }
                }

              // Last circle

              for (n = 0; n < nVxPrCircle; ++n)
                slIdx[idxesPrCircle*j + n] = j * nVxPrCircle + n;
              slIdx[idxesPrCircle*j + nVxPrCircle] = -1;

              slines->coordIndex.finishEditing();

              // Simple Face Indexes

              int32_t* sfIdx = sfaces->coordIndex.startEditing();
              int idxesPrCylSeg = 5*nVxPrCircle;

              for (j = 0; j < numCircles-1; ++j) // For all CylSegments
                {
                  // For all Faces except the last

                  for (n = 0; n < nVxPrCircle-1; ++n) {
                    sfIdx[j*idxesPrCylSeg + n*5]    = (j    * nVxPrCircle) + n;
                    sfIdx[j*idxesPrCylSeg + n*5 +1] = (j    * nVxPrCircle) + n+1;
                    sfIdx[j*idxesPrCylSeg + n*5 +2] = (j+1) * nVxPrCircle  + n+1;
                    sfIdx[j*idxesPrCylSeg + n*5 +3] = (j+1) * nVxPrCircle  + n;
                    sfIdx[j*idxesPrCylSeg + n*5 +4] = -1;
                  }

                  // The last and closing face

                  sfIdx[j*idxesPrCylSeg + n*5]    = j    * nVxPrCircle + n;
                  sfIdx[j*idxesPrCylSeg + n*5 +1] = j    * nVxPrCircle;
                  sfIdx[j*idxesPrCylSeg + n*5 +2] = (j+1)* nVxPrCircle;
                  sfIdx[j*idxesPrCylSeg + n*5 +3] = (j+1)* nVxPrCircle + n;
                  sfIdx[j*idxesPrCylSeg + n*5 +4] = -1;
                }

              sfaces->coordIndex.finishEditing();
              slines->vertexProperty.setValue(vxp);
              sfaces->vertexProperty.setValue(vxp);

              lod->addChild(sShapeSep);
            }

          lod->addChild(cLineSep);
          list->addChild(arc);
        }
    }

  return true;
}


void FdPipeSurface::showHighlight()
{
  SoMaterial* hlMat = FdSymbolDefs::getHighlightMaterial();
  myLineMaterial->diffuseColor.copyFrom(hlMat->diffuseColor);
  myLineMaterial->ambientColor.copyFrom(hlMat->ambientColor);
  myLineMaterial->specularColor.copyFrom(hlMat->specularColor);
  myLineMaterial->emissiveColor.copyFrom(hlMat->emissiveColor);
}


void FdPipeSurface::hideHighlight()
{
  this->updateFdApperance();
}
