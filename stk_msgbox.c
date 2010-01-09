#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "stk_text.h"
#include "stk_base.h"
#include "stk_image.h"
#include "stk_color.h"
#include "stk_font.h"
#include "stk_widget.h"
#include "stk_window.h"
#include "stk_msgbox.h"
#include "stk_default.h"

int STK_MsgBoxCalcDisplayLineWindow(STK_MsgBox *msgbox, int font_height);
int STK_MsgBoxCalcTextArea(STK_MsgBox *msgbox);


STK_MsgBox *STK_MsgBoxNew(Uint16 x, Uint16 y, Uint16 w, Uint16 h, char *str)
{
	STK_MsgBox *msgbox;
	STK_Widget *widget;
	STK_Window *win;
	SDL_Rect rect;	
	int i = 0;
	int width, height;
	
	win = STK_WindowGetTop();
	if (!win)
		return -1;
	if (x >= win->widget.rect.w || y >= win->widget.rect.h)
		return -1;
	
	msgbox = (STK_MsgBox *)STK_Malloc(sizeof(STK_MsgBox));
	widget = (STK_Widget *)msgbox;
	STK_WidgetInitInstance(widget);
	
	widget->name = "MsgBox";
	widget->type = STK_WIDGET_MSGBOX;
	widget->flags = 0;
	widget->border = STK_MSGBOX_BORDER_THICKNESS;
	widget->fixed = 1;
	STK_BaseColorAssign(&widget->bgcolor, STK_COLOR_MSGBOX_BACKGROUND);
	STK_BaseColorAssign(&widget->fgcolor, STK_COLOR_MSGBOX_FOREGROUND);
	
	msgbox->start_line = 0;
	msgbox->end_line = 0;
	msgbox->interval = 4;
	msgbox->log = 0;
	msgbox->log_file_name = NULL;
	msgbox->font = STK_FontGetDefaultFont(0);

	width = 60;
	height = STK_FontGetHeight(msgbox->font) + 2 * widget->border;
	if (width < w)
		width = w;
	if (height < h)
		height = h;
	STK_BaseRectAssign(&rect, x, y, width, height);	
	STK_WidgetSetDims(widget, &rect);

	STK_BaseRectAssign(	&msgbox->textarea, 
				widget->border, 
				widget->border,
				widget->rect.w - 2 * widget->border,
				widget->rect.h - 2 * widget->border );
	msgbox->cur_x = 0;
	msgbox->cur_y = 0;

	if (str) {
		STK_MsgBoxAddMsg(msgbox, str);
	}	

	return msgbox;
}

void STK_MsgBoxDraw(STK_Widget *widget)
{
	STK_MsgBox *msgbox = (STK_MsgBox *)widget;
	SDL_Rect rect;
	Uint32 tmpcolor;
	Uint32 font_height;
	int i;
		
	// fillup background
	tmpcolor = SDL_MapRGB(widget->surface->format, widget->bgcolor.r, widget->bgcolor.g, widget->bgcolor.b);
	SDL_FillRect(widget->surface, NULL, tmpcolor);
	STK_ImageDrawFrame(widget->surface, STK_IMAGE_FRAME_SINGLELINE);

	font_height = STK_FontGetHeight(msgbox->font);
	STK_MsgBoxCalcDisplayLineWindow(msgbox, font_height);
	
	i = msgbox->start_line;
	while (i != msgbox->end_line) {
		rect.x = msgbox->textarea.x;
		rect.y = msgbox->textarea.y + ((i + STK_MSGBOX_LINEBUF_NUM - msgbox->start_line) % STK_MSGBOX_LINEBUF_NUM) * (font_height + msgbox->interval);
		rect.w = msgbox->textarea.w + widget->border - rect.x;
		rect.h = msgbox->textarea.h + widget->border - rect.y;
		// here, we must ensure that linebuf[i]->data is valid
		STK_FontDraw(	msgbox->font,
				// STK_FontGetDefaultFontWithSize(),
				msgbox->linebuf[i]->data, 
				widget, 
				&rect, 
//				&widget->fgcolor, 
				&msgbox->fgcolor[i],
				&widget->bgcolor );
		i++;
		i %= STK_MSGBOX_LINEBUF_NUM;
	}	
}

void STK_MsgBoxClose(STK_Widget *widget)
{
	STK_MsgBox *msgbox = (STK_MsgBox *)widget;
	int i = 0;
	
	// release log file name
	free(msgbox->log_file_name);
	
	// release the text line buffer
	for (i = 0; i< STK_MSGBOX_LINEBUF_NUM; i++) {
		if (msgbox->linebuf[i]) {
			STK_TextFree(msgbox->linebuf[i]);
		}
	
	}

	// release surface
	if (widget->surface)
		SDL_FreeSurface(widget->surface);

	// release STK_MsgBox
	free(msgbox);	
}

int STK_MsgBoxRegisterType()
{
        STK_WidgetFuncs *f;
        STK_WidgetRegisterType( "MsgBox", &f );
        f->draw = STK_MsgBoxDraw;
        f->close = STK_MsgBoxClose;

        return 0;
}




// now, we only use linear array to store output message, not circle array
int STK_MsgBoxCalcDisplayLineWindow(STK_MsgBox *msgbox, int font_height)
{
	int sum;
	int d = (msgbox->end_line + STK_MSGBOX_LINEBUF_NUM - msgbox->start_line) % STK_MSGBOX_LINEBUF_NUM;
	
	sum = font_height*d + msgbox->interval*(d - 1);
	
	while (sum >= msgbox->textarea.h) {
		sum -= (font_height + msgbox->interval);
		// move the top line up
		msgbox->start_line++;
		msgbox->start_line %= STK_MSGBOX_LINEBUF_NUM; 
	}

	return 0;
}

int STK_MsgBoxAddColorMsg(STK_MsgBox *msgbox, char *str, SDL_Color color)
{
	int i;
	
	if (msgbox->end_line >= STK_MSGBOX_LINEBUF_NUM || str == NULL)
		return -1;
	
	if (str) {
		char *psrc;
		char *p, *saveptr;
		char **p1, **p0;
		int i, n; 
		 
		n = 0;
		psrc = (char *)STK_Malloc(strlen(str) + 1);
		strcpy(psrc, str);
		for (i=0; i<strlen(psrc); i++) {
			if (psrc[i] == '\n')
				n++; 
		}
		
		if (n == 0) {
			msgbox->linebuf[msgbox->end_line] = STK_TextNew(str);
			msgbox->fgcolor[msgbox->end_line] = color;
			STK_MsgBoxLog(msgbox);
			msgbox->end_line++;
			msgbox->end_line %= STK_MSGBOX_LINEBUF_NUM;
			
		}
		// multiple line
		else if (n > 0) {
			int t = 0;
			p0 = (char **)STK_Malloc(sizeof(char *) * (n+1));

			p1 = p0;
			p = strtok_r(psrc, "\n", &saveptr);
			while (p) {
				*p1++ = p;
				t++;
				p = strtok_r(NULL, "\n", &saveptr);
			}
			
			p1 = p0;
			for (i=0; i<t; i++) {
				msgbox->linebuf[msgbox->end_line] = STK_TextNew(p1[i]);
				msgbox->fgcolor[msgbox->end_line] = color;
				STK_MsgBoxLog(msgbox);
				msgbox->end_line++;
				msgbox->end_line %= STK_MSGBOX_LINEBUF_NUM;
			}			
		}
		
		
	}	

	STK_WidgetEventRedraw((STK_Widget *)msgbox);

	return 0;
}

int STK_MsgBoxAddMsg(STK_MsgBox *msgbox, char *str)
{
	return STK_MsgBoxAddColorMsg(msgbox, str, ((STK_Widget *)msgbox)->fgcolor);
}

int STK_MsgBoxLog(STK_MsgBox *msgbox)
{
	FILE *fp;
	if (msgbox->log) {
		// if log flag swith on, we need to record the buffer content to log file
		if (!msgbox->log_file_name)
		{
			fprintf(stderr, "STK_MsgBoxLog: I don't know log file name.");
			exit(-1);
		}

		if ((fp = fopen(msgbox->log_file_name, "a")) == NULL) {
			fprintf(stderr, "STK_MsgBoxLog: Can't open log file: %s\n", msgbox->log_file_name);
			exit(-1);
		}
		
		fprintf(fp, "%s\n", msgbox->linebuf[msgbox->end_line]->data);
		
		fclose(fp);
		SDL_Delay(10);
	}

	return 0;
}

int STK_MsgBoxSetFont(STK_MsgBox *msgbox, STK_Font *font)
{
	if (!msgbox || !font)
		return -1;
		
	msgbox->font = font;
	
	STK_WidgetEventRedraw((STK_Widget *)msgbox);
	return 0;
}

int STK_MsgBoxSetSize(STK_MsgBox *msgbox, Uint32 width, Uint32 height)
{
	STK_Widget *widget = (STK_Widget *)msgbox;
	SDL_Rect r;
	
	// should check the validating of width and height
	
	r.x = widget->rect.x;
	r.y = widget->rect.y;
	r.w = width;
	r.h = height;
	
	STK_WidgetSetDims(widget, &r);	
	STK_WidgetEventRedraw(widget);
	
	return 0;
}

void STK_MsgBoxSetLogFileName(STK_MsgBox *msgbox, char *file_name)
{
	msgbox->log_file_name = (char *)STK_Malloc (sizeof(char)*strlen(file_name));
	strcpy(msgbox->log_file_name, file_name);
	msgbox->log = 1;
}
