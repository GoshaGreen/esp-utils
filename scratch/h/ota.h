#ifndef OTA_H
#define OTA_H

#ifndef DEVICE_NAME
#define DEVICE_NAME "device"
#endif // DEVICE_NAME

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include "./wifi.h"

int setupOTA() {
    ArduinoOTA.setPort(8266);
    ArduinoOTA.setHostname(DEVICE_NAME);
#ifdef OTA_PASSWORD
    ArduinoOTA.setPassword(OTA_PASSWORD);
#endif // OTA_PASSWORD
    ArduinoOTA.onStart([]() {
        dPrint("OTA: Started");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        dPrint(String("OTA: Progress: ") + (progress / (total / 100)) + "%%\r", false);
    });
    ArduinoOTA.onEnd([]() {
        dPrint("");
        dPrint("OTA: Finished");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        if (error == OTA_AUTH_ERROR) {
            dPrint("OTA: Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            dPrint("OTA: Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            dPrint("OTA: Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            dPrint("OTA: Receive Failed");
        } else if (error == OTA_END_ERROR) {
            dPrint("OTA: End Failed");
        }
        (void)error;
        ESP.restart();
    });
    ArduinoOTA.begin();
    return 0;
}

void loopOTA() {
    ArduinoOTA.handle();
}

#endif // OTA_H