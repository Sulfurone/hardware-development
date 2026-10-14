import sensor, image, time, lcd
from machine import UART
from modules import ybrgb

# 初始化 UART2 (TX=IO8, RX=IO6)
uart = UART(UART.UART2, 9600, bits=8, parity=None, stop=1, timeout=1000, read_buf_len=4096)

# 初始化 RGB 灯控制
RGB = ybrgb()

# 初始化 LCD 与图像传感器
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=100)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()

# 定义颜色阈值
threshold_green = [(10, 50, -30, -10, -30, 20)]
threshold_yellow = [(-20, 100, -30, 50, 10, 120)]
color_list = [threshold_green, threshold_yellow]
color_names = ["Green", "Yellow"]

# 颜色识别状态
last_color = None
last_send_time = time.ticks_ms()
send_interval = 1000  # ms

# 面积判断阈值
GREEN_MIN_AREA = 1500
YELLOW_MIN_AREA = 3000

def send_color(c):
    global last_color, last_send_time

    current_time = time.ticks_ms()
    if time.ticks_diff(current_time, last_send_time) < send_interval and c == last_color:
        return

    last_send_time = current_time
    last_color = c

    if c == 2:  # Yellow
        uart.write("@Yellow\r\n")
        RGB.set(255, 255, 0)
        print("Send: Yellow")
    elif c == 1:  # Green
        uart.write("@Green\r\n")
        RGB.set(0, 255, 0)
        print("Send: Green")

print("Start Color Recognition...")
while True:
    clock.tick()
    img = sensor.snapshot()
    fps = clock.fps()
    color_detected = 0

    for index, t in enumerate(color_list, start=1):
        blobs = img.find_blobs(t, pixels_threshold=100, area_threshold=100, merge=True, margin=10)
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.pixels())

            # 判断面积
            if index == 1 and largest_blob.pixels() < GREEN_MIN_AREA:
                continue
            elif index == 2 and largest_blob.pixels() < YELLOW_MIN_AREA:
                continue

            img.draw_rectangle(largest_blob.rect())
            img.draw_cross(largest_blob.cx(), largest_blob.cy())
            img.draw_string(largest_blob.x(), largest_blob.y()-20, color_names[index-1], color=(255,255,255), scale=2)
            color_detected = index
            break

    if color_detected > 0:
        send_color(color_detected)

    img.draw_string(0, 0, "%2.1ffps" % fps, color=(0, 60, 128), scale=2.0)
    lcd.display(img)
