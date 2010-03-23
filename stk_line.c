#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#include "stk_base.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_color.h"
#include "stk_line.h"
#include "stk_default.h"



STK_Line *STK_LineNew(Uint16 x, Uint16 y, Uint16 width, Uint16 height)
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
	
	STK_Line *line = (STK_Line *)STK_Malloc(sizeof(STK_Line));
	widget = (STK_Widget *)line;
	STK_WidgetInitInstance(widget);

	widget->name	= "Line";
	widget->type	= STK_WIDGET_LINE;
	// can't get focus
	widget->flags	= 0;			
	widget->border 	= 0;
	widget->fixed	= 0;
	
	rect.x	= x;
	rect.y 	= y;
	rect.w	= (width < STK_LINE_DEFAULT_WIDTH? STK_LINE_DEFAULT_WIDTH : width);
	rect.h	= (height < STK_LINE_DEFAULT_HEIGHT? STK_LINE_DEFAULT_HEIGHT : height);
	// create surface
	STK_WidgetSetDims(widget, &rect);
        STK_WidgetSetColor(widget, STK_COLOR_BACKGROUND, STK_COLOR_LINE_BACKGROUND);	
        STK_WidgetSetColor(widget, STK_COLOR_FOREGROUND, STK_COLOR_LINE_FOREGROUND);

	line->pattern	= STK_LINE_SINGLELINE;
	
	return line;
}

int STK_LineRegisterType()
{
	STK_WidgetFuncs *f;
	STK_WidgetRegisterType( "Line", &f );
	f->draw = STK_LineDraw;
	f->close = STK_LineClose;
	
	return 0;
}


void STK_LineDraw(STK_Widget *widget)
{
	SDL_Rect rect;
	STK_Line *line = (STK_Line *)widget;
	Uint32 tmpcolor;
	
	// first we need clear background before scaling.
	tmpcolor = STK_COLOR2INT(widget->surface, widget->bgcolor);
	SDL_FillRect(widget->surface, NULL, tmpcolor);

	
	// if label is extended, to adapter to the string

//	tmpcolor = STK_COLOR2INT(widget->surface, widget->bgcolor);
//	SDL_FillRect(widget->surface, NULL, tmpcolor);
#if 1
        switch (line->pattern) {
        case STK_LINE_SINGLELINE:
                tmpcolor = STK_COLOR2INT(widget->surface, widget->fgcolor);
                SDL_FillRect(widget->surface, NULL, tmpcolor);  
                
                break;
        case STK_LINE_DOUBLELINE:
                //STK_BaseRectCopy(widget->rect, rect);
                                 
                break;
        }
#endif
	// till now, the surface of label has been filled, but shall we blit it to window surface?
	
}

int STK_LineClose(STK_Widget *widget)
{
	STK_Line *line = (STK_Line *)widget;
	int i;
	
	// now ready to free surface
	if (widget->surface) {
		SDL_FreeSurface(widget->surface);
	}

	// now ready to free line node
	free(line);
	
	return 0;
}
int STK_LineSetPattern(STK_Line *line,Uint8 pattern)
{
        if (!line)
                return -1;

	line->pattern = pattern;

        STK_WidgetEventRedraw((STK_Widget *)line);
        return 0;
}
