#include <stdio.h>
#include <stdlib.h>

#include "stk_mm.h"
#include "stk_text.h"

// 64 bytes
#define MEM_BLOCK_UNIT	64

STK_Text *STK_TextNew(char *str)
{
	STK_Text *text;
	int c;
	char *p;
	
	text = (STK_Text *)STK_Malloc(sizeof(STK_Text));
	if (str) {
		text->length = strlen(str);
		
		c = text->length / MEM_BLOCK_UNIT;
		text->size = (c + 1) * MEM_BLOCK_UNIT;
		
		p = (char *)STK_Malloc(text->size);
		strcpy(p, str);
		text->data = p;
	}
	
	return text;
}

int STK_TextFree(STK_Text *text)
{
	if (text) {
		if (text->data)
			free(text->data);
		free(text);
	}
	
	return 0;
}

int STK_TextSetText(STK_Text *text, char *str)
{
	int len;
	char *p;
	
	if (!text || !str ) {
		fprintf(stderr, "STK_TextSetText: text or str is NULL.\n");
		return -1;
	}
	len = strlen(str);

	while (len > text->size) {
		// expand to twice of the original memory block size
		text->size = text->size * 2;	
	}
	
	p = (char *)STK_Malloc(text->size);
	// free old data buffer
	free(text->data);
	text->data = p;
	text->length = len;

	return 0;	
}

Uint32 STK_TextGetLength(STK_Text *text)
{
	if (!text)
		return -1;
	
	return text->length;
}

STK_Text *STK_TextSubText(STK_Text *text, int start, Uint32 count)
{
	STK_Text *subtext;
	char *p, *pn;
	
	if (!text)
		return -1;
	if (start >= text->length)
		return -1;
		
	p = (char *)text->data + start;
	pn = (char *)STK_Malloc(count+1);
	// here, it will judge whether count is too big internally
	strncpy(pn, p, count); 
	pn[count] = '\0';
	
	// new memory block
	subtext = STK_TextNew(pn);
	free(pn);

	return subtext;
}

int STK_TextAppendStr(STK_Text *text, char *str)
{
	// Insert string into the end of the text.
	STK_TextInsertStr(text, str, text->length);
	
	return 0;
}

int STK_TextInsertStr(STK_Text *text, char *str, int pos)
{
	char *p, *pn;
	int len;
	int newlen;
	
	if (!text || !str ) {
		fprintf(stderr, "STK_TextInsertStr: text or str is NULL.\n");
		return -1;
	}
	
	if (pos > text->length) {
		fprintf(stderr, "STK_TextInsertStr: pos is too large.\n");
		return -1;
	}
	
	len = strlen(str);
	newlen = text->length + len;
	// need to alloc new memory block
	if (newlen >= text->size) {
		while (newlen >= text->size) {
			text->size = text->size * 2;
		}
		
		// insert buffer
		pn = (char *)STK_Malloc(newlen + 1);
		strncpy(pn, text->data, pos);
		strncpy(pn + pos, str, len);
		strcpy(pn + pos + len, text->data + pos);
		// free old data buffer
		free(text->data);
		// assign new data buffer
		text->data = pn;
		text->length = newlen;
	}
	// not neccessory to alloc new memory block
	else {
		int i;
		// move data after specified position
		for (i = text->length; i >= pos; i--) {
			text->data[i + 1] = text->data[i];
		}
		strncpy(text->data + pos, str, len);

		text->length = newlen;
	}
	return 0;
}

int STK_TextRemoveStr(STK_Text *text, int count)
{
	if (text->length == 0)
		return -1;
		
	if (count > text->length) {
                fprintf(stderr, "STK_TextDeleteStr: pos or count is too large.\n");
                return -1;
	}
	
	text->length = text->length - count;
	text->data[text->length] = '\0';
	
	return 0;
}

int STK_TextDeleteStr(STK_Text *text, int pos, int count)
{
	if (text->length == 0)
		return -1;
	
	if (pos >= text->length || count > text->length) {
		fprintf(stderr, "STK_TextDeleteStr: pos or count is too large.\n");
		return -1;	
	}
	
	// here, we don't alloc new buffer for effective
	if (pos + count <= text->length)
		strcpy(text->data + pos, text->data + pos + count);
	else {
		text->data[pos] = '\0';
		text->length = pos;
		text->size = text->length;
	}
	
	return 0;
}

char *STK_TextToStr (STK_Text *text)
{
        if (text->length == 0)
                return NULL;
        return text->data;
}

/*
char *STK_TextGetText (STK_Text *text)
{
	if (text->length == 0)
	  return NULL;
	return text->data;
}
*/

char *STK_TextIndex (STK_Text *text, int pos, char *ch)
{
	if (text->length == 0)
	  return NULL;
	
	if (pos > text->length || pos < 0)
	{
		fprintf(stderr, "STK_TextFindChar: pos is too large or too small.\n");
		return NULL;
	}

	return index( text->data + pos, ch);
}

//STK_TextList

STK_TextNode *STK_TextNodeNew(const char *str)
{
	STK_TextNode *tn;
	tn = STK_TEXTNODE (STK_Malloc (sizeof(STK_TextNode)));
	if (!tn)
	  return NULL;

	tn->text = STK_TextNew(str);
	tn->next = tn;
	tn->prev = tn;
	return tn;
}

int STK_TextNodeFree (STK_TextNode *tn)
{
	if (tn){
		if (!STK_TextFree (tn->text))
		  free(tn);
	}
	return 0;
}

STK_TextList *STK_TextListNew()
{
	STK_TextList *tl;
	tl = STK_TEXTLIST (STK_Malloc (sizeof(STK_TextList)));
	
	if (!tl)
		return NULL;
	tl->node = NULL;
	tl->length = 0;
	
	return tl;
}

int STK_TextListAppendStr (STK_TextList *tl, const char *str)
{
	STK_TextNode *tn = STK_TextNodeNew(str);
	
	if (!tn)
	{
		fprintf(stderr, "Cann't create text node.\n");
		return -1;
	}

	if (!tl->node)
	  tl->node = tn;
	else
	{
		/*
		STK_TextNode *p = (tl->node)->prev;
		tn->next = p->next;
		tn->prev = p;
		(p->next)->prev = tn;
		p->next = tn
		*/

		tn->prev = (tl->node)->prev;
		tn->next = tl->node;
		((tl->node)->prev)->next = tn;
		(tl->node)->prev = tn;
	}
	++tl->length;

	return 0;	
}

STK_Text *STK_TextListGetNthText (STK_TextList *tl, int n)
{
	if (!tl->length)
	{
		fprintf (stderr, "STK_TextListGetNthText: The list is NULL.\n");
		return NULL;
	}

	int index,i,flag;
	index = n % tl->length;
	flag = (n >= 0) ? 1 : -1;
	if (abs(index) > (tl->length +1) / 2)
	  index -= tl->length * flag;
	
	flag = (index >= 0) ? 1 : -1;
	STK_TextNode *p = tl->node;
	
	for(i=0; i < abs(index); ++i)
	  if (flag > 0)
		p = p->next;
	  else
		p = p->prev;

	return p->text;
}

char *STK_TextListGetNthStr (STK_TextList *tl, int n)
{
	if (!STK_TextListGetNthText(tl,n))
	  return NULL;

	return STK_TextGetStr(STK_TextListGetNthText(tl,n));
}

int STK_TextListRemoveNth (STK_TextList *tl, int n)
{
	if (!tl->length)
	{
		fprintf(stderr, "STK_TextListRomoveNth: the list is NULL.\n");
		return -1;
	}

	int index,i;
	index = n % tl->length;
	if (n > tl->length/2)
	  n -= tl->length;
	
	STK_TextNode *node = tl->node;
	for (i=0; i<abs(index); ++i)
	{
		if (index > 0)
			node = node->next;
		else
			node = node->prev;
	}
	
	
	if (node == tl->node)
	  tl->node = node->next;

	(node->next)->prev = node->prev;
	(node->prev)->next = node->next;
	
	--tl->length;
	
	if (!tl->length)
	  tl->node = NULL;

	STK_TextNodeFree(node);

/*
	if (!STK_TextNodeFree(node))
	{
		fprintf (stderr, "STK_TextListRemoveNth: Don't Free Text Node.\n");
		return -1;
	}
*/
	node = NULL;
	return 0;
}

int STK_TextListRemove (STK_TextList *tl)
{
	return STK_TextListRemoveNth (tl, -1);
}

int STK_TextListFree (STK_TextList *tl)
{
	if (!tl)
	  return 0;

	while (tl->length)
		STK_TextListRemove (tl);
	
	return 0;	
}

