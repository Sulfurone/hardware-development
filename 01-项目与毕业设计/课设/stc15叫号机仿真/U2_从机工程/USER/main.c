#include "STC15xx_config.h"
#include "UART.h"

/*
 * U2 slave:
 * - sends a take-ticket request to U1
 * - shows its ticket number, people ahead, and current called window
 *
 * Default wiring:
 * LCD1602 D0-D7 -> P0
 * LCD_RS -> P2.0, LCD_E -> P2.1, LCD_RW -> P2.2
 * Take key -> P1.0, active low
 * UART1 -> P3.0(RXD), P3.1(TXD), crossed with U1
 */

#define LCD_DATA P0

sbit LCD_RS = P2^0;
sbit LCD_E  = P2^1;
sbit LCD_RW = P2^2;
sbit KEY_TAKE = P1^0;

#define CMD_TAKE    0x11

#define FRAME_HEAD  0xA5
#define CMD_TICKET  0x21
#define CMD_CALL    0x22
#define CMD_STATUS  0x23
#define CMD_FULL    0x24

static u16 my_ticket = 0;
static u16 last_called = 0;
static u8 last_window = 0;
static u8 ahead_count = 0;
static bit queue_full = 0;
static bit requesting = 0;
static bit called_me = 0;

static void lcd_refresh(void);

static u8 reverse_bits(u8 dat)
{
    dat = ((dat & 0x55) << 1) | ((dat & 0xAA) >> 1);
    dat = ((dat & 0x33) << 2) | ((dat & 0xCC) >> 2);
    dat = ((dat & 0x0F) << 4) | ((dat & 0xF0) >> 4);
    return dat;
}

static void lcd_bus_write(u8 dat)
{
    /*
     * In the current schematic LCD2 D0-D7 are wired to U2 P0.7-P0.0.
     * Reverse bit order before putting bytes on P0.
     */
    LCD_DATA = reverse_bits(dat);
}

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
    lcd_bus_write(cmd);
    lcd_pulse();
    delay_ms(2);
}

static void lcd_write_data(u8 dat)
{
    LCD_RS = 1;
    LCD_RW = 0;
    lcd_bus_write(dat);
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

static void lcd_refresh(void)
{
    lcd_clear_line(0);
    if (queue_full)
    {
        lcd_puts("Queue Full");
    }
    else if (requesting)
    {
        lcd_puts("Requesting...");
    }
    else if (my_ticket == 0)
    {
        lcd_puts("Press P1 Take");
    }
    else
    {
        lcd_puts("No:");
        lcd_put_u16_4(my_ticket);
        lcd_puts(" Ahead:");
        lcd_put_u8_2(ahead_count);
    }

    lcd_clear_line(1);
    if (called_me)
    {
        lcd_puts("Go Window:");
        lcd_write_data(last_window + '0');
    }
    else if (last_called != 0)
    {
        lcd_puts("Call:");
        lcd_put_u16_4(last_called);
        lcd_puts(" W");
        lcd_write_data(last_window + '0');
    }
    else
    {
        lcd_puts("Waiting...");
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

static void handle_frame(u8 cmd, u16 no, u8 count, u8 window)
{
    if (cmd == CMD_TICKET)
    {
        my_ticket = no;
        ahead_count = count;
        queue_full = 0;
        requesting = 0;
        called_me = 0;
    }
    else if (cmd == CMD_CALL)
    {
        last_called = no;
        last_window = window;
        ahead_count = count;
        requesting = 0;

        if ((my_ticket != 0) && (last_called == my_ticket))
        {
            called_me = 1;
        }
    }
    else if (cmd == CMD_STATUS)
    {
        ahead_count = count;
        last_window = window;
        requesting = 0;
    }
    else if (cmd == CMD_FULL)
    {
        ahead_count = count;
        queue_full = 1;
        requesting = 0;
    }

    lcd_refresh();
}

static void parse_uart_frames(void)
{
    static u8 state = 0;
    static u8 cmd = 0;
    static u8 high = 0;
    static u8 low = 0;
    static u8 count = 0;
    static u8 window = 0;
    static u8 sum = 0;
    u8 dat;
    u16 no;

    while (uart_get_byte(&dat))
    {
        if (state == 0)
        {
            if (dat == FRAME_HEAD)
            {
                sum = FRAME_HEAD;
                state = 1;
            }
        }
        else if (state == 1)
        {
            cmd = dat;
            sum += dat;
            state = 2;
        }
        else if (state == 2)
        {
            high = dat;
            sum += dat;
            state = 3;
        }
        else if (state == 3)
        {
            low = dat;
            sum += dat;
            state = 4;
        }
        else if (state == 4)
        {
            count = dat;
            sum += dat;
            state = 5;
        }
        else if (state == 5)
        {
            window = dat;
            sum += dat;
            state = 6;
        }
        else
        {
            if (dat == sum)
            {
                no = ((u16)high << 8) | low;
                handle_frame(cmd, no, count, window);
            }
            state = 0;
        }
    }
}

static void scan_take_key(void)
{
    if (KEY_TAKE == 0)
    {
        delay_ms(10);
        if (KEY_TAKE == 0)
        {
            requesting = 1;
            queue_full = 0;
            called_me = 0;
            lcd_refresh();
            TX1_write2buff(CMD_TAKE);

            while (KEY_TAKE == 0)
            {
                parse_uart_frames();
            }
        }
    }
}

void main(void)
{
    port_init();
    lcd_init();
    UART1_config();
    EA = 1;

    lcd_refresh();

    while (1)
    {
        parse_uart_frames();
        scan_take_key();
    }
}
