// ############################################
// 
// Derivative of 4x4x4 LED Cube project
// By Christian Moen 2008
// chr@syntaks.org
// Lisence: GPL
//
// Custom effects by Sublime 2013
//
// ############################################

void custom_loadbar (int delay, int wait, int leave);
void custom_random_2 (int voxels, int delay, int accel);
void random_layer_filler (int delay, int state, int z1, int z2, int z3, int z4, int loops);
void custom_rain (unsigned char z, int voxels, int delay, int wait, int ripple, int invert);
void send_rain_voxel_z (unsigned char x, unsigned char y, unsigned char z, int delay, int splash);
void fireworks (int iterations, int delay, int wait, int explode);
void custom_flyplane (unsigned char plane,int direction, int delay, int iterations);

// Light all leds layer by layer,
// then unset layer by layer
void custom_loadbar (int delay, int wait, int leave)
{
	fill(0x00);
	
	int z,y;
	
	for (z=0;z<4;z++)
	{
		for (y=0;y<4;y++)
			cube[z][y] = 0xff;
			
		delay_ms(delay);
	}
	
	delay_ms(wait);
	
	for (z=0;z<leave;z++)
	{
		for (y=0;y<4;y++)
			cube[z][y] = 0x00;
			
		delay_ms(delay);
	}
}


// blink 1 random voxel, blink 2 random voxels..... blink 20 random voxels
// and back to 1 again.
void custom_random_2 (int voxels, int delay, int accel)
{
	int i;
	
	for (i=1;i<voxels;i++)
	{
		random_1(5,i,(delay-(i*accel)));
	}
	
	for (i=voxels;i>=1;i--)
	{
		random_1(5,i,(delay+(i*accel)));
	}
	
}


// Set or clear one to three layers in a random order.
void random_layer_filler (int delay, int state, int z1, int z2, int z3, int z4, int loops)
{
	int x,y,z;
	int loop = 0;
	int z_array[] = {z1,z2,z3,z4};
	
	while (loop<loops)
	{
		x = rand()%4;
		y = rand()%4;
		z = z_array[rand()%4];

		if ((state == 0 && getvoxel(x,y,z) == 0x01) || (state == 1 && getvoxel(x,y,z) == 0x00))
		{
			altervoxel(x,y,z,state);
			delay_ms(delay);
			loop++;
		}	
	}
}


// Rain voxels from the top of the cube to the bottom and produce ripples
// Sends in random order.
void custom_rain (unsigned char z, int voxels, int delay, int wait, int ripple, int invert)
{
	unsigned char loop = voxels;
	unsigned char x, y;
	
	int i, ii;
		
	int last_x, last_y;
	
	int splash = 0;
	
	int state_A = 0;
	int state_B = 1;
	clrplane_z(0);

	fill(0x00);
	
	// Send random rain drops
	while(loop)
	{
		if (invert == 1)
		{
			state_A = 1;
			state_B = 0;
			setplane_z(0);
		}
		
		splash = (voxels - loop);
		
		x = rand()%3;
		y = rand()%3;
		// Prevent the raindrop from being in the same place twice
		if (last_x != x || last_y != y)
		{			
			// Send the voxel flying
			for (i=0; i<=z; i++)
			{
				// Set z height for new droplet
				ii = z-i;
				// Set new rain drop
				setvoxel(x,y,ii);
				// Clear voxel above current droplet
				clrvoxel(x,y,ii+1);
				// Delay
				delay_ms(delay);
				
				if (ii == 0)
				{
					// See included raindrop diagram pdf for colors
					
					// Droplet
					altervoxel(x,y,ii,state_A);
					// Purple
					expand_ring("purple",x,y,0,state_B);
					// Delay
					delay_ms(ripple);
					// Red
					expand_ring("red",x,y,0,state_B);
				}
				if (splash != 0)
				{
					if (ii == 3)
					{
						// Droplet
						altervoxel(x,y,ii,state_B);
						// Purple
						expand_ring("purple",last_x,last_y,0,state_A);
						// Green
						expand_ring("green",last_x,last_y,0,state_B);
						// Delay
						delay_ms(ripple);
						// Red
						expand_ring("red",last_x,last_y,0,state_A);
						// Yellow
						expand_ring("yellow",last_x,last_y,0,state_B);		
					}
					if (ii == 2)
					{
						// Green
						expand_ring("green",last_x,last_y,0,state_A);	
						// Grey
						expand_ring("grey",last_x,last_y,0,state_B);
						// Delay
						delay_ms(ripple);
						// Yellow
						expand_ring("yellow",last_x,last_y,0,state_A);
						// Black
						expand_ring("black",last_x,last_y,0,state_B);
					}
					if (ii == 1)
					{	
						// Grey
						expand_ring("grey",last_x,last_y,0,state_A);
						// Delay
						delay_ms(ripple);	
						// Black
						expand_ring("black",last_x,last_y,0,state_A);
					}
				}
			}
			delay_ms(wait);
			loop--; // one down, loop-- to go. when this hits 0, the loop exits.
			last_x = x;
			last_y = y;
		}
	}
}


// Send a firework flying to the top of the cube from the bottom and explode in 2d
void fireworks (int iterations, int delay, int wait, int explode)
{
	int i, ii;
	
	int x, y, last_x, last_y;
	
	int loop = iterations;
	
	fill(0x00);
	
	while(loop)
	{	
		x = rand()%4;
		y = rand()%4;
		
		// Prevent the firework from being in the same place twice
		if (last_x != x && last_y != y)
		for (i=0; i<4; i++)
		{
			// Set z height for new droplet
			ii = i;
			// Set new fireball
			setvoxel(x,y,ii);
			// 1/2 Delay to blur the motion
			delay_ms(delay/2);
			// Clear voxel below current fireball
			clrvoxel(x,y,ii-1);
			// Delay
			delay_ms(delay);
			
			if (ii == 3)
			{
				// See included raindrop diagram pdf for color definitions
				
				// Set rings
				expand_ring("purple",x,y,3,1);
				// Set center 1 layer down
				setvoxel(x,y,2);
				// Delay
				delay_ms(explode);
				// Set rings
				expand_ring("red",x,y,3,1);
				expand_ring("purple",x,y,2,1);
				setvoxel(x,y,1);
				// Delay
				delay_ms(explode);
				
				// Clear center
				clrvoxel(x,y,3);
				
				// Set rings
				expand_ring("green",x,y,3,1);
				expand_ring("red",x,y,2,1);
				expand_ring("purple",x,y,1,1);
				setvoxel(x,y,0);
				// Delay
				delay_ms(explode);
				
				// Clear rings
				expand_ring("purple",x,y,3,0);
				clrvoxel(x,y,2);
				
				// Set rings
				expand_ring("yellow",x,y,3,1);
				expand_ring("green",x,y,2,1);
				expand_ring("red",x,y,1,1);
				expand_ring("purple",x,y,0,1);
				// Delay
				delay_ms(explode);		
				
				// Clear rings
				expand_ring("red",x,y,3,0);
				expand_ring("purple",x,y,2,0);
				clrvoxel(x,y,1);
				
				// Set rings
				expand_ring("grey",x,y,3,1);
				expand_ring("yellow",x,y,2,1);
				expand_ring("green",x,y,1,1);
				expand_ring("red",x,y,0,1);
				// Delay
				delay_ms(explode);	
				
				// Clear rings
				expand_ring("green",x,y,3,0);
				expand_ring("red",x,y,2,0);
				expand_ring("purple",x,y,1,0);
				clrvoxel(x,y,0);
				
				// Set rings
				expand_ring("black",x,y,3,1);
				expand_ring("grey",x,y,2,1);
				expand_ring("yellow",x,y,1,1);
				expand_ring("green",x,y,0,1);
				// Delay
				delay_ms(explode);	
				
				// Clear rings
				expand_ring("yellow",x,y,3,0);
				expand_ring("green",x,y,2,0);
				expand_ring("red",x,y,1,0);
				expand_ring("purple",x,y,0,0);
				
				// Set rings
				expand_ring("black",x,y,2,1);
				expand_ring("grey",x,y,1,1);
				expand_ring("yellow",x,y,0,1);
				// Delay
				delay_ms(explode);
				
				// Clear rings
				expand_ring("yellow",x,y,2,0);
				expand_ring("green",x,y,1,0);
				expand_ring("red",x,y,0,0);
				// Delay
				delay_ms(explode);
				
				// Clear cube
				fill(0x00);				
				// Delay
				delay_ms(wait);
				loop--; // one down, loop-- to go. when this hits 0, the loop exits.
			}
		last_x = x;
		last_y = y;
		}
	}
}


void custom_flyplane (unsigned char plane, int direction, int delay, int iterations)
{
	int i,ii;
	
	int loop = iterations;
	
	fill(0x00);
	
	while(loop)
	{
		for (i=0;i<4;i++)
		{
			if (direction != 0)
			{
				ii = 3-i;
			} else
			{
				ii = i;
			}
			
			setplane(plane,ii,1);
			delay_ms(delay);
			setplane(plane,ii,0);
			}
		
		for (i=2;i>=0;i--)
		{
			if (direction != 0)
			{
				ii = 3-i;
			} else
			{
				ii = i;
			}
			
			setplane(plane,ii,1);
			delay_ms(delay);
			setplane(plane,ii,0);
		}
		loop--;
	}
}
