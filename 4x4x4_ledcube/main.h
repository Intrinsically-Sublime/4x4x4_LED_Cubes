#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "cube.h"

// Define masks used for status LEDs and input buttons.
#define LED_GREEN 0x01
#define LED_RED 0x02
#define MAIN_BTN 0x08
// Define port used for status and input.
#define LED_PORT PORTB
#define BUTTON_PORT PORTB


// Define LED grid ports
// Each of the grid ports are connected to two rows of leds.
// The upper 4 bits is one row, the lower 4 bits are one row.
#define GRID1 PORTC
#define GRID2 PORTA
#define LAYERS_R 0x0f
#define LAYER_PORT PORTD

void ioinit (void);
void bootmsg (void);
unsigned char check_button (void);

volatile unsigned char current_layer;
#endif

