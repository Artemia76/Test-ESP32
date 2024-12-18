/**
 * @file main.cpp
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

#include <Arduino.h>
#include "app.hpp"
#include "webserver.hpp"
#include "log.hpp"

CLog* Logger;
CWebServer* WebServer;
CApp* Application;

/**
 * @brief Initialize MCU
 * 
 */

void setup()
{
  Logger = new CLog();
  WebServer = new CWebServer(*Logger);
  Application = new CApp (*Logger, *WebServer);
}

/**
 * @brief Main loop of MCU
 * 
 */

void loop()
{
  Application->Loop();
}
