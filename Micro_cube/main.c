// ############################################
// 
// 4x4x4 LED Cube project
// By Christian Moen 2008
// chr@syntaks.org
// Lisence: GPL
//
// ############################################

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>


// Define masks for the layer select.
#define LAYERS_R	0x00	// = 000000  All layers off
#define LAYER_PORT	PORTC

// NOT USED ??????????
#define LAYER1		0x20	// = 100000
#define LAYER2		0x10	// = 010000
#define LAYER3		0x8	// = 001000
#define LAYER4		0x4	// = 000100
#define LAYERS		0x3c	// = 111100  All layers on


// Define LED grid ports
// The upper 4 bits is one row, the lower 4 bits are one row.
#define GRID1		PORTD	// All bits used for columns
#define GRID2		PORTB	// Bits 0,1,2,3,4,5 used for columns, bits 6,7 used by xtal
#define GRID3		PORTC	// Bits 0,1 used for columns, bits 2,3,4,5 used for rows, bit 7 used by reset

void ioinit (void); // initiate IO on the AVR
void delay_ms (uint16_t x); // delay function used throughout the program
void custom_routine (int effect); // effect program launcher

// *** Cube buffer ***
// The 3D image displayed on the cube is buffered in a 2d array 'cube'.
// The 1st dimension in this array is the Z axis of the cube.
// The 2nd dimension of the array is the Y axis.
// Each byte is a stripe of leds running along the X axis at the given
// Z and Y coordinates.
// Only the 4 lower bits are used, since the cube is only 4x4x4.
// This buffer design was chosen to have code compatibility with a 8x8x8 cube.
// "volatile" makes the variables reachable from within the interrupt functions
volatile unsigned char cube[4][4];

// We sometimes want to draw into a temporary buffer so we can modify it
// before writing it to the cube buffer.
// e.g. invert, flip, reverse the cube..
volatile unsigned char tmpcube[4][4];

// What layer the interrupt routine is currently showing.
volatile unsigned char current_layer;
volatile unsigned char fixed_layer_order[] = {0, 1, 3, 2};	// Maps to 0, 1, 2, 3 

// Low level geometric functions
#include "draw.c"
#include "expand.c"

// Static animation data
#include "frames.c"

// Fancy animations to run on the cube
#include "effect.c"
#include "custom_effects.c"


int main (void)
{
	// Initiate IO ports and peripheral devices.
	ioinit();

	int i;

	// Set the layer to start drawing at
	current_layer = 0x00;
	
	// Enable interrupts to start drawing the cube buffer.
	// When interrupts are enabled, ISR(TIMER1_COMPA_vect)
	// will run on timed intervalls.
	sei();

	// Main program loop.
	while (1)
	{
		// Custom routine is 18 long
		for (i=0;i<19;i++)
		{
			custom_routine(i);
		}
		// Comment the loop above and uncomment the line below
		// if you want the effects in random order (produced some bugs.. )
		//launch_effect(rand()%13);
	}

}

// Custom Routine
void custom_routine (int effect)
{
		int speed = 2700;
	switch (effect)
	{
		// Time between effects

		
		// Lights all the layers one by one leaving only the top layer light
		case 0:
			// (speed, wait between fill and empty, layers to leave light)
			custom_loadbar(speed,(speed*3),3); // Leave the top layer light
			delay_ms(speed*2); // Hold top layer light for longer than normal
			break;
			
		// Rain voxels down and send out ripples on the bottom plane
		case 1:
			// (z, voxel count, speed, wait between raindrops,speed of ripples,invert ripples 1/0)
			custom_rain(3,24,speed,(speed*0.2),(speed*0.333),0); // Rain 24 drops down
			break;
			
		// Fireworks
		case 2:
			// (iterations, speed, wait between fireworks,speed for explosion)
			fireworks(20,(speed*0.6),(speed*0.5),500);
			delay_ms(speed);
			break;
			
		// Bounce a plane up and down 
		case 3:
			// (axis, direction, speed, iterations)
			custom_flyplane("z",0,speed,4); // Start at bottom
			delay_ms(speed);
			break;
			
		// Light all the layers one by one
		case 4:
			// (speed, wait between fill and empty, layers to leave light)
			custom_loadbar(speed,(speed*3),0); // Leave all layers light
			delay_ms(speed);
			break;
			
		// Unset all 64 voxels in a random order.
		case 5:
			// (speed, state)
			random_filler2((speed*0.2),0);
			delay_ms(speed);
			break;

		// Set x number of random voxels then unset them.
		// x increases from 1 to voxel count and back to 1.
		// Speed accelerates and then decelerates
		case 6:
			// (voxel count, speed, acceleration)
			custom_random_2(20, (speed*0.333), 5);
			delay_ms(speed);
			break;
			
		// A pixel bouncing around randomly
		case 7:
			// blink
			// Iterations, delay, 
			boingboing(150,(speed*0.5),0x03,0x01);
			delay_ms(speed);
			break;
		
		// Set all 64 voxels in a random order.
		case 8:
			// speed
			random_filler2((speed*0.2),1);
			delay_ms(speed);
			break;

		// Clear middle two Z layers randomly
		case 9:
			// (speed, state, z1, z2, z3, z4, total voxels to clear)
			random_layer_filler ((speed*0.333),0,1,2,-1,-1,32);
			delay_ms(speed*2);
			break;

		// Clear part of top and bottom two Z layers randomly
		case 10:
			// (speed, state, z1, z2, z3, z4, total voxels to clear)
			random_layer_filler ((speed*0.333),0,0,-1,-1,3,22);
			delay_ms(speed*2);
			break;


		// Send voxels randomly back and forth along the Z axis
		case 11:
			// (iterations, speed, pause)
			sendvoxels_rand_z(150,(speed*0.333),(speed*0.5));
			delay_ms(speed);
			break;

		// A snake randomly bounce around the cube.
		case 12:
			// snake
			boingboing(150,(speed*0.5),0x03,0x03);
			break;
		
		// A coordinate bounces randomly around the cube
		// For every position the status of that voxel is toggled.
		case 13:
			// toggle
			boingboing(150,(speed*0.5),0x03,0x02);
			break;
			
		// Spinning spiral
		case 14:
			// (direction, iterations, delay)
			effect_spiral(1,75,(speed*0.7));
			break;
			
		// Spinning plane
		case 15:
			// (direction, iterations, delay)
			effect_spinning_plane(1,50,(speed*0.7));
			delay_ms(speed);
			break;
			
		// Bounce a plane in all the directions.
		case 16:
			// (axis, direction, speed)
			flyplane("y",1,(speed*0.5));
			delay_ms(speed);
			flyplane("x",1,(speed*0.5));
			delay_ms(speed);
			flyplane("z",0,(speed*0.5));
			delay_ms(speed);
			flyplane("y",0,(speed*0.5));
			delay_ms(speed);
			flyplane("x",0,(speed*0.5));
			delay_ms(speed);
			flyplane("z",1,(speed*0.5));
			delay_ms(speed);
			break;
				
		// Light all the layers one by one
		case 17:
			// (speed, wait between fill and empty, layers to leave light)
			custom_loadbar(speed,(speed*2),0); // Leave all layers light
			delay_ms(speed);
			break;
			
		// Dissolve layers one by one from top leaving the bottom layer for the next loop 
		case 18:
			// (speed, state, z1, z2, z3, z4, total voxels to clear)
			random_layer_filler ((speed*0.333),0,3,-1,-1,-1,16);
			random_layer_filler ((speed*0.333),0,2,-1,-1,-1,16);
			random_layer_filler ((speed*0.333),0,1,-1,-1,-1,16);
			delay_ms(speed);
			break;
			
	}
	
}


// Cube buffer draw interrupt routine
ISR(TIMER1_COMPA_vect)
{
	// AND the reverse bitmask onto the layer port.
	// This disables all the layers. rendering all the leds off.
	// We don't want to see the cube updating.
	LAYER_PORT &= LAYERS_R;

	// Take the current 2D image at the current layer along the Z axis
	// and place it on the LED grid.
	
// ########## START re-mapping all bits to appropriate ports and bits ##########
	
	// 16bit temp to store entire 4 x 4 grid for resorting
	int temp = (0x000f & cube[current_layer][0]) 
		| (0x00f0 & (cube[current_layer][1] << 4)) 
		| (0x0f00 & (cube[current_layer][2] << 8))
		| (0xf000 & (cube[current_layer][3] << 12));
	
	// map of all bits #
		
	// = PD1, PD2, PD3, PB1
	// = PD0, PD4, PD5, PD6
	// = PC1, PB5, PD7, PB0
	// = PC0, PB4, PB3, PB2
	
	// order bits need to be mapped in
	
	// PORTD 4, 0, 1, 2, 5, 6, 7, 10 PORTB 11, 3, 15, 14, 13, 9, PORTC 12, 8
		
	// temp bit 4 > PD0	// temp bits 210 > PD3,2,1	// temp bits 765 > PD6,5,4	// temp bit 10 > PD7
	GRID1 = ((0x10 & temp) >> 4) | (0x0e & temp << 1) | ((0xe0 & temp) >> 1) | ((0x400 & temp) >> 3);
	
	// temp bit 11 > PB0	// temp bit 3 > PB1	// temp bit 15 > PB2	// temp bit 14 > PB3	// temp bit 13 > PB4	// temp bit 9 > PB5
	GRID2 = ((0x800 & temp) >> 11) | ((0x8 & temp) >> 2) | ((0x8000 & temp) >> 13) | ((0x4000 & temp) >> 11) | ((0x2000 & temp) >> 9) | ((0x200 & temp) >> 4);
	
	// temp bit 12 > PC0	// temp bit 8 > PC1
	GRID3 = ((0x1000 & temp) >> 12) | ((0x100 & temp) >> 7);
	
// ########## END re-map #########

	// Enable the apropriate layer
	LAYER_PORT |= (0x01 << (5 - fixed_layer_order[current_layer]));

	// The cube only has 4 layers (0,1,2,3)
	// If we are at layer 3 now, we want to go back to layer 0.
	if (current_layer++ == 3)
		current_layer = 0;
}

void ioinit (void)
{
	// ### Initiate I/O
	
	// Data Direction Registers
	// Set pins as outputs
	DDRB = 0x3f;	// Set all bits as outputs except the xtal pins
	DDRC = 0x3f;	// Set all bits as outputs except the reset pin
	DDRD = 0xff;
	
	// Set all ports OFF
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;

	// ### Initiate timers
	// Frame buffer interrupt
	
	TCCR1A = 0;// set entire TCCR1A register to 0
	TCCR1B = 0;// same for TCCR1B
	TCNT1 = 0;	// initial counter value = 0;
	
	// Every 1024th cpu cycle, a counter is incremented.
	// Every time that counter reaches 15, it is reset to 0,
	// and the interrupt routine is executed.
		
	// 16000000/1024/16 = 976.5625 times per second
	// There are 4 layers to update..
	// 16000000/1024/16/4 = 244.140625 FPS
	
	// == flicker free :)
	OCR1A = 15; 			// interrupt at counter = 15
	TCCR1B |= (0x01 << WGM12);	// Set Clear Timer on Compare match (CTC) mode
	TCCR1B |= (0x01 << CS10) | (0x01 << CS12);	// Set prescaler = 1024
	TIMSK1 |= (1 << OCIE1A);	// Enable CTC interrupt
	
	// Disable USART to allow use of PORTD bits 0 and 1 as digital I/O
	UCSR0B = (0<<RXEN0)|(0<<TXEN0);

}

// Delay function used in graphical effects.
void delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--){
    for ( y = 0 ; y < 90 ; y++){
      for ( z = 0 ; z < 6 ; z++){
        asm volatile ("nop");
      }
    }
  }
}
