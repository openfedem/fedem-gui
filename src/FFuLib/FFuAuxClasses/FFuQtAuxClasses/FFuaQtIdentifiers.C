// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <qnamespace.h>

#include "FFuLib/FFuAuxClasses/FFuaIdentifiers.H"


namespace FFuaKeyCode
{
  const int ShiftAccel	= Qt::SHIFT;
  const int CtrlAccel	= Qt::CTRL;
  const int AltAccel	= Qt::ALT;

  const int Escape	= Qt::Key_Escape;
  const int Tab		= Qt::Key_Tab;
  const int Backtab	= Qt::Key_Backtab;
  const int Backspace	= Qt::Key_Backspace;
  const int Return	= Qt::Key_Return;
  const int Enter	= Qt::Key_Enter;
  const int Insert	= Qt::Key_Insert;
  const int Delete	= Qt::Key_Delete;
  const int Pause	= Qt::Key_Pause;
  const int Print	= Qt::Key_Print;
  const int SysReq	= Qt::Key_SysReq;

  const int Home	= Qt::Key_Home;
  const int End		= Qt::Key_End;
  const int Left	= Qt::Key_Left;
  const int Up		= Qt::Key_Up;
  const int Right	= Qt::Key_Right;
  const int Down	= Qt::Key_Down;
  const int Prior	= Qt::Key_PageUp;
  const int PageUp	= Qt::Key_PageUp;
  const int Next	= Qt::Key_PageDown;
  const int PageDown	= Qt::Key_PageDown;

  const int Shift	= Qt::Key_Shift;
  const int Control	= Qt::Key_Control;
  const int Meta	= Qt::Key_Meta;
  const int Alt		= Qt::Key_Alt;
  const int CapsLock	= Qt::Key_CapsLock;
  const int NumLock	= Qt::Key_NumLock;
  const int ScrollLock	= Qt::Key_ScrollLock;

  const int F1	= Qt::Key_F1;
  const int F2	= Qt::Key_F2;
  const int F3	= Qt::Key_F3;
  const int F4	= Qt::Key_F4;
  const int F5	= Qt::Key_F5;
  const int F6	= Qt::Key_F6;
  const int F7	= Qt::Key_F7;
  const int F8	= Qt::Key_F8;
  const int F9	= Qt::Key_F9;
  const int F10	= Qt::Key_F10;
  const int F11	= Qt::Key_F11;
  const int F12	= Qt::Key_F12;
  const int F13	= Qt::Key_F13;
  const int F14	= Qt::Key_F14;
  const int F15	= Qt::Key_F15;
  const int F16	= Qt::Key_F16;
  const int F17	= Qt::Key_F17;
  const int F18	= Qt::Key_F18;
  const int F19	= Qt::Key_F19;
  const int F20	= Qt::Key_F20;
  const int F21	= Qt::Key_F21;
  const int F22	= Qt::Key_F22;
  const int F23	= Qt::Key_F23;
  const int F24	= Qt::Key_F24;
  const int F25	= Qt::Key_F25;
  const int F26	= Qt::Key_F26;
  const int F27	= Qt::Key_F27;
  const int F28	= Qt::Key_F28;
  const int F29	= Qt::Key_F29;
  const int F30	= Qt::Key_F30;
  const int F31	= Qt::Key_F31;
  const int F32	= Qt::Key_F32;
  const int F33	= Qt::Key_F33;
  const int F34	= Qt::Key_F34;
  const int F35	= Qt::Key_F35;

  const int Super_L 	= Qt::Key_Super_L;
  const int Super_R 	= Qt::Key_Super_R;
  const int Menu 	= Qt::Key_Menu;

  const int Space	= Qt::Key_Space;
  const int Exclam	= Qt::Key_Exclam;
  const int QuoteDbl	= Qt::Key_QuoteDbl;
  const int NumberSign	= Qt::Key_NumberSign;
  const int Dollar	= Qt::Key_Dollar;
  const int Percent	= Qt::Key_Percent;
  const int Ampersand	= Qt::Key_Ampersand;
  const int Apostrophe	= Qt::Key_Apostrophe;
  const int ParenLeft	= Qt::Key_ParenLeft;
  const int ParenRight	= Qt::Key_ParenRight;
  const int Asterisk	= Qt::Key_Asterisk;
  const int Plus	= Qt::Key_Plus;
  const int Comma	= Qt::Key_Comma;
  const int Minus	= Qt::Key_Minus;
  const int Period	= Qt::Key_Period;
  const int Slash	= Qt::Key_Slash;
  const int key_0	= Qt::Key_0;
  const int key_1	= Qt::Key_1;
  const int key_2	= Qt::Key_2;
  const int key_3	= Qt::Key_3;
  const int key_4	= Qt::Key_4;
  const int key_5	= Qt::Key_5;
  const int key_6	= Qt::Key_6;
  const int key_7	= Qt::Key_7;
  const int key_8	= Qt::Key_8;
  const int key_9	= Qt::Key_9;
  const int Colon	= Qt::Key_Colon;
  const int Semicolon	= Qt::Key_Semicolon;
  const int Less	= Qt::Key_Less;
  const int Equal	= Qt::Key_Equal;
  const int Greater	= Qt::Key_Greater;
  const int Question	= Qt::Key_Question;
  const int At	= Qt::Key_At;
  const int A	= Qt::Key_A;
  const int B	= Qt::Key_B;
  const int C	= Qt::Key_C;
  const int D	= Qt::Key_D;
  const int E	= Qt::Key_E;
  const int F	= Qt::Key_F;
  const int G	= Qt::Key_G;
  const int H	= Qt::Key_H;
  const int I	= Qt::Key_I;
  const int J	= Qt::Key_J;
  const int K	= Qt::Key_K;
  const int L	= Qt::Key_L;
  const int M	= Qt::Key_M;
  const int N	= Qt::Key_N;
  const int O	= Qt::Key_O;
  const int P	= Qt::Key_P;
  const int Q	= Qt::Key_Q;
  const int R	= Qt::Key_R;
  const int S	= Qt::Key_S;
  const int T	= Qt::Key_T;
  const int U	= Qt::Key_U;
  const int V	= Qt::Key_V;
  const int W	= Qt::Key_W;
  const int X	= Qt::Key_X;
  const int Y	= Qt::Key_Y;
  const int Z	= Qt::Key_Z;
  const int BracketLeft	= Qt::Key_BracketLeft;
  const int Backslash	= Qt::Key_Backslash;
  const int BracketRight	= Qt::Key_BracketRight;
  const int AsciiCircum	= Qt::Key_AsciiCircum;
  const int Underscore	= Qt::Key_Underscore;
  const int QuoteLeft	= Qt::Key_QuoteLeft;
  const int BraceLeft	= Qt::Key_BraceLeft;
  const int Bar		= Qt::Key_Bar;
  const int BraceRight	= Qt::Key_BraceRight;
  const int AsciiTilde	= Qt::Key_AsciiTilde;

  const int nobreakspace	= Qt::Key_nobreakspace;
  const int exclamdown	= Qt::Key_exclamdown;
  const int cent	= Qt::Key_cent;
  const int sterling	= Qt::Key_sterling;
  const int currency	= Qt::Key_currency;
  const int yen		= Qt::Key_yen;
  const int brokenbar	= Qt::Key_brokenbar;
  const int section	= Qt::Key_section;
  const int diaeresis	= Qt::Key_diaeresis;
  const int copyright	= Qt::Key_copyright;
  const int ordfeminine	= Qt::Key_ordfeminine;
  const int guillemotleft	= Qt::Key_guillemotleft;
  const int notsign	= Qt::Key_notsign;
  const int hyphen	= Qt::Key_hyphen;
  const int registered	= Qt::Key_registered;
  const int macron	= Qt::Key_macron;
  const int degree	= Qt::Key_degree;
  const int plusminus	= Qt::Key_plusminus;
  const int twosuperior	= Qt::Key_twosuperior;
  const int threesuperior	= Qt::Key_threesuperior;
  const int acute	= Qt::Key_acute;
#if QT_VERSION < QT_VERSION_CHECK(6,7,0)
  const int mu		= Qt::Key_mu;
#else
  const int mu		= Qt::Key_micro;
#endif
  const int paragraph	= Qt::Key_paragraph;
  const int periodcentered	= Qt::Key_periodcentered;
  const int cedilla	= Qt::Key_cedilla;
  const int onesuperior	= Qt::Key_onesuperior;
  const int masculine	= Qt::Key_masculine;
  const int guillemotright	= Qt::Key_guillemotright;
  const int onequarter	= Qt::Key_onequarter;
  const int onehalf	= Qt::Key_onehalf;
  const int threequarters	= Qt::Key_threequarters;
  const int questiondown	= Qt::Key_questiondown;
  const int Agrave	= Qt::Key_Agrave;
  const int Aacute	= Qt::Key_Aacute;
  const int Acircumflex	= Qt::Key_Acircumflex;
  const int Atilde	= Qt::Key_Atilde;
  const int Adiaeresis	= Qt::Key_Adiaeresis;
  const int Aring	= Qt::Key_Aring;
  const int AE		= Qt::Key_AE;
  const int Ccedilla	= Qt::Key_Ccedilla;
  const int Egrave	= Qt::Key_Egrave;
  const int Eacute	= Qt::Key_Eacute;
  const int Ecircumflex	= Qt::Key_Ecircumflex;
  const int Ediaeresis	= Qt::Key_Ediaeresis;
  const int Igrave	= Qt::Key_Igrave;
  const int Iacute	= Qt::Key_Iacute;
  const int Icircumflex	= Qt::Key_Icircumflex;
  const int Idiaeresis	= Qt::Key_Idiaeresis;
  const int ETH		= Qt::Key_ETH;
  const int Ntilde	= Qt::Key_Ntilde;
  const int Ograve	= Qt::Key_Ograve;
  const int Oacute	= Qt::Key_Oacute;
  const int Ocircumflex	= Qt::Key_Ocircumflex;
  const int Otilde	= Qt::Key_Otilde;
  const int Odiaeresis	= Qt::Key_Odiaeresis;
  const int multiply	= Qt::Key_multiply;
  const int Ooblique	= Qt::Key_Ooblique;
  const int Ugrave	= Qt::Key_Ugrave;
  const int Uacute	= Qt::Key_Uacute;
  const int Ucircumflex	= Qt::Key_Ucircumflex;
  const int Udiaeresis	= Qt::Key_Udiaeresis;
  const int Yacute	= Qt::Key_Yacute;
  const int THORN	= Qt::Key_THORN;
  const int ssharp	= Qt::Key_ssharp;
  const int agrave	= Qt::Key_Agrave;
  const int aacute	= Qt::Key_Aacute;
  const int acircumflex	= Qt::Key_Acircumflex;
  const int atilde	= Qt::Key_Atilde;
  const int adiaeresis	= Qt::Key_Adiaeresis;
  const int aring	= Qt::Key_Aring;
  const int ae		= Qt::Key_AE;
  const int ccedilla	= Qt::Key_Ccedilla;
  const int egrave	= Qt::Key_Egrave;
  const int eacute	= Qt::Key_Eacute;
  const int ecircumflex	= Qt::Key_Ecircumflex;
  const int ediaeresis	= Qt::Key_Ediaeresis;
  const int igrave	= Qt::Key_Igrave;
  const int iacute	= Qt::Key_Iacute;
  const int icircumflex	= Qt::Key_Icircumflex;
  const int idiaeresis	= Qt::Key_Idiaeresis;
  const int eth		= Qt::Key_ETH;
  const int ntilde	= Qt::Key_Ntilde;
  const int ograve	= Qt::Key_Ograve;
  const int oacute	= Qt::Key_Oacute;
  const int ocircumflex	= Qt::Key_Ocircumflex;
  const int otilde	= Qt::Key_Otilde;
  const int odiaeresis	= Qt::Key_Odiaeresis;
  const int division	= Qt::Key_division;
  const int oslash	= Qt::Key_Ooblique;
  const int ugrave	= Qt::Key_Ugrave;
  const int uacute	= Qt::Key_Uacute;
  const int ucircumflex	= Qt::Key_Ucircumflex;
  const int udiaeresis	= Qt::Key_Udiaeresis;
  const int yacute	= Qt::Key_Yacute;
  const int thorn	= Qt::Key_THORN;
  const int ydiaeresis	= Qt::Key_ydiaeresis;

  const int unknown	= Qt::Key_unknown;
}


namespace FFuaOrientation
{
  const int Horizontal = Qt::Horizontal;
  const int Vertical = Qt::Vertical;
}


namespace FFuaTextAlignment
{
  const int AlignAuto = Qt::AlignLeft;
  const int AlignLeft = Qt::AlignLeft;
  const int AlignRight = Qt::AlignRight;
  const int AlignHCenter = Qt::AlignHCenter;
  const int AlignJustify = Qt::AlignJustify;
  const int AlignTop = Qt::AlignTop;
  const int AlignBottom = Qt::AlignBottom;
  const int AlignVCenter = Qt::AlignVCenter;
  const int AlignCenter = Qt::AlignCenter;
  const int AlignHorizontal_Mask = Qt::AlignHorizontal_Mask;
  const int AlignVertical_Mask = Qt::AlignVertical_Mask;
}
