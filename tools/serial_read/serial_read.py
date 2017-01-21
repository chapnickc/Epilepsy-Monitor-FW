import serial
import time
import sys, signal

def signal_handler(signal, frame):
    print("\nProgram exiting gracefully")
    if ser:
        ser.close()
    if f:
        f.close()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

timenow = time.strftime("%Y%m%d_%H%M%S")

baud = 115200
ports = ['/dev/cu.usbmodem1411','/dev/cu.usbmodem1412',  '/dev/cu.usbmodem1422']
for port in ports:
    try:
        print("Trying port {}".format(port))
        ser = serial.Serial(port, baud)
        break
    except:
        print('Could not connnect to port {}'.format(port))

if not ser.is_open:
    print('Serial port is closed')
    ser.close()
    sys.exit()
else:
    outfile = "recording_" + timenow + '.txt'
    with open(outfile, 'wb') as f:
        while ser.is_open:
            line = ser.readline()
            f.write(line)
            print(line.decode('UTF-8').strip())
