from urllib.request import urlopen
from urllib.request import HTTPError
from            bs4 import BeautifulSoup
import serial
import re

ser = serial.Serial('/dev/cu.usbserial-A106TG3S')
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
        html = urlopen("https://weather.com/weather/today/l/33.42,-111.83")
    except HTTPError as e:
        return None
    try:
        bsObj = BeautifulSoup(html.read())
        temp = bsObj.findAll("div", {"class" : "today_nowcard-temp" })[0].find("span").get_text()
    except AttributeError as e:
        return None
    return temp[0:2] 


while (1 == 1):
    speed = getWindSpeed()
    #print("%d" % ord(speed))
    ser.write(speed.encode("ascii"))
    ser.write('\r'.encode("ascii"))
    