#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>                  
#include <util/delay.h>              
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

int convertToNumber(char p[]){
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
    PORTB   = 0x00; //initialize (probably not necessary)

    //turn on the motor
    PORTB |= (1 << PB0); //digitalWrite (enableB, HIGH); //enable the motor    
    PORTB &= ~(1 << PB2); //        digitalWrite (MotorB1, LOW);
    PORTB |=  (1 << PB1); //        digitalWrite (MotorB2, HIGH);
    
    int speed = 3;
    int temperature = 85;//default, it's above "nice" and below "hot"
    char speedChar[5];
    char tempChar[5];
    
    while (1) {
        

        
        if( bit_is_set(UCSR0A, RXC0) ){
            
            
            //reverse direction to stop motor 
            //useful if wind speed is >= 75
            PORTB |=   (1 << PB2); //        digitalWrite (MotorB1, HIGH);
            PORTB &=  ~(1 << PB1); //        digitalWrite (MotorB2, LOW);
            _delay_ms(200); //reverse for 200 milliseconds
            //printString("getting number\n");
            //void readString(char myString[], uint8_t maxLength) {

            //speed = readString();//getNumber();  
            //temperature = getNumber();   //this should wait until the data is ready
            silentReadString(speedChar, 3);
            transmitByte('y');
            
            
            silentReadString(tempChar, 3);
            transmitByte('y');
            
            //myPrintToLCD(speedChar);
            //myPrintToLCD(tempChar);
            temperature = convertToNumber(tempChar);
            printWindSpeedAndTemperature(speedChar, tempChar);
            //set back to the right direction
            PORTB &=  ~(1 << PB2); //        digitalWrite (MotorB1, LOW);
            PORTB |=  (1 << PB1);  //        digitalWrite (MotorB2, HIGH);
            
        }

        //int temperature = getNumber();
        //pwmMotor(speed);
        setTempLeds(temperature);
        //myPrintToLCD(speed);
    
    }                                                  /* End event loop */
    return 0;                            /* This line is never reached */
}


/*
    software pwm
    maximum: 150
    spectrum 150 = 1000 useconds
             3   = 3 / 150 * 1000
                 = 1/50 * 1000
                 20 us
    high high causes motor to stop faster than if you turned it off
    (due to it acting like a generator, magnetic field, etc)

    1 rot / 10 seconds
    0.1 rot / 1 second
    
    3 mph / x mph  * 1000 = 100
    0.1 = 3 / x
    x = 30 ??
    doesn't work because we don't know how many RPMs it is at full voltage
    
    full ~= 5000 r / min
    500 r/ 6 seconds
    83 rotations / second
    3 -> / 80
    240
    
    240 -> 1 rot / second
*/
void pwmMotor(int partsOn){
    int partsMax = 150;
    int timeOn   = (500 * partsOn)/partsMax;
    int timeOff  = 500 - timeOn;
    
    PORTB |= (1 << PB0); //digitalWrite (enableB, HIGH); //enable the motor    
    delay_ms(timeOn);

    PORTB &= ~(1 << PB0); //digitalWrite (enableB, LOW); //stop the motor    
    delay_ms(timeOff);
        
    
}

void setTempLeds(int temperature){
    if(temperature < 90){
        PORTB |= 1 << PB3;    //turn on  green led
        PORTB &= ~(1 << PB4); //turn off red LED
    } else if(temperature >= 90){    
        PORTB &= ~(1 << PB3); //turn off blue led
        PORTB |= 1 << PB4;    //turn on  red LED
    }
}

void myPrintToLCD(char result[]){
        lcd_clrscr();
        lcd_gotoxy(0,0);
        lcd_puts(result);
        _delay_ms(1000);

}

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