#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "cube.h"

// Define masks for the layer select.
#define LAYERS_R	0x00	// = 000000  All layers off
#define LAYER_PORT	PORTC

// Define LED grid ports
#define GRID1		PORTD	// All bits used for columns
#define GRID2		PORTB	// Bits 0,1,2,3,4,5 used for columns, bits 6,7 used by xtal
#define GRID3		PORTC	// Bits 0,1 used for columns, bits 2,3,4,5 used for rows, bit 7 used by reset

void ioinit (void);

volatile unsigned char current_layer;
volatile unsigned char fixed_layer_order[] = {0, 1, 3, 2};	// Maps to 0, 1, 2, 3 
#endif

