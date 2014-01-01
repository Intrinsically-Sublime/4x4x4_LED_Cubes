// ############################################
//
// Custom effects by Sublime 2013
// Lisence: GPL
//
// ############################################

#include "custom_effects.h"

const unsigned char spinning_line[6][2] PROGMEM =
{
	{ 0x84, 0x21 },
	{ 0x0c, 0x30 },
	{ 0x03, 0xc0 },
	{ 0x12, 0x48 },
	{ 0x22, 0x44 },
	{ 0x44, 0x22 },
};

// Light all leds layer by layer,
// then unset layer by layer

void loadbar (int delay, int wait, unsigned char leave)
{
	fill(0x00);
	
	int z,y;
	
	for (z=0;z<4;z++)
	{
		for (y=0;y<CUBE_SIZE;y++)
			cube[z][y] = 0xff;
			
		delay_ms(delay);
	}
	
	delay_ms(wait);
	
	for (z=0;z<leave;z++)
	{
		for (y=0;y<CUBE_SIZE;y++)
			cube[z][y] = 0x00;
			
		delay_ms(delay);
	}
}

// Set or clear up to one layer in a random order.
void random_layer_filler (int delay, unsigned char set_state, int z, int voxels)
{
	int x,y;
	int loop = voxels;
	unsigned char state = 0x00;
	unsigned char new_state = 0x01;
	
	if (set_state == 0) 
	{
		state = 0x01;
		new_state = 0x00;
	}
	
	while (loop)
	{
		x = rand()%CUBE_SIZE;
		y = rand()%CUBE_SIZE;

		if (getvoxel(x,y,z) == state)
		{
			altervoxel(x,y,z,new_state);
			delay_ms(delay);
			loop--;
		}
	}
}

// Set or clear all voxels in a random order.
void effect_random_filler (int delay, unsigned char state)
{
	int loop = CUBE_BYTES;
	
	while (loop)
	{
		for (int z=CUBE_MAX;z>-1;z--)
		random_layer_filler(delay,state,z,1);
		loop--;	
	}
}

// Rain voxels from the top of the cube to the bottom and produce ripples
// Sends in random order.
void custom_rain (int drops, int speed)
{
	unsigned char loop = drops;
	unsigned char x, y;
		
	int last_x = 0;
	int last_y = 0;
	
	while(loop)
	{
		x = rand()%(CUBE_SIZE);
		y = rand()%(CUBE_SIZE);
		// Prevent the raindrop from being in the same place twice
		if (last_x != x || last_y != y)
		{
			sendvoxel_z(x,y,CUBE_MAX,speed);
			ripples(x,y,0,speed*0.5);
			last_x = x;
			last_y = y;
			loop--;
		}
	}
}

// Send a firework flying to the top of the cube from the bottom and explode in 3d
void fireworks (int fireballs, int speed, unsigned char mode)
{
	unsigned char loop = fireballs;
	unsigned char x, y;
		
	int last_x = 0;
	int last_y = 0;
	
	while(loop)
	{
		x = rand()%(CUBE_SIZE);
		y = rand()%(CUBE_SIZE);
		// Prevent the firework from being in the same place twice
		if (last_x != x || last_y != y)
		{
			sendvoxel_z(x,y,0,speed);
			explode(x,y,CUBE_MAX,speed,mode);
			last_x = x;
			last_y = y;
			loop--;
		}
		delay_ms(speed);
	}
}

// Shows an animation of a spinning or spiralling plane or square
void effect_spinning_plane(unsigned char direction, int iterations, int delay, unsigned char mode)
{
	int spiral = 0;
	
	for (int i=0;i<iterations*6;i++)
	{
		// Loop cube levels.
		for (int z=0;z<CUBE_SIZE;z++)
		{
			if (mode == 2 || mode == 3) spiral = z;
			 
			cube[z][0] = (pgm_read_byte(&spinning_line[(i+spiral)%6][0]) >> 4);
			cube[z][1] = pgm_read_byte(&spinning_line[(i+spiral)%6][0]);
			cube[z][2] = (pgm_read_byte(&spinning_line[(i+spiral)%6][1]) >> 4);
			if ((z == 1 || z == 2) && (mode == 1 || mode == 2)) {
				cube[z][1] &= 0x09;
				cube[z][2] &= 0x09;
			}
			cube[z][3] = pgm_read_byte(&spinning_line[(i+spiral)%6][1]);
		}
		delay_ms(delay);
	}
}
