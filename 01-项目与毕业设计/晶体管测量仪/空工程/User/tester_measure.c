#include "tester_measure.h"
#include "app_config.h"
#include "bsp_delay.h"
#include "tester_hw.h"
#include <string.h>

static void DiodeList_Add(DiodeList *list, uint8_t anode, uint8_t cathode, uint16_t vf_mv)
{
    if (list->count < 6) {
        list->item[list->count].anode = anode;
        list->item[list->count].cathode = cathode;
        list->item[list->count].vf_mv = vf_mv;
        list->count++;
    }
}

static DiodeList Scan_Diodes(void)
{
    DiodeList list;
    uint8_t anode;
    uint8_t cathode;

    memset(&list, 0, sizeof(list));
    TesterHW_TpDischargeAll(8);

    for (anode = 0; anode < 3; anode++) {
        for (cathode = 0; cathode < 3; cathode++) {
            uint16_t va;
            uint16_t vk;
            uint16_t vf;

            if (anode == cathode) {
                continue;
            }

            TesterHW_TpAllFloat();
            TesterHW_TpSet(anode, TP_HIGH_680);
            TesterHW_TpSet(cathode, TP_LOW_680);
            Delay_Ms(4);

            va = TesterHW_TpReadMvAvg(anode);
            vk = TesterHW_TpReadMvAvg(cathode);
            vf = (va > vk) ? (uint16_t)(va - vk) : 0;

            if (vf >= DIODE_MIN_MV && vf <= DIODE_MAX_MV) {
                DiodeList_Add(&list, anode, cathode, vf);
            }
        }
    }

    TesterHW_TpAllFloat();
    return list;
}

static uint32_t Measure_ResPair(uint8_t a, uint8_t b, uint8_t high_range)
{
    uint16_t va;
    uint16_t vb;
    uint16_t vd;
    uint32_t series_ohm;
    uint32_t denom;

    TesterHW_TpAllFloat();
    TesterHW_TpSet(a, high_range ? TP_HIGH_470K : TP_HIGH_680);
    TesterHW_TpSet(b, high_range ? TP_LOW_470K : TP_LOW_680);
    Delay_Ms(high_range ? 20 : 4);

    va = TesterHW_TpReadMvAvg(a);
    vb = TesterHW_TpReadMvAvg(b);
    vd = (va > vb) ? (uint16_t)(va - vb) : 0;
    series_ohm = high_range ? RES_HIGH_OHM_SUM : RES_LOW_OHM_SUM;

    if (vd < 20U || vd >= (APP_VCC_MV - 50U)) {
        return 0;
    }

    denom = APP_VCC_MV - vd;
    return (series_ohm * (uint32_t)vd + (denom / 2U)) / denom;
}

static uint8_t Detect_Resistor(PartResult *result)
{
    uint8_t a;
    uint8_t b;

    for (a = 0; a < 3; a++) {
        for (b = (uint8_t)(a + 1); b < 3; b++) {
            uint32_t ohm;

            ohm = Measure_ResPair(a, b, 0);
            if (ohm <= 2UL || ohm >= 20000UL) {
                ohm = Measure_ResPair(a, b, 1);
            }

            if (ohm > 2UL && ohm < 20000000UL) {
                result->type = PART_RESISTOR;
                result->pin_a = a;
                result->pin_b = b;
                result->value = ohm;
                return 1;
            }
        }
    }

    return 0;
}

static uint32_t Measure_CapPairNf(uint8_t a, uint8_t b)
{
    uint32_t elapsed_us = 0;
    uint16_t threshold_mv;

    threshold_mv = (uint16_t)((APP_VCC_MV * 632UL) / 1000UL);
    TesterHW_TpDischargeAll(80);
    TesterHW_TpSet(b, TP_LOW_680);
    TesterHW_TpSet(a, TP_HIGH_470K);

    while (elapsed_us < 3000000UL) {
        if (TesterHW_TpReadMv(a) >= threshold_mv) {
            break;
        }
        Delay_Us(100);
        elapsed_us += 100;
    }

    TesterHW_TpAllFloat();

    if (elapsed_us < 200UL || elapsed_us >= 3000000UL) {
        return 0;
    }

    return (elapsed_us + (CAP_CHARGE_R_OHM / 2000UL)) / (CAP_CHARGE_R_OHM / 1000UL);
}

static uint8_t Detect_Capacitor(PartResult *result)
{
    uint8_t a;
    uint8_t b;

    for (a = 0; a < 3; a++) {
        for (b = (uint8_t)(a + 1); b < 3; b++) {
            uint32_t nf;

            nf = Measure_CapPairNf(a, b);
            if (nf >= 1UL && nf < 100000UL) {
                result->type = PART_CAPACITOR;
                result->pin_a = a;
                result->pin_b = b;
                result->value = nf;
                return 1;
            }
        }
    }

    return 0;
}

static uint8_t Detect_Bjt(const DiodeList *diodes, PartResult *result)
{
    uint8_t pin;

    for (pin = 0; pin < 3; pin++) {
        uint8_t i;
        uint8_t from_base = 0;
        uint8_t to_base = 0;

        for (i = 0; i < diodes->count; i++) {
            if (diodes->item[i].anode == pin) {
                from_base++;
            }
            if (diodes->item[i].cathode == pin) {
                to_base++;
            }
        }

        if (from_base >= 2) {
            result->type = PART_NPN;
            result->pin_b = pin;
            return 1;
        }
        if (to_base >= 2) {
            result->type = PART_PNP;
            result->pin_b = pin;
            return 1;
        }
    }

    return 0;
}

PartResult TesterMeasure_Detect(void)
{
    PartResult result;

    memset(&result, 0, sizeof(result));
    result.diodes = Scan_Diodes();

    if (Detect_Bjt(&result.diodes, &result)) {
        return result;
    }

    if (result.diodes.count == 1) {
        result.type = PART_DIODE;
        result.pin_a = result.diodes.item[0].anode;
        result.pin_b = result.diodes.item[0].cathode;
        result.extra_mv = result.diodes.item[0].vf_mv;
        return result;
    }

    if (result.diodes.count == 2) {
        result.type = PART_DUAL_DIODE;
        return result;
    }

    if (Detect_Resistor(&result)) {
        return result;
    }

    if (Detect_Capacitor(&result)) {
        return result;
    }

    result.type = PART_NONE;
    return result;
}
