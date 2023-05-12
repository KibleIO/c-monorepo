#ifdef __linux__

#ifndef XVFB_HANDLER_H_
#define XVFB_HANDLER_H_

#include <Utilities/LOGGING.h>
#include <Utilities/TIMER.h>
#include <Utilities/UTILS.h>
#include <X11/XWDFile.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/XTest.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>

#include "../LIMITS.h"

struct XVFB_HANDLER {
	Display* _Display;
	int Width;
	int Height;
	XShmSegmentInfo Shm_Info;
	XImage* Shm_Image;
	char* Cropped_Image;
	uint8_t* Frame_Buffer;
	static bool error;

	void (*copy)(XVFB_HANDLER*);
};

bool Initialize_XVFB_Handler(XVFB_HANDLER*, int, int);
bool Init_XVFB_Handler(XVFB_HANDLER*);
void Delete_XVFB_Handler(XVFB_HANDLER*);
void Kill_XVFB_XVFB_Handler(XVFB_HANDLER*);
int Handle_XVFB_Errors(Display*, XErrorEvent*);
char* Render_XVFB_Handler(XVFB_HANDLER*);

#endif

#endif