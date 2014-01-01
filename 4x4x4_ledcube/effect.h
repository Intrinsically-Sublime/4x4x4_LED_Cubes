#ifndef EFFECT_H
#define EFFECT_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "cube.h"

void effect_planboing (int plane, int speed);
void effect_blinky2(void);

void effect_box_shrink_grow (int iterations, int rot, int flip, uint16_t delay);
void effect_box_woopwoop (int delay, unsigned char grow);

void sendvoxel_z (unsigned char x, unsigned char y, unsigned char z, int delay);
void sendplane_rand_z (unsigned char z, int delay, int wait);
void sendvoxels_rand_z (int iterations, int delay, int wait);

void boingboing(uint16_t iterations, int delay, unsigned char mode, unsigned char drawmode);

void effect_rain(int iterations);

void effect_z_updown (int iterations, int delay);
void effect_axis_updown_randsuspend (char axis, int delay, int sleep, unsigned char invert);
void effect_z_updown_move (unsigned char positions[CUBE_BYTES], unsigned char destinations[CUBE_BYTES], char axis);

void draw_positions_axis (char axis, unsigned char positions[CUBE_BYTES], unsigned char invert);

void effect_boxside_randsend_parallel (char axis, int origin, int delay, unsigned char mode);

void effect_random_sparkle_flash (int iterations, int voxels, int delay);
void effect_random_sparkle (void);

int effect_telcstairs_do(unsigned char x, int val, int delay);
void effect_telcstairs (unsigned char invert, int delay, int val);

void effect_wormsqueeze (int size, unsigned char axis, unsigned char direction, int iterations, int delay);

#endif

