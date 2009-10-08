#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_widget.h"
#include "stk_image.h"


Uint32 g_image_dividing_horizontal[2][2][2] = {
	{	// pattern 1
		{ 0x00fefffd, 0x00d5d0ca }, // area 1, topcenter, two pixels
		{ 0x00807e7f, 0x0041403c }, // area 3, bottomcenter, two pixels
	},
	{	// pattern 2
		{ 0x003d423b, 0x0083807b }, // area 1, topcenter, two pixels
		{ 0x00ccd1cb, 0x00fafff9 }, // area 3, bottomcenter, two pixels
	}
};

Uint32 g_image_dividing_vertical[2][2][2] = {
	{	// pattern 1
		{ 0x00807e81, 0x00403f44 }, // area 2, rightcenter, two pixels
		{ 0x00fffeff, 0x00d4cfc9 }  // area 4, leftcenter, two pixels
	},
	{	// pattern 2
		{ 0x00d2cfc8, 0x00fffef8 }, // area 2, rightcenter, two pixels
		{ 0x00473e37, 0x00768477 }  // area 4, leftcenter, two pixels
	}
};

Uint32 g_image_dividing_matrix[2][4][4] = {
	{	// pattern 1
		{ 0x00ffffff, 0x00fdfdfd, 0x00fffffd, 0x00d6cfc7 }, // area 5, topleft, 2x2 pixels
		{ 0x00ffffff, 0x0040403e, 0x007f807b, 0x0040403e }, // area 6, topright, 2x2 pixels
		{ 0x007f7f7f, 0x0040403e, 0x00413f40, 0x00454440 }, // area 7, bottomright, 2x2 pixels
		{ 0x00feffff, 0x00817f80, 0x003d3c3a, 0x0041413f }  // area 8, bottomleft, 2x2 pixels
	},
	{	// pattern 2
		{ 0x00483f3a, 0x0049423a, 0x00433f3c, 0x00817373 }, // area 5, topleft, 2x2 pixels
		{ 0x00423f38, 0x00fffff8, 0x00d3d0c9, 0x00fffffb }, // area 6, topright, 2x2 pixels
		{ 0x00d5d2cb, 0x00fffff8, 0x00fffff8, 0x00fffff8 }, // area 7, bottomright, 2x2 pixels
		{ 0x004b3f41, 0x00c5d0ca, 0x00fffafc, 0x00f7fffc }  // area 8, bottomleft, 2x2 pixels
	}
};


// here, pattern need to vertify furtherly
int STK_ImageFillRect(SDL_Surface *surface, STK_Image *image, SDL_Rect *rect, Uint32 pattern, Uint32 n)
{
	// here miss something
	Uint32 bgcolor = 0x00f4f400;
	int style = image->fillstyle;
	int i = 0, j = 0;
	SDL_Rect r;
	
	switch (style) {
	case STK_IMAGESTYLE_NORMAL:
		if (pattern == 0)
			SDL_FillRect(surface, NULL, bgcolor);
		else if (pattern == 1)
			SDL_FillRect(surface, NULL, 0x00808080);
		break;
	case STK_IMAGESTYLE_HORIZONTAL:
		for (i = 0; i < rect->h ; i++) {
			r.x = rect->x;
			r.y = rect->y + i;
			r.w = rect->w;
			r.h = 1;
			SDL_FillRect(surface, &r, g_image_dividing_horizontal[pattern][n][i]);
		}
		break;
	case STK_IMAGESTYLE_VERTICAL:
		for (i = 0; i < rect->w; i++) {
			r.x = rect->x + i;
			r.y = rect->y;
			r.w = 1;
			r.h = rect->h;
			SDL_FillRect(surface, &r, g_image_dividing_vertical[pattern][n][i]);
		}
		break;
	case STK_IMAGESTYLE_MATRIX: { 
		for (i = 0; i < rect->h; i++) {
			for (j = 0; j < rect->w; j++) {
				r.x = rect->x + j;
				r.y = rect->y + i;
				r.w = 1;
				r.h = 1;
				SDL_FillRect(surface, &r, g_image_dividing_matrix[pattern][n][rect->w*i+j]);
			}
		}

/*		SDL_Surface *s;
		s = SDL_CreateRGBSurfaceFrom((void *)g_image_dividing_matrix[pattern][n], 
						2, 
						2, 
						32,
						8,
						0x00ff0000,
						0x0000ff00,
						0x000000ff,
						0xff000000);
		SDL_BlitSurface(s, NULL, surface, rect);
*/
		break;
		}
	case STK_IMAGESTYLE_PICTURE: {
		SDL_Surface *s;
		SDL_Rect r;
		r.x = 0; 
		r.y = 0;
		r.w = rect->w;
		r.h = rect->h;
		s = SDL_LoadBMP(image->filename);
		// Attention: the size of the picture should match the desired size
		if (s) 
			SDL_BlitSurface(s, &r, surface, rect);			
		break;
		}
	default:
		break;

	}
	
	return 0;
}