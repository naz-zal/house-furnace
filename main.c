/*
 * ----------------------------------------------------------------------------
 * Author: Nazzal Naseer
 * ID: 1637253
 * Description: Firmware program that allows a user to set the room temperature by 
 *              configuring a thermostat.
 * 
 * Created: 26.07.21
 */

#include "defines.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>

#include <util/delay.h>

#include "lcd.h"

#define SLOPE 4.9                                                                       // Values obtained while calibrating the internal temperature
#define INTERCEPT -147.1

static void ioinit(void) {                                                              // Initializing the LCD and the input ports
    lcd_init();
    
    DDRC &= ~(1 << DDC5) | ~(1 << DDC4) | ~(1 << DDC3);
    PORTC |= (1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3);
}

int internal_temp_init() {                                                              // setting up the MCU to read internal temperature data
    ADCSRA |= (1 << ADEN);                                                              // and return the converted temperature
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << MUX3);

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    uint16_t RESULT = ADC;

    return RESULT;
}

int external_temp_init() {                                                              // setting up the MCU to read external temperature data
                                                                                        // with the help of the LM35 temperature sensor
    ADCSRA |= (1 << ADEN); // setting the ADC
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); // pre-scaler
    ADMUX |= (1 << REFS0); // mux[3:0] = 0000

    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    uint16_t RESULT = ADC;

    return RESULT;
}

int button_pressed() {                                                                  // checking what button's been pressed 
    if (!(PINC & (1 << PINC5))) {
        return 1;                                                                       // return value corresponds to the button
    } else if (!(PINC & (1 << PINC4))) {
        return 2;
    } else if (!(PINC & (1 << PINC3))) {
        return 3;
    }
    return 0;
}
FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

int main(void) {

    ioinit();
    stderr = &lcd_str;
    
    char* arrow = "-> ";                                                                                // string definition of the arrow
    int user_choice = 1;                                                                                // user choice corresponding to what sensor they want to use
    int desired_temp = 23;                                                                              // corresponds to the target temperature for the room
    int deadband = 1;                                                                                   
    char* furnace = "off";                                                                              // status of the furnace

    int EXTERNAL_RESULT = external_temp_init();                                                         // reading the room temperature
    float EXTERNAL_TEMP = (5 * 100 * EXTERNAL_RESULT) / 1024.0;

    if (EXTERNAL_TEMP > desired_temp + (deadband / 2)) {
        furnace = "off";
    } else if (EXTERNAL_TEMP < desired_temp + (deadband / 2)) {
        furnace = "on";
    }
    
    while (1) {

        int EXTERNAL_RESULT = external_temp_init();
        float EXTERNAL_TEMP = (5 * 100 * EXTERNAL_RESULT) / 1024.0;

        float CAL_INTERNAL_TEMP = (EXTERNAL_TEMP - (INTERCEPT)) / SLOPE;                              // calibrating the internal temperature
        
        int pressed = button_pressed();                                                               // mapping each button to a specific behavior
        
        if (pressed == 1) {
            desired_temp++; 
        }
        else if (pressed == 2) {
            desired_temp--; 
        }
        else if (pressed == 3) {
            if (user_choice == 1) {
                user_choice = 0; 
                arrow = "<- ";
            }
            else if (user_choice == 0) {
                user_choice = 1; 
                arrow = "-> ";
            }
        }

        if (user_choice == 1) {
            if (EXTERNAL_TEMP > desired_temp + (deadband / 2)) {
                furnace = "off";
            } else if (EXTERNAL_TEMP < desired_temp - (deadband / 2)) {
                furnace = "on";
            }
        }

        else if (user_choice == 0) {
            if (CAL_INTERNAL_TEMP > desired_temp + (deadband / 2)) {
                furnace = "off";
            } else if (CAL_INTERNAL_TEMP < desired_temp - (deadband / 2)) {
                furnace = "on";
            }
        }

        fprintf(stderr, "%3.1f\xDF", CAL_INTERNAL_TEMP);                                              // formatting the display on the LCD
        fprintf(stderr, "C ");
        fprintf(stderr, "%s", arrow);
        fprintf(stderr, "%3.1f\xDF", EXTERNAL_TEMP);
        fprintf(stderr, "C\x1b\xc0");
        
        fprintf(stderr, "     %d\xDF", desired_temp);
        fprintf(stderr, "C   ");
        
        fprintf(stderr, "%s \x1b\x80", furnace);

        _delay_ms(200);


    }

    return 0;
}
