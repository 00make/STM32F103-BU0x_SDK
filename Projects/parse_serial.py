import serial
import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import math
import csv
from datetime import datetime
import os

# 存储历史数据

yinvert = True

# 创建CSV文件


def create_csv_file():
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f'uwb_data_{timestamp}.csv'
    with open(filename, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['时间戳', 'X(cm)', 'Y(cm)', '距离(cm)',
                        '计算距离(cm)', '功率(dB)', '角度(度)'])
    return filename


class DataStore:
    def __init__(self):
        self.x_data = []
        self.y_data = []
        self.csv_file = create_csv_file()

    def add_point(self, x, y):
        self.x_data.append(x)
        self.y_data.append(y)
        # 只保留最近的50个点
        if len(self.x_data) > 50:
            self.x_data.pop(0)
            self.y_data.pop(0)

    def save_to_csv(self, x, y, distance0, distance, power, angle):
        with open(self.csv_file, 'a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([
                datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"),
                f"{x:.2f}",
                f"{y:.2f}",
                f"{distance0:.2f}",
                f"{distance:.2f}",
                f"{power:.2f}",
                f"{angle:.2f}"
            ])


data_store = DataStore()
fig, ax = plt.subplots()


def parse_twr_data(line):
    try:
        json_str = line[6:] if line.startswith('J') else line
        data = json.loads(json_str)['TWR']
        x, y = data["Xcm"], -data["Ycm"] if yinvert else data["Ycm"]
        distance0 = data["D"]
        power = data["P"]
        distance = math.sqrt(x**2 + y**2)
        angle = math.atan2(x, y) * 180 / math.pi  # 使用atan2更准确

        print(f'位置: X={x}cm, Y={y}cm, distance0={distance0}cm, distance={distance:.2f}cm, '
              f'功率={power}dB, 角度={angle:.2f}°')

        data_store.add_point(x, y)
        data_store.save_to_csv(x, y, distance0, distance, power, angle)
    except Exception as e:
        print(f'解析错误: {e}')


def update_plot(frame):
    ax.clear()
    if data_store.x_data:
        ax.scatter(data_store.x_data, data_store.y_data, c='blue', s=50)
        # 标记最新的点
        ax.scatter(data_store.x_data[-1], data_store.y_data[-1],
                   c='red', s=100, label='Beacon')
    ax.set_xlabel('X  (cm)')
    ax.set_ylabel('Y  (cm)')
    ax.set_title('UWB PdoA Real-time')
    ax.grid(True)
    ax.legend()
    # 设置固定的显示范围
    ax.set_xlim(-200, 200)
    ax.set_ylim(-200, 200)


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
