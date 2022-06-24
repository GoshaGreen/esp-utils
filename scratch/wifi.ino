
// #define SERIAL_DEBUG_DISABLE
// #define TIMER_DISABLE
// #define EEPROM_DISABLE
// #define WIFI_DISABLE
#define WIFI_NOT_USE_AP
// #define OTA_DISABLE
#define DEVICE_NAME "utils_test"
#define SERIAL_BAUDRATE 74880
#define EEPROM_SIZE 128
#define MAX_ALLOCATION_ENTRIES 4

#define WIFI_EEPROM_ID 0

enum EEPROM_ID{
    SSIDs = WIFI_EEPROM_ID,
    DATA1 = 1,
    DATA2 = 2
};

#include "./h/utils.h"

void setup() {
    int errCode = 0;
    utilsSetup();

    // wifi_clearSSIDs();
    // wifi_appendSSID("TEST11", "kolokolo");
    // wifi_appendSSID("morgen32", "kolokolo");
    // wifi_appendSSID("GreeNnet", "kolokolo");
    eep_print();

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    utilsLoop();

    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(550);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    // dPrint("bump");
}
