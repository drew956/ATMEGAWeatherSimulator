#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>                  
#include <util/delay.h>   
#include <string.h>           
#include "USART.h"
#include "lcd.h"


void pwmMotor(int partsOn);
void myPrintToLCD(char speed[]);
void setTempLeds(int temperature);
void printWindSpeedAndTemperature(char speed[], char temp[]);

void delay_ms(uint16_t count) {
  while(count--) {
    _delay_ms(1);

  }
}

void delay_us(uint16_t count) {
  while(count--) {
    _delay_us(1);

  }
}

/* 
    Reads a string up to a max length and returns it. 
    Same as the one defined in the USART.c, except it doesn't 
    echo the bytes it receives.
*/
void silentReadString(char myString[], uint8_t maxLength) {
  char response;
  uint8_t i;
  i = 0;
  
  while (i < (maxLength - 1)) {                   /* prevent over-runs */
    response = receiveByte();
    //transmitByte(response);                                    /* echo */
    if (response == '\r') {                     /* enter marks the end */
      break;
    }
    else {
      myString[i] = response;                       /* add in a letter */
      i++;
    }
  }
  myString[i] = 0;                          /* terminal NULL character */
}
/* 
    Turns an ASCII sequence of numbers into their actual integer equivalent
    Does not work with negative numbers.
    That is something that will be implemented in the future.
*/
int convertToNumber(char p[]){
    p = strrev(p);

    int i = 0;
    char thisChar = p[i];
    int sum = 0;
    int mult = 1;
    do{
        sum += (thisChar - '0') * mult;
        mult *= 10;
        i++;
        thisChar = p[i];
    }while(thisChar != 0);
    return sum;
}

volatile int cyclesLater = 0;


int main(void) {
    initUSART();
    lcd_init(LCD_DISP_ON);
    /* 
        pb4 - red led
        pb3 - blue led
        pb2 - in2 for motor driver (Really in3 and in4, but whatever, same idea)
        pb1 - in1 for motor driver
        pb0 - enable bit for motor driver
    */
    DDRB   |= 0b00011111; //set pins to output
    PORTB   = 0x00;       //initialize (probably not necessary)

    //turn on the motor
    PORTB |=  (1 << PB0); //digitalWrite (enableB, HIGH); //enable the motor    
    PORTB &= ~(1 << PB2); //        digitalWrite (MotorB1, LOW);
    PORTB |=  (1 << PB1); //        digitalWrite (MotorB2, HIGH);
    
    int  speed = 3;
    int  temperature = 85;//default, it's above "nice" and below "hot"
    char speedChar[5];   //used for reading in data and displaying it on the LCD
    char tempChar[5];    //used for reading in data and displaying it on the LCD
    
    while (1) {
        

        /* 
            Whenever we receive data
                - try and stop the motor by going reverse for 200 milliseconds
                   - it takes a long while for max speed to update to a slower speed
                   - not sure how to fix this without adding in a longer delay based
                     on the prior windspeed
                - read in the new wind speed value
                - read in the new temperature value
                - update the LCD
                  (because there is no need to update it until we get new data)
                - set the motor in the right direction again.
                
        */
        if( bit_is_set(UCSR0A, RXC0) ){
            
            
            //reverse direction to stop motor 
            //useful if wind speed is >= 75
            PORTB |=   (1 << PB2); //        digitalWrite (MotorB1, HIGH);
            PORTB &=  ~(1 << PB1); //        digitalWrite (MotorB2, LOW);
            _delay_ms(200); //reverse for 200 milliseconds

            silentReadString(speedChar, 5);
            //printString(speedChar);
            
            
            silentReadString(tempChar, 5);
            //printString(tempChar);
            
            printWindSpeedAndTemperature(speedChar, tempChar);
            temperature = convertToNumber(tempChar);
            speed = convertToNumber(speedChar);
            //printWord(temperature);

            //set back to the right direction
            PORTB &=  ~(1 << PB2); //        digitalWrite (MotorB1, LOW);
            PORTB |=  (1 << PB1);  //        digitalWrite (MotorB2, HIGH);
            
        }

        //int temperature = getNumber();
        pwmMotor(speed);
        setTempLeds(temperature);
        //myPrintToLCD(speed);
    
    }                                                  /* End event loop */
    return 0;                            /* This line is never reached */
}


/*
    software pwm
    maximum: 150
    spectrum 150 = 1000 mseconds
             3   = 3 / 150 * 1000
                 = 1/50 * 1000
                 20 ms
    description:
        This function uses PWM to control the speed of the motor.
        It does this by enabling and disabling the motor for a fraction of the time interval
        The fraction is calculated based on the maximum wind speed of 150 miles per hour. 
        This was based on the minimum wind speed of a Level 5 hurricane, which is the maximum 
        danger level possible. (156 miles per hour)
        
    extra info:
        The period of 500 milliseconds was determined by experimentation.
        500 milliseconds was chosen as a good middle value for having 
        the lower values be fast enough and the larger values slow enough.
        Too low and the lower values would not move the motor very much.

*/
void pwmMotor(int partsOn){
    int partsMax = 150;
    int timeOn   = (500 * partsOn)/partsMax; //calculate how long it should be on or off
    int timeOff  = 500 - timeOn;             //based on periods of 500 milliseconds
                                            
    
    PORTB |= (1 << PB0); //digitalWrite (enableB, HIGH); //enable the motor    
    delay_ms(timeOn);

    PORTB &= ~(1 << PB0); //digitalWrite (enableB, LOW); //stop the motor    
    delay_ms(timeOff);
        
    
}

/* 
    Turn on the Green LED if Temperature is less than 90 degrees
        and turn off the Red LED
    Turn on the Red LED if the Temperature is Greater than or equal to 90 degrees
        and turn off the Green LED
*/
void setTempLeds(int temperature){
    if(temperature < 90){
        PORTB |= 1 << PB3;    //turn on  green led
        PORTB &= ~(1 << PB4); //turn off red LED
    } else if(temperature >= 90){    
        PORTB &= ~(1 << PB3); //turn off green led
        PORTB |= 1 << PB4;    //turn on  red LED
    }
}

/* 
    Print a single string to the first line of the LCD
*/
void myPrintToLCD(char result[]){
        lcd_clrscr();
        lcd_gotoxy(0,0);
        lcd_puts(result);
        _delay_ms(1000);

}
/* 
    Print the current WindSpeed and Temperature to the LCD
    on lines 1 and 2 respectively
    It has a 1 second delay, although perhaps this should be removed.
*/
void printWindSpeedAndTemperature(char speed[], char temp[]){
    lcd_clrscr();
    lcd_gotoxy(0,0);
    lcd_puts("Wind speed: ");

    lcd_gotoxy(11, 0);
    lcd_puts(speed);
    
    lcd_gotoxy(0,1);
    lcd_puts("Temperature: ");

    lcd_gotoxy(12,1);
    lcd_puts(temp);

    lcd_gotoxy(15,1);
    lcd_puts("F");
    _delay_ms(1000);
}