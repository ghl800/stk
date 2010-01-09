#ifndef _STK_TEXT_H_
#define _STK_TEXT_H_

#include "SDL.h"
#define STK_TextGetStr(x) ((x)->data)

typedef struct STK_Text {
	char *data;
	Uint32 length;		// the length of string
	Uint32 size;		// the size of the string buffer
	Uint32 charcount;	// the char numbers (may containing ASCII char and Chinese character)
} STK_Text;

typedef struct STK_TextPara {
	STK_Text text;
	int linenum;
	
	struct STK_TextPara *next;
	struct STK_TextPara *prev;
} STK_TextPara;

////////////////////////////////
#define STK_TEXTNODE(x)				((STK_TextNode *)(x))
#define STK_TEXTLIST(x)				((STK_TextList *)(x))

#define STK_TextNodeGetText(x)		((x)->text)
#define STK_TextNodeGetNext(x)		((x)->next)
#define STK_TextNodeGetPrev(x)		((x)->prev)

#define STK_TextListGetLength(x)	((x)->length)

typedef struct STK_TextNode {
	STK_Text *text;
	struct STK_TextNode *next;
	struct STK_TextNode *prev;
} STK_TextNode;

typedef struct STK_TextList {
	STK_TextNode *node;
	Uint32 length;
} STK_TextList;

STK_TextList *STK_TextListNew();
#endif /* _STK_FRAME_H_ */
