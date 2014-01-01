// ############################################
//
// Custom effects by Sublime 2013
// Lisence: GPL
//
// ############################################

#ifndef CUSTOM_EFFECTS_H
#define CUSTOM_EFFECTS_H

#include "cube.h"

// For custom_effects.c
void loadbar (int delay, int wait, unsigned char leave);
void random_layer_filler (int delay, unsigned char set_state, int z, int loops);
void effect_random_filler (int delay, unsigned char state);
void custom_rain (int drops, int speed);
void fireworks (int fireballs, int speed, unsigned char mode);
void effect_spinning_plane(unsigned char direction, int iterations, int delay, unsigned char mode);

// For expand.c 
void ripples (int x, int y, int z, int speed);
void explode (int x, int y, int z, int speed, unsigned char mode);
void dissolve (int speed, unsigned char modifier, unsigned char direction);

unsigned char rings[9][7];
unsigned char disks[13][7];

#endif

