#ifndef WIFI_H
#define WIFI_H

#ifndef DEVICE_NAME
#define DEVICE_NAME "couldNotFindAp"
#endif // DEVICE_NAME

#ifndef WIFI_CONNECTION_TIMOUT_SEC
#define WIFI_CONNECTION_TIMOUT_SEC 10
#endif // WIFI_CONNECTION_TIMOUT_SEC

#ifndef WIFI_EEPROM_ID
#define WIFI_EEPROM_ID 0
#endif // WIFI_EEPROM_ID

#ifndef MAX_SSIDS
#define MAX_SSIDS 4
#endif // MAX_SSIDS

#include "./dprint.h"
#include "./eeprom_helpers.h"

#include <ESP8266WiFiMulti.h>
#include <EEPROM.h>

#ifndef WIFI_NOT_USE_AP
    #include <WiFiClient.h>
    #include <ESP8266WebServer.h>
    bool apEnabled = false;
    ESP8266WebServer serverAp(80);
#endif // WIFI_NOT_USE_AP

#ifndef SSID_EEPROM_SIZE
#define SSID_EEPROM_SIZE 256
#endif // SSID_EEPROM_SIZE

typedef struct {
    String name;
    String pwd;
} SSID;

int setupWifi();
void loopWifi();
int wifi_clearSSIDs();
int wifi_appendSSID(String name, String pwd);
int wifi_updateMulti();
int wifi_popSSIDs();
int wifi_pushSSIDs();

inline void dPrint(const SSID ssid);

enum WIFI_ERR_CODE {
    WIFI_SUCCESS = 0,
    WIFI_CANNOT_READ_EEPROM_DATA = -1,
    WIFI_CANNOT_WRITE_EEPROM_DATA = -2,
    WIFI_SSID_STORAGE_FULL = -3,
    WIFI_EEPROM_ALLOCATION = -4,
    WIFI_READ_EEPROM_DATA = -5,
    WIFI_CANNOT_INIT_EEPROM = -6,
    WIFI_CANNOT_SWITCH_MODE = -7,
};

ESP8266WiFiMulti wifiMulti;

SSID ssidStorage[MAX_SSIDS];
byte ssidNum = 0;
epp_record wifi_record;

int setupWifi() {
    int errCode = 0;
    errCode = setupEEPROM();
    if (errCode) {
        dPrint(String("Cannot capture error"));
        return WIFI_CANNOT_INIT_EEPROM;
    }
    errCode = eep_getAllocation(WIFI_EEPROM_ID, 0, wifi_record);
    if (EEP_SUCCESS != errCode) {
        dPrint("Cannot allocate eeprom for wifi");
        return WIFI_CANNOT_INIT_EEPROM;
    }
    if (wifi_record.size == 0) {
        errCode = eep_getAllocation(WIFI_EEPROM_ID, 1, wifi_record);
        if (EEP_SUCCESS != errCode) {
            dPrint("Cannot allocate eeprom for wifi");
            return WIFI_CANNOT_INIT_EEPROM;
        }
    }
    errCode = wifi_popSSIDs(); // read eeprom and add ssids to wifi multi
    if (errCode) {
        dPrint(String("Read SSID failure: ")+String(errCode)+ ". Clear SSIDs list");
        wifi_clearSSIDs();
    } 
    errCode = wifi_updateMulti();
    if(errCode) {return errCode;}
    errCode = !WiFi.mode(WIFI_STA);
    if(errCode) {return WIFI_CANNOT_SWITCH_MODE;}
    wifiMulti.run(WIFI_CONNECTION_TIMOUT_SEC*1000);
    
    if (WiFi.getAutoConnect() != true) {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
    //    WiFi.persistent(false);
    }
    // WiFi.printDiag(Serial);
    // Serial.setDebugOutput(true);

    for (uint i = 0; i < WIFI_CONNECTION_TIMOUT_SEC; i++) {
        Serial.print(".");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    }

    loopWifi();
    return WIFI_SUCCESS;
}

void loopWifi() {
    // dPrint(String() + WiFi.SSID() + " : " + ((WiFi.status() == WL_CONNECTED)? "connected": "not connected")+ ", Status:" + WiFi.status());
    // wifiMulti.run(WIFI_CONNECTION_TIMOUT_SEC*1000);
    // if (wifiMulti.run() != WL_CONNECTED) { // check status of wifi. 
    

    // }

// #ifndef WIFI_NOT_USE_AP
    // if disconnected more than 30 sec - start AP
//     // if wifi in AP mode and enabled - capture server requests
//     if (apEnabled) {
        
//     }
// #endif // WIFI_NOT_USE_AP
}

int wifi_clearSSIDs() {
    int errCode = WIFI_SUCCESS;
    for (int i = 0; i < MAX_SSIDS; i++) {
        ssidStorage[i].name = "";
        ssidStorage[i].pwd = "";
    }
    ssidNum = 0;
    errCode = wifi_pushSSIDs();
    if(errCode) {return errCode;}
    errCode = wifi_updateMulti();
    if(errCode) {return errCode;}
    return WIFI_SUCCESS;
}

int wifi_appendSSID(String name, String pwd) {
    int errCode = WIFI_SUCCESS;
    if (ssidNum < MAX_SSIDS) {
        ssidStorage[ssidNum] = SSID {name, pwd};
        ssidNum += 1;
    } else {
        return WIFI_SSID_STORAGE_FULL;
    }

    errCode = wifi_pushSSIDs();
    if(errCode) {return errCode;}
    errCode = wifi_updateMulti();
    if(errCode) {return errCode;}

    return WIFI_SUCCESS;
}

int wifi_updateMulti() {
    wifiMulti.cleanAPlist();
    for (uint i = 0; i < ssidNum; i++ ) {
        wifiMulti.addAP(ssidStorage[i].name.c_str(), ssidStorage[i].pwd.c_str());
    }
    return WIFI_SUCCESS;
}

int wifi_popSSIDs() {
    int errCode = WIFI_SUCCESS;
    errCode = eep_read(WIFI_EEPROM_ID, 0, ssidNum);
    if (EEP_SUCCESS != errCode) {
        dPrint(errCode);
        return WIFI_CANNOT_READ_EEPROM_DATA;
    }
    if (ssidNum > MAX_SSIDS) {
        ssidNum = MAX_SSIDS;
        errCode = eep_write(WIFI_EEPROM_ID, 0, ssidNum);
        if (EEP_SUCCESS != errCode) { return WIFI_CANNOT_READ_EEPROM_DATA; }
    }

    unsigned char c;
    uint currIndex = 1;
    unsigned char length = 0;
    for (uint i = 0; i < ssidNum; i++ ) { // read each entry
        // read name
        errCode = eep_read(WIFI_EEPROM_ID, currIndex, length);
        if (EEP_SUCCESS != errCode) { return WIFI_READ_EEPROM_DATA; }
        currIndex++;
        ssidStorage[i].name = "";
        for (int ci = 0; ci < length; ci++) {
            errCode = eep_read(WIFI_EEPROM_ID, currIndex, c);
            if (EEP_SUCCESS != errCode) { return WIFI_READ_EEPROM_DATA; }
            ssidStorage[i].name += char(c);
            currIndex++;
        }

        // read pwd
        errCode = eep_read(WIFI_EEPROM_ID, currIndex, length);
        if (EEP_SUCCESS != errCode) { return WIFI_READ_EEPROM_DATA; }
        currIndex++;
        ssidStorage[i].pwd = "";
        for (int ci = 0; ci < length; ci++) {
            errCode = eep_read(WIFI_EEPROM_ID, currIndex, c);
            if (EEP_SUCCESS != errCode) { return WIFI_READ_EEPROM_DATA; }
            ssidStorage[i].pwd += char(c);
            currIndex++;
        }
    }
    return WIFI_SUCCESS;
}

int wifi_pushSSIDs() {
    int errCode = WIFI_SUCCESS;
    byte ssidStorageSize = 1;
    for (uint i = 0; i < ssidNum; i++ ) {// calc SSID storage size
        ssidStorageSize += ssidStorage[i].name.length() + 1;
        ssidStorageSize += ssidStorage[i].pwd.length() + 1;
    }
    errCode = eep_getAllocation(WIFI_EEPROM_ID, ssidStorageSize, wifi_record);
    if (EEP_SUCCESS != errCode) { return WIFI_EEPROM_ALLOCATION; }
    
    errCode = eep_write(WIFI_EEPROM_ID, 0, ssidNum, false); // write num of entries
    if (EEP_SUCCESS != errCode) { return WIFI_CANNOT_WRITE_EEPROM_DATA; }
    uint currIndex = 1;
    for (uint i = 0; i < ssidNum; i++ ) { // write each entry
        errCode = eep_write(WIFI_EEPROM_ID, currIndex, ssidStorage[i].name.length(), false);
        if (EEP_SUCCESS != errCode) { return WIFI_CANNOT_WRITE_EEPROM_DATA; }
        currIndex++;
        for (uint ci = 0; ci < ssidStorage[i].name.length(); ci++ ){
            errCode = eep_write(WIFI_EEPROM_ID, currIndex, ssidStorage[i].name[ci], false);
            if (EEP_SUCCESS != errCode) { return WIFI_CANNOT_WRITE_EEPROM_DATA; }
            currIndex++;
        }
        errCode = eep_write(WIFI_EEPROM_ID, currIndex, ssidStorage[i].pwd.length(), false);
        if (EEP_SUCCESS != errCode) { return WIFI_CANNOT_WRITE_EEPROM_DATA; }
        currIndex++;
        for (uint ci = 0; ci < ssidStorage[i].pwd.length(); ci++ ){
            errCode = eep_write(WIFI_EEPROM_ID, currIndex, ssidStorage[i].pwd[ci], false);
            if (EEP_SUCCESS != errCode) { return WIFI_CANNOT_WRITE_EEPROM_DATA; }
            currIndex++;
        }
    }

    eep_flush();
    return WIFI_SUCCESS;
}

// #ifndef WIFI_NOT_USE_AP
//     // if wifi in AP mode and enabled - capture server requests
//     if (apEnabled) {
        
//     }
// #endif // WIFI_NOT_USE_AP

//     // if wifi connected - do nothing
//     if (WiFi.status() == WL_CONNECTED) {
//         return;
//     }


//     // if wifi not connected - check availability of all wifi ssid s in eeprom
//     WiFi.mode(WIFI_STA);
//     ESP8266WiFiMulti wifiMulti;
//     // // while not end of epprom
//     // string ssid = "";
//     // string pwd = "";
//     // char c;
//     // for (int i; i < EEPROM_SIZE; i++ ) {
//     //     c = EPPROM[i];
//     //     if (c!= '\0') {
//     //         ssid += c;
//     //         continue
//     //     }
//     //     // read ssid from epprom
//     //     // try to connect to this ssid
//             // wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
//             // wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
//             // wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
//     //     ssid = "";
//     // }

// #ifndef WIFI_NOT_USE_AP
//     // if nothing found - start a station with small html web server which responds on typical ios/android/ms captive requests
//     WiFi.mode(WIFI_AP_STA);
//     // setup wifi

    
//     // setup server
//     apEnabled = true;
// #endif // WIFI_NOT_USE_AP

// }


inline void dPrint(const SSID ssid) { // debug print to console
#ifndef SERIAL_DEBUG_DISABLE
    Serial.print(DEVICE_NAME);
    Serial.print(": ");
    Serial.println(String("SSID: ") + ssid.name + ", password: " + ssid.pwd);
#endif // SERIAL_DEBUG_DISABLE
}

#endif // WIFI_H