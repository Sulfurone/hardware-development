import sensor, image, time, lcd, gc
from maix import KPU
import sys
from machine import UART
from fpioa_manager import fm

# 配置参数
class Config:
    MODEL_PATH = "/sd/model-195667.kmodel"  # 模型路径
    LABELS = ['熟芒果', '生芒果']           # 分类标签
    UART_PORT = UART.UART1                 # 串口端口
    UART_BAUDRATE = 9600                   # 串口波特率
    LCD_ROTATION = 0                       # LCD旋转角度
    SENSOR_SETTINGS = {
        'pixformat': sensor.RGB565,
        'framesize': sensor.QVGA,
        'hmirror': False,
        'vflip': False,
        'skip_frames': 200
    }

# 串口通信类
class UARTComm:
    def __init__(self):
        self.uart = UART(Config.UART_PORT, Config.UART_BAUDRATE,
                        timeout=1000, read_buf_len=256)

    def send_result(self, max_score, index, label):
        """发送分类结果到串口"""
        message = "%d:%.2f:%s\n" % (index, max_score, label)  # 兼容的格式化方式
        try:
            self.uart.write(message.encode())
        except Exception as e:
            print("UART发送失败:", e)

# 初始化硬件
def init_hardware():
    # 初始化LCD
    lcd.init(type=1)
    lcd.rotation(Config.LCD_ROTATION)
    lcd.clear(lcd.WHITE)

    # 初始化摄像头 - 使用直接方法调用确保兼容性
    sensor.reset()
    sensor.set_pixformat(Config.SENSOR_SETTINGS['pixformat'])
    sensor.set_framesize(Config.SENSOR_SETTINGS['framesize'])
    sensor.set_hmirror(Config.SENSOR_SETTINGS['hmirror'])
    sensor.set_vflip(Config.SENSOR_SETTINGS['vflip'])
    sensor.skip_frames(time=Config.SENSOR_SETTINGS['skip_frames'])

    return UARTComm()

# 主函数
def main():
    comm = init_hardware()
    kpu = None

    try:
        # 加载模型
        print("正在加载模型...")
        kpu = KPU()
        kpu.load_kmodel(Config.MODEL_PATH)
        print("模型加载成功!")

        clock = time.clock()

        while True:
            gc.collect()
            clock.tick()

            # 捕获图像
            img = sensor.snapshot()

            # 运行模型推理
            fmap = kpu.run_with_output(img)
            output = fmap[:]

            # 获取分类结果
            max_score = max(output)
            index = output.index(max_score)
            label = Config.LABELS[index]

            # 显示结果
            img.draw_string(10, 10, "状态: %s" % label, scale=2, color=(255, 0, 0))
            img.draw_string(10, 40, "置信度: %.2f" % max_score, scale=2, color=(255, 0, 0))
            img.draw_string(10, 200, "FPS: %.1f" % clock.fps(), scale=2, color=(0, 255, 0))
            lcd.display(img)

            # 发送结果到串口
            comm.send_result(max_score, index, label)

    except Exception as e:
        # 错误处理
        error_img = image.Image(size=(320, 240))
        error_img.draw_string(10, 100, "错误发生:", scale=1.5, color=(255, 0, 0))
        error_img.draw_string(10, 130, str(e), scale=1.5, color=(255, 0, 0))
        lcd.display(error_img)
        sys.print_exception(e)

    finally:
        # 清理资源
        if kpu:
            kpu.deinit()
        gc.collect()

if __name__ == "__main__":
    main()
