#include "XVFB_HANDLER.h"

bool XVFB_HANDLER::error;

//void copy_crop(XVFB_HANDLER* xvfb_handler) {
//	int64_t offset = xvfb_handler->Scrnwidth*4;
//	int64_t h = xvfb_handler->Height;
//	for (int32_t j = 0; j < h; j++) {
//		int64_t offset1 = j * xvfb_handler->Scrnwidth * 4;
//		int64_t offset2 = j * xvfb_handler->Width * 4;
//
//	 	copy(
//		((unsigned char*)xvfb_handler->Shm_Image->data) + offset1,
//		((unsigned char*)xvfb_handler->Shm_Image->data) + offset1 + offset,
//		xvfb_handler->Cropped_Image + offset2);
//	}
//}
//
//void copy_normal(XVFB_HANDLER* xvfb_handler) {
//	xvfb_handler->Cropped_Image = xvfb_handler->Shm_Image->data;
//}

void Correct_XWD_Header(char* bp, unsigned int n) {
	char c;
	char* ep = bp + n;
	char* sp;

	while (bp < ep) {
		sp = bp + 3;
		c = *sp;
		*sp = *bp;
		*bp++ = c;
		sp = bp + 1;
		c = *sp;
		*sp = *bp;
		*bp++ = c;
		bp += 2;
	}
}

bool Initialize_XVFB_Handler(XVFB_HANDLER* xvfb_handler, int w, int h) {
	xvfb_handler->Width      = w;
	xvfb_handler->Height     = h;
	xvfb_handler->error	= false;
	xvfb_handler->Shm_Image = NULL;
	xvfb_handler->_Display = NULL;

	//system(string(string("DISPLAY=:1 xrandr --newmode $(cvt ") + to_string(xvfb_handler->Width) + " " + to_string(xvfb_handler->Height) + " | grep Modeline | cut -c 10-)").c_str());
	//cout << string(string("DISPLAY=:1 xrandr --newmode $(gtf ") + to_string(xvfb_handler->Width) + " " + to_string(xvfb_handler->Height) + " 60 | grep Modeline | cut -c 12-)").c_str() << endl;
	//DISPLAY=:1 xrandr --newmode $(cvt 1680 1050 | grep Modeline | cut -c 10-)
	
	//system(string(string("DISPLAY=:1 xrandr --addmode default \"") + to_string(xvfb_handler->Width) + "x" + to_string(xvfb_handler->Height) + "_60.00\"").c_str());
	//cout << string(string("DISPLAY=:1 xrandr --addmode default \"") + to_string(xvfb_handler->Width) + "x" + to_string(xvfb_handler->Height) + "_60.00\"").c_str() << endl;
	//DISPLAY=:1 xrandr --addmode default "widthxheight_60.00"

	system(string(string("DISPLAY=:1 xrandr -s \"") + to_string(xvfb_handler->Width) + "x" + to_string(xvfb_handler->Height) + "_60.00\"").c_str());
	//cout << string(string("DISPLAY=:1 xrandr -s \"") + to_string(xvfb_handler->Width) + "x" + to_string(xvfb_handler->Height) + "_60.00\"").c_str() << endl;
	//DISPLAY=:1 xrandr -s "widthxheight_60.00"

	return Init_XVFB_Handler(xvfb_handler);
}

bool Init_XVFB_Handler(XVFB_HANDLER* xvfb_handler) {
	int tries = 10;
	//xvfb_handler->_Display = XOpenDisplay(":1");
	while (!xvfb_handler->_Display && tries > 0) {
		cout << "tries... " << tries << endl;
		xvfb_handler->_Display = XOpenDisplay(":1");
		tries--;
		Sleep_Milli(100);
	}
	//XCloseDisplay(xvfb_handler->_Display);

	if (tries <= 0) {
		return false;
	}

	XSetErrorHandler(Handle_XVFB_Errors);

	/*
	XWDFileHeader header;
	int frame_buffer_file = open("/var/tmp/Xvfb_screen0", O_RDONLY);
	if (frame_buffer_file <= 0) {
		log_err(((const JSON_TYPE){
			{"message", "failed to open fb file (first)"},
			JSON_TYPE_END}));
		return false;
	}
	read(frame_buffer_file, (char*)&header, 100);
	Correct_XWD_Header((char*)&header, 100);
	close(frame_buffer_file);
	frame_buffer_file = open("/var/tmp/Xvfb_screen0", O_RDONLY);
	if (frame_buffer_file <= 0) {
		log_err(((const JSON_TYPE){
			{"message", "failed to open fb file (second)"},
			JSON_TYPE_END}));
		return false;
	}
	int offset =
	100 + header.header_size - 100 + header.ncolors * sizeof(XWDColor);
	xvfb_handler->Frame_Buffer =
	(uint8_t*)mmap(0, width * height * 4 + offset, PROT_READ, MAP_SHARED,
	frame_buffer_file, 0);
	if (xvfb_handler->Frame_Buffer == (void*)-1) {
		log_err(((const JSON_TYPE){
			{"message", "failed to mmap fb file"},
			JSON_TYPE_END}));
		return false;
	}
	xvfb_handler->Frame_Buffer += offset;
	*/

	if (XShmQueryExtension(xvfb_handler->_Display)) {
		int scr = XDefaultScreen(xvfb_handler->_Display);

		xvfb_handler->Shm_Image = XShmCreateImage(xvfb_handler->_Display,
			DefaultVisual(xvfb_handler->_Display, scr),
			DefaultDepth(xvfb_handler->_Display, scr),
			ZPixmap, NULL, &xvfb_handler->Shm_Info, xvfb_handler->Width, xvfb_handler->Height);

		xvfb_handler->Shm_Info.shmid = shmget(IPC_PRIVATE, xvfb_handler->Shm_Image->bytes_per_line * xvfb_handler->Shm_Image->height, IPC_CREAT|0777);
		xvfb_handler->Shm_Info.shmaddr = xvfb_handler->Shm_Image->data = (char*)shmat(xvfb_handler->Shm_Info.shmid, NULL, SHM_RND);
		xvfb_handler->Shm_Info.readOnly = false;

		if (!XShmAttach(xvfb_handler->_Display, &xvfb_handler->Shm_Info)) {
			return false;
		}
	} else {
		return false;
	}

	return true;
}

void Kill_XVFB_XVFB_Handler(XVFB_HANDLER* xvfb_handler) {
	// string pid = system_output(string("ps aux | grep Xvfb | grep :") + to_string(xvfb_handler->Display_ID) + " | grep -v grep | tr -s ' ' | tr ' ' '_' | cut -d'_' -f2");
	// system(string(string("kill -9 ") + pid).c_str());
	// TODO: below statement is better
	// system(string(string("ps aux | grep Xvfb | grep :") + to_string(xvfb_handler->Display_ID) + string(" | awk \'{print $2}\' | xargs kill")).c_str());
}

void Delete_XVFB_Handler(XVFB_HANDLER* xvfb_handler) {
	//(void)xvfb_handler;

	//log_dbg("detaching shm");

	if (xvfb_handler->Shm_Image != NULL) {
		XShmDetach(xvfb_handler->_Display, &xvfb_handler->Shm_Info);
		XDestroyImage(xvfb_handler->Shm_Image);

		shmdt(xvfb_handler->Shm_Info.shmaddr);

		xvfb_handler->Shm_Image = NULL;
	}

	

	//log_dbg("closing display");
	// crashes every time
	// cout << xvfb_handler->_Display << endl;
	//XCloseDisplay(xvfb_handler->_Display);

	//log_dbg("killing xvfb");
	// Kill_XVFB_XVFB_Handler(xvfb_handler);
}

char* Render_XVFB_Handler(XVFB_HANDLER* xvfb_handler) {
	if (XVFB_HANDLER::error) {
		Delete_XVFB_Handler(xvfb_handler);
		Init_XVFB_Handler(xvfb_handler);
		XVFB_HANDLER::error = false;
	}

	XShmGetImage(xvfb_handler->_Display, DefaultRootWindow(xvfb_handler->_Display), xvfb_handler->Shm_Image, 0, 0, AllPlanes);
	//xvfb_handler->copy(xvfb_handler);
	//return xvfb_handler->Cropped_Image;
	return xvfb_handler->Shm_Image->data;
	//return (char*)xvfb_handler->Frame_Buffer;
}

int Handle_XVFB_Errors(Display *display, XErrorEvent *theEvent) {
	(void)display;
	char* arr = new char[256];
	XGetErrorText(theEvent->display, int(theEvent->error_code), arr, 256);
	cout << " - Ignoring Xlib error: error code request code - " << int(theEvent->error_code) << " - " <<  int(theEvent->request_code) << " " << arr << endl;
	delete[] arr;

	XVFB_HANDLER::error = true;

	return 0;
}