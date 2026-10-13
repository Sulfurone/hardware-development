#include "can_protocol.h"
#include "ads1256.h"

#define CMD_PING             0x01U
#define CMD_START            0x10U
#define CMD_STOP             0x11U
#define CMD_READ_RESULT      0x20U
#define CMD_SET_SCALE_Q16    0x21U
#define CMD_SET_HYST_UV      0x22U

static uint8_t s_running = 1U;
static uint8_t s_channel = 0U;
static uint8_t s_sequence = 0U;

static uint32_t get_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void put_u32_le(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16);
    p[3] = (uint8_t)(value >> 24);
}

static void send_frame(uint8_t type, uint8_t sequence, uint8_t channel,
                       uint8_t flags, uint32_t value)
{
    CanTxMsg tx;
    uint32_t timeout = 10000U;
    tx.StdId = CAN_RESPONSE_ID;
    tx.ExtId = 0U;
    tx.IDE = CAN_Id_Standard;
    tx.RTR = CAN_RTR_Data;
    tx.DLC = 8U;
    tx.Data[0] = type;
    tx.Data[1] = sequence;
    tx.Data[2] = channel;
    tx.Data[3] = flags;
    put_u32_le(&tx.Data[4], value);
    while ((CAN_Transmit(CAN1, &tx) == CAN_TxStatus_NoMailBox) && (timeout-- != 0U)) { }
}

static void send_ack(uint8_t command, uint8_t sequence, uint8_t status)
{
    send_frame((uint8_t)(command | 0x80U), sequence, s_channel, status, 0U);
}

void CAN_ProtocolInit(void)
{
    GPIO_InitTypeDef gpio;
    CAN_InitTypeDef can;
    CAN_FilterInitTypeDef filter;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_12;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &gpio);

    CAN_DeInit(CAN1);
    CAN_StructInit(&can);
    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = ENABLE;
    can.CAN_AWUM = ENABLE;
    can.CAN_NART = DISABLE;
    can.CAN_RFLM = DISABLE;
    can.CAN_TXFP = ENABLE;
    can.CAN_Mode = CAN_Mode_Normal;
    can.CAN_SJW = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_15tq;
    can.CAN_BS2 = CAN_BS2_2tq;
    can.CAN_Prescaler = 4U; /* 500 kbit/s with 36 MHz APB1. */
    (void)CAN_Init(CAN1, &can);

    /* 16-bit list mode, FIFO0: exact standard identifier 0x201. */
    filter.CAN_FilterNumber = 0U;
    filter.CAN_FilterMode = CAN_FilterMode_IdMask;
    filter.CAN_FilterScale = CAN_FilterScale_32bit;
    filter.CAN_FilterIdHigh = (uint16_t)(CAN_COMMAND_ID << 5);
    filter.CAN_FilterIdLow = 0U;
    filter.CAN_FilterMaskIdHigh = (uint16_t)(0x7FFU << 5);
    filter.CAN_FilterMaskIdLow = 0U;
    filter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    filter.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&filter);
}

void CAN_ProtocolPublish(const MeasurementResult *result)
{
    uint8_t flags = 0U;
    if (result->valid)   { flags |= 0x01U; }
    if (result->clipped) { flags |= 0x02U; }
    send_frame(0x90U, s_sequence, s_channel, flags, result->rms_uV);
    send_frame(0x91U, s_sequence, s_channel, flags, result->low_frequency_mHz);
    send_frame(0x92U, s_sequence, s_channel, flags, result->carrier_frequency_mHz);
    s_sequence++;
}

void CAN_ProtocolPoll(void)
{
    CanRxMsg rx;
    const MeasurementResult *result;
    uint8_t command;
    uint8_t sequence;

    if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0U) {
        return;
    }
    CAN_Receive(CAN1, CAN_FIFO0, &rx);
    if ((rx.IDE != CAN_Id_Standard) || (rx.StdId != CAN_COMMAND_ID) || (rx.DLC < 2U)) {
        return;
    }
    command = rx.Data[0];
    sequence = rx.Data[1];

    switch (command) {
    case CMD_PING:
        send_ack(command, sequence, 0U);
        break;
    case CMD_START:
        if ((rx.DLC < 3U) || (rx.Data[2] > 7U) || !ADS1256_SetChannel(rx.Data[2])) {
            send_ack(command, sequence, 1U);
        } else {
            s_channel = rx.Data[2];
            s_running = 1U;
            Measurement_Init(ADS1256_DATA_RATE_SPS, ADS1256_GetPgaValue());
            send_ack(command, sequence, 0U);
        }
        break;
    case CMD_STOP:
        s_running = 0U;
        send_ack(command, sequence, 0U);
        break;
    case CMD_READ_RESULT:
        result = Measurement_Get();
        CAN_ProtocolPublish(result);
        break;
    case CMD_SET_SCALE_Q16:
        if (rx.DLC < 6U) {
            send_ack(command, sequence, 1U);
        } else {
            Measurement_SetScaleQ16(get_u32_le(&rx.Data[2]));
            send_ack(command, sequence, 0U);
        }
        break;
    case CMD_SET_HYST_UV:
        if (rx.DLC < 6U) {
            send_ack(command, sequence, 1U);
        } else {
            Measurement_SetHysteresisUv(get_u32_le(&rx.Data[2]));
            send_ack(command, sequence, 0U);
        }
        break;
    default:
        send_ack(command, sequence, 2U);
        break;
    }
}

uint8_t CAN_ProtocolIsRunning(void)
{
    return s_running;
}

uint8_t CAN_ProtocolChannel(void)
{
    return s_channel;
}
