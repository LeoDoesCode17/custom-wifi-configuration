#pragma once         // Include this header only once per build
#include <Arduino.h> // Needed for the Arduino `String` type and uint16_t
#include "core/type.h"

namespace wifi_config_manager
{
    bool start();                   // Turn on AP + web server. Returns false if the AP failed to start
    void stop();                    // Turn off web server + AP (safe to call even if not running)
    void update();                  // Must be called from loop(): lets the web server serve clients
    bool isActive();                // true while configuration mode is running
    bool hasSubmittedCredentials(); // true after the user submitted the web form
    String getApSsid();             // Name of the Wi-Fi network the ESP32 creates (for the LCD guide)
    String getApPassword();         // Password of that network (for the LCD guide)
    String getApIp();               // IP address of the web page (for the LCD guide)

    WifiConfig getWifiConfig(); // Everything received for Wi-Fi (replaces getReceivedSsid)
    MqttConfig getMqttConfig(); // Everything received for the MQTT broker
}