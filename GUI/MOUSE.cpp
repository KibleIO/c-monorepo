#include "MOUSE.h"
//#include "nuklear.h"

int MOUSE::Current_X;
int MOUSE::Current_Y;

int open_restricted(const char* path, int flags, void* user_data) {
    bool* grab = (bool*)user_data;
    int fd = open(path, flags);
    if (fd < 0) {
        log_err("Failed to open " + string(path));
	}
    else if (*grab && ioctl(fd, EVIOCGRAB, (void*) 1) == -1) {
        log_err("grab request failed");
	}
    return fd < 0 ? -1 : fd;
}

void close_restricted(int fd, void* user_data) {
    close(fd);
}

void sighandler(int signal, siginfo_t* siginfo, void* userdata) {
    //stop = 1;
}

void   Initialize_Mouse(MOUSE* mouse, int minimum_x, int maximum_x, int minimum_y, int maximum_y, float sensitivity, string path, EVENT* event_status) {
	log_dbg("Initializing mouse " + path);
	int fd;
	libevdev *dev;

	mouse->Minimum_X   = minimum_x;
	mouse->Maximum_X   = maximum_x;
	mouse->Minimum_Y   = minimum_y;
	mouse->Maximum_Y   = maximum_y;
	mouse->Current_X   = (minimum_x + maximum_x) / 2;
	mouse->Current_Y   = (minimum_y + maximum_y) / 2;
	mouse->Sensitivity = sensitivity;

	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0) {
		log_err("Something went wrong opening file " + path);
		return;
	}

	if (libevdev_new_from_fd(fd, &dev) != 0) {
		log_err("Failed to initialize libevdev");
		return;
	}

	strcpy(mouse->mouse_info.name, libevdev_get_name(dev));
	mouse->mouse_info.vendor  = libevdev_get_id_vendor(dev);
	mouse->mouse_info.product = libevdev_get_id_product(dev);
	mouse->mouse_info.version = libevdev_get_id_version(dev);
	mouse->mouse_info.busid   = libevdev_get_id_bustype(dev);

	for (int aux = INPUT_PROP_POINTER; aux < INPUT_PROP_MAX; aux++) {
		mouse->mouse_info.properties[aux] = libevdev_has_property(dev, aux);
	}

	if (libevdev_has_event_type(dev, EV_KEY)) {
		for (int aux = KEY_RESERVED; aux < KEY_MAX; aux++) {
			mouse->mouse_info.key[aux] = libevdev_has_event_code(dev, EV_KEY, aux);
		}
	} else {
		mouse->mouse_info.key[0] = -1;
	}

	if (libevdev_has_event_type(dev, EV_REL)) {
		for (int aux = REL_X; aux < REL_MAX; aux++) {
			mouse->mouse_info.rel[aux] = libevdev_has_event_code(dev, EV_REL, aux);
		}
	} else {
		mouse->mouse_info.rel[0] = -1;
	}

	if (libevdev_has_event_type(dev, EV_ABS)) {
		input_absinfo* info;
		for (int aux = ABS_X; aux < ABS_MAX; aux++) {
			mouse->mouse_info.abs[aux] = libevdev_has_event_code(dev, EV_ABS, aux);
			if (mouse->mouse_info.abs[aux]) {
				info = const_cast<input_absinfo*>(libevdev_get_abs_info(dev, aux));
				copy((char*) info, ((char*) info) + sizeof(input_absinfo), (char*)&mouse->mouse_info.abs_info[aux]);
			}
		}
	} else {
		mouse->mouse_info.abs[0] = -1;
	}

	libevdev_free(dev);
	close(fd);

	mouse->interface = {
        .open_restricted = open_restricted,
        .close_restricted = close_restricted,
	};

	mouse->grab = false;
	mouse->li = libinput_path_create_context(&mouse->interface, &mouse->grab);
    if (!mouse->li) {
            log_err("Something went wrong!");
            return;
    }

	mouse->device = libinput_path_add_device(mouse->li, path.c_str());
    if (!mouse->device) {
            log_err("Couldn't load device from libinput! " + path);
            libinput_unref(mouse->li);
            mouse->li = NULL;
            return;
    }
	mouse->fds.fd = libinput_get_fd(mouse->li);
    mouse->fds.events = POLLIN;
    mouse->fds.revents = 0;

    memset(&mouse->act, 0, sizeof(mouse->act));
    mouse->act.sa_sigaction = sighandler;
    mouse->act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGINT, &mouse->act, NULL) == -1) {
        log_err("failed to set up signal handling");
        return;
    }

	mouse->Event_Status = event_status;
    mouse->Listening      = true;
    mouse->Event_Listener = new thread(Listen_Mouse, mouse);
	log_dbg("Mouse " + string(mouse->mouse_info.name) + " initialized");
}

MOUSE* Construct_Mouse (int minimum_x, int maximum_x, int minimum_y, int maximum_y, float sensitivity, string path, EVENT* event_status)              {
	MOUSE* mouse = new MOUSE();
	Initialize_Mouse(mouse, minimum_x, maximum_x, minimum_y, maximum_y, sensitivity, path, event_status);
	return mouse;
}

void   Delete_Mouse    (MOUSE* mouse)                                                                               {
	//delete mouse;
	log_dbg("Deleting mouse " + string(mouse->mouse_info.name));
}

void         Listen_Mouse      (MOUSE* mouse)                                                                             {
	log_dbg("Listening to mouse " + string(mouse->mouse_info.name));
	while (mouse->Listening) {
	    if (poll(&mouse->fds, 1, 1) > 0) {
	    	libinput_event* Event;
	        libinput_dispatch(mouse->li);
	        while ((Event = libinput_get_event(mouse->li))) {
	        	MOUSE_EVENT_ELEMENT* event_element = new MOUSE_EVENT_ELEMENT();
	        	event_element->Event = Event;
	            mouse->Mouse_Event_Stack.Add(event_element);
				Set_Event(mouse->Event_Status);

	            libinput_dispatch(mouse->li);
	        }
	    }

	}
}

//void Handle_Mouse_NK(nk_context* context, MOUSE** mouse, int len) {
//	if (!context) {
//		log_err("Context is null");
//		return;
//	}
//
//	if (!mouse) {
//		log_err("Mouse list is null");
//		return;
//	}
//
//	nk_input_begin (context);
//
//	for (int k = 0; k < len; k++) {
//		if (mouse[k] != NULL) {
//			for (int i = mouse[k]->Mouse_Event_Stack.getLength(); i > 0; i--) {
//				libinput_event_pointer* lep;
//				MOUSE_EVENT_ELEMENT* element = (MOUSE_EVENT_ELEMENT*)mouse[k]->Mouse_Event_Stack.GetElement(0);
//				switch(libinput_event_get_type(element->Event)) {
//					case LIBINPUT_EVENT_POINTER_MOTION:
//	                    lep = libinput_event_get_pointer_event(element->Event);
//						mouse[k]->Current_X += libinput_event_pointer_get_dx_unaccelerated(lep) * mouse[k]->Sensitivity;
//						if(mouse[k]->Current_X > mouse[k]->Maximum_X) {
//							mouse[k]->Current_X = mouse[k]->Maximum_X;
//						} else if (mouse[k]->Current_X < mouse[k]->Minimum_X) {
//							mouse[k]->Current_X = mouse[k]->Minimum_X;
//						}
//						mouse[k]->Current_Y += libinput_event_pointer_get_dy_unaccelerated(lep) * mouse[k]->Sensitivity;
//						if (mouse[k]->Current_Y > mouse[k]->Maximum_Y) {
//							mouse[k]->Current_Y = mouse[k]->Maximum_Y;
//						} else if(mouse[k]->Current_Y < mouse[k]->Minimum_Y) {
//							mouse[k]->Current_Y = mouse[k]->Minimum_Y;
//						}
//	                    break;
//	                case LIBINPUT_EVENT_POINTER_BUTTON:
//	                	lep = libinput_event_get_pointer_event(element->Event);
//	                	if (libinput_event_pointer_get_button(lep) == BTN_LEFT) {
//							log_dbg("Left click registered");
//	                		nk_input_button(context, NK_BUTTON_LEFT, mouse[k]->Current_X, mouse[k]->Current_Y, libinput_event_pointer_get_button_state(lep));
//							log_dbg(to_string(mouse[k]->Current_X) + " " + to_string(mouse[k]->Current_Y));
//	                	}
//	                	break;
//					default:
//						//Write_Notice(string("Unexpected mouse type ") + to_string(libinput_event_get_type(element->Event)));
//						break;
//				}
//				libinput_event_destroy(element->Event);
//				mouse[k]->Mouse_Event_Stack.Remove(element);
//				delete element;
//			}
//			nk_input_motion(context, mouse[k]->Current_X, mouse[k]->Current_Y);
//		}
//	}
//	nk_input_end(context);
//}

void Handle_Mouse_X11(int display_ID, MOUSE** mouse, int len) {
	Display* dpy = XOpenDisplay(string(string(":") + to_string(display_ID)).c_str());
	if (!dpy) {
		log_err("Unable to open display :" + to_string(display_ID));
		return;
	}

	for (int k = 0; k < len; k++) {
		if (mouse[k] != NULL) {
			for (int i = mouse[k]->Mouse_Event_Stack.getLength(); i > 0; i--) {
				libinput_event_pointer* lep;
				MOUSE_EVENT_ELEMENT* element = (MOUSE_EVENT_ELEMENT*)mouse[k]->Mouse_Event_Stack.GetElement(0);
				switch(libinput_event_get_type(element->Event)) {
					case LIBINPUT_EVENT_POINTER_MOTION:
	                    lep = libinput_event_get_pointer_event(element->Event);
						mouse[k]->Current_X += libinput_event_pointer_get_dx_unaccelerated(lep) * mouse[k]->Sensitivity;
						if(mouse[k]->Current_X > mouse[k]->Maximum_X) {
							mouse[k]->Current_X = mouse[k]->Maximum_X;
						} else if (mouse[k]->Current_X < mouse[k]->Minimum_X) {
							mouse[k]->Current_X = mouse[k]->Minimum_X;
						}
						mouse[k]->Current_Y += libinput_event_pointer_get_dy_unaccelerated(lep) * mouse[k]->Sensitivity;
						if (mouse[k]->Current_Y > mouse[k]->Maximum_Y) {
							mouse[k]->Current_Y = mouse[k]->Maximum_Y;
						} else if(mouse[k]->Current_Y < mouse[k]->Minimum_Y) {
							mouse[k]->Current_Y = mouse[k]->Minimum_Y;
						}
	                    break;
	                case LIBINPUT_EVENT_POINTER_BUTTON:
	                	lep = libinput_event_get_pointer_event(element->Event);
	                	if (libinput_event_pointer_get_button(lep) == BTN_LEFT) {
							log_dbg("left click registered " + to_string(mouse[k]->Current_X) + " " + to_string(mouse[k]->Current_Y));
	                		XTestFakeButtonEvent(dpy, 1, libinput_event_pointer_get_button_state(lep), CurrentTime);
	                	} else if (libinput_event_pointer_get_button(lep) == BTN_RIGHT) {
							log_dbg("right click registered " + to_string(mouse[k]->Current_X) + " " + to_string(mouse[k]->Current_Y));
	                		XTestFakeButtonEvent(dpy, 3, libinput_event_pointer_get_button_state(lep), CurrentTime);
	                	}
	                	break;
					default:
						//Write_Notice(string("Unexpected mouse type ") + to_string(libinput_event_get_type(element->Event)));
						break;
				}
				libinput_event_destroy(element->Event);
				mouse[k]->Mouse_Event_Stack.Remove(element);
				delete element;
			}
			XTestFakeMotionEvent(dpy, 0, mouse[k]->Current_X, mouse[k]->Current_Y, CurrentTime);
		}
	}

	XFlush(dpy);
	XCloseDisplay(dpy);
}
