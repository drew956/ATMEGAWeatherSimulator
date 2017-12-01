from urllib.request import urlopen
from urllib.request import HTTPError
from            bs4 import BeautifulSoup
import serial
import re
import sys
import time 
PORT = '/dev/cu.usbserial-A106TG3S'
BAUDRATE = 9600
TIMEOUT = None

ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT, write_timeout=None)
print(ser.name)
   
def getWindSpeed():
    try:
        html = urlopen("https://weather.com/weather/today/l/33.42,-111.83")
    except HTTPError as e:
        return None
    try:
        bsObj = BeautifulSoup(html.read())
        windspeed = bsObj.findAll("span", {"id" : "dp0-details-wind" })[0].find("span").get_text()
        matchObj  = re.search(r'(\d+)', windspeed)
        windspeed = matchObj.group()        
    except AttributeError as e:
        return None
    return windspeed


def getTemperature():
    try:
        #html = urlopen("https://weather.com/weather/today/l/33.42,-111.83") #mesa
        html = urlopen("https://weather.com/weather/today/l/VEXX6492:1:VE") #hot place (africa)
    except HTTPError as e:
        return None
    try:
        bsObj = BeautifulSoup(html.read())
        temp = bsObj.findAll("div", {"class" : "today_nowcard-temp" })[0].find("span").get_text()
    except AttributeError as e:
        return None
    return temp[0:2] 

speed1 = sys.argv[1];
speed = speed1
while (1 == 1):
    #speed = '50'#getWindSpeed()
    ser.reset_input_buffer() #these are necessary
    ser.reset_output_buffer()

    temperature = '92'#getTemperature()
    print(speed)
    print(temperature)
    print("\r\n")
    ser.write(speed.encode("ascii"))
    ser.write('\r'.encode("ascii"))
    ack = ser.read(1)
    print(ack)
    
    ser.write(temperature.encode("ascii"))
    ser.write('\r'.encode("ascii"))
    ack = ser.read(1)
    print(ack)
    print("going to sleep")
    #time.sleep(5)