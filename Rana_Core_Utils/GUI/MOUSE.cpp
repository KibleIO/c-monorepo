//PLATFORMS: Linux, Windows, OSX

#include "MOUSE.h"
//#include "nuklear.h"

int MOUSE::Current_X;
int MOUSE::Current_Y;
bool MOUSE::Clicked;

// Linux specific code {{{
#ifdef __linux__
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
	(void)user_data;
    close(fd);
}

void sighandler(int signal, siginfo_t* siginfo, void* userdata) {
	(void)signal;
	(void)siginfo;
	(void)userdata;
}

void Initialize_Mouse(
MOUSE* mouse, int minimum_x, int maximum_x, int minimum_y, int maximum_y,
float sensitivity, string path, EVENT* event_status) {
	log_dbg("Initializing mouse " + path);
	int fd;
	libevdev *dev;

	mouse->Minimum_X   = minimum_x;
	mouse->Maximum_X   = maximum_x;
	mouse->Minimum_Y   = minimum_y;
	mouse->Maximum_Y   = maximum_y;
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
			mouse->mouse_info.key[aux] =
			libevdev_has_event_code(dev, EV_KEY, aux);
		}
	} else {
		mouse->mouse_info.key[0] = -1;
	}

	if (libevdev_has_event_type(dev, EV_REL)) {
		for (int aux = REL_X; aux < REL_MAX; aux++) {
			mouse->mouse_info.rel[aux] =
			libevdev_has_event_code(dev, EV_REL, aux);
		}
	} else {
		mouse->mouse_info.rel[0] = -1;
	}

	if (libevdev_has_event_type(dev, EV_ABS)) {
		input_absinfo* info;
		for (int aux = ABS_X; aux < ABS_MAX; aux++) {
			mouse->mouse_info.abs[aux] =
			libevdev_has_event_code(dev, EV_ABS, aux);
			if (mouse->mouse_info.abs[aux]) {
				info =
				const_cast<input_absinfo*>(libevdev_get_abs_info(dev, aux));
				copy((char*) info, ((char*) info) + sizeof(input_absinfo),
				(char*)&mouse->mouse_info.abs_info[aux]);
			}
		}
	} else {
		mouse->mouse_info.abs[0] = -1;
	}

	libevdev_free(dev);
	close(fd);

	mouse->interface.open_restricted = open_restricted;
	mouse->interface.close_restricted = close_restricted;

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
    //mouse->Event_Listener = new thread(Listen_Mouse, mouse);
	log_dbg("Mouse " + string(mouse->mouse_info.name) + " initialized");
}

MOUSE* Construct_Mouse(
int minimum_x, int maximum_x, int minimum_y, int maximum_y, float sensitivity,
string path, EVENT* event_status) {
	MOUSE* mouse = new MOUSE();

	Initialize_Mouse(
	mouse, minimum_x, maximum_x, minimum_y, maximum_y,
	sensitivity, path, event_status);

	return mouse;
}

void Delete_Mouse(MOUSE* mouse) {
	log_dbg("Deleting mouse " + string(mouse->mouse_info.name));
	mouse->Listening = false;
	if (mouse->Event_Listener) {
		mouse->Event_Listener->join();
		delete mouse->Event_Listener;
	}
	if (mouse->device) {
		libinput_device_unref(mouse->device);
	}
	if (mouse->li) {
		libinput_unref(mouse->li);
	}
	while (mouse->Events.size() > 0) {
		MOUSE_EVENT_ELEMENT* event = NULL;
		mouse->Events.pop(event);
		delete event;
	}
	delete mouse;
	mouse = NULL;
}

void Listen_Mouse_Once(MOUSE* mouse) {
	if (poll(&mouse->fds, 1, 1) > 0) {
		libinput_event* Event;
	    libinput_dispatch(mouse->li);
	    while ((Event = libinput_get_event(mouse->li))) {
	    	MOUSE_EVENT_ELEMENT* event_element = new MOUSE_EVENT_ELEMENT();
	    	event_element->Event = Event;
	        mouse->Events.push(event_element);
			Set_Event(mouse->Event_Status);

	        libinput_dispatch(mouse->li);
	    }
	}
}

void Listen_Mouse(MOUSE* mouse) {
	log_dbg("Listening to mouse " + string(mouse->mouse_info.name));
	while (mouse->Listening) {
	    if (poll(&mouse->fds, 1, 1) > 0) {
	    	libinput_event* Event;
	        libinput_dispatch(mouse->li);
	        while ((Event = libinput_get_event(mouse->li))) {
	        	MOUSE_EVENT_ELEMENT* event_element = new MOUSE_EVENT_ELEMENT();
	        	event_element->Event = Event;
	            mouse->Events.push(event_element);
				Set_Event(mouse->Event_Status);

	            libinput_dispatch(mouse->li);
	        }
	    }
	}
}

void Handle_Mouse_X11(int display_ID, Queue<MOUSE_EVENT_T*>* events) {
	Display* dpy = XOpenDisplay(":1");

	if (!dpy) {
		log_err("Unable to open display :" + to_string(display_ID));
		return;
	}

	for (int i = events->size(); i > 0; i--) {
		MOUSE_EVENT_T* m_event = NULL;
		events->pop(m_event);
		if (m_event->clicked) {
			XTestFakeButtonEvent(
			dpy, m_event->button, m_event->state, CurrentTime);
		} else {
			if (m_event->state == MOUSE_ABS_COORD) {
				XTestFakeMotionEvent(
				dpy, 0, m_event->x, m_event->y, CurrentTime);
			} else {
				XTestFakeRelativeMotionEvent(
				dpy, m_event->x, m_event->y, CurrentTime);
			}
		}
		delete m_event;
	}

	XFlush(dpy);
	XCloseDisplay(dpy);
}
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Handle_Mouse_WINAPI(MOUSE_EVENT_T* m_event) {
	INPUT ip;
	ip.type = INPUT_MOUSE;
	ip.mi.time = 0;
	ip.mi.mouseData = 0;
	ip.mi.dwExtraInfo = 0;
	if (m_event->clicked) {
		switch (m_event->button) {
		case MOUSE_BUTTON_LEFT:
			if (m_event->state) {
				ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			}
			else {
				ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			}
			break;
		case MOUSE_BUTTON_RIGHT:
			if (m_event->state) {
				ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			}
			else {
				ip.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			}
			break;
		case MOUSE_BUTTON_MIDDLE:
			if (m_event->state) {
				ip.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
			}
			else {
				ip.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
			}
			break;
		case MOUSE_BUTTON_SCROLL_UP:
			ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
			ip.mi.mouseData = m_event->state * 45;
			break;
		case MOUSE_BUTTON_SCROLL_DOWN:
			ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
			ip.mi.mouseData = m_event->state * -45;
			break;
		}
		SendInput(1, &ip, sizeof(INPUT));
	} else {
		ip.mi.dx = (m_event->x * 0xFFFF) / (GetSystemMetrics(SM_CXSCREEN) - 1);

		ip.mi.dy = (m_event->y * 0xFFFF) / (GetSystemMetrics(SM_CYSCREEN) - 1);

		ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &ip, sizeof(INPUT));
	}
	delete m_event;
}

void Handle_Mouse_WINAPI(int display_ID, Queue<MOUSE_EVENT_T*>* events) {
	INPUT ip;

	//Set up the INPUT structure
	ip.type = INPUT_MOUSE;
	ip.mi.time = 0;
	ip.mi.mouseData = 0;
	ip.mi.dwExtraInfo = 0;

	for (int i = events->size(); i > 0; i--) {
		MOUSE_EVENT_T* m_event;
		events->pop(m_event);
		if (m_event->clicked) {
			switch (m_event->button) {
			case MOUSE_BUTTON_LEFT:
				if (m_event->state) {
					ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				}
				else {
					ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
				}
				break;
			case MOUSE_BUTTON_RIGHT:
				if (m_event->state) {
					ip.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				}
				else {
					ip.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				}
				break;
			case MOUSE_BUTTON_MIDDLE:
				if (m_event->state) {
					ip.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				}
				else {
					ip.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
				}
				break;
			case MOUSE_BUTTON_SCROLL_UP:
				ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
				ip.mi.mouseData = m_event->state * 45;
				break;
			case MOUSE_BUTTON_SCROLL_DOWN:
				ip.mi.dwFlags = MOUSEEVENTF_WHEEL;
				ip.mi.mouseData = m_event->state * -45;
				break;
			}
			SendInput(1, &ip, sizeof(INPUT));
		} else {
			ip.mi.dx = (
			m_event->x * 0xFFFF) / (GetSystemMetrics(SM_CXSCREEN) - 1);

			ip.mi.dy = (
			m_event->y * 0xFFFF) / (GetSystemMetrics(SM_CYSCREEN) - 1);

			ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			SendInput(1, &ip, sizeof(INPUT));
		}

		delete m_event;
	}
}
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}
