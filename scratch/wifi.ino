
// #define SERIAL_DEBUG_DISABLE
// #define TIMER_DISABLE
// #define EEPROM_DISABLE
// #define WIFI_DISABLE
#define WIFI_NOT_USE_AP
#define OTA_DISABLE
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

    wifi_clearSSIDs();
    wifi_appendSSID("TEST11", "kolokolo");
    wifi_appendSSID("morgen32", "kolokolo");
    wifi_appendSSID("GreeNnet", "kolokolo");
    eep_print();

    // epp_record alloc_record;
    // eep_clear();
    // eep_print();
    // errCode = eep_getAllocation(SSIDs, 0, alloc_record);
    // dPrint(errCode);
    // dPrint(alloc_record);
    // errCode = eep_getAllocation(1, 10, alloc_record);
    // dPrint(errCode);
    // dPrint(alloc_record);
    // for (int i = 0; i < 10; i++) {
    //     errCode = eep_write(DATA1, i, i*2);
    //     if (errCode) {dPrint(errCode);}
    // }
    // errCode = eep_getAllocation(2, 5, alloc_record);
    // dPrint(errCode);
    // dPrint(alloc_record);
    // for (int i = 0; i < 5; i++) {
    //     errCode = eep_write(DATA2, i, i*16);
    //     if (errCode) {dPrint(errCode);}
    // }
    // // eep_print();

    // dPrint(" ========= EXTEND ARRAY1");
    // errCode = eep_getAllocation(1, 16, alloc_record);
    // for (int i = 10; i < 16; i++) {
    //     errCode = eep_write(DATA1, i, i*4, false);
    //     if (errCode) {dPrint(errCode);}
    // }
    // eep_flush();
    // dPrint(errCode);
    // // eep_print();

    // dPrint(" ========= REDUCE ARRAY1");
    // errCode = eep_getAllocation(1, 5, alloc_record);
    // dPrint(errCode);
    // // eep_print();

    // dPrint(" ========= READ OVERFLOW ARRAY1");
    // String row = "";
    // byte value = 0;
    // for (uint i = 0 ; i< 10 ; i ++) {
    //     errCode = eep_read(DATA1,i,value);
    //     if (errCode) {dPrint(errCode);}
    //     row += byte2str(value) + " ";
    // }
    // dPrint(row);
    // // eep_print();


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
