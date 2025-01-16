import serial
import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import math


class DataStore:
    def __init__(self):
        self.x_data = []
        self.y_data = []
        self.last_angle = None

    def add_point(self, x, y):
        self.x_data.append(x)
        self.y_data.append(y)
        if len(self.x_data) > 50:  # 只保留最近的50个点
            self.x_data.pop(0)
            self.y_data.pop(0)

    def process_angle(self, new_angle):
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


data_store = DataStore()
fig, ax = plt.subplots()


def parse_twr_data(line):
    try:
        json_str = line[6:] if line.startswith('J') else line
        data = json.loads(json_str)['TWR']
        x, y = data["Xcm"], data["Ycm"]
        distance = math.sqrt(x**2 + y**2)
        angle = math.atan2(x, y) * 180 / math.pi

        data_store.add_point(x, y)
        data_store.process_angle(angle)

        print(f'X={x:4.0f}cm, Y={y:4.0f}cm, Distance={
              distance:4.0f}cm, Angle={angle:4.0f}°')
    except Exception as e:
        print(f'解析错误: {e}')


def update_plot(frame):
    ax.clear()
    if data_store.x_data:
        # 绘制历史轨迹
        ax.scatter(data_store.x_data, data_store.y_data, c='blue', s=50)
        # 当前位置
        ax.scatter(data_store.x_data[-1], data_store.y_data[-1],
                   c='red', s=100, label='Beacon')
        # 角度指示线
        if data_store.last_angle is not None:
            rad = math.radians(data_store.last_angle)
            ax.plot([0, 100 * math.sin(rad)], [0, 100 * math.cos(rad)],
                    'r-', label=f'{data_store.last_angle:.0f}°', linewidth=2)

    ax.grid(True)
    ax.legend()
    ax.set_xlabel('X (cm)')
    ax.set_ylabel('Y (cm)')
    ax.set_title('UWB PdoA Real-time')
    ax.set_xlim(-100, 100)
    ax.set_ylim(100, -100)


def main():
    ani = FuncAnimation(fig, update_plot, interval=100)
    plt.show(block=False)

    with serial.Serial('/dev/tty.usbmodem2401', 115200) as ser:
        print(f'串口 {ser.name} 已打开')
        try:
            while True:
                if ser.in_waiting:
                    parse_twr_data(ser.readline().decode('utf-8').strip())
                plt.pause(0.01)
        except KeyboardInterrupt:
            print('\n程序已终止')
            plt.close()


if __name__ == '__main__':
    main()
