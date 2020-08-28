#ifndef RECTANGLE_LIB_H
#define RECTANGLE_LIB_H

#pragma pack(1) // must include this for testing structures with unity/ceedling

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

extern double pi;

typedef struct line_t {
	// pixel information
	uint16_t * pixel_x;
	uint16_t * pixel_y;
	uint32_t size, used;
	// triangle information
	uint16_t dx, dy; // triangle leg information
	uint16_t length; // hypotenuse information
}line;

// haven't made any use of this! might be completely pointless!
typedef struct rectangle_t {
	line height; // skeleton line that will build the width lines off of
	line width[2]; // the top/bottom lines that will define the dx/dy of the body lines
	// the border sides, come from height line shifted by the beginning/end of width[1]!
	line sides[2];
	// THESE ARE THE FILLER PIXELS!
	uint16_t * pixel_x;
	uint16_t * pixel_y;
	uint16_t size, used;

	uint16_t min_x, max_x, min_y, max_y;

	// need these two so that rectangle will always properly reset!
	uint16_t magnitude_h, magnitude_w;
	// these come from the first pixel of the height line
	uint16_t base_x, base_y;
}rectangle;

// tested
line line_init(uint16_t length);
void add_pixel(line * vector, int16_t x, int16_t y);
void calc_line(line * vector, float theta, uint16_t start_x, uint16_t start_y);
void calc_width(line * vector, float theta, uint16_t height_x, uint16_t height_y);
void clean_line(line * vector);
// untested
void shift_line(line * vector, int16_t shift_x, int16_t shift_y); // !!!!

// rectangle functions

void rectangle_update(rectangle * source, float theta, int16_t shift_x, int16_t shift_y);
void garbage_collect(rectangle * source);

rectangle rectangle_init(line * height, line * top_line, line * bot_line, line * left_line, line * right_line);
rectangle create_rectangle(uint16_t base_x, uint16_t base_y, float theta, uint16_t height, uint16_t width);
void rectangle_fill(rectangle * source);
void add_filler(rectangle * source, int16_t x, int16_t y);
bool find_border_pixel(rectangle * source, uint16_t x, uint16_t y);
void clean_rectangle(rectangle * source);


#endif // RECTANGLE_LIB_H