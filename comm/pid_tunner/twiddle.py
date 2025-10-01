import serial
from time import sleep
from random import randint

serial_port = input("insert ESP32 location (default = \"/dev/ttyACM0\"):\n")

if serial_port == "":
    serial_port = '/dev/ttyACM0'

print("\nusing serial port: ",serial_port)

esp32 = serial.Serial(serial_port, 115200)

def twiddle(k, dk, ksi=.3, target=None):
    if not target:
        target = run_pid_test(*k)

    for i, _ in enumerate(k):
        k[i] += dk[i]
        new_error = run_pid_test(*k)

        if new_error < target:
            target = new_error
            dk[i] *= 1 + ksi

        else:
            k[i] -= 2*dk[i]
            new_error = run_pid_test(*k)

            if new_error < target:
                target = new_error
                dk[i] *= 1 + ksi

            else:
                k[i] += dk[i]
                dk[i] *= 1 - ksi
               

    return k, dk, ksi, target

def run_pid_test(kp, ki=0, kd=0):
    encoded = (f"<{0},{0},{0},{0},{3},{1000*kp},{1000*ki},{1000*kd},{9},{0},{0},{0}>".encode())
    print("sending: ", encoded)
    esp32.write(encoded)
    sleep(7)
    error = esp32.readline()
    error = error.decode()
    print("error rec = ",error)
    return abs(float(error))

params = [[0], [0.5]]
while True:
    command = input("(1) run one\n"
                    "(2) run n\n"
                    "(3) close\n").strip()

    if command == "3":
        break

    n = 1
    if command == "2":
        n = int(input("\tn: ").strip())

    for _ in range(n):
        params = twiddle(*params)
        print("-------------------------------------------------------------------------------",
              "best (kp, ki, kd):", params[0],
              "\nbest error:", params[3],
              "\ncurrent (dks): ", params[1])


esp32.close()

