/**
 * @file app.hpp
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

#ifndef APP_HPP
#define APP_HPP

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <atomic>

#include "webserver.hpp"
#include "log.hpp"

/**
 * @brief Main application class
 * 
 */

class CApp : public CWebServerEvent 
{
    friend class CWebServerEvent;
public:
    /**
     * @brief Construct a new Application object
     * 
     * @param pLog Text log handler
     * @param pwebServer Web Server to subscribe events
     */
    CApp(CLog& pLog, CWebServer& pWebServer);

    /**
     * @brief Destroy the main application object
     * 
     */
    ~CApp();

    void Loop ();

protected:

    /**
     * @brief Event from web server on GET command
     * 
     * @param pCommand 
     * @return String 
     */
    String onGET(const String& pCommand);

    /**
     * @brief Event from web server on POST command
     * 
     * @param pCommand 
     * @param pData 
     */
    void onPOST(const String& pCommand, const String& pData);

private:

    /**
     * @brief Set RGB Led Color
     * 
     */
    void _set_RGB ();

    /**
     * @brief decode RGB HTML String to R,G,B value
     * 
     * @param pHexRGB 
     */
    void _get_RGB (const String& pHexRGB);

    /**
     * @brief Neo RGB led object
     * 
     */
    Adafruit_NeoPixel _pixels;

    /**
     * @brief Switch LED on or off
     * 
     */
    std::atomic<bool> _switch;

    /**
     * @brief RGB component
     * 
     */
    std::atomic<uint8_t> _red;
    std::atomic<uint8_t> _green;
    std::atomic<uint8_t> _blue;

    /**
     * @brief PIN number of light sensor
     * 
     */
    const uint8_t _lightSensor;

    /**
     * @brief delay to wait on main loop
     * 
     */
    const int _delayON;
    
    /**
     * @brief Log handler to write message on terminal
     * 
     */
    CLog& _log;
};

#endif