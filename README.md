# ATMEGAWeatherSimulator
Python and C code for creating a mini windmill to simulate wind speeds and other weather-based information.

Project plan:
    Connect Raspberry Pi running Python3 to atmega168, which has a DC motor and other peripherals connected to it.
    The Raspberry Pi will use the python BeautifulSoup4 library to webscrape the current wind speed and temperature, and other information.
    This information will be transmitted to the atmega168 every so often, which will be used to simulate wind speed by turning a dc motor with a fan.
    Essentially, the fan will become a tiny "windmill".
    
    Due to potential limitations in the speed of the DC motor, Pulse Width Modulation is what I intend on using.
    
    
Current_Ideas:
    11-23-2017
        if we can, add a buzzer to go off if the wind speed reaches a Level 1 warning speed or higher.

Current To-Do:
    Connect Atmega to Motor, get prototype working.
    Connect Atmega to Raspberry Pi, get data from internet simulated on Motor
    Connect LCD and LEDS to Atmega, get them to light up with the appropriate data.
    
    Potentially design a communication protocol to allow the atmega to communicate and request specific data 
        or, alternatively, have both sides already know what is being requested and in what format the data will be returned

    
   
    

