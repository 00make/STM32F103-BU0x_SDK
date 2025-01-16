import serial
import json

def parse_twr_data(line):
    try:
        # 移除所有以J开头的前缀
        if line.startswith('J'):
            json_str = line[6:]  # 跳过前6个字符 (如"JS006C")
        # 解析JSON数据
        data = json.loads(json_str)
        # 提取Xcm、Ycm、D和P
        xcm = data['TWR']['Xcm']
        ycm = data['TWR']['Ycm']
        distance = data['TWR']['D']
        power = data['TWR']['P']
        print(f'位置: X={xcm}cm, Y={ycm}cm, 距离={distance}cm, 功率={power}dB')
    except Exception as e:
        print(f'解析错误: {e}')

def main():
    # 打开串口
    ser = serial.Serial('/dev/tty.usbmodem2401', 115200)
    print('---- 已打开串行端口', ser.name, '----\n')
    
    try:
        while True:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8').strip()
                parse_twr_data(line)
    except KeyboardInterrupt:
        print('\n程序已终止')
        ser.close()

if __name__ == '__main__':
    main()
