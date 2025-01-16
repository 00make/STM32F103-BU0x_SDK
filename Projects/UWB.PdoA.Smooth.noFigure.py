import serial
import json
import math
import time


class DataStore:
    """
    数据存储类，用于存储和处理 UWB 数据。
    包括坐标、时间戳、速度、方向等，并提供滑动平均滤波功能。
    """

    def __init__(self):
        self.x_data = []  # 存储滤波后的 X 坐标
        self.y_data = []  # 存储滤波后的 Y 坐标
        self.last_angle = None  # 上一个角度值，用于角度跳变处理
        self.window_size = 10  # 滑动平均窗口大小
        self.x_window = []  # X 坐标滑动窗口
        self.y_window = []  # Y 坐标滑动窗口
        self.timestamps = []  # 时间戳
        self.speeds = []  # 存储滤波后的速度
        self.speed_window = []  # 速度滑动窗口
        self.directions = []  # 方向

    def moving_average(self, value, window):
        """
        滑动平均滤波函数。

        Args:
            value: 要滤波的新数据点。
            window: 滑动窗口列表。

        Returns:
            滤波后的数据点。
        """
        window.append(value)
        if len(window) > self.window_size:
            window.pop(0)
        return sum(window) / len(window)

    def add_point(self, x, y):
        """
        添加一个新的数据点，并进行滤波处理，计算速度和方向。

        Args:
            x: X 坐标。
            y: Y 坐标。

        Returns:
            滤波后的 X 和 Y 坐标。
        """
        timestamp = time.time()

        # 应用滑动平均滤波
        x_filtered = self.moving_average(x, self.x_window)
        y_filtered = self.moving_average(y, self.y_window)

        self.x_data.append(x_filtered)
        self.y_data.append(y_filtered)
        self.timestamps.append(timestamp)

        # 只保留最近的 200 个点
        if len(self.x_data) > 150:
            self.x_data.pop(0)
            self.y_data.pop(0)
            self.timestamps.pop(0)

        # 计算速度和方向
        if len(self.x_data) > 1:
            delta_x = self.x_data[-1] - self.x_data[-2]
            delta_y = self.y_data[-1] - self.y_data[-2]
            delta_t = self.timestamps[-1] - self.timestamps[-2]
            speed = math.sqrt(delta_x**2 + delta_y**2) / delta_t

            # 对速度应用滑动平均滤波
            speed_filtered = self.moving_average(speed, self.speed_window)

            # 计算方向时，如果速度很小，则保持上一次的方向，避免方向抖动
            direction = math.atan2(
                delta_y, delta_x) * 180 / math.pi if speed_filtered > 0.1 else self.directions[-1] if self.directions else 0

            self.speeds.append(speed_filtered)
            self.directions.append(direction)
        else:
            self.speeds.append(0)
            self.directions.append(0)

        return x_filtered, y_filtered

    def process_angle(self, new_angle):
        """
        处理角度跳变问题，例如从 359 度跳到 1 度。

        Args:
            new_angle: 新的角度值。

        Returns:
            处理后的角度值。
        """
        if self.last_angle is None:
            self.last_angle = new_angle
            return new_angle

        diff = new_angle - self.last_angle
        # 处理角度跳变
        if diff > 180:
            diff -= 360
        elif diff < -180:
            diff += 360

        self.last_angle = new_angle
        return new_angle


def parse_twr_data(line):
    """
    解析串口接收到的 TWR 数据。

    Args:
        line: 串口接收到的一行数据。
    """
    try:
        json_str = line[6:] if line.startswith('J') else line
        data = json.loads(json_str)['TWR']
        x, y = data["Xcm"], data["Ycm"]

        # 获取滤波后的坐标
        x_filtered, y_filtered = data_store.add_point(x, y)
        distance = math.sqrt(x_filtered**2 + y_filtered**2)
        angle = math.atan2(x_filtered, y_filtered) * 180 / math.pi

        data_store.process_angle(angle)

        # 获取速度和方向
        speed_filtered = data_store.speeds[-1] if data_store.speeds else 0
        direction = data_store.directions[-1] if data_store.directions else 0

        # 打印信息
        print(f'Raw: X={x:4.0f}cm, Y={y:4.0f}cm | '
              f'Filtered: X={x_filtered:4.0f}cm, Y={y_filtered:4.0f}cm | '
              f'Distance={distance:4.0f}cm, Angle={angle:4.0f}° | '
              f'Speed={speed_filtered:4.2f}cm/s, Direction={direction:4.0f}°')
    except Exception as e:
        print(f'解析错误: {e}')


if __name__ == '__main__':
    # 初始化数据存储对象和绘图对象
    data_store = DataStore()

    # 启动串口通信
    with serial.Serial('/dev/tty.usbmodem2401', 115200) as ser:
        print(f'串口 {ser.name} 已打开')

        try:
            while True:
                if ser.in_waiting:
                    parse_twr_data(ser.readline().decode('utf-8').strip())

        except KeyboardInterrupt:
            print('\n程序已终止')
