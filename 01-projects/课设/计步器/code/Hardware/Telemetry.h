#ifndef __TELEMETRY_H
#define __TELEMETRY_H

#include "stm32f10x.h"

void Telemetry_Init(void);
void Telemetry_SendByte(uint8_t byte);
void Telemetry_SendString(char *str);
uint8_t Telemetry_ReadByte(uint8_t *byte);
void Telemetry_ReportMotionFault(float lat, float lon);
void Telemetry_ReportRoadFault(float lat, float lon);
void Telemetry_ReportLearnPoint(uint16_t index, float lat, float lon, float temp, float hum, float smoke, uint8_t gps_state);
void Telemetry_ReportSmokeAlarm(float temp, float smoke, float lat, float lon);
void Telemetry_ReportStatus(float temp, float hum, float smoke, float lat, float lon, int is_stuck, int is_smoke_alarm, int is_road_fault);

#endif
