import serial
from time import sleep
from random import randint

esp32 = serial.Serial('/dev/ttyUSB0', 115200)

def twiddle(k, dk, ksi=.3, target=None):
    if not target:
        target = run_pid_test(k)

    for i, _ in enumerate(k):
        k[i] += dk[i]
        new_error = run_pid_test(k)

        if new_error < target:
            target = new_error
            dk[i] *= 1 + ksi

        else:
            k[i] -= 2*dk[i]
            new_error = run_pid_test(k)

            if new_error < target:
                target = new_error
                dk[i] *= 1 + ksi

            else:
                k[i] += dk[i]
                dk[i] *= 1 - ksi
               

    return k, dk, ksi, target

def run_pid_test(ks):
    if len(ks) == 2:
        print(f"<{3},{ks[0]},{0},{0},{ks[1]},{0},{0}>")
        esp32.write(f"<{3},{1000*ks[0]},{0},{0},{1000*ks[1]},{0},{0}>".encode())
    else:
        print(f"<{3},{ks[0]},{ks[1]},{ks[2]},{ks[3]},{ks[4]},{ks[5]}>")
        esp32.write(f"<{3},{1000*ks[0]},{1000*ks[1]},{1000*ks[2]},{1000*ks[3]},{1000*ks[4]},{1000*ks[5]}>".encode())
    sleep(7)
    error = esp32.readline()
    error = error.decode()
    print(error)
    return abs(float(error))

params = [[0, 0], [0.5, 0.5]]
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

