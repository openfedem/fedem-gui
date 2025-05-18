// SPDX-FileCopyrightText: 2023 SAP SE
//
// SPDX-License-Identifier: Apache-2.0
//
// This file is part of FEDEM - https://openfedem.org
////////////////////////////////////////////////////////////////////////////////

#include <codecvt>
#include <locale>
#include <string>


std::string CaConvert(const wchar_t* zStr)
{
  using cwt = std::codecvt_utf8_utf16<wchar_t>;
  std::wstring_convert<cwt> converter;
  return converter.to_bytes(std::wstring(zStr));
}
