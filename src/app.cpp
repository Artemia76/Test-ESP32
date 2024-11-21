/**
 * @file app.cpp
 * @author Artemia
 * @brief Test-ESP32 Project
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#include "app.hpp"

/*****************************************************************************/

CApp::CApp (CLog& pLog, CWebServer& pWebServer) :
    CWebServerEvent(pWebServer),
    _log(pLog),
    _red(127),
    _green(127),
    _blue(127),
    _switch(false),
    _lightSensor(10),
    _delayON(100),
    _pixels(1, 48, NEO_GRB + NEO_KHZ800)
{
    _log.Message("Starting TEST-ESP32");
    _pixels.begin();
    pinMode(_lightSensor, INPUT);
}

/*****************************************************************************/

CApp::~CApp ()
{
  _log.Message("Ending TEST-ESP32");
}

/*****************************************************************************/

void CApp::Loop ()
{
  _pixels.clear();
  _set_RGB();
  delay(_delayON);
}

/*****************************************************************************/

void CApp::_set_RGB ()
{
  _pixels.setPixelColor(
    0,
    _pixels.Color(
      _switch?static_cast<uint8_t>(_red):static_cast<uint8_t>(0),
      _switch?static_cast<uint8_t>(_green):static_cast<uint8_t>(0),
      _switch?static_cast<uint8_t>(_blue):static_cast<uint8_t>(0)
    )
  );
  _pixels.show();
}

/*****************************************************************************/

void CApp::_get_RGB (const String& pHexRGB)
{
  std::string HexValue = pHexRGB.c_str();
  std::regex pattern("#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})\\b");
  std::smatch match;
  if (std::regex_match(HexValue, match, pattern))
  {
      _red = static_cast<uint8_t>(std::stoi(match[1].str(), nullptr, 16));
      _green = static_cast<uint8_t>(std::stoi(match[2].str(), nullptr, 16));
      _blue = static_cast<uint8_t>(std::stoi(match[3].str(), nullptr, 16));
#ifdef DEBUG
      _log.Message("R=" + String(_red) + " V=" + String(_green) + " B=" + String(_blue));
#endif
  }
}

/*****************************************************************************/

String CApp::onGET (const String& pCommand)
{
  String Response;
  if (pCommand=="on")
  {
    _switch = true;
  } else if (pCommand=="off")
  {
    _switch = false;
  } else if (pCommand=="lireLuminosite")
  {
    Response = String(analogRead(_lightSensor));
  }
  return Response;
}

/*****************************************************************************/

void CApp::onPOST (const String& pCommand, const String& pData)
{
  if (pCommand == "set")
  {
    _get_RGB(pData);
  }
}