//PLATFORMS: Linux, Windows, OSX (TODO)

#include "DEVICE_MANAGER.h"

Queue<KEYBOARD_EVENT_T*> DEVICE_MANAGER::Keyboard_Events;
Queue<MOUSE_EVENT_T*> DEVICE_MANAGER::Mouse_Events;

void Initialize_Device_Manager(
	DEVICE_MANAGER* dev_man, int w, int h, EVENT* event_status) {

	// Linux specific code {{{
	#ifdef __linux__
	//local device list
	dev_man->current_dev = new DEVICE_NODE*[MAX_DEV];
	dev_man->c_d_size = 0;

	//initializing device lists
	dev_man->previous_dev = new DEVICE_NODE*[MAX_DEV];
	dev_man->p_d_size = 0;

	for (int i = 0; i < MAX_DEV; i++) {
		dev_man->current_dev[i] = new DEVICE_NODE();
		dev_man->current_dev[i]->hw.mouse = NULL;
		dev_man->current_dev[i]->hw.keyboard = NULL;
	}
	for (int i = 0; i < MAX_DEV; i++) {
		dev_man->previous_dev[i] = new DEVICE_NODE();
		dev_man->previous_dev[i]->hw.mouse = NULL;
		dev_man->previous_dev[i]->hw.keyboard = NULL;
	}
	dev_man->sending = false;
	#endif
	// }}} Windows specific code {{{
	#ifdef _WIN64
	#endif
	// }}} OSX specific code {{{
	#ifdef __APPLE__
	//TODO apple code
	#endif
	// }}}

	dev_man->w = w;
	dev_man->h = h;

	dev_man->client = NULL;
	dev_man->server = NULL;
	dev_man->Event_Status = event_status;

	MOUSE::Current_X = 0;
	MOUSE::Current_Y = 0;

	dev_man->receiving = false;
}

void Device_Server_Start(DEVICE_MANAGER* dev_man) {
	TIMER* tm;
	uint8_t ptype;

	tm = new TIMER;
	Initialize_TIMER(tm);

	dev_man->receiving = true;
	while (dev_man->receiving) {
		if (!dev_man->server) {
			Sleep_Milli_TIMER(tm, 16);
			continue;
		}

		if (!dev_man->server->Receive((char*)&ptype, sizeof(uint8_t))) {
			log_err("could not receive input packet");
			dev_man->receiving = false;
			continue;
		}

		if (ptype == MOUSE_PACKET) {
			MOUSE_EVENT_T* m_event = new MOUSE_EVENT_T;

			if (
			!dev_man->server->Receive((char*)m_event, sizeof(MOUSE_EVENT_T))) {
				log_err("could not receive mouse event");
				dev_man->receiving = false;
				continue;
			}

			if (dev_man->client) {
				if (
				!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
				!dev_man->client->Send((char*)m_event, sizeof(MOUSE_EVENT_T))) {
					log_err("could not forward mouse event");
					dev_man->client = NULL;
				}
				delete m_event;
			} else {
				// Linux specific code {{{
				#ifdef __linux__
				DEVICE_MANAGER::Mouse_Events.push(m_event);
				#endif
				// }}} Windows specific code {{{
				#ifdef _WIN64
				Handle_Mouse_WINAPI(m_event);
				#endif
				// }}} OSX specific code {{{
				#ifdef __APPLE__
				//TODO apple code
				#endif
				// }}}
			}
		} else if (ptype == KEY_PACKET) {
			KEYBOARD_EVENT_T* k_event = new KEYBOARD_EVENT_T;
			if (
			!dev_man->server->Receive((char*)k_event, sizeof(KEYBOARD_EVENT_T))) {
				log_err("could not receive keyboard event");
				dev_man->receiving = false;
				continue;
			}
			if (dev_man->client) {
				if (
				!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
				!dev_man->client->Send((char*)k_event,
				sizeof(KEYBOARD_EVENT_T))) {
					log_err("could not forward keyboard event");
					dev_man->client = NULL;
				}
				delete k_event;
			} else {
				// Linux specific code {{{
				#ifdef __linux__
				DEVICE_MANAGER::Keyboard_Events.push(k_event);
				#endif
				// }}} Windows specific code {{{
				#ifdef _WIN64
				Handle_Keyboard_WINAPI(k_event);
				#endif
				// }}} OSX specific code {{{
				#ifdef __APPLE__
				//TODO apple code
				#endif
				// }}}
			}
		} else {
			log_err("unknown packet type");
		}
	}

	Delete_TIMER(tm);
	delete tm;
}

void Device_Server_Stop(DEVICE_MANAGER* dev_man) {
	dev_man->receiving = false;
}

void Device_Server_Listen(DEVICE_MANAGER* dev_man, Server* server) {
	dev_man->server = server;
}

void Device_Client_Connect(DEVICE_MANAGER* dev_man, Client* client) {
	dev_man->client = client;
}

// Linux specific code {{{
#ifdef __linux__
void Send_Keyboard_Data(DEVICE_NODE* dev, DEVICE_MANAGER* dev_man) {
	uint8_t ptype;
	KEYBOARD* keyboard = dev->hw.keyboard;
	if (!keyboard) {
		log_err("keyboard is null");
		return;
	}
	for (int i = keyboard->Events.size(); i > 0; i--) {
		KEYBOARD_EVENT_T* k_event = NULL;
		keyboard->Events.pop(k_event);
		ptype = KEY_PACKET;
		if (dev_man->client) {
			if (
			!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)k_event, sizeof(KEYBOARD_EVENT_T))) {
				log_err("could not send keyboard event");
				dev_man->client = NULL;
			}
			delete k_event;
		} else {
			DEVICE_MANAGER::Keyboard_Events.push(k_event);
		}
	}
}

void Send_Mouse_Data(DEVICE_NODE* dev, DEVICE_MANAGER* dev_man) {
	MOUSE_EVENT_T* m_event;
	double vert;
	int rel_x, rel_y;
	uint8_t ptype;

	MOUSE* mouse = dev->hw.mouse;
	if (!mouse) {
		return;
	}

	Listen_Mouse_Once(dev->hw.mouse);

	for (int i = mouse->Events.size(); i > 0; i--) {
		libinput_event_pointer* lep;
		MOUSE_EVENT_ELEMENT* element = NULL;
		mouse->Events.pop(element);
		switch (libinput_event_get_type(element->Event)) {
		case LIBINPUT_EVENT_POINTER_MOTION:
			lep = libinput_event_get_pointer_event(element->Event);
			rel_x = libinput_event_pointer_get_dx(lep);
			rel_y = libinput_event_pointer_get_dy(lep);
			mouse->Current_X += rel_x;
			if (mouse->Current_X > mouse->Maximum_X) {
				mouse->Current_X = mouse->Maximum_X;
			} else if (mouse->Current_X < mouse->Minimum_X) {
				mouse->Current_X = mouse->Minimum_X;
			}
			mouse->Current_Y += rel_y;
			if (mouse->Current_Y > mouse->Maximum_Y) {
				mouse->Current_Y = mouse->Maximum_Y;
			} else if (mouse->Current_Y < mouse->Minimum_Y) {
				mouse->Current_Y = mouse->Minimum_Y;
			}
			m_event = new MOUSE_EVENT_T;
			/*
			m_event->x = rel_x;
			m_event->y = rel_y;
			m_event->state = MOUSE_REL_COORD;
			*/
			m_event->x = mouse->Current_X;
			m_event->y = mouse->Current_Y;
			m_event->state = MOUSE_ABS_COORD;
			m_event->clicked = false;
			if (dev_man->client) {
				ptype = MOUSE_PACKET;
				if (
					!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
					!dev_man->client->Send((char*)m_event,
						sizeof(MOUSE_EVENT_T))) {
					log_err("could not sent mouse movement");
					dev_man->client = NULL;
				}
				delete m_event;
			}
			else {
				DEVICE_MANAGER::Mouse_Events.push(m_event);
				}
			break;

		case LIBINPUT_EVENT_POINTER_BUTTON:
			m_event = new MOUSE_EVENT_T;
			lep = libinput_event_get_pointer_event(element->Event);
			if (libinput_event_pointer_get_button(lep) == BTN_LEFT) {
				log_dbg("left mouse clicked " +
					to_string(mouse->Current_X) + " " +
					to_string(mouse->Current_Y));
				m_event->button = MOUSE_BUTTON_LEFT;
			} else if (
				libinput_event_pointer_get_button(lep) == BTN_RIGHT) {
				log_dbg("right mouse clicked " +
					to_string(mouse->Current_X) + " " +
					to_string(mouse->Current_Y));
				m_event->button = MOUSE_BUTTON_RIGHT;
			} else if (
				libinput_event_pointer_get_button(lep) == BTN_MIDDLE) {
				log_dbg("middle mouse clicked " +
					to_string(mouse->Current_X) + " " +
					to_string(mouse->Current_Y));
				m_event->button = MOUSE_BUTTON_MIDDLE;
			}
			m_event->x = mouse->Current_X;
			m_event->y = mouse->Current_Y;
			m_event->clicked = true;
			m_event->state = libinput_event_pointer_get_button_state(lep);
			if (dev_man->client) {
				ptype = MOUSE_PACKET;
				if (
					!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
					!dev_man->client->Send((char*)m_event,
						sizeof(MOUSE_EVENT_T))) {
					log_err("could not send mouse click");
					dev_man->client = NULL;
				}
				delete m_event;
			} else {
				DEVICE_MANAGER::Mouse_Events.push(m_event);
			}
			break;

		case LIBINPUT_EVENT_POINTER_AXIS:
			m_event = new MOUSE_EVENT_T;
			lep = libinput_event_get_pointer_event(element->Event);

			vert = 0;

			if (
			libinput_event_pointer_has_axis(lep,
			LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)) {
				vert = libinput_event_pointer_get_axis_value(
				lep, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
			}

			m_event->x = mouse->Current_X;
			m_event->y = mouse->Current_Y;
			m_event->button = (
			vert > 0) ? MOUSE_BUTTON_SCROLL_DOWN : MOUSE_BUTTON_SCROLL_UP;

			m_event->clicked = true;
			m_event->state = true;
			if (dev_man->client) {
				ptype = MOUSE_PACKET;
				if (
					!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
					!dev_man->client->Send((char*)m_event,
						sizeof(MOUSE_EVENT_T))) {
					log_err("could not send mouse click");
					dev_man->client = NULL;
				}
				delete m_event;
				m_event = new MOUSE_EVENT_T;
				m_event->x = mouse->Current_X;
				m_event->y = mouse->Current_Y;
				m_event->button = (
				vert > 0) ? MOUSE_BUTTON_SCROLL_DOWN : MOUSE_BUTTON_SCROLL_UP;

				m_event->clicked = true;
				m_event->state = false;
				ptype = MOUSE_PACKET;
				if (
					!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
					!dev_man->client->Send((char*)m_event,
						sizeof(MOUSE_EVENT_T))) {
					log_err("could not send mouse click");
					dev_man->client = NULL;
				}
				delete m_event;
			} else {
				DEVICE_MANAGER::Mouse_Events.push(m_event);
			}
			break;
		default:
			log_err("unknown mouse event type");
			break;
		}
	}
}

bool Check_Device_Node(DEVICE_NODE* init) {
	return init->okay;
}

void Device_Client_Start(DEVICE_MANAGER* dev_man) {
	dev_man->sending = true;

	while (dev_man->sending) {
		for (int i = 0; i < dev_man->p_d_size; i++) {
			DEVICE_NODE* dev = dev_man->previous_dev[i];
			if (Check_Device_Node(dev)) {
				if (dev->type == _MOUSE) {
					Send_Mouse_Data(dev, dev_man);
				}
				else if (dev->type == _KEYBOARD) {
					Send_Keyboard_Data(dev, dev_man);
				}
			}
		}
	}
}

void Device_Client_Stop(DEVICE_MANAGER* dev_man) {
	dev_man->sending = false;
}

void Device_Client_Close_Connection(DEVICE_MANAGER* dev_man) {
	if (dev_man->client) {
		dev_man->client->CloseConnection();
		delete dev_man->client;
		dev_man->client = NULL;
	}
}

void Device_Server_Close_Connection(DEVICE_MANAGER* dev_man) {
	if (dev_man->server) {
		dev_man->server->CloseConnection();
		delete dev_man->server;
		dev_man->server = NULL;
	}
}

void Initialize_Device_Node(DEVICE_NODE* init, string ss, int t) {
	init->str = ss;
	init->type = t;
	init->okay = true;
}

void Delete_Device_Node(DEVICE_NODE* init) {
	if (!init->okay) {
		return;
	}
	init->okay = false;
	if (init->type == _MOUSE &&
		init->hw.mouse != NULL) {
		Delete_Mouse(init->hw.mouse);
	}
	else if (init->type == _KEYBOARD &&
		init->hw.keyboard != NULL) {
		Delete_Keyboard(init->hw.keyboard);
	}
}

int Get_Bit(unsigned int *bits, unsigned int bit) {
	return (bits[bit / 32] >> (bit % 32)) & 1;
}

void Add_Device_Node(DEVICE_MANAGER* dev_man, DEVICE_NODE* ptr) {
	if (ptr->type == _GARBAGE) {
		for (int i = 0; i < dev_man->p_d_size; i++) {
			//if an empty spot is found
			if (!Check_Device_Node(dev_man->previous_dev[i])) {
				log_dbg(
					"adding garbage " + ptr->str + " " + to_string(ptr->type) +
					" 0");
				Initialize_Device_Node(
					dev_man->previous_dev[i], ptr->str, ptr->type);
				return;
			}
		}
		//or create new device
		if (dev_man->p_d_size < MAX_DEV) {
			log_dbg(
				"adding empty " + ptr->str + " " + to_string(ptr->type) + " 0");
			Initialize_Device_Node(
				dev_man->previous_dev[dev_man->p_d_size++], ptr->str, ptr->type);
		}
		else {
			cout << "Max devices reached" << endl;
			return; //whoa, max devices reached
		}
	}
	else {
		int temp_index;
		for (int i = 0; i < dev_man->p_d_size; i++) {
			//if an empty spot is found
			if (!Check_Device_Node(dev_man->previous_dev[i])) {
				log_dbg(
					"adding device " + ptr->str + " " + to_string(ptr->type));
				Initialize_Device_Node(
					dev_man->previous_dev[i], ptr->str, ptr->type);
				temp_index = i;
				goto label1;
			}
		}

		if (dev_man->p_d_size < MAX_DEV) { //no gaps! create new device
			temp_index = dev_man->p_d_size;
			log_dbg("adding empty " + ptr->str + " " + to_string(ptr->type));
			Initialize_Device_Node(
				dev_man->previous_dev[dev_man->p_d_size++], ptr->str, ptr->type);
		}
		else {
			cout << "Max devices reached" << endl;
			return; //whoa, max devices reached
		}

	label1:

		if (ptr->type == _MOUSE) {
			log_dbg("constructing mouse");
			dev_man->previous_dev[temp_index]->hw.mouse =
				Construct_Mouse(0, dev_man->w, 0, dev_man->h, 0.5, ptr->str,
					dev_man->Event_Status);
		}
		else {
			log_dbg("constructing keyboard");
			dev_man->previous_dev[temp_index]->hw.keyboard =
				Construct_Keyboard(ptr->str, dev_man->Event_Status);
		}
	}
}

DEVICE_NODE* Exists_Device_Node(DEVICE_MANAGER* dev_man, string ptr) {
	for (int i = 0; i < dev_man->p_d_size; i++) {
		if (dev_man->previous_dev[i]->str == ptr) {
			return dev_man->previous_dev[i];
		}
	}
	return NULL;
}

void Refresh_Devices(DEVICE_MANAGER* dev_man) {
	bool found;

	if ((dev_man->dp = opendir(INPUT_PATH)) == NULL) {
		log_err("Error opening input directory " + to_string(errno));
		return;
	}

	while ((dev_man->dirp = readdir(dev_man->dp)) != NULL) {
		sprintf(dev_man->path, "%s/%s", INPUT_PATH, dev_man->dirp->d_name);
		lstat(dev_man->path, &dev_man->buffer);
		if (!S_ISDIR(dev_man->buffer.st_mode)) {
			DEVICE_NODE* temp = Exists_Device_Node(dev_man, dev_man->path);
			if (temp == NULL) {
				if ((dev_man->fd = open(dev_man->path, O_RDONLY)) < 0) {
					log_err("could not open " + string(dev_man->path));
					continue;
				}
				memset(dev_man->types, 0, sizeof(dev_man->types));
				ioctl(dev_man->fd, EVIOCGBIT(0, EV_MAX), dev_man->types);

				if (Get_Bit(dev_man->types, EV_KEY)) {
					memset(dev_man->events, 0, sizeof(dev_man->events));
					ioctl(
						dev_man->fd, EVIOCGBIT(EV_KEY, KEY_MAX), dev_man->events);
					if (Get_Bit(dev_man->events, KEY_B)) {
						Initialize_Device_Node(
							dev_man->current_dev[dev_man->c_d_size++],
							string(dev_man->path), _KEYBOARD);
					}
					else if (Get_Bit(dev_man->events, BTN_LEFT)) {
						Initialize_Device_Node(
							dev_man->current_dev[dev_man->c_d_size++],
							string(dev_man->path), _MOUSE);
					}
					else {
						Initialize_Device_Node(
							dev_man->current_dev[dev_man->c_d_size++],
							string(dev_man->path), _GARBAGE);
					}
				}
				else {
					Initialize_Device_Node(
						dev_man->current_dev[dev_man->c_d_size++],
						string(dev_man->path), _GARBAGE);
				}
				close(dev_man->fd);
			}
			else {
				Initialize_Device_Node(
					dev_man->current_dev[dev_man->c_d_size++],
					string(dev_man->path), temp->type);
			}
		}
	}
	closedir(dev_man->dp);

	for (int i = 0; i < dev_man->p_d_size; i++) { //for all history elements
		if (Check_Device_Node(dev_man->previous_dev[i])) {
			found = false;
			int k;
			for (k = 0; k < dev_man->c_d_size; k++) { //search for duplicates
				if (
					Check_Device_Node(dev_man->current_dev[k]) &&
					dev_man->current_dev[k]->str == dev_man->previous_dev[i]->str) {
					found = true;
					break;
				}
			}
			if (found) { //if there was a duplicate, element is okay
				Delete_Device_Node(dev_man->current_dev[k]);
			}
			else { //otherwise the device was unplugged
				Delete_Device_Node(dev_man->previous_dev[i]);
			}
		}
	}
	for (int i = 0; i < dev_man->c_d_size; i++) { //for all not touched...
		if (Check_Device_Node(dev_man->current_dev[i])) {
			Add_Device_Node(dev_man, dev_man->current_dev[i]); //add new!
		}
	}

	dev_man->c_d_size = 0;
}

void Delete_Device_Manager(DEVICE_MANAGER* dev_man) {
	log_dbg("deleting device manager");
	log_tmp("stop server");
	Device_Server_Stop(dev_man);
	log_tmp("stop client");
	Device_Client_Stop(dev_man);

	for (int i = 0; i < MAX_DEV; i++) {
		log_tmp(to_string(i));
		Delete_Device_Node(dev_man->current_dev[i]);
		delete dev_man->current_dev[i];
		Delete_Device_Node(dev_man->previous_dev[i]);
		delete dev_man->previous_dev[i];
	}
	log_dbg("cd");
	delete [] dev_man->current_dev;
	log_dbg("pd");
	delete [] dev_man->previous_dev;
	log_dbg("done deleting device manager");
}

#endif
// }}} Windows specific code {{{
#ifdef _WIN64
void Send_Keyboard_Data(DEVICE_MANAGER* dev_man, KEYBOARD_EVENT_T* k_event) {
	uint8_t ptype;

	dev_man->client_mtx.lock();
	if (dev_man->client) {
		ptype = KEY_PACKET;
		if (
			!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)k_event, sizeof(KEYBOARD_EVENT_T))) {
			log_err("could not send keyboard data");
			dev_man->client = NULL;
		}
		delete k_event;
	}
	else {
		DEVICE_MANAGER::Keyboard_Events.push(k_event);
	}
	dev_man->client_mtx.unlock();
}

void Send_Mouse_Data(DEVICE_MANAGER* dev_man, MOUSE_EVENT_T* m_event) {
	uint8_t ptype;

	MOUSE::Current_X = m_event->x;
	MOUSE::Current_Y = m_event->y;

	dev_man->client_mtx.lock();
	if (dev_man->client) {
		ptype = MOUSE_PACKET;
		if (
			!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
			!dev_man->client->Send((char*)m_event, sizeof(MOUSE_EVENT_T))) {
			log_err("could not send mouse data");
			dev_man->client = NULL;
		}
		delete m_event;
	}
	else {
		DEVICE_MANAGER::Mouse_Events.push(m_event);
	}
	dev_man->client_mtx.unlock();
}
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}
