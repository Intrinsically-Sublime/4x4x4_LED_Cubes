// ############################################
//
// Custom effects by Sublime 2013
// Lisence: GPL
//
// ############################################

#include "custom_effects.h"

void ripples (int x, int y, int z, int speed)
{	
	for (int i=1;i<9;i++)
	{
		for (int ii=0;ii<4;ii++)
		{
			cube[z][ii] = (rings[i][3+y-ii] >> (3-x));
		}
		delay_ms(speed);
	}
	clrplane_z(z);
}

void explode (int x, int y, int z, int speed, unsigned char mode)
{
	int Yi = 0;
	int disk = 0;
	int steps = 13;
	int modes = 8;
	
	if (mode == 99) mode = rand()%modes;
	
	if (mode |= 0) steps = 8;
	
	for (int i=0;i<CUBE_SIZE*steps;i++)
	{
		cube[z][Yi] = (disks[disk][3+y-Yi] >> (3-x));
		Yi++;
		
		if (i%CUBE_SIZE == 0)
		{
			delay_ms(speed);
			disk++; 
			Yi = 0;
			shift(AXIS_Z,-1);
		}
	}
	
	// dissolve and rain down
	if (mode == 1)		dissolve(speed,0,0);
	// dissolve and rain down
	else if (mode == 2)	dissolve(speed,CUBE_SIZE,0);
	// dissolve upwards and rain down
	else if (mode == 3)	dissolve(speed,CUBE_SIZE,2);
	// dissolve upwards and rain down
	else if (mode == 4)	dissolve(speed,2,2);
	// dissolve back and forth and rain down
	else if (mode == 5)	dissolve(speed,CUBE_SIZE,3);
	// dissolve back and forth
	else if (mode == 6)	dissolve(speed,CUBE_SIZE,4);
	// random
	else if (mode == 7)	dissolve((rand()%speed*0.5)+(speed*0.5),rand()%5+1,rand()%5);
	// turn off all leds
	else	fill(0x00);
}

// (speed, loop modifier , direction 0-1-2-3-4)
void dissolve (int speed, unsigned char modifier, unsigned char direction)
{	
	for (int i=0;i<CUBE_SIZE;i++)
	{
		for (int ii=0;ii<CUBE_SIZE;ii++)
		{
			int loop = CUBE_SIZE+modifier;
	
			while (loop)
			{
				int x = rand()%CUBE_SIZE;
				int y = rand()%CUBE_SIZE;
				int flop = loop;
				if (direction == 1 || (flop%2 == 0 && (direction == 3 || direction == 4))) clrvoxel(x,y,0+ii+i);
				else clrvoxel(x,y,CUBE_MAX-ii-i);
				loop--;
				delay_ms(10);
			}	
		}
		delay_ms(speed*0.5);
		if (direction |= 4) shift(AXIS_Z,-1);
	}
}

unsigned char rings[9][7] = {

	// 0
	{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000000,
	0b00000000
	},
	
	// 1
	{
	0b00000000,
	0b00000000,
	0b00001000,
	0b00010100,
	0b00001000,
	0b00000000,
	0b00000000
	},
	
	// 2	
	{
	0b00000000,
	0b00000000,
	0b00011100,
	0b00010100,
	0b00011100,
	0b00000000,
	0b00000000
	},
	
	// 3	
	{
	0b00000000,
	0b00001000,
	0b00010100,
	0b00100010,
	0b00010100,
	0b00001000,
	0b00000000
	},
	
	// 4	
	{
	0b00000000,
	0b00011100,
	0b00100010,
	0b00100010,
	0b00100010,
	0b00011100,
	0b00000000
	},
	
	// 5		
	{
	0b00001000,
	0b00010100,
	0b00100010,
	0b01000001,
	0b00100010,
	0b00010100,
	0b00001000
	},
	
	// 6		
	{
	0b00011100,
	0b00100010,
	0b01000001,
	0b01000001,
	0b01000001,
	0b00100010,
	0b00011100
	},
		
	// 7	
	{
	0b00010100,
	0b00100010,
	0b01000001,
	0b00000000,
	0b01000001,
	0b00100010,
	0b00010100
	},
		
	// 8	
	{
	0b00100010,
	0b01000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b01000001,
	0b00100010
	}
};


unsigned char disks[13][7] = {

	// 0
	{
	0b00000000,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000000,
	0b00000000
	},
	
	// 1
	{
	0b00000000,
	0b00000000,
	0b00001000,
	0b00011100,
	0b00001000,
	0b00000000,
	0b00000000
	},
	
	// 2	
	{
	0b00000000,
	0b00000000,
	0b00011100,
	0b00011100,
	0b00011100,
	0b00000000,
	0b00000000
	},
	
	// 3	
	{
	0b00000000,
	0b00001000,
	0b00011100,
	0b00111110,
	0b00011100,
	0b00001000,
	0b00000000
	},
	
	// 4	
	{
	0b00000000,
	0b00011100,
	0b00111110,
	0b00111110,
	0b00111110,
	0b00011100,
	0b00000000
	},
	
	// 5		
	{
	0b00001000,
	0b00011100,
	0b00111110,
	0b01111111,
	0b00111110,
	0b00011100,
	0b00001000
	},
	
	// 6		
	{
	0b00011100,
	0b00111110,
	0b01111111,
	0b01111111,
	0b01111111,
	0b00111110,
	0b00011100
	},
		
	// 7	
	{
	0b00111110,
	0b01111111,
	0b01111111,
	0b01110111,
	0b01111111,
	0b01111111,
	0b00111110
	},
		
	// 8	
	{
	0b01111111,
	0b01111111,
	0b01110111,
	0b01100011,
	0b01110111,
	0b01111111,
	0b01111111
	},
		
	// 9	
	{
	0b01111111,
	0b01110111,
	0b01100011,
	0b01000001,
	0b01100011,
	0b01110111,
	0b01111111,
	},
		
	// 10	
	{
	0b01110111,
	0b01100011,
	0b01000001,
	0b00000000,
	0b01000001,
	0b01100011,
	0b01110111,
	},
		
	// 11	
	{
	0b01100011,
	0b01000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b01000001,
	0b01100011,
	},
		
	// 12	
	{
	0b01000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b01000001,
	}
};
