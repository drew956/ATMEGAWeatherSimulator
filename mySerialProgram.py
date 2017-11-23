#serial input output attempt

import serial
ser = serial.Serial('/dev/cu.usbserial-A106TG3S')
print(ser.name)

for i in range(10):
    x = ser.read(1) #read one byte
    print(ord(x))
    
