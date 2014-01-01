#include "effect.h"
#include "draw.h"
#include <math.h>
#include <avr/interrupt.h>

// Draw a plane on one axis and send it back and forth once.
void effect_planboing (int plane, int speed)
{
	int i;
	for (i=0;i<CUBE_SIZE;i++)
	{
		fill(0x00);
        setplane(plane, i);
		delay_ms(speed);
	}
	
	for (i=CUBE_MAX;i>=0;i--)
	{
		fill(0x00);
        setplane(plane,i);
		delay_ms(speed);
	}
}

void effect_blinky2()
{
	int i,r;
	fill(0x00);
	
	for (r=0;r<2;r++)
	{
		i = 750;
		while (i>0)
		{
			fill(0x00);
			delay_ms(i);
			
			fill(0xff);
			delay_ms(100);
			
			i = i - (15+(1000/(i/10)));
		}
		
		delay_ms(1000);
		
		i = 750;
		while (i>0)
		{
			fill(0x00);
			delay_ms(751-i);
			
			fill(0xff);
			delay_ms(100);
			
			i = i - (15+(1000/(i/10)));
		}
	}

}

void effect_box_shrink_grow (int iterations, int rot, int flip, uint16_t delay)
{
	int x, i, xyz;
	for (x=0;x<iterations;x++)
	{
		for (i=0;i<8;i++)
		{
            xyz = CUBE_MAX-i; // This reverses counter i between 0 and 3.
            if (i > CUBE_MAX) 
                xyz = i-CUBE_SIZE; // at i > 4, i 5-7 becomes xyz 0-3.
            
            fill(0x00); delay_ms(1);
            cli(); // disable interrupts while the cube is being rotated
			box_wireframe(0,0,0,xyz,xyz,xyz);

            if (flip > 0) // upside-down
                mirror_z();

            if (rot == 1 || rot == 3)
                mirror_y();

            if (rot == 2 || rot == 3)
                mirror_x();
            
            sei(); // enable interrupts
			delay_ms(delay);
	        fill(0x00);
        }
    }
}

// Creates a wireframe box that shrinks or grows out from the center of the cube.
void effect_box_woopwoop (int delay, unsigned char grow)
{
	int i,ii;
	
	int hc = (CUBE_SIZE*0.5);
	
	fill(0x00);
	for (i=0;i<hc;i++)
	{
        ii = i;
        if (grow > 0)
            ii = 1-i;

		box_wireframe(hc+ii,hc+ii,hc+ii,(hc-1)-ii,(hc-1)-ii,(hc-1)-ii);
		delay_ms(delay);
		fill(0x00);
	}
}


// Send a voxel flying from one side of the cube to the other
// If its at the bottom, send it to the top..
void sendvoxel_z (unsigned char x, unsigned char y, unsigned char z, int delay)
{
	int i, ii;
	for (i=0; i<CUBE_SIZE; i++)
	{
		if (z == CUBE_MAX)
		{
			ii = CUBE_MAX-i;
			clrvoxel(x,y,ii+1);
		} else
		{
			ii = i;
			clrvoxel(x,y,ii-1);
		}
		setvoxel(x,y,ii);
		delay_ms(delay);
	}
}

// Send all the voxels from one side of the cube to the other
// Start at z and send to the opposite side.
// Sends in random order.
void sendplane_rand_z (unsigned char z, int delay, int wait)
{
	unsigned char loop = 16;
	unsigned char x, y;

	fill(0x00);

	setplane_z(z);
	
	// Send voxels at random untill all have crossed the cube.
	while(loop)
	{
		x = rand()%CUBE_SIZE;
		y = rand()%CUBE_SIZE;
		if (getvoxel(x,y,z))
		{
			// Send the voxel flying
			sendvoxel_z(x,y,z,delay);
			delay_ms(wait);
			loop--; // one down, loop-- to go. when this hits 0, the loop exits. 
		}
	}
}

// For each coordinate along X and Y, a voxel is set either at level 0 or at level 7
// for n iterations, a random voxel is sent to the opposite side of where it was.
void sendvoxels_rand_z (int iterations, int delay, int wait)
{
	unsigned char x, y, last_x = 0, last_y = 0, i;

	fill(0x00);

	// Loop through all the X and Y coordinates
	for (x=0;x<CUBE_SIZE;x++)
	{
		for (y=0;y<CUBE_SIZE;y++)
		{
			// Then set a voxel either at the top or at the bottom
			if (rand()%2 == 0)
			{
				setvoxel(x,y,0);
			} else
			{
				setvoxel(x,y,CUBE_MAX);
			}
		}
	}

	for (i=0;i<iterations;i++)
	{
		// Pick a random x,y position
		x = rand()%CUBE_SIZE;
		y = rand()%CUBE_SIZE;
		// but not the sameone twice in a row
		if (y != last_y && x != last_x)
		{
			// If the voxel at this x,y is at the bottom
			if (getvoxel(x,y,0))
			{
				// send it to the top
				sendvoxel_z(x,y,0,delay);
			} else
			{
				// if its at the top, send it to the bottom
				sendvoxel_z(x,y,CUBE_MAX,delay);
			}
			delay_ms(wait);
			
			// Remember the last move
			last_y = y;
			last_x = x;
		}
	}

}


// Big ugly function :p but it looks pretty
void boingboing(uint16_t iterations, int delay, unsigned char mode, unsigned char drawmode)
{
	fill(0x00);		// Blank the cube

	int x, y, z;		// Current coordinates for the point
	int dx, dy, dz;	// Direction of movement
	int lol, i;		// lol?
	unsigned char crash_x, crash_y, crash_z;

	y = rand()%CUBE_SIZE;
	x = rand()%CUBE_SIZE;
	z = rand()%CUBE_SIZE;

	// Coordinate array for the snake.
	int snake[CUBE_SIZE][3];
	for (i=0;i<CUBE_SIZE;i++)
	{
		snake[i][0] = x;
		snake[i][1] = y;
		snake[i][2] = z;
	}
	
	
	dx = 1;
	dy = 1;
	dz = 1;
	
	while(iterations)
	{
		crash_x = 0;
		crash_y = 0;
		crash_z = 0;
	

		// Let's mix things up a little:
		if (rand()%3 == 0)
		{
			// Pick a random axis, and set the speed to a random number.
			lol = rand()%3;
			if (lol == 0)
				dx = rand()%3 - 1;
			
			if (lol == 1)
				dy = rand()%3 - 1;
				
			if (lol == 2)
				dz = rand()%3 - 1;
		}

	    // The point has reached 0 on the x-axis and is trying to go to -1
        // aka a crash
		if (dx == -1 && x == 0)
		{
			crash_x = 0x01;
			if (rand()%3 == 1)
			{
				dx = 1;
			} else
			{
				dx = 0;
			}
		}
		
        // y axis 0 crash
		if (dy == -1 && y == 0)
		{
			crash_y = 0x01;
			if (rand()%3 == 1)
			{
				dy = 1;
			} else
			{
				dy = 0;
			}
		}
		
        // z axis 0 crash
		if (dz == -1 && z == 0)
		{
			crash_z = 0x01;
			if (rand()%3 == 1)
			{
				dz = 1;
			} else
			{
				dz = 0;
			}
		}
	    
        // x axis high crash
		if (dx == 1 && x == CUBE_MAX)
		{
			crash_x = 0x01;
			if (rand()%3 == 1)
			{
				dx = -1;
			} else
			{
				dx = 0;
			}
		}
		
        // y axis high crash
		if (dy == 1 && y == CUBE_MAX)
		{
			crash_y = 0x01;
			if (rand()%3 == 1)
			{
				dy = -1;
			} else
			{
				dy = 0;
			}
		}
		
        // z azis high crash
		if (dz == 1 && z == CUBE_MAX)
		{
			crash_z = 0x01;
			if (rand()%3 == 1)
			{
				dz = -1;
			} else
			{
				dz = 0;
			}
		}
		
		// mode bit 0 sets crash action enable
		if (mode | 0x01)
		{
			if (crash_x)
			{
				if (dy == 0)
				{
					if (y == CUBE_MAX)
					{
						dy = -1;
					} else if (y == 0)
					{
						dy = +1;
					} else
					{
						if (rand()%2 == 0)
						{
							dy = -1;
						} else
						{
							dy = 1;
						}
					}
				}
				if (dz == 0)
				{
					if (z == CUBE_MAX)
					{
						dz = -1;
					} else if (z == 0)
					{
						dz = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dz = -1;
						} else
						{
							dz = 1;
						}
					}	
				}
			}
			
			if (crash_y)
			{
				if (dx == 0)
				{
					if (x == CUBE_MAX)
					{
						dx = -1;
					} else if (x == 0)
					{
						dx = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dx = -1;
						} else
						{
							dx = 1;
						}
					}
				}
				if (dz == 0)
				{
					if (z == 3)
					{
						dz = -1;
					} else if (z == 0)
					{
						dz = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dz = -1;
						} else
						{
							dz = 1;
						}
					}	
				}
			}
			
			if (crash_z)
			{
				if (dy == 0)
				{
					if (y == CUBE_MAX)
					{
						dy = -1;
					} else if (y == 0)
					{
						dy = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dy = -1;
						} else
						{
							dy = 1;
						}
					}	
				}
				if (dx == 0)
				{
					if (x == CUBE_MAX)
					{
						dx = -1;
					} else if (x == 0)
					{
						dx = 1;
					} else
					{
						if (rand()%2 == 0)
						{
							dx = -1;
						} else
						{
							dx = 1;
						}
					}	
				}
			}
		}
		
		// mode bit 1 sets corner avoid enable
		if (mode | 0x02)
		{
			if (	// We are in one of 8 corner positions
				(x == 0 && y == 0 && z == 0) ||
				(x == 0 && y == 0 && z == CUBE_MAX) ||
				(x == 0 && y == CUBE_MAX && z == 0) ||
				(x == 0 && y == CUBE_MAX && z == CUBE_MAX) ||
				(x == CUBE_MAX && y == 0 && z == 0) ||
				(x == CUBE_MAX && y == 0 && z == CUBE_MAX) ||
				(x == CUBE_MAX && y == CUBE_MAX && z == 0) ||
				(x == CUBE_MAX && y == CUBE_MAX && z == CUBE_MAX)
			)
			{
				// At this point, the voxel would bounce
				// back and forth between this corner,
				// and the exact opposite corner
				// We don't want that!
			
				// So we alter the trajectory a bit,
				// to avoid corner stickyness
				lol = rand()%3;
				if (lol == 0)
					dx = 0;
				
				if (lol == 1)
					dy = 0;
					
				if (lol == 2)
					dz = 0;
			}
		}

        // one last sanity check
        if (x == 0 && dx == -1)
            dx = 1;
	
        if (y == 0 && dy == -1)
            dy = 1;
	
        if (z == 0 && dz == -1)
            dz = 1;
	
        if (x == CUBE_MAX && dx == 1)
            dx = -1;
	
        if (y == CUBE_MAX && dy == 1)
            dy = -1;
	
        if (z == CUBE_MAX && dz == 1)
            dz = -1;
	
	
		// Finally, move the voxel.
		x = x + dx;
		y = y + dy;
		z = z + dz;
		
		if (drawmode == 0x01) // show one voxel at time
		{
			setvoxel(x,y,z);
			delay_ms(delay);
			clrvoxel(x,y,z);	
		} else if (drawmode == 0x02) // flip the voxel in question
		{
			flpvoxel(x,y,z);
			delay_ms(delay);
		} if (drawmode == 0x03) // draw a snake
		{
			for (i=CUBE_MAX;i>=0;i--)
			{
				snake[i][0] = snake[i-1][0];
				snake[i][1] = snake[i-1][1];
				snake[i][2] = snake[i-1][2];
			}
			snake[0][0] = x;
			snake[0][1] = y;
			snake[0][2] = z;
				
			for (i=0;i<CUBE_SIZE;i++)
			{
				setvoxel(snake[i][0],snake[i][1],snake[i][2]);
			}
			delay_ms(delay);
			for (i=0;i<CUBE_SIZE;i++)
			{
				clrvoxel(snake[i][0],snake[i][1],snake[i][2]);
			}
		}
		
	
		iterations--;
	}
}

void effect_rain (int iterations, int speed)
{
	int i, ii;
	int rnd_x;
	int rnd_y;
	int rnd_num;
	
	for (ii=0;ii<iterations;ii++)
	{
		rnd_num = rand()%CUBE_MAX+1;
		
		for (i=0; i < rnd_num;i++)
		{
			rnd_x = rand()%CUBE_SIZE;
			rnd_y = rand()%CUBE_SIZE;
			setvoxel(rnd_x,rnd_y,CUBE_MAX);
		}
		
		delay_ms(speed);
		shift(AXIS_Z,-1);
	}
}

void effect_z_updown (int iterations, int delay)
{
	unsigned char positions[CUBE_BYTES];
	unsigned char destinations[CUBE_BYTES];

	int i,y,move;
	
	for (i=0; i<CUBE_BYTES; i++)
	{
		positions[i] = 4;
		destinations[i] = rand()%CUBE_SIZE;
	}

	for (i=0; i<CUBE_SIZE; i++)
	{
		effect_z_updown_move(positions, destinations, AXIS_Z);
		delay_ms(delay);
	}
	
	for (i=0;i<iterations;i++)
	{
		for (move=0;move<CUBE_SIZE;move++)
		{
			effect_z_updown_move(positions, destinations, AXIS_Z);
			delay_ms(delay);
		}

		delay_ms(delay*4);


		for (y=0;y<(CUBE_BYTES*0.5);y++)
		{
				destinations[rand()%CUBE_BYTES] = rand()%CUBE_SIZE;
		}
		
	}

}

void effect_z_updown_move (unsigned char positions[CUBE_BYTES], unsigned char destinations[CUBE_BYTES], char axis)
{
	int px;
	for (px=0; px<CUBE_BYTES; px++)
	{
		if (positions[px]<destinations[px])
		{
			positions[px]++;
		}
		if (positions[px]>destinations[px])
		{
			positions[px]--;
		}
	}
		
	draw_positions_axis (AXIS_Z, positions,0);
}

void effect_axis_updown_randsuspend (char axis, int delay, int sleep, unsigned char invert)
{
	unsigned char positions[CUBE_BYTES];
	unsigned char destinations[CUBE_BYTES];

	int i,px;
	
    // Set random positions
	for (i=0; i<CUBE_BYTES; i++)
	{
		positions[i] = 0; // Set all starting positions to 0
		destinations[i] = rand()%CUBE_SIZE;
	}

    // Loop to allow destination reach all the way
	for (i=0; i<CUBE_SIZE; i++)
	{
        // For every iteration, move all position one step closer to their destination
		for (px=0; px<CUBE_BYTES; px++)
		{
			if (positions[px]<destinations[px])
			{
				positions[px]++;
			}
		}
        // Draw the positions and take a nap
		draw_positions_axis (axis, positions,invert);
		delay_ms(delay);
	}
	
    // Set all destinations (opposite from the side they started out)
	for (i=0; i<CUBE_BYTES; i++)
	{
		destinations[i] = CUBE_SIZE;
	}
	
    // Suspend the positions in mid-air for a while
	delay_ms(sleep);
	
    // Then do the same thing one more time
	for (i=0; i<CUBE_SIZE; i++)
	{
		for (px=0; px<CUBE_BYTES; px++)
		{
			if (positions[px]<destinations[px])
			{
				positions[px]++;
			}
			if (positions[px]>destinations[px])
			{
				positions[px]--;
			}
		}
		draw_positions_axis (axis, positions,invert);
		delay_ms(delay);
	}
}

void draw_positions_axis (char axis, unsigned char positions[CUBE_BYTES], unsigned char invert)
{
	int x, y, p;
	
	fill(0x00);
	
	for (x=0; x<CUBE_SIZE; x++)
	{
		for (y=0; y<CUBE_SIZE; y++)
		{
			if (invert)
			{
				p = (CUBE_MAX-positions[(x*CUBE_SIZE)+y]);
			} else
			{
				p = positions[(x*CUBE_SIZE)+y];
			}
		
			if (axis == AXIS_Z)
				setvoxel(x,y,p);
				
			if (axis == AXIS_Y)
				setvoxel(x,p,y);
				
			if (axis == AXIS_X)
				setvoxel(p,y,x);
		}
	}
	
}


void effect_boxside_randsend_parallel (char axis, int origin, int delay, unsigned char mode)
{
	int i;
	int done;
	unsigned char cubepos[CUBE_BYTES];
	unsigned char pos[CUBE_BYTES];
	int notdone = 1;
	int notdone2 = 1;
	int sent = 0;
	
	for (i=0;i<CUBE_BYTES;i++)
	{
		pos[i] = 0;
	}
	
	while (notdone)
	{
		if (mode == 1)
		{
			notdone2 = 1;
			while (notdone2 && sent<CUBE_BYTES)
			{
				i = rand()%CUBE_BYTES;
				if (pos[i] == 0)
				{
					sent++;
					pos[i] += 1;
					notdone2 = 0;
				}
			}
		} else if (mode == 2)
		{
			if (sent<CUBE_BYTES)
			{
				pos[sent] += 1;
				sent++;
			}
		}
		
		done = 0;
		for (i=0;i<CUBE_BYTES;i++)
		{
			if (pos[i] > 0 && pos[i] <CUBE_MAX)
			{
				pos[i] += 1;
			}
				
			if (pos[i] == CUBE_MAX)
				done++;
		}
		
		if (done == CUBE_BYTES)
			notdone = 0;
		
		for (i=0;i<CUBE_BYTES;i++)
		{
			if (origin == 0)
			{
				cubepos[i] = pos[i];
			} else
			{
				cubepos[i] = (CUBE_MAX-pos[i]);
			}
		}
		
		
		delay_ms(delay);
		draw_positions_axis(axis,cubepos,0);
	}
	
}

// Set n number of voxels at random positions
void effect_random_sparkle_flash (int iterations, int voxels, int delay)
{
	int i,v;
	int x,y,z;
	
	for (i = 0; i < iterations; i++)
	{
		for (v=0;v<=voxels;v++)
			x = rand()%CUBE_SIZE;
			y = rand()%CUBE_SIZE;
			z = rand()%CUBE_SIZE;
			setvoxel(x,y,z);
			
		delay_ms(delay);
		fill(0x00);
	}
}

// blink 1 random voxel, blink 2 random voxels..... blink 20 random voxels
// and back to 1 again.
void effect_random_sparkle (void)
{
	int i;
	
	for (i=1;i<15;i++)
	{
		effect_random_sparkle_flash(5,i,200);
	}
	
	for (i=15;i>=1;i--)
	{
		effect_random_sparkle_flash(5,i,200);
	}
	
}

int effect_telcstairs_do(unsigned char x, int val, int delay)
{
	int y,z;

	for(y = 0, z = x; y <= z; y++, x--)
	{
		if(x < CUBE_SIZE && y < CUBE_SIZE)
		{
			cube[x][y] = val;
		}
	}
	delay_ms(delay);
	return z;
}

void effect_telcstairs (unsigned char invert, int delay, int val)
{
	int x;

	if(invert)
	{
		for(x = CUBE_SIZE*2; x >= 0; x--)
		{
			x = effect_telcstairs_do(x,val,delay);
		}
	}
	else
	{
		for(x = 0; x < CUBE_SIZE*2; x++)
		{
			x = effect_telcstairs_do(x,val,delay);
		}
	}
}

void effect_wormsqueeze (int size, unsigned char axis, unsigned char direction, int iterations, int delay)
{
	int x, y, i,j,k, dx, dy;
	int cube_size;
	int origin = 0;
	
	if (direction == -1)
		origin = CUBE_MAX;
	
	cube_size = CUBE_MAX-(size-1);
	
	x = rand()%cube_size;
	y = rand()%cube_size;
	
	for (i=0; i<iterations; i++)
	{
		dx = ((rand()%3)-1);
		dy = ((rand()%3)-1);
	
		if ((x+dx) > 0 && (x+dx) < cube_size)
			x += dx;
			
		if ((y+dy) > 0 && (y+dy) < cube_size)
			y += dy;
	
		shift(axis, direction);
		

		for (j=0; j<size;j++)
		{
			for (k=0; k<size;k++)
			{
				if (axis == AXIS_Z)
					setvoxel(x+j,y+k,origin);
					
				if (axis == AXIS_Y)
					setvoxel(x+j,origin,y+k);
					
				if (axis == AXIS_X)
					setvoxel(origin,y+j,x+k);
			}
		}
		
		delay_ms(delay);
	}
}

