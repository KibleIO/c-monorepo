#include "MOUSE.h"

int open_restricted(const char* path, int flags, void* user_data) {
    bool* grab = (bool*)user_data;
    int fd = open(path, flags);
    if (fd < 0)
        cout << "failed to open " << path << endl;
    else if (*grab && ioctl(fd, EVIOCGRAB, (void*) 1) == -1)
        cout << "grab request failed" << endl;
    return fd < 0 ? -1 : fd;
}

void close_restricted(int fd, void* user_data) {
    close(fd);
}

void sighandler(int signal, siginfo_t* siginfo, void* userdata) {
    //stop = 1;
}

void log_handler(struct libinput *li, enum libinput_log_priority priority, const char *format, va_list args) {
	/*static int is_tty = -1;

	if (is_tty == -1)
		is_tty = isatty(STDOUT_FILENO);

	if (is_tty) {
		if (priority >= LIBINPUT_LOG_PRIORITY_ERROR)
			printf(ANSI_RED);
		else if (priority >= LIBINPUT_LOG_PRIORITY_INFO)
			printf(ANSI_HIGHLIGHT);
	}

	vprintf(format, args);

	if (is_tty && priority >= LIBINPUT_LOG_PRIORITY_INFO)
		printf(ANSI_NORMAL);
		*/
	cout << format << endl;
}

int fdddd;

void   Initialize_Mouse(MOUSE* mouse, int minimum_x, int maximum_x, int minimum_y, int maximum_y, float sensitivity, string path) {
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
		cout << "something went wrong opening file " << path << endl;
		return;
	}
	
	if (libevdev_new_from_fd(fd, &dev) != 0) {
		cout << "failed to initialize libevdev" << endl;
		return;
	}
	
	mouse->mouse_info.vendor  = libevdev_get_id_vendor(dev);
	mouse->mouse_info.product = libevdev_get_id_product(dev);
	mouse->mouse_info.version = libevdev_get_id_version(dev);
	mouse->mouse_info.busid   = libevdev_get_id_bustype(dev);
	
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
				mouse->mouse_info.abs_info[aux].minimum    = info->minimum;
				mouse->mouse_info.abs_info[aux].maximum    = info->maximum;
				mouse->mouse_info.abs_info[aux].fuzz       = info->fuzz;
				mouse->mouse_info.abs_info[aux].flat       = info->flat;
				mouse->mouse_info.abs_info[aux].resolution = info->resolution;
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
            cout << "something went wrong!" << endl;
            return;
    }
    
    //libinput_log_set_handler(mouse->li, log_handler);
	//libinput_log_set_priority(mouse->li, LIBINPUT_LOG_PRIORITY_DEBUG);
    
	mouse->device = libinput_path_add_device(mouse->li, path.c_str());
    if (!mouse->device) {
            cout << "couldn't load device from libinput! " << path.c_str() << endl;
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
        cout << "failed to set up signal handling" << endl;
        return;
    }
    
    /*
    cout << path.c_str() << endl;
	
	
	int counter = 10;
	
	do {
		cout << "sleeping..." << endl;
		sleep(1);
		fdddd = open(path.c_str(), O_RDONLY);
		cout << "log(-1) failed: " << strerror(errno) << endl;
		counter--;
		cout << fdddd << " " << counter << endl;
	} while (fdddd == -1 && counter != 0);
    
    if (counter == 0) {
    	return;
    }
    */
    mouse->Listening      = true;
    
    /*
    cout << "starting to listen..." << endl;
    char* buffer = new char[10000];
    while (mouse->Listening) {
    	if (read(fdddd, buffer, sizeof(input_event)) != -1) {
    		cout << "yo we got something" << endl;
		} //else {
		//	cout << "read wrong" << endl;
		//}
    	/*
	    if (poll(&mouse->fds, 1, 1) > 0) {
	    	cout << "okay got something" << endl;
	    	libinput_event* Event;
	        libinput_dispatch(mouse->li);
	        while ((Event = libinput_get_event(mouse->li))) {
	        	//MOUSE_EVENT_ELEMENT* event_element = new MOUSE_EVENT_ELEMENT();
	        	//event_element->Event = Event;
	            //mouse->Mouse_Event_Stack.Add(event_element);
	            
	            cout << "yooooo an event" << endl;
	            
	            libinput_event_destroy(Event);
	            libinput_dispatch(mouse->li);
	        }
	    }
	}
	*/
    
    mouse->Event_Listener = new thread(Listen_Mouse, mouse);
}

MOUSE* Construct_Mouse (int minimum_x, int maximum_x, int minimum_y, int maximum_y, float sensitivity, string path)              {
	MOUSE* mouse = new MOUSE();
	Initialize_Mouse(mouse, minimum_x, maximum_x, minimum_y, maximum_y, sensitivity, path);
	return mouse;
}

void   Delete_Mouse    (MOUSE* mouse)                                                                               {
	//delete mouse;
}

void         Listen_Mouse      (MOUSE* mouse)                                                                             {
	
	//char* buffer = new char[10000];
	
	
	while (mouse->Listening) {
		
		/*
		if (read(fdddd, buffer, sizeof(input_event)) != -1) {
    		cout << "yo we got something" << endl;
		}
		*/
		
		
		
	    if (poll(&mouse->fds, 1, 1) > -1) {
	    	libinput_event* Event;
	        libinput_dispatch(mouse->li);
	        while ((Event = libinput_get_event(mouse->li))) {
	        	MOUSE_EVENT_ELEMENT* event_element = new MOUSE_EVENT_ELEMENT();
	        	event_element->Event = Event;
	            mouse->Mouse_Event_Stack.Add(event_element);
	            
	            //libinput_event_destroy(Event);
	            libinput_dispatch(mouse->li);
	        }
	    }
	    
	}
}