import lcd
import sensor
import time
import ujson
import os
from maix import GPIO, KPU
from board import board_info
from fpioa_manager import fm
from image import Image
from machine import UART

# ===== 硬件初始化 =====
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))
sensor.skip_frames(time=100)

# 初始化BOOT按键
fm.register(board_info.BOOT_KEY, fm.fpioa.GPIOHS0)
boot_gpio = GPIO(GPIO.GPIOHS0, GPIO.IN, GPIO.PULL_UP)

# 初始化UART串口
uart = UART(UART.UART2, 9600, 8, None, 1, timeout=1000)

# ===== 常量定义 =====
class STATE:
    IDLE = 0
    INIT = 1
    TRAIN_CLASS_1 = 2
    TRAIN_CLASS_2 = 3
    CLASSIFY = 4

class EVENT:
    BOOT_KEY = 1
    BOOT_KEY_LONG_PRESS = 2

CLASS_NAMES = {0: "mango", 1: "apple"}
FEATURE_FILE = "/sd/features.dat"
THRESHOLD = 98.5
max_train_pic = 5

# ===== 存储功能 =====
features = []

def save_features():
    try:
        with open(FEATURE_FILE, "w") as f:
            data = {
                "features": features,
                "threshold": THRESHOLD,
                "version": "1.0"
            }
            ujson.dump(data, f)
        print("Features saved")
    except Exception as e:
        print("Save error:", e)

def load_features():
    global features, THRESHOLD
    try:
        if FEATURE_FILE in os.listdir("/sd"):
            with open(FEATURE_FILE, "r") as f:
                data = ujson.load(f)
                features = data["features"]
                THRESHOLD = data.get("threshold", 98.5)
                print("Loaded {} classes".format(len(features)))
        else:
            features = []
    except:
        features = []

# ===== 按钮检测 =====
class Button:
    def __init__(self, callback):
        self.callback = callback
        self.last_state = 1
        self.press_time = 0

    def update(self):
        current = boot_gpio.value()
        now = time.ticks_ms()

        if current != self.last_state:
            if current == 0:  # 按下
                self.press_time = now
            else:  # 释放
                if now - self.press_time > 1000:  # 长按
                    self.callback(EVENT.BOOT_KEY_LONG_PRESS)
                else:  # 短按
                    self.callback(EVENT.BOOT_KEY)
            self.last_state = current

# ===== 主应用 =====
class SelfLearningApp:
    def __init__(self):
        self.state = STATE.INIT
        self.train_class = 0
        self.train_count = 0
        self.kpu = KPU()
        self.kpu.load_kmodel("/sd/KPU/self_learn_classifier/mb-0.25.kmodel")
        self.button = Button(self.handle_event)
        self.last_reported_class = None
        self.last_uart_send_time = 0  # 新增：记录上次发送时间
        load_features()

    def handle_event(self, event):
        if event == EVENT.BOOT_KEY_LONG_PRESS:
            self.state = STATE.INIT
            features.clear()
            save_features()
            return

        if self.state == STATE.INIT:
            self.state = STATE.TRAIN_CLASS_1
            self.train_class = 0
            self.train_count = 0
            features.append([])

        elif self.state in (STATE.TRAIN_CLASS_1, STATE.TRAIN_CLASS_2):
            img = sensor.snapshot()
            feature = self.kpu.run_with_output(img, get_feature=True)
            features[self.train_class].append(feature)
            self.train_count += 1

            if self.train_count >= max_train_pic:
                self.train_class += 1
                if self.train_class >= 2:
                    self.state = STATE.CLASSIFY
                    save_features()
                else:
                    features.append([])
                    self.train_count = 0

    def run(self):
        while True:
            self.button.update()
            img = sensor.snapshot()
            current_time = time.ticks_ms()  # 获取当前时间

            if self.state == STATE.INIT:
                img.draw_string(80, 100, "Press to start", lcd.WHITE, scale=2)

            elif self.state in (STATE.TRAIN_CLASS_1, STATE.TRAIN_CLASS_2):
                img.draw_string(10, 10, "Training {}".format(CLASS_NAMES[self.train_class]), lcd.WHITE)
                img.draw_string(10, 30, "Samples: {}/{}".format(self.train_count, max_train_pic), lcd.WHITE)

            elif self.state == STATE.CLASSIFY:
                if features:
                    feature = self.kpu.run_with_output(img, get_feature=True)
                    max_score = 0
                    best_class = -1
                    for i in range(2):  # 只检查两个类别
                        for cf in features[i]:
                            score = self.kpu.feature_compare(cf, feature)
                            if score > max_score:
                                max_score = score
                                best_class = i

                    if max_score > THRESHOLD and best_class != -1:
                        class_name = CLASS_NAMES.get(best_class, "unknown")
                        img.draw_string(10, 10, "Class: {}".format(class_name), lcd.GREEN)
                        img.draw_string(10, 30, "Score: {:.1f}%".format(max_score), lcd.GREEN)

                        # 新增：每秒发送一次串口信息
                        if best_class == 0:  # 只对mango发送
                            if time.ticks_diff(current_time, self.last_uart_send_time) > 1000:  # 1秒间隔
                                uart.write("@Yellow\r\n")
                                print("UART sent: mango (1s interval)")
                                self.last_uart_send_time = current_time
                        self.last_reported_class = best_class

            lcd.display(img)
            time.sleep_ms(50)

# ===== 主程序 =====
if __name__ == "__main__":
    if "sd" not in os.listdir("/"):
        lcd.draw_string(0, 0, "Insert SD card!", lcd.RED)
        while True: pass

    app = SelfLearningApp()
    app.run()
