#ifndef EEPROM_HELPERS_H
#define EEPROM_HELPERS_H
// EEPROM allocator + functions

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 1024
#endif // EEPROM_SIZE

#ifndef MAX_ALLOCATION_ENTRIES
#define MAX_ALLOCATION_ENTRIES 16
#endif // MAX_ALLOCATION_ENTRIES

#define ALLOCATION_SIZE_BYTES 1
#define MAX_ALLOCATION_LENGTH 256*ALLOCATION_SIZE_BYTES
#define ALLOCATION_TABLE_STARTS (EEPROM_SIZE-MAX_ALLOCATION_ENTRIES)

#include <EEPROM.h>
#include "./dprint.h"

typedef struct {
    byte id;
    unsigned int offset;
    byte size;
} epp_record;

enum EEP_ERR_CODE {
    EEP_SUCCESS = 0,
    EEP_ALREADY_INITIALIZED = -1,
    EEP_SIZE_FAILURE = -2,
    EEP_WRONG_RECORD_ID = -3,
    EEP_ALLOCATION_TABLE_CORRUPTED = -4,
    EEP_NO_FREE_MEMORY = -5,
    EEP_INDEX_ERROR = -6,
};

int setupEEPROM();
inline int eep_getTotalSize();
void eep_clear();
int eep_resize(byte id, byte size);
void eep_print();
inline int eep_checkId(byte id);
int eep_getAllocation(byte id, byte size, epp_record& alloc_record);
int eep_getOffset(byte id, unsigned int &offset);
int eep_memcpy(unsigned int newOffset, unsigned int startOffset, unsigned int count, bool rev = false);
int eep_read(byte id, byte index, byte &value);
int eep_write(byte id, byte index, byte value, bool flush = true);
char* eep_getArray(byte id);
inline void dPrint(const epp_record record);

uint8_t *sizes;

int setupEEPROM() {
    if (EEPROM.length()) {
        if (EEPROM.length() != EEPROM_SIZE) {
            return EEP_ALREADY_INITIALIZED;
        }
        return 0;
    }
    EEPROM.begin(EEPROM_SIZE);
    if (EEPROM.length() != EEPROM_SIZE) {
        return EEP_SIZE_FAILURE;
    }
    // dPrint(String("sizes starts") + (EEPROM_SIZE - MAX_ALLOCATION_ENTRIES));
    sizes = &EEPROM[ALLOCATION_TABLE_STARTS];

    return 0;
}

inline int eep_getTotalSize() {
    return EEPROM.length();
}

int eep_getAllocation(byte id, byte size, epp_record& alloc_record) {
    int errCode = eep_checkId(id);
    if (EEP_SUCCESS != errCode) {return errCode;}
    alloc_record.id = id;
    errCode = eep_getOffset(id, alloc_record.offset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    if (size && size != sizes[id]) {
        errCode = eep_resize(id, size);
        if (EEP_SUCCESS != errCode) {return errCode;}
    }
    alloc_record.size = sizes[id];
    return EEP_SUCCESS;
}

String byte2str(byte b) {
    String res = "";
    byte c1 = ((b & 0b11110000 )>>4) + 48;
    if (c1 >= 58) {c1 += 7;}
    byte c2 = ((b & 0b00001111 )) + 48;
    if (c2 >= 58) {c2 += 7;}
    res += char(c1);
    res += char(c2);
    return res;
}

void eep_print() { // debug function
    const byte rowLen = 16;
    dPrint("Print EEPROM data: ");
    for (unsigned int i = 0; i < eep_getTotalSize(); i+=rowLen) {
        String rowHex = "";
        String row = "";
        for (unsigned int j = 0; j < rowLen; j++) {
            rowHex += String(byte2str(EEPROM[i+j])) + " ";
            row += String(char(EEPROM[i+j]));
        }
        dPrint(rowHex + "  -  " + row);
        // dPrint(rowHex);
    }
    dPrint("");
}

int eep_resize(byte id, byte size) {
    int errCode = eep_checkId(id);
    if (EEP_SUCCESS != errCode) {return errCode;}
    if (size == sizes[id]) {return EEP_SUCCESS;}

    unsigned int startOffset = 0;
    errCode = eep_getOffset(id, startOffset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    startOffset += sizes[id];
    unsigned int endOffset = 0;
    errCode = eep_getOffset(MAX_ALLOCATION_ENTRIES-1, endOffset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    endOffset += sizes[MAX_ALLOCATION_ENTRIES-1];
    unsigned int count = endOffset - startOffset;

    // TODO: check bondaries

    sizes[id] = size;

    unsigned int newOffset = 0;
    errCode = eep_getOffset(id, newOffset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    newOffset += sizes[id];

    // dPrint(String("") + newOffset + " " + startOffset + " " + count + " " + endOffset);
    errCode = eep_memcpy(newOffset, startOffset, count, size > sizes[id]);
    if (EEP_SUCCESS != errCode) {return errCode;}

    return EEP_SUCCESS;
}

int eep_getOffset(byte id, unsigned int &offset) {
    int errCode = eep_checkId(id);
    if (EEP_SUCCESS != errCode) {return errCode;}
    offset = 0;
    for (unsigned int i = 0; i < id; i++) {
        offset += sizes[i];
    }
    return EEP_SUCCESS;
}

inline int eep_checkId(byte id) {
    if (id > MAX_ALLOCATION_ENTRIES) {return EEP_WRONG_RECORD_ID;}
    return EEP_SUCCESS;
}

int eep_memcpy(unsigned int dst, unsigned int src, unsigned int count, bool rev) {
    unsigned char temp[count];
    // read data
    for (unsigned int i = 0; i < count; i++) {
        temp[i] = EEPROM[src+i];
        EEPROM[src+i] = 0;
    }
    for (unsigned int i = 0; i < count; i++) {
        EEPROM[dst+i] = temp[i];
    }
    if (src>dst && (src-dst)>count) {
        for (unsigned int i = count; i < src-dst; i++) {
            EEPROM[dst+i] = 0;
        }
    }
    EEPROM.commit();
    return EEP_SUCCESS;
}

int eep_getArray(byte id, unsigned char* array) {
    unsigned int offset = 0;
    int errCode = eep_getOffset(id, offset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    array = &EEPROM[ offset ];
    return EEP_SUCCESS;
}

int eep_write(byte id, byte index, byte value, bool flush) {
    unsigned int offset = 0;
    int errCode = eep_getOffset(id, offset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    if (index > sizes[id]-1) {return EEP_INDEX_ERROR;}
    EEPROM[ offset + index ] = value;
    if (flush) {EEPROM.commit();}
    return EEP_SUCCESS;
}

int eep_read(byte id, byte index, byte &value) {
    unsigned int offset = 0;
    int errCode = eep_getOffset(id, offset);
    if (EEP_SUCCESS != errCode) {return errCode;}
    if (index > sizes[id]-1) {return EEP_INDEX_ERROR;}
    value = EEPROM[ offset + index ];
    return EEP_SUCCESS;
}

void eep_clear() {
    for (unsigned int i = 0; i < eep_getTotalSize(); i++){
        EEPROM[i] = 0;
    }
    EEPROM.commit();
}

inline void eep_flush() {
    EEPROM.commit();
}

inline void dPrint(const epp_record record) { // debug print to console
#ifndef SERIAL_DEBUG_DISABLE
    Serial.print(DEVICE_NAME);
    Serial.print(": ");
    Serial.println(String("EEP record id: ") + record.id + ", offset: " + record.offset + ", size: " + record.size);
#endif // SERIAL_DEBUG_DISABLE
}

#endif // EEPROM_HELPERS_H