#ifndef UTILS_H
#define UTILS_H


#ifndef DEVICE_NAME
#define DEVICE_NAME "device"
#endif // DEVICE_NAME

#include "./dprint.h"
#include "./eeprom_helpers.h"
#include "./wifi.h"
#include "./ota.h"

void utilsSetup() {
    int errCode = 0;

#ifndef SERIAL_DEBUG_DISABLE
    dPrintInit();
#endif // SERIAL_DEBUG_DISABLE

#ifndef TIMER_DISABLE
#endif // TIMER_DISABLE

#ifndef EEPROM_DISABLE
    errCode = setupEEPROM();
    if (errCode) {
        dPrint(String("EEPROM initialization error: ") + errCode);
    } else {
        dPrint(String("Total EEPROM : ")+String(eep_getTotalSize())+String(" Bytes") );
    }
#endif // EEPROM_DISABLE

#ifndef WIFI_DISABLE
    errCode = setupWifi();
    if (errCode) {
        dPrint(String("Wifi initialization error: ") + errCode);
    }
#endif // WIFI_DISABLE

#ifdef OTA
#endif // OTA

}

void utilsLoop() {
#ifndef WIFI_DISABLE
    loopWifi();
#endif // WIFI_DISABLE
// ESP.getFreeHeap()

}

#endif // UTILS_H
