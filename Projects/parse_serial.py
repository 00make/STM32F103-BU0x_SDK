import serial
import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import math
from datetime import datetime

# 创建CSV文件


class DataStore:
    def __init__(self):
        self.x_data = []
        self.y_data = []
        self.last_angle = None
        self.cumulative_angle = 0

    def add_point(self, x, y):
        self.x_data.append(x)
        self.y_data.append(y)
        # 只保留最近的50个点
        if len(self.x_data) > 50:
            self.x_data.pop(0)
            self.y_data.pop(0)

    def process_angle(self, new_angle):
        if self.last_angle is None:
            self.last_angle = new_angle
            return new_angle

        # 计算角度差
        diff = new_angle - self.last_angle

        # 处理角度跳变
        if diff > 180:
            diff -= 360
        elif diff < -180:
            diff += 360

        # 累积角度
        self.cumulative_angle += diff
        self.last_angle = new_angle

        return self.cumulative_angle


data_store = DataStore()
fig, ax = plt.subplots()


def parse_twr_data(line):
    try:
        json_str = line[6:] if line.startswith('J') else line
        data = json.loads(json_str)['TWR']
        x, y = data["Xcm"], data["Ycm"]
        distance0 = data["D"]

        distance = math.sqrt(x**2 + y**2)  # 计算距离 这个已经滤波 比较准确了
        raw_angle = math.atan2(x, y) * 180 / math.pi

        # 处理累积角度和功率
        angle = data_store.process_angle(raw_angle)

        print(f'位置: X={x}cm, Y={y}cm, distance0={distance0}cm, distance={distance:.2f}cm, '
              f' 原始角度={raw_angle:.2f}°, 累积角度={angle:.2f}°')

        data_store.add_point(x, y)

    except Exception as e:
        print(f'解析错误: {e}')


def update_plot(frame):
    ax.clear()
    if data_store.x_data:
        # 绘制历史轨迹点
        ax.scatter(data_store.x_data, data_store.y_data, c='blue', s=50)
        # 标记最新的点
        ax.scatter(data_store.x_data[-1], data_store.y_data[-1],
                   c='red', s=100, label='Beacon')

        # 获取当前的角度值
        current_angle = data_store.last_angle

        # 计算线的终点坐标
        # 角度线（红色）- 长度固定为100
        angle_rad = math.radians(current_angle)
        angle_x = 100 * math.sin(angle_rad)
        angle_y = 100 * math.cos(angle_rad)
        ax.plot([0, angle_x], [0, angle_y], 'r-',
                label=f'angle: {current_angle:.1f}°', linewidth=2)

    ax.set_xlabel('X  (cm)')
    ax.set_ylabel('Y  (cm)')
    ax.set_title('UWB PdoA Real-time')
    ax.grid(True)
    ax.legend()
    # 设置固定的显示范围
    ax.set_xlim(-100, 100)
    ax.set_ylim(100, -100)


def main():
    ani = FuncAnimation(fig, update_plot, interval=100)
    plt.show(block=False)

    with serial.Serial('/dev/tty.usbmodem2401', 115200) as ser:
        print(f'---- 已打开串行端口 {ser.name} ----\n')
        try:
            while True:
                if ser.in_waiting:
                    parse_twr_data(ser.readline().decode('utf-8').strip())
                plt.pause(0.01)  # 让matplotlib有时间更新图形
        except KeyboardInterrupt:
            print('\n程序已终止')
            plt.close()


if __name__ == '__main__':
    main()
