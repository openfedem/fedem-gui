// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_SENSOR_H
#define FD_SENSOR_H

#include "vpmDisplay/FdBase.H"

class FmSensorBase;


class FdSensor : public FdObject
{
  Fmd_HEADER_INIT();

public:
  FdSensor(FmSensorBase* pt);

  virtual bool updateFdTopology(bool updateChildrenDisplay);
  virtual bool updateFdDetails();
  virtual bool updateFdApperance();

  virtual FaVec3 findSnapPoint(const SbVec3f& pointOnObject,
                               const SbMatrix& objToWorld,
                               SoDetail* pickDetail, SoPickedPoint* pPoint);

protected:
  virtual ~FdSensor();

  virtual SoNodeKitListPart* getListSw() const;
};

#endif
