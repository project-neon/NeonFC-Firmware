import serial
from random import randint

# No Linux, a porta não será "COMX" e sim algo parecido com a linha abaixo
# comport = serial.Serial('/dev/ttyUSB0', 115200)
esp32 = serial.Serial("COM6", 115200)

i = 0
while i < 200000:
    v_r = str(randint(0, 99))
    v_l = str(randint(0, 99))
    vel = (v_r+" "+v_l)
    esp32.write(vel.encode())
    bateria = esp32.readline().decode('ascii')
    print(v_r, v_l)
    print(bateria)

esp32.close()
