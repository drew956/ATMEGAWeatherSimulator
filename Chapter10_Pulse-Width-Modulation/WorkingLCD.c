#include <stdlib.h>
#include <avr/io.h>                  
#include <util/delay.h>  
#include "USART.h"            
#include "lcd.h"


/*
    Working example of LCD Screen
*/

int main(void) {
    initUSART();
    printString("About to init LCD\n");
    lcd_init(LCD_DISP_ON);   
    printString("LCD Initted\n");
    lcd_clrscr();
    printString("LCD screen cleared");
    lcd_puts("Test 1\n");
    printString("Test1 cleared");
    lcd_puts("Line 2");
    printString("Line 2 cleared");
    
    while (1) {
        lcd_clrscr();
        lcd_gotoxy(0,0);
        lcd_puts("Line 2");
        _delay_ms(1000);
        printString("Line 2 line 2 line 2 woo hoo!\n");
    }                                                  /* End event loop */
    return 0;                            /* This line is never reached */
}

