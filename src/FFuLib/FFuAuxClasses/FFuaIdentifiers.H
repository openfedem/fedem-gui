// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#ifndef FFUA_IDENTIFIERS_H
#define FFUA_IDENTIFIERS_H


/*!
  Ensure that these variables never have the same name as other
  Qt definitions (in qkeycode.h), even for those who are variables
  in qkeycode.h (accel modifiers).
*/

namespace FFuaKeyCode
{
  // Accelerator modifiers
  extern const int ShiftAccel;
  extern const int CtrlAccel;
  extern const int AltAccel;

  // Misc. keys
  extern const int Escape;
  extern const int Tab;
  extern const int Backtab;
  extern const int Backspace;
  extern const int Return;
  extern const int Enter;
  extern const int Insert;
  extern const int Delete;
  extern const int Pause;
  extern const int Print;
  extern const int SysReq;

  // Cursor movement
  extern const int Home;
  extern const int End;
  extern const int Left;
  extern const int Up;
  extern const int Right;
  extern const int Down;
  extern const int Prior;
  extern const int PageUp;
  extern const int Next;
  extern const int PageDown;

  // Modifiers
  extern const int Shift;
  extern const int Control;
  extern const int Meta;
  extern const int Alt;
  extern const int CapsLock;
  extern const int NumLock;
  extern const int ScrollLock; // see translateKeyEvent()

  // Function keys
  extern const int F1;
  extern const int F2;
  extern const int F3;
  extern const int F4;
  extern const int F5;
  extern const int F6;
  extern const int F7;
  extern const int F8;
  extern const int F9;
  extern const int F10;
  extern const int F11;
  extern const int F12;
  extern const int F13;
  extern const int F14;
  extern const int F15;
  extern const int F16;
  extern const int F17;
  extern const int F18;
  extern const int F19;
  extern const int F20;
  extern const int F21;
  extern const int F22;
  extern const int F23;
  extern const int F24;
  // F25...F35 only on X11
  extern const int F25;
  extern const int F26;
  extern const int F27;
  extern const int F28;
  extern const int F29;
  extern const int F30;
  extern const int F31;
  extern const int F32;
  extern const int F33;
  extern const int F34;
  extern const int F35;

  // Extra keys
  extern const int Super_L;
  extern const int Super_R;
  extern const int Menu;

  // 7 bit printable ASCII
  extern const int Space;
  extern const int Exclam;
  extern const int QuoteDbl;
  extern const int NumberSign;
  extern const int Dollar;
  extern const int Percent;
  extern const int Ampersand;
  extern const int Apostrophe;
  extern const int ParenLeft;
  extern const int ParenRight;
  extern const int Asterisk;
  extern const int Plus;
  extern const int Comma;
  extern const int Minus;
  extern const int Period;
  extern const int Slash;
  extern const int key_0;
  extern const int key_1;
  extern const int key_2;
  extern const int key_3;
  extern const int key_4;
  extern const int key_5;
  extern const int key_6;
  extern const int key_7;
  extern const int key_8;
  extern const int key_9;
  extern const int Colon;
  extern const int Semicolon;
  extern const int Less;
  extern const int Equal;
  extern const int Greater;
  extern const int Question;
  extern const int At;
  extern const int A;
  extern const int B;
  extern const int C;
  extern const int D;
  extern const int E;
  extern const int F;
  extern const int G;
  extern const int H;
  extern const int I;
  extern const int J;
  extern const int K;
  extern const int L;
  extern const int M;
  extern const int N;
  extern const int O;
  extern const int P;
  extern const int Q;
  extern const int R;
  extern const int S;
  extern const int T;
  extern const int U;
  extern const int V;
  extern const int W;
  extern const int X;
  extern const int Y;
  extern const int Z;
  extern const int BracketLeft;
  extern const int Backslash;
  extern const int BracketRight;
  extern const int AsciiCircum;
  extern const int Underscore;
  extern const int QuoteLeft;
  extern const int BraceLeft;
  extern const int Bar;
  extern const int BraceRight;
  extern const int AsciiTilde;

  // Latin1 codes adapted from X: keysymdef.h,v 1.21 94/08/28 16:17:06

  extern const int nobreakspace;
  extern const int exclamdown;
  extern const int cent;
  extern const int sterling;
  extern const int currency;
  extern const int yen;
  extern const int brokenbar;
  extern const int section;
  extern const int diaeresis;
  extern const int copyright;
  extern const int ordfeminine;
  extern const int guillemotleft; // left angle quotation mark
  extern const int notsign;
  extern const int hyphen;
  extern const int registered;
  extern const int macron;
  extern const int degree;
  extern const int plusminus;
  extern const int twosuperior;
  extern const int threesuperior;
  extern const int acute;
  extern const int mu;
  extern const int paragraph;
  extern const int periodcentered;
  extern const int cedilla;
  extern const int onesuperior;
  extern const int masculine;
  extern const int guillemotright; // right angle quotation mark
  extern const int onequarter;
  extern const int onehalf;
  extern const int threequarters;
  extern const int questiondown;
  extern const int Agrave;
  extern const int Aacute;
  extern const int Acircumflex;
  extern const int Atilde;
  extern const int Adiaeresis;
  extern const int Aring;
  extern const int AE;
  extern const int Ccedilla;
  extern const int Egrave;
  extern const int Eacute;
  extern const int Ecircumflex;
  extern const int Ediaeresis;
  extern const int Igrave;
  extern const int Iacute;
  extern const int Icircumflex;
  extern const int Idiaeresis;
  extern const int ETH;
  extern const int Ntilde;
  extern const int Ograve;
  extern const int Oacute;
  extern const int Ocircumflex;
  extern const int Otilde;
  extern const int Odiaeresis;
  extern const int multiply;
  extern const int Ooblique;
  extern const int Ugrave;
  extern const int Uacute;
  extern const int Ucircumflex;
  extern const int Udiaeresis;
  extern const int Yacute;
  extern const int THORN;
  extern const int ssharp;
  extern const int agrave;
  extern const int aacute;
  extern const int acircumflex;
  extern const int atilde;
  extern const int adiaeresis;
  extern const int aring;
  extern const int ae;
  extern const int ccedilla;
  extern const int egrave;
  extern const int eacute;
  extern const int ecircumflex;
  extern const int ediaeresis;
  extern const int igrave;
  extern const int iacute;
  extern const int icircumflex;
  extern const int idiaeresis;
  extern const int eth;
  extern const int ntilde;
  extern const int ograve;
  extern const int oacute;
  extern const int ocircumflex;
  extern const int otilde;
  extern const int odiaeresis;
  extern const int division;
  extern const int oslash;
  extern const int ugrave;
  extern const int uacute;
  extern const int ucircumflex;
  extern const int udiaeresis;
  extern const int yacute;
  extern const int thorn;
  extern const int ydiaeresis;

  extern const int unknown;
}


namespace FFuaOrientation
{
  extern const int Horizontal;
  extern const int Vertical;
}


namespace FFuaTextAlignment
{
  // Horizontal flags
  extern const int AlignAuto;
  extern const int AlignLeft;
  extern const int AlignRight;
  extern const int AlignHCenter;
  extern const int AlignJustify;

  // Vertical flags
  extern const int AlignTop;
  extern const int AlignBottom;
  extern const int AlignVCenter;

  // 2D flag
  extern const int AlignCenter;

  // Masks
  extern const int AlignHorizontal_Mask;
  extern const int AlignVertical_Mask;
}

#endif
