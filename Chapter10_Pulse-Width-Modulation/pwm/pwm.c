#include <avr/io.h>                  
#include <util/delay.h>              
#include "pinDefines.h"
#include "USART.h"
#include "macros.h"

void pwmMotor(int partsOn);

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
int myGetNumber(void) {
  // Gets a numerical 0-255 from the serial port.
  // Converts from string to number.
  char hundreds = '0';
  char tens = '0';
  char ones = '0';
  char thisChar = '0';
  do {                                                   /* shift over */
    if(thisChar >= '0' && thisChar <= '9'){
        hundreds = tens;
        tens = ones;
        ones = thisChar;
        transmitByte(thisChar);                                    /* echo */
    }
    thisChar = receiveByte();                   /* get a new character */
  } while ( thisChar >= '0' && thisChar <= '9' );                     /* until type return */
  return (100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
}

int main(void) {
    initUSART();
    DDRB   |= 0b00000111; //set pins to output
    PORTB   = 0x00; //initialize (probably not necessary)

    //turn on the motor
    PORTB |= (1 << PB0); //digitalWrite (enableB, HIGH); //enable the motor    
    PORTB &= ~(1 << PB1); //        digitalWrite (MotorB1, LOW);
    PORTB |=  (1 << PB2); //        digitalWrite (MotorB2, HIGH);

    while (1) {
        /* 
            uint8_t receiveByte(void) {
              loop_until_bit_is_set(UCSR0A, RXC0);       
              return UDR0;                                
            }
        */
        
        /*printString("The current partsmax is: ");
        printByte(partsMax);
        printString("\n");
        
        if(bit_is_set(UCSR0A, RXC0)){
            printString("Bit is set\n");
            partsMax = myGetNumber();
            //transmitByte(partsMax);
            clear_bit(UCSR0A, RXC0);

        } */
        
        pwmMotor(50);
    /*
        PORTB |= (1 << PB0); //digitalWrite (enableB, HIGH); //enable the motor    
        _delay_ms(1000);

            printString("Motion Forward\n\n"); //        Serial.println ("Motion Forward");
            PORTB &= ~(1 << PB1); //        digitalWrite (MotorB1, LOW);
            PORTB |=  (1 << PB2); //        digitalWrite (MotorB2, HIGH);


            //3s forward
            _delay_ms(3000);

            printString("Motion Backwards\n\n"); //       Serial.println ("Motion Backwards");
            //reverse
            PORTB &=  (1 << PB1); //        digitalWrite (MotorB1,HIGH);
            PORTB |= ~(1 << PB2); //        digitalWrite (MotorB2,LOW);  

            //5s backwards
            _delay_ms(3000);


            printString("Stopping motors\n\n"); //        Serial.println ("Stoping motors");


        PORTB &= ~(1 << PB0); //digitalWrite (enableB, LOW); //enable the motor    
        _delay_ms(3000); 
    */
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