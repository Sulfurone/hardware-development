#ifndef TESTER_TYPES_H
#define TESTER_TYPES_H

#include "stm32f10x.h"

typedef enum {
    TP_FLOAT = 0,
    TP_LOW_680,
    TP_HIGH_680,
    TP_LOW_470K,
    TP_HIGH_470K
} TpMode;

typedef enum {
    PART_NONE = 0,
    PART_DIODE,
    PART_DUAL_DIODE,
    PART_RESISTOR,
    PART_CAPACITOR,
    PART_NPN,
    PART_PNP,
    PART_UNKNOWN
} PartType;

typedef struct {
    uint8_t anode;
    uint8_t cathode;
    uint16_t vf_mv;
} Diode;

typedef struct {
    Diode item[6];
    uint8_t count;
} DiodeList;

typedef struct {
    PartType type;
    uint8_t pin_a;
    uint8_t pin_b;
    uint8_t pin_c;
    uint32_t value;
    uint16_t extra_mv;
    DiodeList diodes;
} PartResult;

#endif
