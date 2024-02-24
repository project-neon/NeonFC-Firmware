import serial
from random import randint

# No Linux, a porta não será "COMX" e sim algo parecido com a linha abaixo
esp32 = serial.Serial('/dev/ttyUSB0', 115200)
# esp32 = serial.Serial("COM6", 115200)

i = 0
while i < 200000:
    # n1 = float(input())
    # n2 = float(input())
    id1 = str(5)
    v_l1 = str(.3)
    v_a1 = str(0)

    id2 = str(7)
    v_l2 = str(.3)
    v_a2 = str(0)

    id3 = str(8)
    v_l3 = str(.3)
    v_a3 = str(0)

    vel = (f"<{id1},{v_l1},{v_a1},{id2},{v_l2},{v_a2},{id3},{v_l3},{v_a3}>")
    esp32.write(vel.encode())
    print(vel)

esp32.close()
