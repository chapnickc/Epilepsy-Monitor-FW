import serial
import os, sys, signal
import re, time, glob


def signal_handler(signal, frame):
    """
    Gracefully exit the program when
    ^c is pressed.
    """
    print("\nProgram exiting gracefully")
    if ser:
        ser.close()
    if f:
        f.close()
    sys.exit(0)

def stream_to_txt(port, baud = 115200):
    global ser
    with serial.Serial(port, baud) as ser:
        print("writing")
        _write_ppg(ser)



def _write_ppg(ser):
    global f
    timenow = time.strftime("%Y%m%d_%H%M%S")
    if not os.path.exists('./data/'):
        os.makedirs('./data')
    outfile = "./data/recording_" + timenow + '.txt'
    with open(outfile, 'w') as f:
        while ser.is_open:
            line = ser.readline().decode('utf8').strip()
            sample = re.findall('(\(\d+?,\d+?\))',line)
            for s in sample:
                f.write(s+'\n')
                print(s)



if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)


    # list all available ports
    ports = glob.glob('/dev/cu.usb*')
    stream_to_txt(ports[0],115200)

