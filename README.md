4x4x4 LED cube code for Atmega microcontrollers by Sublime
==========================================================

Based on 4x4x4 and 8x8x8 LED cube by Christian Moen
http://www.instructables.com/id/LED-Cube-4x4x4/

License GPLv3 or newer

Versions:

* 4x4x4_ledcube - Set up for Atmega32 using 8mhz internal clock. Hold down button while resetting to enter random mode and set the random seed.

* Micro_cube - The same code as above except for an Arduino Pro Mini 168 with outputs remapped (can be used with 328) and no button.

Notes:

* Optimized code to compile under 14k for Atmega168 with Arduino bootloader.

* Made code more portable for different size cubes.

* Removed serial initialization since it was never complete for the 4x4x4 cube and only took up space.

* New effects include, Fireworks with multiple explosion types, Rain with ripples on bottom layer, Spinning-plane-square, Spiralling-plane-square
