#include "effect.h"
#include "draw.h"
#include "gameoflife.h"
#include "expand.c"
#include "custom_effects.c"

void launch_effect (int effect)
{
	int speed = 2800;
	int speed_2 = speed * 0.5;
	int speed_3 = speed * 0.75;
	int speed_4 = speed * 4;

	fill(0x00);
			
	switch (effect)
	{
		// Matrix style rain
		case 0x00:
			// (iterations)
			effect_rain(100, speed);
			break;
			
		// Rain voxels down and send out ripples on the bottom plane
		case 1:
			// (drops, speed of raindrop)
			custom_rain(20,speed);
			break;
			
		// Fireworks
		case 2:
			// (fireballs, speed, explosion type (0 - 6, 99=random))
			fireworks(30,(speed_2),99);
			break;
			
		// Send random voxels up and down
		case 3:
			// (voxels, iterations, speed)
			sendvoxels_rand_z(20,220,(speed*2));
			break;
			
		// Randomly fill and unfill the cube	
		case 4:
			// (speed, state)
			for (int i = 1; i < 5;i++)
			{
				effect_random_filler((speed*0.1),i%2);
			}
			break;
				
		// Send voxels randomly up and down random heights on Z
		case 5:
			// (voxels, speed)
			effect_z_updown(20,(speed_3));
			break;
			
		// Blink entire cube slow to quick to slow	
		case 6:
			effect_blinky2();
			break;
			
		// Expand and contract wire frame square from corners	
		case 7: 
			for (unsigned char i = 0; i < 2;i++) // Repeat twice
			{
				for (unsigned char ii=0;ii<8;ii++) // All 8 corners
				{
					// (iterations per corner, N/A, N/A, Speed)
					effect_box_shrink_grow (1, ii%4, ii & 0x04, speed);
				}
			}
			break;
			
		// Expand and contract wire frame square from center
		case 8:
			// (speed, direction)
			effect_box_woopwoop(speed,0);
			effect_box_woopwoop(speed,1);
			effect_box_woopwoop(speed,0);
			effect_box_woopwoop(speed,1);
			break;
			
		// Send plane from one side to another
		case 9:
			// (axis, speed)
			effect_planboing (AXIS_Z, speed);
			effect_planboing (AXIS_X, speed);
			effect_planboing (AXIS_Y, speed);
			effect_planboing (AXIS_Z, speed);
			effect_planboing (AXIS_X, speed);
			effect_planboing (AXIS_Y, speed);
			break;
		
		// Fill and empty the cube on a diagonal
		case 10:
			// (direction, speed, pattern)
			fill(0x00);
			effect_telcstairs(0,(speed_3),0xff);
			effect_telcstairs(0,(speed_3),0x00);
			effect_telcstairs(1,(speed_3),0xff);
			effect_telcstairs(1,(speed_3),0xff);
			break;
			
		// Send voxels from one side to another and pause some in the middle
		case 11:
			// (axis, speed ,pause, direction)
			effect_axis_updown_randsuspend(AXIS_Z, (speed_2),(speed_4),0);
			effect_axis_updown_randsuspend(AXIS_Z, (speed_2),(speed_4),1);
			effect_axis_updown_randsuspend(AXIS_Z, (speed_2),(speed_4),0);
			effect_axis_updown_randsuspend(AXIS_Z, (speed_2),(speed_4),1);
			effect_axis_updown_randsuspend(AXIS_X, (speed_2),(speed_4),0);
			effect_axis_updown_randsuspend(AXIS_X, (speed_2),(speed_4),1);
			effect_axis_updown_randsuspend(AXIS_Y, (speed_2),(speed_4),0);
			effect_axis_updown_randsuspend(AXIS_Y, (speed_2),(speed_4),1);
			break;
			
		// Wiggle a string (worm) around the cube
		case 12:
			// (voxels, axis, direction, iterations, speed)
			effect_wormsqueeze (1, AXIS_Z, 1, 100, speed);
			break;
			
		// Play conways game of life
		case 13:
			// Create a random starting point for the Game of Life effect.
			for (unsigned char i = 0; i < 15;i++)
			{
				int x = rand()%CUBE_SIZE;
				int y = rand()%CUBE_SIZE;
				int z = rand()%CUBE_SIZE;
				setvoxel(x,y,z);
			}
			// (iterations, speed)
			gol_play(20, (speed_2));
			break;
			
		// Send plane form one side to the other in random wave
		case 14:
			// (axis, origin, speed, mode(1 or 2))
			effect_boxside_randsend_parallel (AXIS_Z, 0 , (speed_2),1);
			delay_ms(speed);
			effect_boxside_randsend_parallel (AXIS_Z, 1 , (speed_2),1);
			delay_ms(speed);
			
			effect_boxside_randsend_parallel (AXIS_Z, 0 , (speed_2),2);
			delay_ms(speed);
			effect_boxside_randsend_parallel (AXIS_Z, 1 , (speed_2),2);
			delay_ms(speed);
			
			effect_boxside_randsend_parallel (AXIS_Y, 0 , (speed_2),1);
			delay_ms(speed);
			effect_boxside_randsend_parallel (AXIS_Y, 1 , (speed_2),1);
			delay_ms(speed);
			break;
			
		// Snake
		case 15:
			// (iterations, speed, mode, drawmode)
			boingboing(250, (speed_2), 0x01, 0x02);
			break;
			
		// Wiggle a string (worm) around the cube
		case 16:
			// (voxels, axis, direction, iterations, speed)
			effect_wormsqueeze (1, AXIS_Z, -1, 100, speed);
			break;
			
		// Sparkle
		case 17:
			effect_random_sparkle();
			break;
			
		// Spinning spiral
		case 18:// (direction, iterations, delay, mode)
			// Mode: 0 = plane, 1 = square, 2 = spiral square, 3 spiral plane
			for (int i = 0; i < 4;i++)
			{
				effect_spinning_plane(1,10,(speed*0.7),i);
			}
			break;
			
		case 19:
			// (speed, wait between fill and empty, layers to empty)
			loadbar(speed,(speed*2),0); // Leave all layers light
			break;
			
		// Dissolve layers one by one from top 
		case 20:
			// (speed, state, z ,total voxels to clear)
			fill(0xff);
			for (int i = 3; i >= 0;i--)
			{
				random_layer_filler ((speed*0.333),0,i,CUBE_BYTES);
			}
			break;
			
		// In case the effect number is out of range:
		default:
			effect_rain(100, speed);
			break;
		
		

	}
}

