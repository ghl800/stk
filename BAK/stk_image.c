#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_image.h"


typedef struct STK_ImageDividing {
	topcenter;
	bottomcenter;
	rightcenter;
	leftcenter;
	topleft;
	topright;
	bottomright;
	bottomleft;
} STK_ImageDividing;

STK_ImageDividing g_image_dividing = {
	
	.topcenter = { 0x0000ff00, 0x00ff0000 }, // area 1, two pixels
	.bottomcenter = { 0x000000ff, 0x0000ff00 }, // area 3, two pixels
	.rightcenter = { 0x0000ff00, 0x00ff0000 }, // area 2, two pixels
	.leftcenter = { 0x0000ff00, 0x00ff0000 }, // area 4, two pixels
};

STK_ImageDividing g_image_dividing_matrix = {
	.topleft = { 0x00ffff77, 0x00ff7777, 0x00ff0088, 0x00ffffff }, // area 5, 2x2 pixels
	.topright = { 0x0000ff77, 0x00ff5566, 0x00ff0080, 0x00ff8080 }, // area 6, 2x2 pixels
	.bottomright = { 0x0000ff8080, 0x00ffffff, 0x00808080, 0x00404040 }, // area 7, 2x2 pixels
	.bottomleft = { 0x0000ff00, 0x00ffff00, 0x00008080, 0x00ff7799 } // area 8, 2x2 pixels
};


// here, pattern need to vertify furtherly
SDL_Surface *STK_ImageFillRect(SDL_Widget *widget, SDL_Image *image, SDL_Rect *rect, Uint32 pattern)
{
	SDL_Surface *surface = widget->surface;
	Uint32 bgcolor = widget->bgcolor;
	int style = image->fillstyle;
	int i = 0;
	SDL_Rect r;
	
	switch (style) {
	case STK_IMAGESTYLE_NORMAL:
		SDL_FillRect(surface, NULL, bgcolor);
		break;
	case STK_IMAGESTYLE_HORIZONTAL:
		for (i = 0; i < rect.h ; i++) {
			r.x = rect->x;
			r.y = rect->y + i;
			r.w = rect->w;
			r.h = 1;
			SDL_FillRect(surface, &r, g_image_dividing[pattern][i]);
		}
		break;
	case STK_IMAGESTYLE_VERTICAL:
		for (i = 0; i < rect.w; i++) {
			r.x = rect->x + i;
			r.y = rect->y;
			r.w = 1;
			r.h = rect->h;
			SDL_FileRect(surface, &r, g_image_dividing[pattern][i]);
		}
		break;
	case STK_IMAGESTYLE_MATRIX: 
		SDL_Surface *s;
		s = SDL_CreateRGBSurfaceFrom((void *)g_image_dividing_matrix[pattern], 
						2, 
						2, 
						32,
						8,
						0x00ff0000,
						0x0000ff00,
						0x000000ff,
						0xff000000);
		SDL_BlitSurface(s, NULL, surface, rect);
		break;
	default:
		break;

	}
	
}
