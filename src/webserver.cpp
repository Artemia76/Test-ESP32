/**
 * @file webserver.cpp
 * @author Artemia
 * @brief Test-ESP32 Project
 * @version 0.1
 * @date 2024-11-12
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

#include <algorithm>
#include <Arduino.h>
#include "webserver.hpp"

/*****************************************************************************/

CWebServerEvent::CWebServerEvent(CWebServer& pWebServer) : _webServer(pWebServer)
{
    _webServer._subscribe(this);
}

/*****************************************************************************/

CWebServerEvent::~CWebServerEvent()
{
    _webServer._unSubscribe(this);
}

/*****************************************************************************/

CWebServer::CWebServer (CLog& pLog) : _log (pLog),
    _server (80),
    _ssid (WIFI_SSID),
    _password (WIFI_PASSWORD)
{
    if(!SPIFFS.begin())
    {
        _log.Message ("Erreur SPIFFS...");
        return;
    }

    File root =  SPIFFS.open("/");
    File file  = root.openNextFile();

    while(file)
    {
        _log.Message ("file: " + String(file.name()));
        file.close();
        file = root.openNextFile();
    }

    WiFi.begin(_ssid, _password);
    _log.Message ("Connecting to " + String(_ssid));

    while (WiFi.status() != WL_CONNECTED)
    {
        _log.Message (".", false);
        delay(500);
    }

    // Print local IP address and start web server
    _log.Message ("\n");
    _log.Message ("WiFi connected.");
    _log.Message ("IP address: ");
    _log.Message (WiFi.localIP().toString());

    //--------------------------------------------SERVER
    _server.on("/",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html", "text/html");
    });

    _server.on("/w3.css",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/w3.css", "text/css");
    });

    _server.on("/script.js",HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/script.js", "text/javascript");
    });

    _server.on("/lireLuminosite",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetLuminosite (request);
    });
    

    _server.on("/on",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetON (request);
    });

    _server.on("/off",HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        this->_onGetOFF (request);
    });

    _server.on("/set",HTTP_POST, [this](AsyncWebServerRequest *request)
    {
        this->_onGetSet (request);
    });
    _server.begin();
    _log.Message ("Server Online.");
}

/*****************************************************************************/

CWebServer::~CWebServer ()
{
    _server.end();
}

/*****************************************************************************/

void CWebServer::_onGetON (AsyncWebServerRequest *request)
{
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onGET("on");
        }
    }
    request->send(200);
    #ifdef DEBUG
        _log.Message ("Received ON.");
    #endif
}

/*****************************************************************************/

void CWebServer::_onGetOFF (AsyncWebServerRequest *request)
{
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onGET("off");
        }
    }
    request->send(200);
    #ifdef DEBUG
        _log.Message ("Received ON.");
    #endif
}

/*****************************************************************************/

void CWebServer::_onGetLuminosite (AsyncWebServerRequest *request)
{
    String Value;
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Value= Subcriber->onGET("lireLuminosite");
        }
    }
    request->send(200,"text/plain", Value);
#ifdef DEBUG
    _log.Message ("Received lireLuminosite = " +luminosite);
#endif
}

/*****************************************************************************/

void CWebServer::_onGetSet (AsyncWebServerRequest *request)
{
    int paramsNr = request->params();
    AsyncWebParameter * j = request->getParam(0); // 1st parameter
    for (auto Subcriber : _subscribers)
    {
        if (Subcriber != nullptr)
        {
            Subcriber->onPOST("set",j->value());
        }
    }
    request->send(200);
#ifdef DEBUG
    _log.Message ("rgb: ");
    _log.Message (j->value());                     // value ^
    _log.Message ("");
#endif
}

/*****************************************************************************/

void CWebServer::_subscribe(CWebServerEvent* pSubscriber)
{
    if (std::find(_subscribers.begin(), _subscribers.end(),pSubscriber) == _subscribers.end())
    {
        _subscribers.push_back(pSubscriber);
    }
}

/*****************************************************************************/

void CWebServer::_unSubscribe(CWebServerEvent* pSubscriber)
{
    _subscribers.erase(std::remove(_subscribers.begin(), _subscribers.end(), pSubscriber), _subscribers.end());
}