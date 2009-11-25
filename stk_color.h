#ifndef _STK_COLOR_H_
#define _STK_COLOR_H_

#include "SDL.h"

// these data here is prepared for SDL_MapRGB(surface->format, MACRO)
// Global
#define STK_COLOR_GLOBAL_BACKGROUND	0xd4, 0xd4, 0xd4
#define STK_COLOR_GLOBAL_FOREGROUND	0x00, 0x00, 0x00	// red, green, blue
// Label
#define STK_COLOR_LABEL_BACKGROUND	STK_COLOR_GLOBAL_BACKGROUND
#define STK_COLOR_LABEL_FOREGROUND	STK_COLOR_GLOBAL_FOREGROUND
// Button
#define STK_COLOR_BUTTON_BACKGROUND	STK_COLOR_GLOBAL_BACKGROUND
#define STK_COLOR_BUTTON_FOREGROUND	STK_COLOR_GLOBAL_FOREGROUND
// Entry
#define STK_COLOR_ENTRY_BACKGROUND	0xf4, 0xf4, 0xf4
#define STK_COLOR_ENTRY_FOREGROUND	STK_COLOR_GLOBAL_FOREGROUND
// RadioButton
#define STK_COLOR_RADIOBUTTON_BACKGROUND	STK_COLOR_GLOBAL_BACKGROUND
#define STK_COLOR_RADIOBUTTON_FOREGROUND	STK_COLOR_GLOBAL_FOREGROUND
// RadioGroup
#define STK_COLOR_RADIOGROUP_BACKGROUND		STK_COLOR_GLOBAL_BACKGROUND
#define STK_COLOR_RADIOGROUP_FOREGROUND		STK_COLOR_GLOBAL_FOREGROUND
// ProgressBar
#define STK_COLOR_PROGRESSBAR_BACKGROUND	0xf4, 0xf4, 0xf4
#define STK_COLOR_PROGRESSBAR_FOREGROUND	STK_COLOR_GLOBAL_FOREGROUND
#define STK_COLOR_PROGRESSBAR_CONTENT		0x00, 0xd0, 0xd0
// MsgBox
#define STK_COLOR_MSGBOX_BACKGROUND	0xf4, 0xf4, 0xf4
#define STK_COLOR_MSGBOX_FOREGROUND	0x10, 0x10, 0x10


enum STK_ColorType {
	STK_COLOR_BACKGROUND,
	STK_COLOR_FOREGROUND
};


#endif /* _STK_COLOR_H_ */