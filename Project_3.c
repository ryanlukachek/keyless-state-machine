/*
 * File:   Project_3.c
 * Author: Ryan Lukachek
 * Class: EE 200
 * Created on November 15, 2020, 11:21 PM
 */

/* define FCY before including libpic30.h */
#define FCY 3685000UL

#include <p33EP64MC502.h>
#include <libpic30.h>
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include "EE200_LCD.h"// include header to access C functions

#pragma config ICS = PGD2 // Communicate on PGED2 (pin 14) and PGEC2 (pin 15)
#pragma config JTAGEN = OFF// Disable JTAG in order to use RB

/* declare functions */
void initialize_IO_ports (void);
void initialize_INT(void);
void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt (void);


typedef enum {
    S0, S1, S2, S3
} STATES;

/* define states outside of main so that it can be accessed by the ISR */
STATES state = S0; /* set initial state to S0 */

void initialize_IO_ports (void) {
    /* Configure pins for digital I/O */
    ANSELAbits.ANSA0 = 0; // use pin 2 for digital I/O
    ANSELAbits.ANSA1 = 0; // use pin 3 for digital I/O
    ANSELB = 0;// use peripheral pins associated with PORTB for digital I/O
    TRISB = 0xC0F0;// set RB8 to RB13 for output, the rest for input
    PORTB = 0;// set all LCD inputs low

    /* Set I/O pin direction */
    TRISAbits.TRISA0 = 1;  // configure RA0 (pin 2) for input
    TRISAbits.TRISA1 = 1;  // configure RA1 (pin 3) for input
    TRISAbits.TRISA2 = 1;  // configure RA2 (pin 9) for input
}

void initialize_INT (void){
    RPINR0bits.INT1R = 0x24;// assign INT1 to RP36 (pin 11)
    INTCON2bits.GIE = 1;// set global interrupt enable
    INTCON2bits.INT1EP = 1;// interrupt on negative edge
    IFS1bits.INT1IF = 0;// clear interrupt flag
    IEC1bits.INT1IE = 1;// enable interrupt                                                               
  }

void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt (void){
    char Line_1_char_Array[] = "STATE = S0";
    
    /*Set LEDs to S0*/
    LATBbits.LATB0 = 0;
    LATBbits.LATB1 = 0;
    LATBbits.LATB2 = 0;
    /*Display State on LCD*/
    Position_LCD_Cursor(0x00);
        Write_LCD_String(Line_1_char_Array);
    
    state = S0;// set state to S0
    IFS1bits.INT1IF = 0;// clear interrupt flag                     
  }  
  
int main(void) {
    int input, state_num;
    unsigned int counter = 0;
    unsigned int s, u, l;
    char Line_1_char_Array[16];
    char Line_2_char_Array[16];
    

    initialize_IO_ports ();
    initialize_INT ();
    Init_LCD_Module();

    while (1) {// infinite loop 

       s = PORTAbits.RA2 ^ 1;// read pin 9 input
       u = (PORTAbits.RA1 ^1) << 1;// read pin 3 input
       l = (PORTAbits.RA0 ^1) << 2;// read pin 2 input
       input = l | u | s;
       
        
       switch (state) {// determine next state
            case S0:
                switch (input) {
                    case 0: state = S0; break;
                    case 1: state = S0; break;
                    case 2: state = S1; break;
                    case 3: state = S0; break;
                    case 4: state = S0; break;
                    case 5: state = S0; break;
                    case 6: state = S0; break;
                    case 7: state = S0; break;
                }; break;
            case S1:
                switch (input) {
                    case 0: state = S1; break;
                    case 1: state = S1; break;
                    case 2: state = S2; break;
                    case 3: state = S1; break;
                    case 4: state = S0; break;
                    case 5: state = S0; break;
                    case 6: state = S0; break;
                    case 7: state = S0; break;
                };  break;
            case S2:
                switch (input) {
                    case 0: state = S2; break;
                    case 1: state = S3; break;
                    case 2: state = S2; break;
                    case 3: state = S2; break;
                    case 4: state = S0; break;
                    case 5: state = S0; break;
                    case 6: state = S0; break;
                    case 7: state = S0; break;
                }; break;
            case S3:
                switch (input) {
                    case 0: state = S3; break;
                    case 1: state = S2; break;
                    case 2: state = S3; break;
                    case 3: state = S3; break;
                    case 4: state = S0; break;
                    case 5: state = S0; break;
                    case 6: state = S0; break;
                    case 7: state = S0; break;
                };  break;
        }
        
        switch (state) {// set output
            
            case S0:// locked 
                LATBbits.LATB0 = 0;
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 0;
                state_num = 0;
                break;            
            case S1:// driver door unlocked 
                LATBbits.LATB0 = 1;
                LATBbits.LATB1 = 0;
                LATBbits.LATB2 = 0;
                state_num = 1;
                break;                
            case S2:// all doors unlocked 
                LATBbits.LATB0 = 1;
                LATBbits.LATB1 = 1;
                LATBbits.LATB2 = 0;
                state_num = 2;
                break;                
            case S3:// side door open 
                LATBbits.LATB0 = 1;
                LATBbits.LATB1 = 1;
                LATBbits.LATB2 = 1;
                state_num = 3;
                break;
        }
   
        sprintf(Line_1_char_Array, "STATE = S%d", state_num);// convert state value to string

        /*Write State Value to LCD*/
        Position_LCD_Cursor(0x00);
        Write_LCD_String(Line_1_char_Array);
        
        /*Clock*/
        LATBbits.LATB3 = 1;// clk high
        __delay_ms(1500); // delay 1.5 sec
        LATBbits.LATB3 = 0;// clk low
        __delay_ms(1500); // delay 1.5 sec         
        
        /*Counter*/
        counter += 1;
        sprintf(Line_2_char_Array, "Count = %d", counter);// convert counter value to string
        /*Write Counter Value to LCD*/
        Position_LCD_Cursor(0x40);
        Write_LCD_String(Line_2_char_Array);
           
        ClrWdt();// restart the watchdog timer
     
    }
    return 0;
}