#include "STC15xx_config.h"
#include "UART.h"

/*
 * U1 master:
 * - keeps the waiting queue
 * - scans four service-window keys
 * - calls the next ticket and sends status to U2 through UART1
 *
 * Default wiring:
 * LCD1602 D0-D7 -> P0
 * LCD_RS -> P2.0, LCD_E -> P2.1, LCD_RW -> P2.2
 * Buzzer -> P1.7
 * Window keys K1-K4 -> P2.4/P2.5/P2.6/P2.7, active low
 * UART1 -> P3.0(RXD), P3.1(TXD), crossed with U2
 */

#define LCD_DATA P0

sbit LCD_RS = P2^0;
sbit LCD_E  = P2^1;
sbit LCD_RW = P2^2;
sbit BUZZER = P1^7;

sbit KEY_WIN1 = P2^4;
sbit KEY_WIN2 = P2^5;
sbit KEY_WIN3 = P2^6;
sbit KEY_WIN4 = P2^7;

#define MAX_QUEUE   20

#define CMD_TAKE    0x11

#define FRAME_HEAD  0xA5
#define CMD_TICKET  0x21
#define CMD_CALL    0x22
#define CMD_STATUS  0x23
#define CMD_FULL    0x24

static u16 queue_buf[MAX_QUEUE];
static u8 queue_head = 0;
static u8 queue_tail = 0;
static u8 queue_count = 0;
static u16 next_ticket = 1;
static u16 last_u2_ticket = 0;
static u8 last_window = 0;

static void lcd_write_cmd(u8 cmd);
static void lcd_write_data(u8 dat);
static void lcd_show_ready(void);
static void lcd_show_ticket(u16 no);
static void lcd_show_call(u8 window, u16 no);

static void port_init(void)
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;

    P0 = 0x00;
    P1 = 0xFF;
    P2 = 0xF8;
    P3 = 0xFF;
    LCD_RW = 0;
    BUZZER = 0;
}

static void lcd_pulse(void)
{
    LCD_E = 1;
    delay_ms(1);
    LCD_E = 0;
}

static void lcd_write_cmd(u8 cmd)
{
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DATA = cmd;
    lcd_pulse();
    delay_ms(2);
}

static void lcd_write_data(u8 dat)
{
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DATA = dat;
    lcd_pulse();
    delay_ms(1);
}

static void lcd_goto(u8 row, u8 col)
{
    lcd_write_cmd((row == 0 ? 0x80 : 0xC0) + col);
}

static void lcd_puts(char code *s)
{
    while (*s)
    {
        lcd_write_data(*s++);
    }
}

static void lcd_clear_line(u8 row)
{
    u8 i;

    lcd_goto(row, 0);
    for (i = 0; i < 16; i++)
    {
        lcd_write_data(' ');
    }
    lcd_goto(row, 0);
}

static void lcd_put_u8_2(u8 n)
{
    if (n > 99)
    {
        n = 99;
    }

    lcd_write_data(n / 10 + '0');
    lcd_write_data(n % 10 + '0');
}

static void lcd_put_u16_4(u16 n)
{
    if (n > 9999)
    {
        n = 9999;
    }

    lcd_write_data((n / 1000) % 10 + '0');
    lcd_write_data((n / 100) % 10 + '0');
    lcd_write_data((n / 10) % 10 + '0');
    lcd_write_data(n % 10 + '0');
}

static void lcd_init(void)
{
    delay_ms(20);
    lcd_write_cmd(0x38);
    lcd_write_cmd(0x0C);
    lcd_write_cmd(0x06);
    lcd_write_cmd(0x01);
    delay_ms(5);
}

static void lcd_show_ready(void)
{
    lcd_clear_line(0);
    lcd_puts("Ready Wait:");
    lcd_put_u8_2(queue_count);

    lcd_clear_line(1);
    lcd_puts("Next No:");
    lcd_put_u16_4(next_ticket);
}

static void lcd_show_ticket(u16 no)
{
    lcd_clear_line(0);
    lcd_puts("Take No:");
    lcd_put_u16_4(no);

    lcd_clear_line(1);
    lcd_puts("Wait Total:");
    lcd_put_u8_2(queue_count);
}

static void lcd_show_call(u8 window, u16 no)
{
    lcd_clear_line(0);
    lcd_write_data('W');
    lcd_write_data(window + '0');
    lcd_puts(" Call:");
    lcd_put_u16_4(no);

    lcd_clear_line(1);
    lcd_puts("Wait Total:");
    lcd_put_u8_2(queue_count);
}

static void lcd_show_empty(u8 window)
{
    lcd_clear_line(0);
    lcd_write_data('W');
    lcd_write_data(window + '0');
    lcd_puts(" No Customer");

    lcd_clear_line(1);
    lcd_puts("Wait Total:");
    lcd_put_u8_2(queue_count);
}

static void beep(u8 times)
{
    u8 i;

    for (i = 0; i < times; i++)
    {
        BUZZER = 1;
        delay_ms(80);
        BUZZER = 0;
        delay_ms(70);
    }
}

static bit uart_get_byte(u8 *dat)
{
    u8 i;
    u8 old_es;

    old_es = ES;
    ES = 0;

    if (COM1.RX_Cnt == 0)
    {
        ES = old_es;
        return 0;
    }

    *dat = RX1_Buffer[0];
    for (i = 1; i < COM1.RX_Cnt; i++)
    {
        RX1_Buffer[i - 1] = RX1_Buffer[i];
    }
    COM1.RX_Cnt--;

    ES = old_es;
    return 1;
}

static void uart_send_frame(u8 cmd, u16 no, u8 count, u8 window)
{
    u8 high = (u8)(no >> 8);
    u8 low = (u8)no;
    u8 sum = FRAME_HEAD + cmd + high + low + count + window;

    TX1_write2buff(FRAME_HEAD);
    TX1_write2buff(cmd);
    TX1_write2buff(high);
    TX1_write2buff(low);
    TX1_write2buff(count);
    TX1_write2buff(window);
    TX1_write2buff(sum);
}

static void queue_push(u16 no)
{
    if (queue_count >= MAX_QUEUE)
    {
        return;
    }

    queue_buf[queue_tail] = no;
    queue_tail++;
    if (queue_tail >= MAX_QUEUE)
    {
        queue_tail = 0;
    }
    queue_count++;
}

static u16 queue_pop(void)
{
    u16 no;

    if (queue_count == 0)
    {
        return 0;
    }

    no = queue_buf[queue_head];
    queue_head++;
    if (queue_head >= MAX_QUEUE)
    {
        queue_head = 0;
    }
    queue_count--;

    return no;
}

static u8 queue_count_ahead(u16 ticket)
{
    u8 i;
    u8 pos;

    if (ticket == 0)
    {
        return queue_count;
    }

    pos = queue_head;
    for (i = 0; i < queue_count; i++)
    {
        if (queue_buf[pos] == ticket)
        {
            return i;
        }

        pos++;
        if (pos >= MAX_QUEUE)
        {
            pos = 0;
        }
    }

    return 0;
}

static void handle_take_request(void)
{
    u16 ticket;
    u8 ahead;

    if (queue_count >= MAX_QUEUE)
    {
        uart_send_frame(CMD_FULL, 0, queue_count, 0);
        lcd_clear_line(0);
        lcd_puts("Queue Full");
        lcd_clear_line(1);
        lcd_puts("Wait Total:");
        lcd_put_u8_2(queue_count);
        beep(3);
        return;
    }

    ticket = next_ticket++;
    if (next_ticket > 9999)
    {
        next_ticket = 1;
    }

    queue_push(ticket);
    last_u2_ticket = ticket;
    ahead = queue_count_ahead(ticket);

    uart_send_frame(CMD_TICKET, ticket, ahead, 0);
    lcd_show_ticket(ticket);
}

static void handle_uart(void)
{
    u8 dat;

    while (uart_get_byte(&dat))
    {
        if (dat == CMD_TAKE)
        {
            handle_take_request();
        }
    }
}

static void service_call(u8 window)
{
    u16 no;
    u8 ahead;

    no = queue_pop();
    if (no == 0)
    {
        lcd_show_empty(window);
        beep(1);
        uart_send_frame(CMD_STATUS, last_u2_ticket,
                        queue_count_ahead(last_u2_ticket), window);
        return;
    }

    last_window = window;
    ahead = queue_count_ahead(last_u2_ticket);
    lcd_show_call(window, no);
    beep(2);
    uart_send_frame(CMD_CALL, no, ahead, window);
}

static void scan_window_keys(void)
{
    if (KEY_WIN1 == 0)
    {
        delay_ms(10);
        if (KEY_WIN1 == 0)
        {
            service_call(1);
            while (KEY_WIN1 == 0) handle_uart();
        }
    }

    if (KEY_WIN2 == 0)
    {
        delay_ms(10);
        if (KEY_WIN2 == 0)
        {
            service_call(2);
            while (KEY_WIN2 == 0) handle_uart();
        }
    }

    if (KEY_WIN3 == 0)
    {
        delay_ms(10);
        if (KEY_WIN3 == 0)
        {
            service_call(3);
            while (KEY_WIN3 == 0) handle_uart();
        }
    }

    if (KEY_WIN4 == 0)
    {
        delay_ms(10);
        if (KEY_WIN4 == 0)
        {
            service_call(4);
            while (KEY_WIN4 == 0) handle_uart();
        }
    }
}

void main(void)
{
    port_init();
    lcd_init();
    UART1_config();
    EA = 1;

    lcd_show_ready();
    uart_send_frame(CMD_STATUS, 0, 0, 0);

    while (1)
    {
        handle_uart();
        scan_window_keys();
    }
}
