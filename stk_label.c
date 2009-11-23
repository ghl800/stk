#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#include "stk_base.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_label.h"

#define STK_LABEL_DEFAULT_WIDTH		60
#define STK_LABEL_DEFAULT_HEIGHT	30


static void STK_LabelCalculatePattern(STK_Label *label, SDL_Rect *rect);


STK_Widget *STK_LabelNew( char *str, Uint16 x, Uint16 y )
{
	STK_Widget *widget;
	SDL_Rect rect;
	STK_Window *win;
	
	// here, check the validation of x and y
	win = STK_WindowGetTop();
	if (!win) 
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	STK_Label *label = (STK_Label *)STK_Malloc(sizeof(STK_Label));
	widget = (STK_Widget *)label;
	STK_WidgetInitInstance(widget);
	widget->bgcolor.r = 0x77;
	widget->bgcolor.g = 0x99;
	widget->bgcolor.b = 0x11;
	
	widget->name	= "Label";
	widget->type	= STK_WIDGET_LABEL;
	// can't get focus
	widget->flags	= 0;			
	
	rect.x	= x;
	rect.y 	= y;
	rect.w	= STK_LABEL_DEFAULT_WIDTH;
	rect.h	= STK_LABEL_DEFAULT_HEIGHT;
	// create surface
	STK_WidgetSetDims(widget, &rect);

	if (str) {
		label->caption = (char *)STK_Malloc(strlen(str) + 1);
		// after this copy, the last char left in caption is 0
		strcpy(label->caption, str);
	}
	else {
		label->caption = NULL;
	}
	
	label->alignment = STK_LABEL_TOPLEFT;
	label->pattern	= STK_LABEL_NORMAL;
	// set it as extended
	label->fixed = 1;
	
	return (STK_Widget *)label;
}

int STK_LabelRegisterType()
{
	STK_WidgetFuncs *f;
	STK_WidgetRegisterType( "Label", &f );
	f->draw = STK_LabelDraw;
	f->close = STK_LabelClose;
	
	return 0;
}


void STK_LabelDraw(STK_Widget *widget)
{
	SDL_Rect rect;
	STK_Label *label = (STK_Label *)widget;
	Uint32 tmpcolor;
	
	// if label is extended, to adapter to the string
	if (!label->fixed) {
		// backup widget's rect
		STK_BaseRectCopy(&rect, &widget->rect);
		// force rect to get eventual label->caption width and height 
		STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &rect, label->caption);
		
		// if areas don't equal, need to reset dimensions of this 
		//   label: free previous surface and alloc a new one
		if (!STK_BaseRectEqual(&rect, &widget->rect))
			STK_WidgetSetDims(widget, &rect);
	}
	
	// fill background
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(widget->surface, NULL, tmpcolor);
	
	if (label->caption) {
		SDL_Color bg = { 0, 0xff, 0xff, 0 };
		// set the background of label being transparent, magic num is for test only
		Uint32 colorkey_label_bg = SDL_MapRGB(widget->surface->format, bg.r, bg.g, bg.b);
		SDL_SetColorKey(widget->surface, SDL_SRCCOLORKEY, colorkey_label_bg);
		
		// calculate the rect area of font surface: 
		// results are placed in rect as to alignment.
		STK_LabelCalculatePattern(label, &rect);
		
		// draw text using widget's fgcolor and (local background only for test colorkey)
		STK_FontDraw( STK_FontGetDefaultFontWithSize(), label->caption, widget, &rect, &widget->fgcolor, &bg );
	}
	
	// till now, the surface of label has been filled, but shall we blit it to window surface?
	
}

int STK_LabelClose(STK_Widget *widget)
{
	STK_Label *label = (STK_Label *)widget;
	
	// now ready to free surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}
	// now ready to free string
	if (label->caption) {
		free(label->caption);
		label->caption = NULL;
	}
	// now ready to free label node
	free(label);
	
	return 0;
}

/*
int STK_LabelSetColor(STK_Widget *widget, int which, Uint32 color)
{
	if (label == NULL) 
		return 1;
		
	switch (which) {
		case STK_LABEL_COLOR_FOREGROUND:
			widget->fgcolor = color;
			break;
		case STK_LABEL_COLOR_BACKGROUND:
			label->bgcolor = color;
			break;
		default:
			break;
	}
	
	// need to submit a redraw event
	STK_WidgetEventRedraw(widget);

	return 0;
}
*/

static void STK_LabelCalculatePattern(STK_Label *label, SDL_Rect *rect)
{
	STK_Widget *widget = (STK_Widget *)label;
	SDL_Rect dims = {0};
	int dw, dh;
	
	/* Calculate the total size of the string in pixels */
	STK_FontAdapter(STK_FontGetDefaultFontWithSize(), &dims, label->caption);
	dw = widget->rect.w - dims.w;
	dh = widget->rect.h - dims.h;
	// if widget is too small, limit the drawing area in widget->rect
	if (dw <= 0 && dh <= 0) {
		rect->x = 0;
		rect->w = widget->rect.w;
		rect->y = 0;
		rect->h = widget->rect.h;
		return;
	}
	
	switch (label->alignment) {
	case STK_LABEL_TOPLEFT:
		rect->x = 0;
		rect->y = 0;
		rect->w = dims.w;
		rect->h = dims.h;
		break;		
	case STK_LABEL_TOPCENTER:
		rect->x = 0 + (widget->rect.w - dims.w)/2 + 1;
		rect->y = 0;
		rect->w = dims.w;
		rect->h = dims.h;
		break;
	case STK_LABEL_CENTER:
		rect->x = 0 + (widget->rect.w - dims.w)/2;
		rect->y = 0 + (widget->rect.h - dims.h)/2 + 1;
		rect->w = dims.w;
		rect->h = dims.h;
		break;
	case STK_LABEL_CENTERLEFT:
		rect->x = 0;
		rect->y = 0 + (widget->rect.h - dims.h)/2 + 1;
		rect->w = dims.w;
		rect->h = dims.h;
		break;			
	}
}

int STK_LabelSetAlignment(STK_Widget *widget, int alignment)
{
	STK_Label *label = (STK_Label *)widget;
	label->alignment = alignment;
	
	return 0;
}

int STK_LabelSetText(STK_Widget *widget, char * text)
{
	STK_Label *label = (STK_Label *)widget;
	
	if (text == NULL)
		return 1;
	
	if (label->caption) {
		free(label->caption);
		label->caption = NULL;
	}
	// because the c string have a zero char in the end, malloc text size + 1 
	label->caption = (char *)STK_Malloc(strlen(text) + 1);
	// after this copy, the last char left in caption is 0
	strcpy(label->caption, text);
	
	STK_WidgetEventRedraw(widget);
			
	return 0;
}

char *STK_LabelGetText(STK_Widget *widget)
{
	STK_Label *label = (STK_Label *)widget;
		
	return label->caption;
}

