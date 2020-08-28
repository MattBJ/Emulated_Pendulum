#include "rectangle_lib.h"

double pi = 3.141592653589793;

line line_init(uint16_t length){
	line out;
	out.pixel_x = malloc(sizeof(uint16_t));
	out.pixel_y = malloc(sizeof(uint16_t));
	out.used = 0;
	out.size = 1;
	out.dx = out.dy = 0; // this isn't important for now
	out.length = length;
	return(out);
}

void add_pixel(line * vector, int16_t x, int16_t y){
	if(vector->size == vector->used){
		// double size allocation
		vector->size += vector->size;
		vector->pixel_x = realloc(vector->pixel_x, vector->size * sizeof(uint16_t));
		vector->pixel_y = realloc(vector->pixel_y, vector->size * sizeof(uint16_t));
	}
	// input the new pixel
	vector->pixel_x[vector->used]   = x;
	vector->pixel_y[vector->used++] = y; // also, update the used!
	return;
}

// calc line can be used for height and width:
	// height -- parallel with body
	// height: use the radians on unit circle, start_x and start_y are at center of wheel

	// width -- normal to body
	// width: theta = height theta - pi/2
	// width: start_x and start_y = 1/2 length

// this function based solely on magnitude, direction, and offset
void calc_line(line * vector, float theta, uint16_t start_x, uint16_t start_y){
	// using an altered Bresenham's Line drawing alg.
	// https://classic.csunplugged.org/wp-content/uploads/2014/12/Lines.pdf

	// using length and angle, can calculate the line
	// going to be used with width and height

	// for now, won't convert to a fixed point Q format..

	uint16_t i;

	int16_t cur_x = start_x, cur_y = start_y;
	// looks like I'm going to have to use my circle library for LUTs
	int16_t dx, dy, decision, A, B;
	int8_t increment_x, increment_y;

	// going to add rounding for when it casts back to integer
	double tmp;
	tmp = ((double)(vector->length) * sin(theta) );
	if(tmp<0){
		dy = ((int16_t)(tmp - 0.5));
		increment_y = -1;
	} else { // 4
		dy = ((int16_t)(tmp + 0.5));
		increment_y = 1;
	}
	tmp = ((double)(vector->length) * cos(theta) );
	if(tmp<0){ // -4
		dx = ((int16_t)(tmp - 0.5));
		increment_x = -1;
	} else {
		dx = ((int16_t)(tmp + 0.5));
		increment_x = 1;
	}

	// things change if dy > dx or vice versa
	// if slope is in quadrant 2/3
	// if negative slope (-dy), draw next cols pixel one row below previous
	uint8_t testing = 0;
	// also, apparently supposed to add the first pixel?

// NOTE: putting an initial add_pixel, AND, reversing the order of increments and add_pix

	add_pixel(vector,cur_x,cur_y); // fill in first pixel

	if(fabs(dy) > fabs(dx)){ // magnitude of dy > dx
		A = 2 * fabs(dx);
		B = A - 2 * fabs(dy);
		decision = A - fabs(dy);
		// will default increment the y
		while( (cur_x != (dx + start_x)) || (cur_y != (dy + start_y) ) ){
			testing++;
			cur_y += increment_y;
			if(decision>=0){
				cur_x += increment_x;
				add_pixel(vector,cur_x,cur_y);
				decision += B;
			} else {
				add_pixel(vector,cur_x,cur_y);
				decision += A;
			}
		}	
	} else { // magnitude of dx > dy
		A = 2 * fabs(dy);
		B = A - 2 * fabs(dx);
		decision = A - fabs(dx);
		while( (cur_x != (dx + start_x)) || (cur_y != (dy + start_y)) ){ // stops when cur x == dx + start_x AND cur y == dy + start_y
			testing++;
			cur_x += increment_x;
			if(decision >= 0){
				cur_y += increment_y;
				add_pixel(vector,cur_x,cur_y);
				decision += B;
			} else {
				add_pixel(vector,cur_x,cur_y);
				decision += A;
			}
		}
	}

	clean_line(vector); // clean up the allocation

	vector->dx = fabs(dx);
	vector->dy = fabs(dy);
	return;
}

// when updating circle and when calculating width
void shift_line(line * vector, int16_t shift_x, int16_t shift_y){
	uint16_t i;
	for(i=0;i<vector->used;i++){
		vector->pixel_x[i] += shift_x;
		vector->pixel_y[i] += shift_y;
	}
}

void calc_width(line * vector, float theta, uint16_t height_x, uint16_t height_y){
	// find the total length of the line
		// whichever dx/dy is greate = number of pixels total

	// if total pixels = even: re calculate total pixels by incrementing line length!
		// keep incrementing line length until the 'pixel array length' is odd

	// if total pixels = odd: create full line using theta - pi/2 AND length / 2
		// shift the line by finding dx/dy of midpoint, easy!
	theta -= pi/2;
	int16_t dx, dy;
	bool loop = true;

	// going to add rounding for when it casts back to integer
	double tmp;
	while(loop){ // this loop guarantees the largest delta is EVEN, such that the new line pixel array is ODD (because EVEN plus one (for initial pix) = odd)
		tmp = ((double)(vector->length) * sin(theta) );
		if(tmp<0){
			dy = ((int16_t)(tmp - 0.5));
		} else { // 4
			dy = ((int16_t)(tmp + 0.5));
		}
		tmp = ((double)(vector->length) * cos(theta) );
		if(tmp<0){ // -4
			dx = ((int16_t)(tmp - 0.5));
		} else {
			dx = ((int16_t)(tmp + 0.5));
		}
	// whichever delta is larger magnitude represents the number of pixels in the vector
		if(fabs(dx) > fabs(dy)){
			if(!(dx % 2)){ // pixel array is odd, dx + initial pixel = odd
				loop = false;
			}
		} else {
			if(!(dy % 2)){ // pixel array is odd, dy + initial pixel = odd
				loop = false;
			}
		}
		if(loop){
			vector->length += 1;
		}
	}
	// now our array is odd!
	calc_line(vector,theta,(height_x),(height_y));

	// shift full line by the dy/dx from midpoint to
	uint16_t midpoint = (vector->used)/2; // rounds down, but arrays start at 0 so its good
	
	shift_line(vector,	(height_x - vector->pixel_x[midpoint]),
						(height_y - vector->pixel_y[midpoint]));
	return;
}

void clean_line(line * vector){
	if(vector->used < vector->size){
		vector->size = vector->used;
		vector->pixel_x = realloc(vector->pixel_x, vector->size * sizeof(uint16_t));
		vector->pixel_y = realloc(vector->pixel_y, vector->size * sizeof(uint16_t));
		
	}
	return;
}


// --------------------------------------------------------------------------------------
// untested
// --------------------------------------------------------------------------------------

// need to build 5 lines here
	// height line is MOST important for building lines
rectangle rectangle_init(	line * height, line * top_line, line * bot_line,
							line * left_line, line * right_line){
	rectangle out;
	out.height = (*height);
	out.width[0] = (*top_line);
	out.width[1] = (*bot_line);
	out.sides[0] = (*left_line);
	out.sides[1] = (*right_line);
	out.size = 1;
	out.pixel_x = malloc(sizeof(uint16_t));
	out.pixel_y = malloc(sizeof(uint16_t));
	out.used = 0;

	out.magnitude_h = height->length;
	out.magnitude_w = top_line->length;
	out.base_x = height->pixel_x[0];
	out.base_y = height->pixel_y[0];

	out.min_x = 0;
	out.max_x = 0;
	out.min_y = 0;
	out.max_x = 0;

	return out;
}

// different than calc line: Creates line from two points instead of magnitude/direction




void garbage_collect(rectangle * source){
	// free up all resources
	free(source->height.pixel_x);
	free(source->height.pixel_y);	

	free(source->width[0].pixel_x);
	free(source->width[0].pixel_y);

	free(source->width[1].pixel_x);
	free(source->width[1].pixel_y);

	free(source->sides[0].pixel_x);
	free(source->sides[0].pixel_y);

	free(source->sides[1].pixel_x);
	free(source->sides[1].pixel_y);

	free(source->pixel_x);
	free(source->pixel_y);
	return;
}

// --------------------------------------------------------------------------------------
// THESE NEED TO BE UPDATED
// --------------------------------------------------------------------------------------

void rectangle_update(rectangle * source, float theta, int16_t shift_x, int16_t shift_y){

	// reset the rectangle by garbage collection
	garbage_collect(source);
	source->height = line_init(source->magnitude_h);
	source->width[0] = line_init(source->magnitude_w);
	source->width[1] = line_init(source->magnitude_w);
	source->sides[0] = line_init(source->magnitude_h);
	source->sides[1] = line_init(source->magnitude_h);

	source->pixel_x = malloc(sizeof(uint16_t));
	source->pixel_y = malloc(sizeof(uint16_t));
	source->size = 1;
	source->used = 0;


	// calculate new height

	// first update the base of the rectangle
	source->base_x += shift_x;
	source->base_y += shift_y;

	calc_line(&(source->height),theta,source->base_x,source->base_y);




	// top width
	calc_width(&(source->width[0]),theta,(source->height.pixel_x[source->height.used - 1])
										,(source->height.pixel_y[source->height.used - 1]));

	// bottom width
	calc_width(&(source->width[1]),theta,source->base_x,source->base_y);

	// make the sides
	calc_line(&(source->sides[0]),theta,source->base_x,source->base_y);

	calc_line(&(source->sides[1]),theta,source->base_x,source->base_y);

	// shifting the side lines by the bottom width's beginning/end pixel position
	shift_line(&(source->sides[0])	,source->base_x - (source->width[1].pixel_x[0])
									,source->base_y - (source->width[1].pixel_y[0]));
	shift_line(&(source->sides[1])	,source->base_x - (source->width[1].pixel_x[(source->width[1].used) - 1])
									,source->base_y - (source->width[1].pixel_y[(source->width[1].used) - 1]));

	rectangle_fill(source);

	return;
}

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------

rectangle create_rectangle(uint16_t base_x, uint16_t base_y, float theta, uint16_t height, uint16_t width){
	// first create the 5 lines
	line skele_height, top, bot, left, right;
	skele_height = line_init(height);
	left = line_init(height);
	right = line_init(height);
	top = line_init(width);
	bot = line_init(width);

	calc_line(&skele_height,theta,base_x,base_y);
	// the left and right are same as skele height, but needs to be shifted
	calc_line(&left,theta,base_x,base_y);
	calc_line(&right,theta,base_x,base_y);

	calc_width(&top,theta	,(skele_height.pixel_x[skele_height.used - 1])
							,(skele_height.pixel_y[skele_height.used - 1]));
	calc_width(&bot,theta	,(base_x)
							,(base_y));

	// shift the right/left lines by using bottom line beginniing/end
	shift_line(&left	,base_x - (bot.pixel_x[0])
						,base_y - (bot.pixel_y[0]));
	shift_line(&right	,base_x - (bot.pixel_x[bot.used - 1])
						,base_y - (bot.pixel_y[bot.used - 1]));

	// all lines have been made

	rectangle out = rectangle_init(&skele_height,&top,&bot,&left,&right);

	// now need to fill the pixels in!

	rectangle_fill(&out);
	return out;
}

void rectangle_fill(rectangle * source){

	// uint16_t static count = 0;
	// count++;

	uint16_t min_x, min_y, max_x, max_y;

	// use the width lines since they'll usually be smaller
	min_x = source->width[0].pixel_x[0];
	min_y = source->width[0].pixel_y[0];

	max_x = min_x;
	max_y = min_y;
	uint32_t i, j;
	// this can be modularized ---------------------------------
	for(i=0;i<source->width[0].used;i++){
		min_x =		(min_x > (source->width[0].pixel_x[i]))? (source->width[0].pixel_x[i]) :
					(min_x > (source->width[1].pixel_x[i]))? (source->width[1].pixel_x[i]) :
					min_x;

		max_x =		(max_x < (source->width[0].pixel_x[i]))? (source->width[0].pixel_x[i]) :
					(max_x < (source->width[1].pixel_x[i]))? (source->width[1].pixel_x[i]) :
					max_x;

		min_y = 	(min_y > (source->width[0].pixel_y[i]))? (source->width[0].pixel_y[i]) :
					(min_y > (source->width[1].pixel_y[i]))? (source->width[1].pixel_y[i]) :
					min_y;

		max_y =		(max_y < (source->width[0].pixel_y[i]))? (source->width[0].pixel_y[i]) :
					(max_y < (source->width[1].pixel_y[i]))? (source->width[1].pixel_y[i]) :
					max_y;
	}

	// update the rectangle members
	source->min_x = min_x;
	source->max_x = max_x;
	source->min_y = min_y;
	source->max_y = max_y;

	// count used for debugging certain calls

	// if(count == 3){
	// 	printf("minx %hu\nmax x %hu\nminy %hu\nmaxy %hu\n\n",min_x,max_x,min_y,max_y);
	// }

	// now we have the total area we need to fill in


	// want to simply use the filler pixels to paint
	// start filling on the first border find,
	// stop filling AFTER last border find

	/*
	if border detected, fill.
	if border detected
	*/

	uint8_t state = 0;
	uint16_t enclosed_pixels = 0;
	uint16_t k;
	// these states are
	// states: 	0 = pre_border
	//			1 = border detection
	//			2 = looking_for_enclosing

	for(i=min_x;i<max_x+1;i++){
		state = 0; // initialize state
		enclosed_pixels = 0; // initialize enclosure count
		for(j=min_y;j<max_y+1;j++){
			// this loops through the total area that we might fill in
			switch(state){
				case 0: // pre_border
					if(find_border_pixel(source,i,j)){
						state = 1; // border detected
						add_filler(source,i,j); // fill in current border
					}
					break;
				case 1: // inside border
					if(find_border_pixel(source,i,j))
						add_filler(source,i,j);
					else{
						state = 2; // begin to look for other border
						enclosed_pixels++; // 1 pixel enclosed rn
					}
					break;
				case 2: // looking for the other border
					if(find_border_pixel(source,i,j)){

						for(k=0;k<enclosed_pixels;k++){
							add_filler(source,i,(j-(enclosed_pixels-k)));
						}
						// add enclosed pixels
						state = 1;
						add_filler(source,i,j); // add border pixel
					} else {
						enclosed_pixels++;
					}
					break;
				default:
					printf("DEFAULT CASE DETECTED: %hhu\n",state);
					exit(-1);
			}
		} // for loop y
	} // for loop x

	clean_rectangle(source);

	return;
}

bool find_border_pixel(rectangle * source, uint16_t x, uint16_t y){
	// go through the width and side pixels
	uint32_t i;
	for(i=0;i<source->width[0].used;i++){
		if((source->width[0].pixel_x[i] == x) && (source->width[0].pixel_y[i] == y))
			return true;
		if( (source->width[1].pixel_x[i] == x) && (source->width[1].pixel_y[i] == y) )
			return true;
	}
	for(i=0;i<source->sides[0].used;i++){
		if((source->sides[0].pixel_x[i] == x) && (source->sides[0].pixel_y[i] == y))
			return true;
		if( (source->sides[1].pixel_x[i] == x) && (source->sides[1].pixel_y[i] == y) )
			return true;
	}
	return false;
}

void add_filler(rectangle * source, int16_t x, int16_t y){
	if(source->size == source->used){
		// double size allocation
		source->size += source->size;
		source->pixel_x = realloc(source->pixel_x, source->size * sizeof(uint16_t));
		source->pixel_y = realloc(source->pixel_y, source->size * sizeof(uint16_t));
	}
	// input the new pixel
	source->pixel_x[source->used]   = x;
	source->pixel_y[source->used++] = y; // also, update the used!
	return;
}

void clean_rectangle(rectangle * source){
	if(source->used < source->size){
		source->size = source->used;
		source->pixel_x = realloc(source->pixel_x, source->size * sizeof(uint16_t));
		source->pixel_y = realloc(source->pixel_y, source->size * sizeof(uint16_t));
	}
	return;
}