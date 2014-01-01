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
	
	// Indicate that the device has just booted.
	bootmsg();
	
	// This variable specifies which layer is currently being drawn by the
	// cube interrupt routine. We assign a value to it to make sure it's not >CUBE_MAX.
	current_layer = 0x00;	

	// Enable interrupts
	// This starts the routine that draws the cube content
	sei();
	
	// Check button on boot
	unsigned char mode = check_button();
	
	while (mode == 0)
		// Show the effects in a predefined order
		for (int i=0; i<EFFECTS_TOTAL; i++)
		{
			launch_effect(i);
		}

	while (mode == 1)
		{
			//launch_effect(4);
			launch_effect(rand()%EFFECTS_TOTAL);
		}
}

ISR(TIMER2_COMP_vect)
{
	// AND the reverse bitmask onto the layer port.
	// This disables all the layers. rendering all the leds off.
	// We don't want to see the cube updating.
	LAYER_PORT &= LAYERS_R;

	// Take the current 2D image at the current layer along the Z axis
	// and place it on the LED grid.
	GRID1 = (0x0f & cube[current_layer][0]) | (0xf0 & (cube[current_layer][1] << 4));
	GRID2 = (0x0f & cube[current_layer][2]) | (0xf0 & (cube[current_layer][3] << 4));

	// Enable the appropriate layer
	LAYER_PORT |= (0x01 << (7 - current_layer));

	// The cube only has 4 layers (0,1,2,3)
	// If we are at layer 3 now, we want to go back to layer 0.
	if (current_layer++ == 3)
		current_layer = 0;
}


void ioinit (void)
{
	// ### Initiate I/O
	
	// Data Direction Registers
	// Bit set to 1 means it works as an output
	// Bit set to 0 means it is an input
	DDRA = 0xff;	// Inner cube byte
	DDRB = 0xf7;	// ISP and 0-1: led. 3: button
	DDRC = 0xff;	// Outer cube byte
	DDRD = 0xff;	// Layer select
	
	// Set all ports OFF, and enable pull up resistors where needed.
	PORTA = 0x00;
	PORTC = 0x00;
	PORTB = 0x08; // Enable pull up button.
	PORTD = 0x00;

	// ### Initiate timers and USART

	// Frame buffer interrupt
	TCNT2 = 0x00;	// initial counter value = 0;
	TIMSK |= (1 << OCIE2); // Enable CTC interrupt
	
	// Every 1024th cpu cycle, a counter is incremented.
	// Every time that counter reaches 15, it is reset to 0,
	// and the interrupt routine is executed.
	// 14745600/1024/15 = 960 times per second
	// There are 4 layers to update..
	// 14745600/1024/15/4 = 240 FPS
	
	// Changes for 8mhz internal clock (Sublime)
	
	// 8000000/1024/8 = 976.5625 times per second
	// There are 4 layers to update..
	// 8000000/1024/8/4 = 244.140625 FPS
	
	// == flicker free :)
	OCR2 = 8; 		// interrupt at counter = 8 (Sublime)	// interrupt at counter = 15
	TCCR2 = 0x05; 		// prescaler = 1024
	TCCR2 |= (1 << WGM01);	// Clear Timer on Compare Match (CTC) mode
}

// Blink the status LEDs a little to indicate that the device has just booted.
// This is usefull to see if an error is making the device reboot when not supposed to.
// And it looks cool.
void bootmsg (void)
{
	int i;
	LED_PORT |= LED_GREEN;
	for (i = 0; i < 2; i++)
	{
		// Blinky
		delay_ms(1000);
		LED_PORT &= ~LED_GREEN;
		LED_PORT |= LED_RED;
		// Blink
		delay_ms(1000);
		LED_PORT &= ~LED_RED;
		LED_PORT |= LED_GREEN;
	}
	delay_ms(1000);
	LED_PORT &= ~LED_GREEN;
}

// Checks the button and sets the random seed
unsigned char check_button (void)
{
	if (!(PINB & MAIN_BTN))
	{
	int i = 1;
		while (!(PINB & MAIN_BTN))
		{
			srand(i++);
		}
	return 1;
	}
	else return 0;
}
