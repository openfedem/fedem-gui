// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FD_SYMBOL_DEFS_H
#define FD_SYMBOL_DEFS_H

#include <array>

class SoMaterial;
class SoDrawStyle;
#ifdef USE_SMALLCHANGE
class SmDepthBuffer;
#endif
class FdSymbolKit;


namespace FdSymbolDefs
{
  enum SymbolType {
    COORD_SYST,
    LINK_COORD_SYS,
    INT_LINK_COORD_SYS,
    CENTER_OF_GRAVITY,
    FORCE,
    TORQUE,
    AXIAL_DAMPER,
    AXIAL_SPRING,
    SENSOR,

    S_SINGLE_GAGE,
    S_DOUBLE_GAGE_90,
    S_TRIPLE_GAGE_60,
    S_TRIPLE_GAGE_45,
    L_SINGLE_GAGE,
    L_DOUBLE_GAGE_90,
    L_TRIPLE_GAGE_60,
    L_TRIPLE_GAGE_45,

    HIGHER_PAIR,

    REVJOINT,
    REVJOINT_MASTER,
    REVJOINT_SLAVE,

    BALLJOINT,
    BALLJOINT_MASTER,
    BALLJOINT_SLAVE,

    LINJOINT_MASTER,
    LINJOINT_LINE,
    LINPRISM_SLAVE,
    LINCYL_SLAVE,

    FREEJOINT,
    FREEJOINT_SLAVE,
    FREEJOINT_LINE,

    RIGIDJOINT,
    RIGIDJOINT_MASTER,
    RIGIDJOINT_SLAVE,

    CAMJOINT_MASTER,
    CAMJOINT_SLAVE,

    STICKER,
    POINT,
    REFCS,

    SYMBOL_COUNT };

  enum MaterialType {
    HIGHLIGHT,
    DEFAULT,
    TRIAD,
    GROUND,
    JOINT,
    SPR_DMP,
    STICK,
    LOAD,
    HP,
    LINK_CS,
    REF_PLANE,
    SENSOR_MAT,
    STRAIN_ROS,
    MATERIAL_COUNT };

  void init();

  FdSymbolKit* getSymbol(SymbolType symbolIndex);
  SoMaterial* getMaterial(MaterialType materialIndex = DEFAULT);
  SoMaterial* getHighlightMaterial();

  float getSymbolScale();
  void setSymbolScale(float scale);
  void setSymbolLineWidth(int width);

  SoDrawStyle* getGlobalSymbolStyle();
  SoDrawStyle* getHighlightSymbolStyle();
#ifdef USE_SMALLCHANGE
  SmDepthBuffer* getHighlightDepthBMod();
#endif

  using FdColor = std::array<float,3>;

  void setDefaultColor(const FdColor& color);
  void setSensorColor(const FdColor& color);
  void setTriadColor(const FdColor& color);
  void setGndTriadColor(const FdColor& color);
  void setJointColor(const FdColor& color);
  void setSprDaColor(const FdColor& color);
  void setStickerColor(const FdColor& color);
  void setLoadColor(const FdColor& color);
  void setHPColor(const FdColor& color);
  void setLinkCoordSysColor(const FdColor& color);
  void setRefPlaneColor(const FdColor& color);
  void setStrainRosetteColor(const FdColor& color);
}

#endif
