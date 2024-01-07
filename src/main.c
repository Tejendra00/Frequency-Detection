/* Name: 2.4.2.c
 * Author: Tejendra Patel
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include "m_usb.h"
#define Timer_Freq 15625UL      //16MHz/1024          

int a;       //firstrising_edge
int b;       //firstfalling_edge
int c;       //secondfalling_edge
int freq;

int readICR3() {
    while (!bit_is_set(TIFR3, ICF3));
    return ICR3;
}

void clearICF3Flag() {
    set(TIFR3, ICF3);
}

void updateFrequency(int a, int b, int c) {
    if (a < b && b < c) {               // Check for valid condition
        int period = c - a;            // Time between two rising edge
        freq = Timer_Freq / period;   

        if (freq > 600 && freq < 800) {
            set(PORTB, 6);                // Green Led On at 662 hz
            clear(PORTB, 5);
            clear(PORTB,2);               
        } 
        else if (freq > 20 && freq < 30) {
            clear(PORTB, 6);                
            set(PORTB, 5);
            clear(PORTB,2);              // Red Led On at 25 Hz
        } 
        else if(freq>150 && freq<300){     // Blue Led On at 200Hz
            clear(PORTB,6);
            clear(PORTB,5);
            set(PORTB,2);
            }
        else {
            clear(PORTB, 6);
            clear(PORTB, 5);
            clear(PORTB,2);
        }
    }
}


int main(void)
{
    _clockdivide(0); // Set the clock speed to 16MHz
    clear(DDRC, 7);    // Set pin 7 of port B as an output
    set(DDRB, 6); 
    set(DDRB, 5); 
    set(DDRB, 2); 
    m_usb_init();  


    // 1024 Prescaler
    set(TCCR3B,CS32);
    clear(TCCR3B,CS31);
    set(TCCR3B,CS30); 


    for (;;)
    { 
        set(TCCR3B,ICES3);          //store timer value on rising edge
        a=readICR3();               
        clearICF3Flag();

        clear(TCCR3B,ICES3);        //store timer value on falling edge 
        b=readICR3();   
        clearICF3Flag(); 

        set(TCCR3B,ICES3);          //store timer value on rising edge
        c=readICR3();
        clearICF3Flag(); 
        
        updateFrequency(a,b,c);  

        m_usb_tx_uint(freq);
        m_usb_tx_string("\n");
        clear(PORTB,6);
        clear(PORTB,5);
        clear(PORTB,2);
       
    }

    return 0;   /* Never reached */
}
