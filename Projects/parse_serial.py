import serial
import json


def parse_twr_data(line):
    try:
        json_str = line[6:] if line.startswith('J') else line
        data = json.loads(json_str)['TWR']
        print(f'位置: X={data["Xcm"]}cm, Y={data["Ycm"]}cm, 距离={
              data["D"]}cm, 功率={data["P"]}dB')
    except Exception as e:
        print(f'解析错误: {e}')


def main():
    with serial.Serial('/dev/tty.usbmodem2401', 115200) as ser:
        print(f'---- 已打开串行端口 {ser.name} ----\n')
        try:
            while True:
                if ser.in_waiting:
                    parse_twr_data(ser.readline().decode('utf-8').strip())
        except KeyboardInterrupt:
            print('\n程序已终止')


if __name__ == '__main__':
    main()
