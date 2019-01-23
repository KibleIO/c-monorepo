#include <Utilities/DEVICE_MANAGER.h>

Queue<KEYBOARD_EVENT*> DEVICE_MANAGER::Keyboard_Events;
Queue<MOUSE_EVENT*> DEVICE_MANAGER::Mouse_Events;

mutex s_lock;
mutex c_lock;

void Device_Server_Start(DEVICE_MANAGER* dev_man) {
	Timer tm;
	uint8_t ptype;
	dev_man->receiving = true;
	while (dev_man->receiving) {
		s_lock.lock();
		tm.sleepMilli(16);
		if (!dev_man->server) {
			s_lock.unlock();
			continue;
		} 
		
		s_lock.unlock();
		s_lock.lock();
		if (!dev_man->server->Receive((char*)&ptype, sizeof(uint8_t))) {
			log_err("could not receive input packet");
		}
		s_lock.unlock();
		if (ptype == MOUSE_PACKET) {
			MOUSE_EVENT* m_event = new MOUSE_EVENT;
			s_lock.lock();
			if (!dev_man->server->Receive((char*)m_event, sizeof(MOUSE_EVENT))) {
				log_err("could not receive mouse event");
			}
			s_lock.unlock();
			DEVICE_MANAGER::Mouse_Events.push(m_event);
			c_lock.lock();
			if (dev_man->client) {
				if (!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
					!dev_man->client->Send((char*)m_event, sizeof(MOUSE_EVENT))) {
					log_err("could not forward mouse event");
				}
			}
			c_lock.unlock();
		} else if (ptype == KEY_PACKET) {
			KEYBOARD_EVENT* k_event = new KEYBOARD_EVENT;
			s_lock.lock();
			if (!dev_man->server->Receive((char*)k_event, sizeof(KEYBOARD_EVENT))) {
				log_err("could not receive keyboard event");
			}
			s_lock.unlock();
			DEVICE_MANAGER::Keyboard_Events.push(k_event);
			c_lock.lock();
			if (dev_man->client) {
				if (!dev_man->client->Send((char*)&ptype, sizeof(uint8_t)) ||
					!dev_man->client->Send((char*)k_event, sizeof(KEYBOARD_EVENT))) {
					log_err("could not forward keyboard event");
				}
			}
			c_lock.unlock();
		} else {
			log_err("unknown packet type");
		}
	}
}

void Device_Server_Stop(DEVICE_MANAGER* dev_man) {
	dev_man->receiving = false;
	Device_Client_Close_Connection(dev_man);
	Device_Server_Close_Connection(dev_man);
}

void Send_Keyboard_Data(DEVICE_NODE* dev, Client* client) {
	uint8_t ptype;
	KEYBOARD* keyboard = dev->hw.keyboard;
	if (!keyboard) {
		log_err("keyboard is null");
		return;
	}
	for (int i = keyboard->Events.size(); i > 0; i--) {
		KEYBOARD_EVENT* k_event; 
		keyboard->Events.pop(k_event);
		ptype = KEY_PACKET;
		c_lock.lock();
		if (client) {
			if (!client->Send((char*)&ptype, sizeof(uint8_t)) ||
				!client->Send((char*)k_event, sizeof(KEYBOARD_EVENT))) {
				log_err("could not send keyboard event");
			}
		}
		c_lock.unlock();
		DEVICE_MANAGER::Keyboard_Events.push(k_event);
	}
}

void Send_Mouse_Data(DEVICE_NODE* dev, Client* client) {
	MOUSE_EVENT* m_event;
	uint8_t ptype;

	MOUSE* mouse = dev->hw.mouse;
	if (!mouse) {
		log_err("mouse is null");
		return;
	}

	for (int i = mouse->Events.size(); i > 0; i--) {
		libinput_event_pointer* lep;
		MOUSE_EVENT_ELEMENT* element; 
		mouse->Events.pop(element);
		switch (libinput_event_get_type(element->Event)) {
			case LIBINPUT_EVENT_POINTER_MOTION:
	            lep = libinput_event_get_pointer_event(element->Event);
				mouse->Current_X += 
					libinput_event_pointer_get_dx_unaccelerated(lep) * 
					mouse->Sensitivity;
				if (mouse->Current_X > mouse->Maximum_X) {
					mouse->Current_X = mouse->Maximum_X;

				} else if (mouse->Current_X < mouse->Minimum_X) {
					mouse->Current_X = mouse->Minimum_X;
				}
				mouse->Current_Y +=
					libinput_event_pointer_get_dy_unaccelerated(lep) *
					mouse->Sensitivity;
				if (mouse->Current_Y > mouse->Maximum_Y) {
					mouse->Current_Y = mouse->Maximum_Y;
				} else if(mouse->Current_Y < mouse->Minimum_Y) {
					mouse->Current_Y = mouse->Minimum_Y;
				}
				m_event = new MOUSE_EVENT;
				m_event->x = mouse->Current_X;
				m_event->y = mouse->Current_Y;
				m_event->clicked = false;
				m_event->state = 0;
				c_lock.lock();
				if (client) {
					ptype = MOUSE_PACKET;
					if (!client->Send((char*)&ptype, sizeof(uint8_t)) || 
						!client->Send((char*)m_event, sizeof(MOUSE_EVENT))) {
						log_err("could not sent mouse movement");
					}
				}
				c_lock.unlock();
				DEVICE_MANAGER::Mouse_Events.push(m_event);
	            break;

	         case LIBINPUT_EVENT_POINTER_BUTTON:
	         	lep = libinput_event_get_pointer_event(element->Event);
	        	if (libinput_event_pointer_get_button(lep) == BTN_LEFT) {
					log_dbg("left mouse clicked " +
						to_string(mouse->Current_X) + " " +
						to_string(mouse->Current_Y));
	        	}
				m_event = new MOUSE_EVENT;
				m_event->x = mouse->Current_X;
				m_event->y = mouse->Current_Y;
				m_event->clicked = true;
				m_event->state = libinput_event_pointer_get_button_state(lep);
				c_lock.lock();
				if (client) {
					ptype = MOUSE_PACKET;
					if (!client->Send((char*)&ptype, sizeof(uint8_t)) ||
						!client->Send((char*)m_event, sizeof(MOUSE_EVENT))) {
						log_err("could not send mouse click");
					}
				}
				c_lock.unlock();
				DEVICE_MANAGER::Mouse_Events.push(m_event);
	        	break;

			default:
				log_err("unknown mouse event type");
				break;
		}
	}
}

bool Check_Device_Node(DEVICE_NODE* init) {
	return       init->okay;
}

void Device_Client_Start(DEVICE_MANAGER* dev_man) {
	Timer tm;

	dev_man->sending = true;

	while (dev_man->sending) {
		tm.sleepMilli(16);
		for (int i = 0; i < dev_man->p_d_size; i++) {
			DEVICE_NODE* dev = dev_man->previous_dev[i];
			if (Check_Device_Node(dev)) {
				if (dev->type == _MOUSE) {
					Send_Mouse_Data(dev, dev_man->client);
				} else if (dev->type == _KEYBOARD) {
					Send_Keyboard_Data(dev, dev_man->client);
				} 
			}
		}
	}
}

void Device_Client_Stop(DEVICE_MANAGER* dev_man) {
	dev_man->sending = false;
	Device_Client_Close_Connection(dev_man);
}

void Device_Server_Listen(DEVICE_MANAGER* dev_man, int port) {
	Server* server = new Server();

	log_dbg("listening on " + to_string(port));
	if (!server->Listen(port)) {
		log_err("failed to listen on " + to_string(port));
		return;
	}
	log_dbg("bound");

	s_lock.lock();
	dev_man->server = server;
	s_lock.unlock();
}

void Device_Server_Close_Connection(DEVICE_MANAGER* dev_man) {
	s_lock.lock();
	if (dev_man->server) {
		dev_man->server->CloseConnection();
		delete dev_man->server;
		dev_man->server = NULL;
	}
	s_lock.unlock();
}

void Device_Client_Connect(DEVICE_MANAGER* dev_man, int port, string ip) {
	Client* client = new Client();
	int counter = 1000;

	log_dbg("connecting to " + ip + ":" + to_string(port));
	while (!client->OpenConnection(port, ip) /*&& counter-- > 0*/);
	if (counter == 0) {
		log_err("could not connect to " + ip + ":" + to_string(port));
		delete client;
		return;
	}
	log_dbg("connected");


	c_lock.lock();
	dev_man->client = client;
	c_lock.unlock();
}

void Device_Client_Close_Connection(DEVICE_MANAGER* dev_man) {
	c_lock.lock();
	if (dev_man->client) {
		dev_man->client->CloseConnection();
		delete dev_man->client;
		dev_man->client = NULL;
	}
	c_lock.unlock();
}

void Initialize_Device_Node(DEVICE_NODE* init, string ss, int t) {
	init->str     = ss;
	init->type    = t;
	init->okay    = true;
}


void Delete_Device_Node(DEVICE_NODE* init) {
	if (!init->okay) {
		return;
	}
	init->okay = false;
	if (init->type == _MOUSE &&
		init->hw.mouse != NULL) {
		Delete_Mouse(init->hw.mouse);
	} else if (init->type == _KEYBOARD &&
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
			if (!Check_Device_Node(dev_man->previous_dev[i])) { //any empty gaps?
				log_dbg("adding garbage " + ptr->str + " " + to_string(ptr->type) + " 0");
				Initialize_Device_Node(dev_man->previous_dev[i], ptr->str, ptr->type);
				return;
			}
		}
		if (dev_man->p_d_size < MAX_DEV) { //no gaps! create new device
			log_dbg("adding empty " + ptr->str + " " + to_string(ptr->type) + " 0");
			Initialize_Device_Node(dev_man->previous_dev[dev_man->p_d_size++], ptr->str, ptr->type);
		} else {
			cout << "Max devices reached" << endl;
			return; //whoa, max devices reached
		}
	} else {
		int temp_index;
		for (int i = 0; i < dev_man->p_d_size; i++) {
			if (!Check_Device_Node(dev_man->previous_dev[i])) { //any empty gaps?
				log_dbg("adding device " + ptr->str + " " + to_string(ptr->type));
				Initialize_Device_Node(dev_man->previous_dev[i], ptr->str, ptr->type);
				temp_index = i;
				goto label1;
			}
		}

		if (dev_man->p_d_size < MAX_DEV) { //no gaps! create new device
			temp_index = dev_man->p_d_size;
			log_dbg("adding empty " + ptr->str + " " + to_string(ptr->type));
			Initialize_Device_Node(dev_man->previous_dev[dev_man->p_d_size++], ptr->str, ptr->type);
		} else {
			cout << "Max devices reached" << endl;
			return; //whoa, max devices reached
		}

		label1:

		if (ptr->type == _MOUSE) { 
			log_dbg("constructing mouse");
			dev_man->previous_dev[temp_index]->hw.mouse = Construct_Mouse(0, dev_man->w, 0, dev_man->h, 0.5, ptr->str, dev_man->Event_Status);
		} else {
			log_dbg("constructing keyboard");
			dev_man->previous_dev[temp_index]->hw.keyboard = Construct_Keyboard(ptr->str, dev_man->Event_Status);
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
        	DEVICE_NODE* temp = Exists_Device_Node(dev_man, dev_man->path); //variable name kevin
			if (temp == NULL) {
				if ((dev_man->fd = open(dev_man->path, O_RDONLY)) < 0) {
					log_err("could not open " + string(dev_man->path));
					continue;
	            } 
			    memset(dev_man->types, 0, sizeof(dev_man->types));
			    ioctl(dev_man->fd, EVIOCGBIT(0, EV_MAX), dev_man->types);

			    if (Get_Bit(dev_man->types, EV_KEY)) {
			        memset(dev_man->events, 0, sizeof(dev_man->events));
			        ioctl(dev_man->fd, EVIOCGBIT(EV_KEY, KEY_MAX), dev_man->events);
			        if (Get_Bit(dev_man->events, KEY_B)) {
			        	Initialize_Device_Node(dev_man->current_dev[dev_man->c_d_size++], string(dev_man->path), _KEYBOARD);
			        } else if (Get_Bit(dev_man->events, BTN_LEFT)) {
			        	Initialize_Device_Node(dev_man->current_dev[dev_man->c_d_size++], string(dev_man->path), _MOUSE);
			        } else {
			        	Initialize_Device_Node(dev_man->current_dev[dev_man->c_d_size++], string(dev_man->path), _GARBAGE);
			        }
			    } else {
			    	Initialize_Device_Node(dev_man->current_dev[dev_man->c_d_size++], string(dev_man->path), _GARBAGE);
			    }
			    close(dev_man->fd);
            } else {
            	Initialize_Device_Node(dev_man->current_dev[dev_man->c_d_size++], string(dev_man->path), temp->type);
			}
        }
	}
    closedir(dev_man->dp);

    for (int i = 0; i < dev_man->p_d_size; i++) { //for all history elements
    	if (Check_Device_Node(dev_man->previous_dev[i])) { //if the device is valid
	    	found = false;
	    	int k;
	    	for (k = 0; k < dev_man->c_d_size; k++) { //search the current array for any duplicates
	    		if (Check_Device_Node(dev_man->current_dev[k]) &&
					dev_man->current_dev[k]->str == dev_man->previous_dev[i]->str) {
	    			found = true;
	    			break;
	    		}
	    	}
	    	if (found) { //if there was a duplicate then the history element is okay
	    		Delete_Device_Node(dev_man->current_dev[k]);
	    	} else { //otherwise the device was unplugged
	    		Delete_Device_Node(dev_man->previous_dev[i]);
	    	}
    	}
    }
    for (int i = 0; i < dev_man->c_d_size; i++) { //for all elements not touched...
    	if (Check_Device_Node(dev_man->current_dev[i])) {
    		Add_Device_Node(dev_man, dev_man->current_dev[i]); //add them into the list as they are new!
    	}
    }

	dev_man->c_d_size = 0;
}

void Initialize_Device_Manager(DEVICE_MANAGER* dev_man, int w, int h, EVENT* event_status) {
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

	dev_man->w = w;
	dev_man->h = h;

	dev_man->client = NULL;
	dev_man->server = NULL;
	dev_man->Event_Status = event_status;

	dev_man->receiving = false;
	dev_man->sending = false;
}

void Delete_Device_Manager(DEVICE_MANAGER* dev_man) {
	Device_Server_Stop(dev_man);
	Device_Client_Stop(dev_man);

	for (int i = 0; i < MAX_DEV; i++) {
		Delete_Device_Node(dev_man->current_dev[i]);
		delete dev_man->current_dev[i];
		Delete_Device_Node(dev_man->previous_dev[i]);
		delete dev_man->previous_dev[i];
	}
	delete dev_man->current_dev;
	delete dev_man->previous_dev;
	delete dev_man;
	dev_man = NULL;
}
