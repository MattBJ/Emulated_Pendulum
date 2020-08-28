#ifndef CIRCLE_LIB_H
#define CIRCLE_LIB_H

#pragma pack(1) // MUST include this to pack everything

// Include any necessary headers in here as well

#include <stdio.h> // For outputting/debugging (also want to touch up on my gdb soon again...)
#include <stdint.h>// standard integers

#include <stdbool.h>
#include <stdlib.h> // for things like memory allocation

// 'globablly accessible' typedef data structures created here
// ********************************************************************************************


typedef struct octet_t {
	// 2 dynamically alloc/reallocated arrays for each pixel's position
	uint16_t * pixel_x;
	uint16_t * pixel_y;
	uint32_t used, size, border_count;
	// used is the current number of total pixels
	// size is the space allocated for the arrays
	// border_count is the size of the border at beginning of each array
}octet;

typedef struct circle_t {
	// preallocated array of 8 octets
	octet slice[8];
	uint16_t center_x, center_y, radius;
	uint32_t total_pixels; // total pixels in the CIRCLE
	// should I track boolean 'fill'?
}circle;

// ********************************************************************************************

// TESTED
octet octet_init();
void new_pixel(octet * manager, uint16_t pos_x, uint16_t pos_y);
void octet_border_draw(octet * manager, uint16_t radius);
void clean_octet(octet * manager);
void octet_fill(octet * manager);
void octet_shift(octet * manager, uint16_t center_x, uint16_t center_y);
circle circle_init(uint16_t center_x, uint16_t center_y, uint16_t radius);
// TO BE TESTED
octet octet_copy_init(octet * original);
void octet_copy(circle * pizza, octet * oct_2, uint8_t n, uint16_t center_x, uint16_t center_y);
void border_reverse(octet * slice);
circle create_circle(uint16_t center_x, uint16_t center_y, uint16_t radius, bool fill);

#endif // CIRCLE_LIB_H
