#include "SmartHomeServerClientWifi.h"

void SmartHomeServerClientWifiClass::scheduleReconnect()
{
    currentState = SmartHomeServer_WIFI_CONNECT;
    currentStateChangedAt = millis();
}

bool SmartHomeServerClientWifiClass::hasMessage()
{
    return currentState == SmartHomeServer_MESSAGE_RECEIVED;
}

void SmartHomeServerClientWifiClass::markMessageConsumed()
{
    currentState = SmartHomeServer_READING_DATA;
    currentStateChangedAt = millis();
}


void SmartHomeServerClientWifiClass::run()
{
    if (millis() - lastMsgFromServerAt > WIFI_RESET_IF_NO_MSG_FROM_SERVER_FOR)
    {
        Log.log("No msg from server, resetting wifi");
        currentState = SmartHomeServer_WIFI_CONNECT;
        currentStateChangedAt = millis();
        lastMsgFromServerAt = millis();
    }

    switch (currentState)
    {
    case SmartHomeServer_INIT:
    {

        // pinMode(NINA_RESETN, OUTPUT);
        // digitalWrite(NINA_RESETN, 1);
        // delay(100);
        // digitalWrite(NINA_RESETN, 0);
        // delay(100);

        if (WiFi.status() == WL_NO_MODULE)
        {
            Log.log("No wifi module detected");
            delay(10000);
            return;
        }

        static const char *expectedFirmeware = WIFI_FIRMWARE_LATEST_VERSION;
        static const char *actualFirmware = WiFi.firmwareVersion();

        if (strcmp(expectedFirmeware, actualFirmware) != 0)
        {
            char buf[1024];
            snprintf(buf, sizeof(buf), "Firmware upgrade needed: expected: %s, actual: %s", expectedFirmeware, actualFirmware);
            Log.log(buf);
            delay(10000);
            return;
        }

        currentState = SmartHomeServer_WIFI_CONNECT;
        currentStateChangedAt = millis();

        break;
    }
    case SmartHomeServer_WIFI_CONNECT:
    {
        WiFi.disconnect();
        currentState = SmartHomeServer_WIFI_CONNECTING;
        currentStateChangedAt = millis();
        break;
    }
    case SmartHomeServer_WIFI_CONNECTING:
        if (WiFi.begin(WIFI_SSID, WIFI_PASS) == WL_CONNECTED)
        {
            Log.log("Wifi Connected");

            // WiFi.localIP() hangs
            // IPAddress localIP = WiFi.localIP();
            // char buf[255];
            // sniprintf(buf, sizeof(buf), "SmartHomeServerClientWifi - wifi connected. Listening on: %u.%u.%u.%u:%u", localIP[0], localIP[1], localIP[2], localIP[3], LOCAL_UDP_PORT);
            // Log.log(buf);

            if (Udp.begin(LOCAL_UDP_PORT))
            {
                Log.log("Listening on UDP");
                currentState = SmartHomeServer_READING_DATA;
                currentStateChangedAt = millis();
            }
            else
            {
                Log.log("Udp.begin() - error");
            }
        }

        break;
    case SmartHomeServer_MESSAGE_RECEIVED:
        if (receivedMessageLengh == 2)
        {
            Log.log("Handling UDP request");
            uint16_t value = toUint16_t(receivedMessage, 0);
            DacDfr0971.setDacMillivoltage(value);
            bool sendResult = true;

            // send it
            uint8_t buf[2];
            writeUint16(DacDfr0971.lastSentValue, buf, 0);

            Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
            Udp.write(buf, sizeof(buf));
            Udp.endPacket();

            lastMsgFromServerAt = millis();
        }
        else
        {
            Log.log("Ignoring UDP msg");
        }

        markMessageConsumed();

        break;
    case SmartHomeServer_READING_DATA:
        if (Udp.parsePacket())
        {
            IPAddress remoteIp = Udp.remoteIP();
            uint16_t remotePort = Udp.remotePort();
            char buf[255];
            sniprintf(buf, sizeof(buf), "Received msg from: %u.%u.%u.%u:%u", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3], remotePort);
            Log.log(buf);

            receivedMessageLengh = Udp.read(receivedMessage, sizeof(receivedMessage));
            currentState = SmartHomeServer_MESSAGE_RECEIVED;
            currentStateChangedAt = millis();
        }
        break;

    default:
        break;
    }
}


SmartHomeServerClientWifiClass SmartHomeServerClientWifi;
