#ifndef _STK_LINE_H_
#define _STK_LINE_H_

#include "stk_widget.h"
enum STK_LinePattern {
        STK_LINE_SINGLELINE = 0,
        STK_LINE_DOUBLELINE,
        STK_LINE_CHAINLINE
};

typedef struct STK_Line {
	STK_Widget widget;
	Uint8 pattern;
	
} STK_Line;

STK_Line* STK_LineNew(Uint16 x, Uint16 y, Uint16 width, Uint16 height);
int STK_LineClose(STK_Widget *widget);
void STK_LineDraw(STK_Widget *widget);
int STK_LineRegisterType();
#endif /* _STK_LINE_H_ */
