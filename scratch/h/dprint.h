#ifndef DPRINT_H
#define DPRINT_H

#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE 115200
#endif // SERIAL_BAUDRATE

#ifndef DEVICE_NAME
#define DEVICE_NAME "device"
#endif // DEVICE_NAME

template <typename T>
inline void dPrint(const T msg);
inline void dPrintInit();

inline void dPrintInit() {
#ifndef SERIAL_DEBUG_DISABLE
    Serial.begin(SERIAL_BAUDRATE);
    while (!Serial) {;}
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("==============================");
    dPrint("Serial initialized");
#endif // SERIAL_DEBUG_DISABLE
}

template <typename T>
inline void dPrint(const T msg) { // debug print to console
#ifndef SERIAL_DEBUG_DISABLE
    Serial.print(DEVICE_NAME);
    Serial.print(": ");
    Serial.println(String(msg));
#endif // SERIAL_DEBUG_DISABLE
}

#endif // DPRINT_H
