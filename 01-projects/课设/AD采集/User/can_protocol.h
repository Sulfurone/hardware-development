#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H

#include "stm32f10x.h"
#include "measurement.h"

#define CAN_NODE_ID          1U
#define CAN_COMMAND_ID       (0x200U + CAN_NODE_ID)
#define CAN_RESPONSE_ID      (0x180U + CAN_NODE_ID)

void CAN_ProtocolInit(void);
void CAN_ProtocolPoll(void);
void CAN_ProtocolPublish(const MeasurementResult *result);
uint8_t CAN_ProtocolIsRunning(void);
uint8_t CAN_ProtocolChannel(void);

#endif
