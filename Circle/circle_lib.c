#include "circle_lib.h"


// Going to be rebuilding the circle_lib library, but this time using TDD

octet octet_init(){
	octet new_octet;


	new_octet.size = 1; // initial size allocated
	new_octet.used = 0;
	new_octet.border_count = 0;

	new_octet.pixel_x = malloc(sizeof(uint16_t) * new_octet.size);
	new_octet.pixel_y = malloc(sizeof(uint16_t) * new_octet.size);

	return new_octet;
}

// this function is only called when creating original octet
void new_pixel(octet * manager, uint16_t pos_x, uint16_t pos_y){
	if(manager->size == manager->used){
		// double the size allocation
		manager->size += manager->size;
		manager->pixel_x = realloc(manager->pixel_x, manager->size * sizeof(uint16_t));
		manager->pixel_y = realloc(manager->pixel_y, manager->size * sizeof(uint16_t));
	}
	// input the new position
	manager->pixel_x[manager->used]   = pos_x;
	manager->pixel_y[manager->used++] = pos_y; // increment the used pixels by 1
	return;
}

void octet_border_draw(octet * manager, uint16_t radius){
	// this will create the pixels in the original
	// using bresenheim's circle border drawing algorithm: https://www.geeksforgeeks.org/bresenhams-circle_border-drawing-algorithm/

	float decision = 3 - (2*radius); // initial decision

	// relative positions to the origin
	uint16_t x_pos = 0;
    uint16_t y_pos = radius; // STARTING IN OCTET 2, going COUNTER CLOCKWISE

    uint32_t total_pix = 0;
    while(x_pos <= y_pos){
    	total_pix++;

    	// add new x/y pixel to the arrays
    	new_pixel(manager,x_pos,y_pos); // how will I mock a nested function?

    	x_pos++;
    	if(decision<0)
    		decision += (4*x_pos) + 6;
    	else{
    		decision += 4*(x_pos-y_pos) + 10;
    		y_pos--;
    	}
    	// after updating, have to check condition
    }
	// set the border_count to the total tracked in this function
    manager->border_count = total_pix;
    // printf("Octet border draw: manager->border_count = %hu\n",manager->border_count);
    return;
}

// call this after drawing/filling an octet
void clean_octet(octet * manager){
	if(manager->used < manager->size){
		manager->size = manager->used;
		manager->pixel_x = realloc(manager->pixel_x, manager->size * sizeof(uint16_t));
		manager->pixel_y = realloc(manager->pixel_y, manager->size * sizeof(uint16_t));
	}
	return;
}


// octet fill function!

void octet_fill(octet * manager){
	// will make generous use of border_count element!
	uint16_t border_track = 0;
	while(border_track < (manager->border_count)){ // which border pixel are we at?
		uint16_t i;
		// now we have the x/y position of our border pixel
		// i represents y-coordinate
		for(i=0;i < manager->pixel_y[border_track]; i++){ // loop only goes from x-axis to border y!
			if(i >= manager->pixel_x[border_track]){ // only when we're above the diagonal line
				new_pixel(manager,manager->pixel_x[border_track],i);
			}
		} // added all the new pixels in this collumn
		border_track++;
	}
	return;
}

// makes it much more efficient to just shift the first octet
void octet_shift(octet * manager,uint16_t center_x, uint16_t center_y){
	uint32_t i;
	for(i=0; i < manager->used; i++){
		manager->pixel_x[i] += center_x;
		manager->pixel_y[i] += center_y;
	}
	return;
}

circle circle_init(uint16_t center_x, uint16_t center_y, uint16_t radius){
	circle ret; // allocates space for octet array and other members
	ret.center_x = center_x;
	ret.center_y = center_y;
	ret.radius = radius;
	ret.total_pixels = 0;
	return ret;
}

octet octet_copy_init(octet * original){
	octet ret;
	uint8_t static count=0;
	// printf("called octet_copy_init %hhu times\n",count++);
	// printf("Original size: %u\n",original->size);
	ret.pixel_x = malloc(sizeof(uint16_t) * original->size);
	if(ret.pixel_x == NULL)
		printf("ERROR MALLOC X\n");
	else
		printf("Success x\n");
	ret.pixel_y = malloc(sizeof(uint16_t) * original->size);
	if(ret.pixel_y == NULL)
		printf("ERROR MALLOC Y\n");
	else
		printf("Success y\n");
	ret.size = original->size;
	ret.used = original->used;
	ret.border_count = original->border_count;
	// printf("ret.border_count = %hu\n",ret.border_count);
	return ret;
}

// TO BE TESTED
// ************************************************************************************

void octet_copy(circle * pizza, octet * oct_2, uint8_t n, uint16_t center_x, uint16_t center_y){
	uint32_t i;
	// printf("Octet copy: Slice number %hhu\n",n);
	switch(n){
		case 0: // octet 1, x <--> y
			pizza->slice[n] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[n].pixel_x[i] = (oct_2->pixel_y[i] - center_y) + center_x;
				pizza->slice[n].pixel_y[i] = (oct_2->pixel_x[i] - center_x) + center_y;
			}
			break;
		case 1: // octet 3, -x, y
			pizza->slice[2] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[2].pixel_x[i] = -(oct_2->pixel_x[i] - center_x) + center_x;
				pizza->slice[2].pixel_y[i] = oct_2->pixel_y[i];
			}
			break;
		case 2: // octet 4, x <--> -y
			pizza->slice[3] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[3].pixel_x[i] = -(oct_2->pixel_y[i] - center_y) + center_x;
				pizza->slice[3].pixel_y[i] = (oct_2->pixel_x[i] - center_x) + center_y;
			}
			break;
		case 3: // octet 5, -x <--> -y
			pizza->slice[4] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[4].pixel_x[i] = -(oct_2->pixel_y[i] - center_y) + center_x;
				pizza->slice[4].pixel_y[i] = -(oct_2->pixel_x[i] - center_x) + center_y;
			}
			break;
		case 4: // octet 6, -x, -y
			pizza->slice[5] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[5].pixel_x[i] = -(oct_2->pixel_x[i] - center_x) + center_x;
				pizza->slice[5].pixel_y[i] = -(oct_2->pixel_y[i] - center_y) + center_y;
			}
			break;
		case 5: // octet 7, x, -y
			pizza->slice[6] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[6].pixel_x[i] = oct_2->pixel_x[i];
				pizza->slice[6].pixel_y[i] = -(oct_2->pixel_y[i] - center_y) + center_y;
			}
			break;
		case 6: // octet 8, -x <--> y
			pizza->slice[7] = octet_copy_init(oct_2);
			for(i=0;i<oct_2->used;i++){
				pizza->slice[7].pixel_x[i] = (oct_2->pixel_y[i] - center_y) + center_x;
				pizza->slice[7].pixel_y[i] = -(oct_2->pixel_x[i] - center_x) + center_y;
			}
			break;
		default:
			printf("Error: Value n=%hhu outside of acceptible range\n",n);// error!
			exit(-1);
	}
}

// this is called after all octets have been copied

void border_reverse(octet * slice){
	uint32_t i;
	uint32_t border_count = slice->border_count;
	uint16_t cpy_x, cpy_y;
	// printf("Border reverse: border count = %hu\n",border_count);
	for(i=0;i < border_count/2; i++){ // 4/2 = 2;
		//
		// copy lower bound
		cpy_x = slice->pixel_x[i];
		cpy_y = slice->pixel_y[i];
		// replace lower with upper
		slice->pixel_x[i] = slice->pixel_x[border_count - 1 - i];
		slice->pixel_y[i] = slice->pixel_y[border_count - 1 - i];
		// replace upper with copy
		slice->pixel_x[border_count - 1 - i] = cpy_x;
		slice->pixel_y[border_count - 1 - i] = cpy_y;
	}	
}

circle create_circle(uint16_t center_x, uint16_t center_y, uint16_t radius, bool fill){
	// init the original octet
	octet oct_2 = octet_init();
	// draw the octet border
	octet_border_draw(&oct_2,radius);
	// fill the octet if needed
	if(fill){
		octet_fill(&oct_2);
	}
	// clean the octet
	clean_octet(&oct_2);
	// shift the octet
	octet_shift(&oct_2,center_x,center_y);
	// init the circle
	circle pizza = circle_init(center_x, center_y, radius);
	// make the pizza slice copies
	uint8_t i;
	for(i=0;i<7;i++){
		octet_copy(&pizza,&oct_2,i,center_x,center_y);
	}
	pizza.slice[1] = oct_2;
	for(i=0;i<8;i++){
		if(i%2){
		// reverse the slices 2,4,6,8
			// printf("Reversing border %hhu\n",i);
			border_reverse(&(pizza.slice[i]));
		}
	}
	pizza.total_pixels = oct_2.used * 8;
	// return the circle
	return pizza;
}

// to make this modular, going to create a function to make copies
// n represents nth copy
// this function takes a SHIFTED octet and copies it


// shifting a circle

void shift_circle(circle * pizza, int16_t shift_x, int16_t shift_y){
	uint8_t i;
	uint32_t j;
	for(i=0;i<8;i++){
		for(j=0;j<pizza->slice[i].used;j++){
			pizza->slice[i].pixel_x[j] += shift_x;
			pizza->slice[i].pixel_y[j] += shift_y;
		}
	}
	pizza->center_x += shift_x;
	pizza->center_y += shift_y;
	return;
}