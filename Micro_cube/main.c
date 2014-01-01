/*
 * Code to controll an 4x4x4 ledcube using avr
 * See license.txt for license.
 */
#include "main.h"
#include "effect.h"
#include "launch_effect.h"
#include "draw.h"
#include "cube.h"

// Main loop
// the AVR enters this function at boot time
int main (void)
{

	// This function initiates IO ports, timers, interrupts and
	// serial communications
	ioinit();
	
	// This variable specifies which layer is currently being drawn by the
	// cube interrupt routine. We assign a value to it to make sure it's not >CUBE_MAX.
	current_layer = 0x00;	

	// Enable interrupts
	// This starts the routine that draws the cube content
	sei();
	
	while (1)
		// Show the effects in a predefined order
		for (int i=0; i<EFFECTS_TOTAL; i++)
		{
			launch_effect(i);
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
	
// ########## START re-mapping all column bits to appropriate ports and bits ##########
	
	// 16bit temp to store entire 4 x 4 grid for resorting
	int temp = (0x000f & cube[current_layer][0]) 
		| (0x00f0 & (cube[current_layer][1] << 4)) 
		| (0x0f00 & (cube[current_layer][2] << 8))
		| (0xf000 & (cube[current_layer][3] << 12));
	
	// map of all columns and corresponding port bits #
		
	// = PD1, PD2, PD3, PB1
	// = PD0, PD4, PD5, PD6
	// = PC1, PB5, PD7, PB0
	// = PC0, PB4, PB3, PB2
	
	// order temp bits need to be mapped to the ports
	
	// PORTD 4, 0, 1, 2, 5, 6, 7, 10 PORTB 11, 3, 15, 14, 13, 9, PORTC 12, 8
		
	// temp bit 4 > PD0	// temp bits 210 > PD3,2,1	// temp bits 765 > PD6,5,4	// temp bit 10 > PD7
	GRID1 = ((0x10 & temp) >> 4) | (0x0e & temp << 1) | ((0xe0 & temp) >> 1) | ((0x400 & temp) >> 3);
	
	// temp bit 11 > PB0	// temp bit 3 > PB1	// temp bit 15 > PB2	// temp bit 14 > PB3	// temp bit 13 > PB4	// temp bit 9 > PB5
	GRID2 = ((0x800 & temp) >> 11) | ((0x8 & temp) >> 2) | ((0x8000 & temp) >> 13) | ((0x4000 & temp) >> 11) | ((0x2000 & temp) >> 9) | ((0x200 & temp) >> 4);
	
	// temp bit 12 > PC0	// temp bit 8 > PC1
	GRID3 = ((0x1000 & temp) >> 12) | ((0x100 & temp) >> 7);
	
// ########## END re-map column bits #########

	// Enable the appropriate layer
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
