import sensor, image, time, lcd
from machine import UART
from modules import ybrgb

# 初始化 UART2
uart = UART(UART.UART2, 9600, bits=8, parity=None, stop=1, timeout=1000, read_buf_len=4096)

# 初始化 RGB 灯
RGB = ybrgb()

# 初始化 LCD 和传感器
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=100)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()

# 定义芒果颜色阈值
threshold_mango = [(30, 80, 10, 50, 20, 80)]  # 成熟芒果的橙黄色
color_list = [threshold_mango]
color_names = ["Mango"]

# 初始化颜色识别状态
last_color = None
last_send_time = time.ticks_ms()
send_interval = 1000

def send_color(c):
    global last_color, last_send_time
    current_time = time.ticks_ms()
    if time.ticks_diff(current_time, last_send_time) < send_interval and c == last_color:
        return
    last_send_time = current_time
    last_color = c
    if c == 1:  # Mango
        uart.write("@Mango\r\n")
        RGB.set(255, 165, 0)
        print("Send: Mango")

print("Start Mango Recognition...")
while True:
    clock.tick()
    img = sensor.snapshot()
    fps = clock.fps()
    color_detected = 0

    for index, t in enumerate(color_list, start=1):
        blobs = img.find_blobs(t, pixels_threshold=50, area_threshold=100, merge=True, margin=20)
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.pixels())
            img.draw_rectangle(largest_blob.rect())
            img.draw_cross(largest_blob.cx(), largest_blob.cy())
            img.draw_string(largest_blob.x(), largest_blob.y()-20, color_names[index-1], color=(255,255,255), scale=2)
            color_detected = index
            break

    if color_detected > 0:
        send_color(color_detected)

    img.draw_string(0, 0, "%2.1ffps" % fps, color=(0, 60, 128), scale=2.0)
    lcd.display(img)
