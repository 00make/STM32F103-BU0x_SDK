import serial
import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import math
import numpy as np


class ParticleFilter:
    def __init__(self, n_particles=100):
        self.n_particles = n_particles
        self.particles = None
        self.weights = np.ones(n_particles) / n_particles
        self.initialized = False
        
    def initialize(self, x, y):
        # 初始化粒子，在初始位置周围添加少量噪声
        self.particles = np.random.normal(
            loc=np.array([[x, y]]),
            scale=np.array([[1.0, 1.0]]),
            size=(self.n_particles, 2)
        )
        self.initialized = True
        
    def predict(self):
        # 添加过程噪声
        self.particles += np.random.normal(0, 0.5, self.particles.shape)
        
    def update(self, measurement):
        if not self.initialized:
            self.initialize(measurement[0], measurement[1])
            return measurement
            
        # 预测步骤
        self.predict()
        
        # 计算每个粒子的权重
        diff = self.particles - measurement
        self.weights = np.exp(-0.5 * np.sum(diff**2, axis=1))
        self.weights += 1e-300  # 防止权重为0
        self.weights /= sum(self.weights)  # 归一化
        
        # 重采样
        indices = np.random.choice(
            self.n_particles,
            self.n_particles,
            p=self.weights
        )
        self.particles = self.particles[indices]
        
        # 返回状态估计（加权平均）
        return np.average(self.particles, weights=self.weights, axis=0)


class DataStore:
    def __init__(self):
        self.x_data = []
        self.y_data = []
        self.last_angle = None
        self.particle_filter = ParticleFilter(n_particles=100)
        
    def add_point(self, x, y):
        # 使用粒子滤波进行平滑
        filtered_pos = self.particle_filter.update(np.array([x, y]))
        x_filtered, y_filtered = filtered_pos
        
        self.x_data.append(x_filtered)
        self.y_data.append(y_filtered)
        if len(self.x_data) > 50:  # 只保留最近的50个点
            self.x_data.pop(0)
            self.y_data.pop(0)
            
        return x_filtered, y_filtered

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
        
        # 获取滤波后的坐标
        x_filtered, y_filtered = data_store.add_point(x, y)
        distance = math.sqrt(x_filtered**2 + y_filtered**2)
        angle = math.atan2(x_filtered, y_filtered) * 180 / math.pi
        
        data_store.process_angle(angle)

        print(f'Raw: X={x:4.0f}cm, Y={y:4.0f}cm | Filtered: X={x_filtered:4.0f}cm, '
              f'Y={y_filtered:4.0f}cm | Distance={distance:4.0f}cm, Angle={angle:4.0f}°')
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
