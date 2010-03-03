#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk_signal.h"
#include "stk_mm.h"

// function declaration
static int STK_SignalConnectToObject(STK_Object *object, char *signal, F_Signal_Callback callback, void *userdata);
static int STK_SignalEmitToObject(STK_Object *object, char *signal, void *signaldata);
static int STK_SignalDisconnectFromObject(STK_Object *object, char *signal);
static int STK_SignalCloseFromObject(STK_Object *object);

// slist_head point to the head of the global signal list.
static SignalListNode *slist_head = NULL;

SignalListNode *STK_SignalGetListHead()
{
	return slist_head;
}

// initial the head pointer
int STK_SignalInit()
{
	slist_head = NULL;
	return 0;
}

// register a new signal into the signal list.
int STK_SignalNew( char *signal )
{
	// if global signal list is blank
	if (slist_head == NULL) {
		slist_head = (SignalListNode *)STK_Malloc(sizeof(SignalListNode));
		slist_head->signal = signal;
		slist_head->type = 0;
		slist_head->call_list = NULL;
		slist_head->next = NULL;
	}
	// if global signal list is not blank
	else {
		SignalListNode *l = STK_SignalGetListHead();
		// walk along the signal list, until the end of it
		while (l->next) {
			// check specified 
			if (!strcmp(l->signal, signal)) {
				fprintf(stderr, "Signal [%s] already exists.\n", signal);
				return 1;
			}
			l = l->next;
		}
		
		// add new signal node to the end of the global signal list
		l->next = (SignalListNode *)STK_Malloc(sizeof(SignalListNode));
		l->next->signal = signal;
		l->next->type = 0;
		l->next->call_list = NULL;
		l->next->next = NULL;
	}
	
	return 0;
}

// Connecting function: connect the 'widget' with 'callback' function on 'signal' 
int STK_SignalConnect(STK_Widget *widget, char *signal, F_Signal_Callback callback, void *userdata)
{
	// force converting STK_Widget type to STK_Object type 
	return STK_SignalConnectToObject((STK_Object *)widget, signal, callback, userdata);
}

// Disconnecting signal: disconnect the 'widget' from 'signal'
int STK_SignalDisconnectFromWidget (STK_Widget *widget, char *signal)
{
	// 
	return STK_SignalDisconnectFromObject((STK_Object *)widget, signal);
}

// closing signal: remove the 'widget' in the all 'signal'
int STK_SignalClose(STK_Widget *widget)
{
	return STK_SignalCloseFromObject((STK_Object *)widget);
}

// Emit signal function: emit the 'signal' to 'widget' to trigger the corresponding callback function
int STK_SignalEmit(STK_Widget *widget, char *signal, void *signaldata)
{
	// force converting STK_Widget type to STK_Object type 
	return STK_SignalEmitToObject((STK_Object *)widget, signal, signaldata);
}


static int STK_SignalConnectToObject(STK_Object *object, char *signal, F_Signal_Callback callback, void *userdata)
{
	// if global signal list is blank
	if (slist_head == NULL) {
		fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
		return -1;	
	}
	// if global signal list is not blank
	else {
		SignalListNode *l = slist_head;
		// repeat to the end of the global signal list
		while (l) {
			// if equal, means find it, then break
			if (!strcmp(l->signal, signal))
			  break;
			
			l = l->next;
		}
		
		if (l == NULL) {
			fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
			return -1;
		}
#if DEBUG	
		printf("1. signal %s, object %x, l->call_list %x\n", signal, object, l->call_list);
#endif	
		// find the signal node in the list
		// if that node's call_list is blank, insert to it as first node
		if (l->call_list == NULL) {
			l->call_list = (CallbackListNode *)STK_Malloc(sizeof(CallbackListNode));
			// object is actually an address
			l->call_list->object = object;
			// fill the callback function
			l->call_list->callback = callback;
			// fill up user defined parameters
			l->call_list->userdata = userdata;
			l->call_list->next = NULL;
			
		}
		else {
			CallbackListNode *cbl = l->call_list;
			// to the end of the callback list
			while (cbl->next)
				cbl = cbl->next;
			// add new node to the end of the callback list
			cbl->next = (CallbackListNode *)STK_Malloc(sizeof(CallbackListNode));
			cbl->next->object = object;
			cbl->next->callback = callback;
			cbl->next->userdata = userdata;
			cbl->next->next = NULL;
			
		}
#if DEBUG
		printf("2. signal %s, object %x, l->call_list %x\n", signal, object, l->call_list);
#endif
		return 0;
	}
}

static int STK_SignalDisconnectFromObject(STK_Object *object, char *signal)
{
	// if global signal list is blank
	if (slist_head == NULL) {
		fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
		return -1;	
	}
	// if global signal list is not blank

	SignalListNode *l = slist_head;
	// repeat to the end of the global signal list
	while (l) {
		// if equal, means find it, then break
		if (!strcmp(l->signal, signal))
		   break;
		l = l->next;
	}
		
	if (l == NULL) {
		fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
		return -1;
	}

	// find the signal node in the list
	CallbackListNode *cbl = l->call_list;
	CallbackListNode *p = cbl;
	
	while(p) {
		if (p->object == object)
		  break;
		cbl = p;
		p = p->next;
	}
		
	if (!p){
		fprintf(stderr, "Signal [%s] doesn't exist.\n", signal);
		return -1;
	}

	if (p == l->call_list)
	{
		l->call_list = NULL;
	}
	else
	{
		cbl->next = p->next;
	}
		
	p->object = NULL;
	p->callback = NULL;
	p->userdata = NULL;
	p->next = NULL;
	free(p);
	return 0;		

}

static int STK_SignalCloseFromObject(STK_Object *object)
{
	// if global signal list is blank
	if (slist_head == NULL) {
		fprintf(stderr, "Signal list doesn't exist.\n");
		return -1;	
	}
	// if global signal list is not blank

	SignalListNode *l = slist_head;
	
	// find the signal node in the list
	CallbackListNode *cbl, *p, *prev;

	// repeat to the end of the global signal list
	while (l) {
		cbl = l->call_list;
		prev = NULL;
		p = NULL;		
		while(cbl) {
			if (cbl->object == object)
			{
                                p = cbl;
                                if (cbl == l->call_list)
                                {
                                        prev = NULL;
                                        l->call_list = cbl->next;
				}
				if (prev)
				        prev->next = p->next;
				cbl = cbl->next;
				p->object = NULL;
				p->callback = NULL;
				p->userdata = NULL;
				p->next = NULL;
				free(p);
                                p = NULL;
			}
			else
			{
			        prev = cbl;
			        cbl = cbl->next;
                        }
		}

		l = l->next;
	}
	
#if DEBUG
	l = slist_head;
	while (l){
		fprintf(stderr, "Signal Close: signal: %s  object %x ", l->signal, object); 
		cbl = l->call_list;
		p = cbl;
		
		while(p) {
			fprintf(stderr, " calllist: %x, calllist->object %x", p, p->object);
			p = p->next;
		}
		fprintf(stderr, "\n");
		l = l->next;
	}
#endif

	return 0;
		
}

static int STK_SignalEmitToObject(STK_Object *object, char *signal, void *signaldata)
{
	SignalListNode *l;
	CallbackListNode *cbl, *selfcbl;
#if DEBUG	
	fprintf(stderr, "signal emit  ");
#endif
//	printf("1. object %d\n", object);
	l = slist_head;
	selfcbl = NULL;
	cbl = NULL;

	while (l) {
	        // first, find the match signal node
//		fprintf(stderr, "signal %s, l->signal %s ", signal, l->signal);
		if (!strcmp(l->signal, signal)) {
			cbl = l->call_list;
			// walk along the callback list node
//			fprintf(stderr, " object %x ", object);
			while (cbl) {
			// check whether object in the list match with the specific object
//			if (cbl && cbl->object)
//				fprintf(stderr, " calllist %x, calllist->object %x", cbl, cbl->object);
//			else
//				fprintf(stderr, " call_list %x\n", object, l->call_list);
					    
			if (object == cbl->object) {
				if (selfcbl == NULL) {
					selfcbl = (CallbackListNode *)STK_Malloc(sizeof(CallbackListNode));
					// object is actually an address
					selfcbl->object = cbl->object;
					// fill the callback function
					selfcbl->callback = cbl->callback;
					// fill up user defined parameters
					selfcbl->userdata = cbl->userdata;
					selfcbl->next = NULL;
				}
				else {
					CallbackListNode *tmp = selfcbl;
					// to the end of the callback list
					while (tmp->next)
						tmp = tmp->next;
					// add new node to the end of the callback list
					tmp->next = (CallbackListNode *)STK_Malloc(sizeof(CallbackListNode));
					tmp->next->object = cbl->object;
					tmp->next->callback = cbl->callback;
					tmp->next->userdata = cbl->userdata;
					tmp->next->next = NULL;
				}		
/*				
				// get the callback function registered before
				F_Signal_Callback callback = cbl->callback;
				// do call it: this is the real execute point
				// two parameters: signaldata, userdata. They can play a important part when program.
				fprintf(stderr, " callback %x\n", cbl->callback);
				callback( object, signaldata, cbl->userdata );
*/
				}
				cbl = cbl->next;
			}
		}
		l = l->next;
	}
	
        //run callback	
#if DEBUG
        fprintf(stderr, " signal %s object %x ", signal, object);
#endif
	cbl = selfcbl;
	while (cbl)
	{
#if DEBUG
	        fprintf(stderr, " cbl %x, cbl->object %x, cbl->callback %x ",cbl, cbl->object, cbl->callback); 
#endif
	        // get the callback function registered before
	        F_Signal_Callback callback = cbl->callback;
	        // do call it: this is the real execute point
	        // two parameters: signaldata, userdata. They can play a important part when program.
//	        fprintf(stderr, " callback %x\n", cbl->callback);
	        callback( object, signaldata, cbl->userdata );
                cbl = cbl->next;	        
	}
#if DEBUG
	fprintf(stderr, "\n");
#endif	
	// remove selfback
        cbl = selfcbl;
	while (selfcbl)
	{
	        cbl = selfcbl;
	        selfcbl = selfcbl->next;
	        
	        cbl->object = NULL;
	        cbl->callback = NULL;
	        cbl->userdata = NULL;
	        cbl->next = NULL;
	        free(cbl);
        }	     
        selfcbl = NULL;
        cbl=NULL;
/*
	fprintf(stderr, "signal emit  ");
	l = slist_head;
	while (l) {
		// first, find the match signal node
		fprintf(stderr, "signal %s, l->signal %s ", signal, l->signal);
		if (!strcmp(l->signal, signal)) {
			// check this node whether have callback list node

			if (l->call_list) {
				cbl = l->call_list;
				// walk along the callback list node
				fprintf(stderr, " object %x ", object);
				while (cbl) {
					// check whether object in the list match with the specific object
					if (cbl && cbl->object)
						fprintf(stderr, " calllist %x, calllist->object %x", cbl, cbl->object);
					else
						fprintf(stderr, " call_list %x\n", object, l->call_list);
					    
					if (object == cbl->object) {
						// get the callback function registered before
						F_Signal_Callback callback = cbl->callback;
						// do call it: this is the real execute point
						// two parameters: signaldata, userdata. They can play a important part when program.
						fprintf(stderr, " callback %x\n", cbl->callback);
						callback( object, signaldata, cbl->userdata );
					}
					cbl = cbl->next;
				}
			}
			else {
				//fprintf(stderr, "No handlers for signal %s.\n", signal);
			}
		}
		l = l->next;
	}
*/
	return 0;
}



