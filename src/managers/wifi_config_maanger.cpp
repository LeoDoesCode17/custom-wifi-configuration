#include "wifi_config_manager.h"
#include <WiFi.h>      // WiFi.mode(), WiFi.softAP(), WiFi.softAPIP()
#include <WebServer.h> // Simple HTTP server (built into the ESP32 Arduino core)

namespace
{
    // --- Configuration constants -------------------------------------------
    const char *AP_SSID = "ESP32_IoT_Setup"; // Network name the user will see on their phone (later use mac address)
    const char *AP_PASSWORD = "12345678";    // WPA2 password (must be at least 8 characters)

    // --- State ---------------------------------------------------------------
    WebServer server(80);            // HTTP server listening on port 80
    bool active = false;             // Is configuration mode running?
    bool submitted = false;          // Did the user submit the form?
    bool handlersRegistered = false; // Make sure we register URL handlers only once
    String receivedSsid;             // SSID typed in the web form
    String receivedPassword;         // Password typed in the web form

    // The web page, stored in flash (PROGMEM) instead of RAM.
    // R"rawliteral( ... )rawliteral" is a raw string: no need to escape quotes.
    const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Wi-Fi Setup</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 40px auto; max-width: 400px; padding: 0 20px; background: #f4f4f9; }
    input { width: 100%; padding: 10px; margin: 8px 0 16px; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }
    button { width: 100%; background-color: #007BFF; color: white; padding: 12px; border: none; border-radius: 4px; font-size: 16px; }
  </style>
</head>
<body>
  <h2>Wi-Fi Configuration</h2>
  <form action="/save" method="POST">
    <label for="ssid">Wi-Fi SSID:</label>
    <input type="text" id="ssid" name="ssid" required>
    <label for="pass">Wi-Fi Password:</label>
    <input type="password" id="pass" name="pass">
    <button type="submit">Save</button>
  </form>
</body>
</html>
)rawliteral";

    // Called when a browser requests "GET /"  -> send the form page
    void handleRoot()
    {
        server.send_P(200, "text/html", INDEX_HTML); // 200 = OK; send_P sends data stored in PROGMEM
    }

    // Called when the form is submitted: "POST /save"
    void handleSave()
    {
        // Reject the request if the SSID field is missing or empty
        if (!server.hasArg("ssid") || server.arg("ssid").length() == 0)
        {
            server.send(400, "text/plain", "Missing SSID"); // 400 = Bad Request
            return;                                         // Stop here, nothing to save
        }

        receivedSsid = server.arg("ssid");     // Read the "ssid" form field
        receivedPassword = server.arg("pass"); // Read the "pass" form field (may be empty for open networks)

        // For now we only print the credentials to Serial (no WiFi.begin yet)
        Serial.println("[WifiConfig] Credentials received:");
        Serial.print("  SSID    : ");
        Serial.println(receivedSsid);
        Serial.print("  Password: ");
        Serial.println(receivedPassword); // Debug only: remove once you store these securely

        submitted = true; // Tell the UI that the form was submitted

        // Reply to the browser so the user knows it worked
        server.send(200, "text/html",
                    "<h2>Credentials received!</h2><p>You can close this page.</p>");
    }
}

namespace wifi_config_manager
{
    bool start()
    {
        if (active) // Already running? Nothing to do
        {
            return true;
        }

        submitted = false; // Reset the state from any previous session
        receivedSsid = "";
        receivedPassword = "";

        WiFi.mode(WIFI_AP); // Access Point mode: the ESP32 creates its own network

        if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) // Start the AP; returns false on failure
        {
            WiFi.mode(WIFI_OFF); // Clean up the radio
            return false;        // Let the caller (UI) show an error
        }

        if (!handlersRegistered)                       // Register URL routes only on the first start,
        {                                              // otherwise they would be duplicated every time
            server.on("/", HTTP_GET, handleRoot);      // GET  /      -> show form
            server.on("/save", HTTP_POST, handleSave); // POST /save  -> receive form data
            handlersRegistered = true;
        }

        server.begin(); // Start listening for HTTP clients
        active = true;  // Mark configuration mode as running

        Serial.print("[WifiConfig] AP started: ");
        Serial.print(AP_SSID);
        Serial.print("  IP: ");
        Serial.println(WiFi.softAPIP()); // Default AP IP is 192.168.4.1
        return true;
    }

    void stop()
    {
        if (!active) // Not running? Nothing to stop
        {
            return;
        }

        server.stop();               // Stop listening for HTTP clients
        WiFi.softAPdisconnect(true); // Shut down the access point
        WiFi.mode(WIFI_OFF);         // Turn the Wi-Fi radio off
        active = false;              // Mark configuration mode as stopped
        Serial.println("[WifiConfig] Stopped");
    }

    void update()
    {
        if (active) // Only serve clients while config mode is on
        {
            server.handleClient(); // Process any pending HTTP request (non-blocking)
        }
    }

    bool isActive() { return active; }
    bool hasSubmittedCredentials() { return submitted; }
    String getApSsid() { return String(AP_SSID); }
    String getApPassword() { return String(AP_PASSWORD); }
    String getApIp() { return WiFi.softAPIP().toString(); }
    String getReceivedSsid() { return receivedSsid; }
}