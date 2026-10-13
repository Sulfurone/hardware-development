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
sensor.set_auto_gain(False)  # 必须关闭自动增益
sensor.set_auto_whitebal(False)  # 必须关闭白平衡
clock = time.clock()

# 定义颜色阈值 (只保留绿色和黄色)
threshold_green = [(10, 50, -30, -10, -30, 20)]      # 绿色范围更大，涵盖亮绿到深绿
threshold_yellow = [(-20, 100, -30, 50, 10, 120)]    # 黄色范围更大，涵盖浅黄到深黄
color_list = [threshold_green, threshold_yellow]
color_names = ["Green", "Yellow"]

# 初始化颜色识别状态
last_color = None
last_send_time = time.ticks_ms()
send_interval = 1000  # 发送间隔1000ms

def send_color(c):
    global last_color, last_send_time

    # 检查是否达到发送间隔
    current_time = time.ticks_ms()
    if time.ticks_diff(current_time, last_send_time) < send_interval and c == last_color:
        return

    # 更新最后发送时间和颜色
    last_send_time = current_time
    last_color = c

    # 根据颜色发送不同的消息
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
    color_detected = 0  # 0表示无

    for index, t in enumerate(color_list, start=1):
        blobs = img.find_blobs(t, pixels_threshold=100, area_threshold=100, merge=True, margin=10)
        if blobs:
            # 取最大的 blob
            largest_blob = max(blobs, key=lambda b: b.pixels())
            img.draw_rectangle(largest_blob.rect())
            img.draw_cross(largest_blob.cx(), largest_blob.cy())
            img.draw_string(largest_blob.x(), largest_blob.y()-20, color_names[index-1], color=(255,255,255), scale=2)
            color_detected = index
            break  # 只检测并处理一种颜色




    # 根据识别结果发送信息
    if color_detected > 0:
        send_color(color_detected)

    img.draw_string(0, 0, "%2.1ffps" % fps, color=(0, 60, 128), scale=2.0)
    lcd.display(img)
