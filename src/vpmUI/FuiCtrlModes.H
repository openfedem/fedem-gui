// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FUI_CTRL_MODES_H
#define FUI_CTRL_MODES_H


namespace FuiCtrlModes
{
  enum funcMode{NEUTRAL_MODE,
		CREATE_MODE,
		ROTATE_MODE,
		DELETE_MODE,
		ADDLINEPOINT_MODE,
		REMOVELINEPOINT_MODE,
		MOVEGROUP_MODE};

  enum neutralMode{EXAM_NEUTRAL,
		   MOVEELEM_NEUTRAL,
		   DRAW_NEUTRAL,
		   MOVELINE_NEUTRAL};

  void setCtrlModellerState(bool isOpen);
  bool isCtrlModellerOpen();

  void setMode(int newMode);
  int  getMode();

  void setState(int newState);
  int  getState();

  void setNeutralType(int newType);
  int  getNeutralType();

  void setNeutralState(int newState);
  int  getNeutralState();

  void done();
  void cancel();

  void setTip();
}

#endif
