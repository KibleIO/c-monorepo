#include <Utilities/DEVICE_FINDER.h>

volatile bool DEVICE_FINDER::locked;

void Initialize_Device_Node(DEVICE_NODE* init, string ss, int p, int t) {
	init->str     = ss;
	init->port    = p;
	init->type    = t;
	init->running = true;
	init->okay    = true;
	init->thr = NULL;
}

void Delete_Device_Node(DEVICE_NODE* init) {
	init->okay    = false;
}

//returns true if valid string object
bool Check_Device_Node(DEVICE_NODE* init) {
	return       init->okay;
}

int Get_Bit(unsigned int *bits, unsigned int bit) {
    return (bits[bit / 32] >> (bit % 32)) & 1;
}

void Send_Data(DEVICE_FINDER* dev_finder, DEVICE_NODE* ptr) {
	log_dbg("adding " + ptr->str + " " + to_string(ptr->type) + " " + to_string(ptr->port));
	Client* c = new Client();
	Integer _temp;
	x32_input_event input_event_convert;
	fd_set set;
	timeval timeout;
	int rv;
	char* buffer = new char[sizeof(input_event)];
	int counterr = 1000;
	Timer wait;

	while (!c->OpenConnection(ptr->port, dev_finder->address) && counterr-- > 0);

	if (counterr <= 0) {
		log_err("couldn't connect " + ptr->str + " " + dev_finder->address + ":" + to_string(ptr->port));
		ptr->running = false;
		delete buffer;
		c->CloseConnection();
		delete c;
		return;
	}

	int fd = open(ptr->str.c_str(), O_RDONLY);
	if (fd == -1) {
		log_err("couldn't open file... " + ptr->str);
		ptr->running = false;
		delete buffer;
		c->CloseConnection();
		delete c;
		return;
	}

	log_dbg("sending " + ptr->str + " " + to_string(ptr->type));
	while (ptr->running) {
		while (DEVICE_FINDER::locked) {
			if (!ptr->running) {
				delete buffer;
    			close(fd);
				c->CloseConnection();
				delete c;
				return;
			}
			wait.sleepMilli(16);
		}
		FD_ZERO(&set); /* clear the set */
		FD_SET(fd, &set); /* add our file descriptor to the set */
		timeout.tv_sec = 2; //wait two seconds before quitting
		timeout.tv_usec = 0;
		rv = select(fd + 1, &set, NULL, NULL, &timeout);
		if (rv == -1) {
			break;
		} else if (rv == 0) {
			continue;
		} else {
			if (read(fd, buffer, sizeof(input_event)) != -1) {
				_temp.data = _DATA; //keep sending

				#ifdef __arm__

				input_event_convert.time.tv_sec  = (*((input_event*)buffer)).time.tv_sec;
        input_event_convert.time.tv_usec = (*((input_event*)buffer)).time.tv_usec;
        input_event_convert.type         = (*((input_event*)buffer)).type;
        input_event_convert.code         = (*((input_event*)buffer)).code;
        input_event_convert.value        = (*((input_event*)buffer)).value;

				if (!c->Send(_temp.bytes, 4) ||
					!c->Send((char*)&input_event_convert, sizeof(x32_input_event))) {
					log_err("connection cut... exiting. port: " + to_string(ptr->port));
					delete buffer;
    			close(fd);
    			return;
				}

				#else

				if (!c->Send(_temp.bytes, 4) ||
					!c->Send(buffer, sizeof(input_event))) {
					log_err("connection cut... exiting. port: " + to_string(ptr->port));
					delete buffer;
	    			close(fd);
	    			return;
				}

				#endif

			}
		}
	}
	log_dbg(ptr->str + " done running");

	_temp.data = _STOP; //stop the thread
	c->Send(_temp.bytes, 4);

	delete buffer;
    close(fd);
	c->CloseConnection();
	delete c;

	cout << "ending " << ptr->str << endl;
}

void Add_Device_Node(DEVICE_FINDER* dev_finder, DEVICE_NODE* ptr) {

	int temp_port;
	log_dbg("iris_id: " + to_string(dev_finder->iris_id));
	switch (dev_finder->type_id) {
	case DFID_RANA:
		temp_port = (dev_finder->iris_id * 10000) + PORT_RANA + 1 + dev_finder->index_queue->Capture();
		break;
	case DFID_IRIS:
		temp_port = (dev_finder->iris_id * 10000) + PORT_IRIS + 1 + dev_finder->index_queue->Capture();
		break;
	case DFID_THEMIS:
		temp_port = 1;
		break;
	default:
		temp_port = 0;
		break;
	}


	if (ptr->type == _GARBAGE) {
		for (int i = 0; i < dev_finder->p_d_size; i++) {
			if (!Check_Device_Node(dev_finder->previous_dev[i])) { //any empty gaps?
				log_dbg("adding garbage " + ptr->str + " " + to_string(ptr->type) + " 0");
				Initialize_Device_Node(dev_finder->previous_dev[i], ptr->str, 0, ptr->type);
				return;
			}
		}
		if (dev_finder->p_d_size < MAX_DEV) { //no gaps! create new device
			log_dbg("adding empty " + ptr->str + " " + to_string(ptr->type) + " 0");
			Initialize_Device_Node(dev_finder->previous_dev[dev_finder->p_d_size++], ptr->str, 0, ptr->type);
		} else {
			cout << "Max devices reached" << endl;
			return; //whoa, max devices reached
		}
	} else {
		int temp_index;
		for (int i = 0; i < dev_finder->p_d_size; i++) {
			if (!Check_Device_Node(dev_finder->previous_dev[i])) { //any empty gaps?
				dev_finder->previous_dev[i]->port = 4;

				log_dbg("adding device " + ptr->str + " " + to_string(ptr->type) + " " + to_string(temp_port));
				Initialize_Device_Node(dev_finder->previous_dev[i], ptr->str, temp_port, ptr->type);
				temp_index = i;
				goto label1;
			}
		}

		if (dev_finder->p_d_size < MAX_DEV) { //no gaps! create new device
			temp_index = dev_finder->p_d_size;
			dev_finder->previous_dev[temp_index]->port = 4;
			log_dbg("adding empty " + ptr->str + " " + to_string(ptr->type) + " " + to_string(temp_port));
			Initialize_Device_Node(dev_finder->previous_dev[dev_finder->p_d_size++], ptr->str, temp_port, ptr->type);
		} else {
			cout << "Max devices reached" << endl;
			return; //whoa, max devices reached
		}

		label1:

		if (ptr->type == _MOUSE) { //this is technically wasteful, only half of each array would be used
			dev_finder->mouse[temp_index] = Construct_Mouse(0, dev_finder->w, 0, dev_finder->h, 0.5, ptr->str, dev_finder->Event_Status);
		} else {
			dev_finder->keyboard[temp_index] = Construct_Keyboard(ptr->str, dev_finder->Event_Status);
		}

		if (dev_finder->type_id == DFID_THEMIS){
			return;
		}

		if (dev_finder->client != NULL) {
			Integer send_int;
			//lets let the server know that we've got a new device!
			send_int.data = dev_finder->previous_dev[temp_index]->type; //either keyboard or mouse
			if (!dev_finder->client->Send(send_int.bytes, 4)) {
				cout << "failed to send device type" << endl;
				return;
			}
			send_int.data = dev_finder->previous_dev[temp_index]->port; //port
			if (!dev_finder->client->Send(send_int.bytes, 4)) {
				cout << "failed to send port" << endl;
				return;
			}
			if (dev_finder->previous_dev[temp_index]->type == _MOUSE) {
				if (!dev_finder->client->Send((char*)&dev_finder->mouse[temp_index]->mouse_info, sizeof(Mouse_Info))) {
					cout << "failed to send MouseInfo" << endl;
					return;
				}
			}
			dev_finder->previous_dev[temp_index]->running = true;
			dev_finder->previous_dev[temp_index]->thr = new thread(Send_Data, dev_finder, dev_finder->previous_dev[temp_index]);
		}
	}
}

DEVICE_NODE* Exists_Device_Node(DEVICE_FINDER* dev_finder, string ptr) {
	for (int i = 0; i < dev_finder->p_d_size; i++) {
		if (dev_finder->previous_dev[i]->str == ptr) {
			return dev_finder->previous_dev[i];
		}
	}
	return NULL;
}

void Remove_Device_Node(DEVICE_FINDER* dev_finder, DEVICE_NODE* ptr) {
	log_dbg("removing device: " + ptr->str);
	for (int i = 0; i < dev_finder->p_d_size; i++) {
		if (dev_finder->previous_dev[i] == ptr) {
			if (dev_finder->type_id == DFID_RANA) {
				dev_finder->index_queue->Release(((ptr->port % 10000) - (PORT_RANA + 1)));
				cout << "releaseing port: " << (((ptr->port % 10000) - (PORT_RANA + 1))) << endl;
			} else if (dev_finder->type_id == DFID_IRIS) {
				dev_finder->index_queue->Release(((ptr->port % 10000) - (PORT_IRIS + 1)));
				cout << "releaseing port: " << (((ptr->port % 10000) - (PORT_IRIS + 1))) << endl;
			}
			if (ptr->type == _MOUSE) { //this is technically wasteful, only half of each array would be used
				new thread(&Delete_Mouse, dev_finder->mouse[i]); //ALEC PLEASE PLEASE FIX ME PLEASE
				dev_finder->mouse[i] = NULL;
			} else if (ptr->type == _MOUSE) {
				new thread(&Delete_Keyboard, dev_finder->keyboard[i]); //ALEC PLEASE PLEASE FIX ME PLEASE
				dev_finder->keyboard[i] = NULL;
			}
			Delete_Device_Node(ptr);
			ptr->running = false;
		}
	}
}

void Refresh_Devices(DEVICE_FINDER* dev_finder) {
	bool found;

	if ((dev_finder->dp = opendir(INPUT_PATH)) == NULL) {
        log_err("Error opening input directory " + to_string(errno));
        return;
    }

    while ((dev_finder->dirp = readdir(dev_finder->dp)) != NULL) {
    	sprintf(dev_finder->path, "%s/%s", INPUT_PATH, dev_finder->dirp->d_name);
        lstat(dev_finder->path, &dev_finder->buffer);
        if (!S_ISDIR(dev_finder->buffer.st_mode)) {
        	DEVICE_NODE* temp = Exists_Device_Node(dev_finder, dev_finder->path); //variable name kevin
			if (temp == NULL) {
				if ((dev_finder->fd = open(dev_finder->path, O_RDONLY)) < 0) {
	                //fprintf(stderr, "ERR %d:\n%s\n", errno, strerror(errno));
	            } else {
			        memset(dev_finder->types, 0, sizeof(dev_finder->types));
			        ioctl(dev_finder->fd, EVIOCGBIT(0, EV_MAX), dev_finder->types);

			        if (Get_Bit(dev_finder->types, EV_KEY)) {
			            memset(dev_finder->events, 0, sizeof(dev_finder->events));
			            ioctl(dev_finder->fd, EVIOCGBIT(EV_KEY, KEY_MAX), dev_finder->events);
			            if (Get_Bit(dev_finder->events, KEY_B)) {
			            	Initialize_Device_Node(dev_finder->current_dev[dev_finder->c_d_size++], string(dev_finder->path), 0, _KEYBOARD);
			            } else if (Get_Bit(dev_finder->events, BTN_LEFT)) {
			            	Initialize_Device_Node(dev_finder->current_dev[dev_finder->c_d_size++], string(dev_finder->path), 0, _MOUSE);
			            } else {
			            	Initialize_Device_Node(dev_finder->current_dev[dev_finder->c_d_size++], string(dev_finder->path), 0, _GARBAGE);
			            }
			        } else {
			        	Initialize_Device_Node(dev_finder->current_dev[dev_finder->c_d_size++], string(dev_finder->path), 0, _GARBAGE);
			        }
			        close(dev_finder->fd);
				}
            } else {
            	Initialize_Device_Node(dev_finder->current_dev[dev_finder->c_d_size++], string(dev_finder->path), 0, temp->type);
			}
        }
	}
    closedir(dev_finder->dp);

    for (int i = 0; i < dev_finder->p_d_size; i++) { //for all history elements
    	if (Check_Device_Node(dev_finder->previous_dev[i])) { //if the device is valid
	    	found = false;
	    	int k;
	    	for (k = 0; k < dev_finder->c_d_size; k++) { //search the current array for any duplicates
	    		if (Check_Device_Node(dev_finder->current_dev[k]) && dev_finder->current_dev[k]->str == dev_finder->previous_dev[i]->str) {
	    			found = true;
	    			break;
	    		}
	    	}
	    	if (found) { //if there was a duplicate then the history element is okay
	    		Delete_Device_Node(dev_finder->current_dev[k]);
	    	} else { //otherwise the device was unplugged
	    		Remove_Device_Node(dev_finder, dev_finder->previous_dev[i]);
	    	}
    	}
    }
    for (int i = 0; i < dev_finder->c_d_size; i++) { //for all elements not touched...
    	if (Check_Device_Node(dev_finder->current_dev[i])) {
    		Add_Device_Node(dev_finder, dev_finder->current_dev[i]); //add them into the list as they are new!
    	}
    }

	dev_finder->c_d_size = 0;
}

bool Initialize_NSYNC(DEVICE_FINDER* dev_finder, int iris_id, string ip_address) {
	log_dbg("Initializing NSYNC with iris_id" + to_string(iris_id));
	Client* c_temp = new Client();
	Integer send_int;
	int counter = 5;
	dev_finder->iris_id = iris_id;

	dev_finder->address = ip_address;

	while (true) {
		int temp_port;
		if (dev_finder->type_id == DFID_RANA) {
			temp_port = PORT_RANA + (dev_finder->iris_id * 10000);
		} else if (dev_finder->type_id == DFID_IRIS) {
			temp_port = PORT_IRIS + (dev_finder->iris_id * 10000);
		}
		if (!c_temp->OpenConnection(temp_port, dev_finder->address) && counter-- > 0) {
			log_dbg("attempted " + dev_finder->address + " port " + to_string(temp_port));
		} else {
			break;
		}
		if (counter <= 0) {
			log_err("couldn't connect");
			return false;
		}
		sleep(1);
	}

	log_dbg("Sending devices");
	dev_finder->client = c_temp;
	for (int i = 0; i < dev_finder->p_d_size; i++) {
		if (Check_Device_Node(dev_finder->previous_dev[i]) && dev_finder->previous_dev[i]->type != _GARBAGE) {
			//lets let the server know that we've got a device!
			send_int.data = dev_finder->previous_dev[i]->type; //either keyboard or mouse
			log_dbg("sending device type: " + to_string(dev_finder->previous_dev[i]->type));
			if (!dev_finder->client->Send(send_int.bytes, 4)) {
				log_dbg("failed to send device type: " + to_string(dev_finder->previous_dev[i]->type));
				continue;
			}
			dev_finder->previous_dev[i]->port %= 10000;
			dev_finder->previous_dev[i]->port += (dev_finder->iris_id * 10000);
			send_int.data = dev_finder->previous_dev[i]->port; //port
			log_dbg("sending device port: " + to_string(dev_finder->previous_dev[i]->port));
			if (!dev_finder->client->Send(send_int.bytes, 4)) {
				log_dbg("failed to send device port: " + to_string(dev_finder->previous_dev[i]->port));
				continue;
			}
			if (dev_finder->previous_dev[i]->type == _MOUSE) {
				log_dbg("sending mouse info");
				if (!dev_finder->client->Send((char*)&dev_finder->mouse[i]->mouse_info, sizeof(Mouse_Info))) {
					log_dbg("failed to send mouse info");
					return false;
				}
			}
			dev_finder->previous_dev[i]->running = true;

			dev_finder->previous_dev[i]->thr = new thread(Send_Data, dev_finder, dev_finder->previous_dev[i]);
		}
	}
	return true;
}

void Delete_Device_Finder(DEVICE_FINDER* dev_finder) {
	delete dev_finder->current_dev;
	delete dev_finder->previous_dev;
	delete dev_finder->mouse;
	delete dev_finder->keyboard;
	if (dev_finder->client != NULL) {
		delete dev_finder->client;
	}
	if (dev_finder->index_queue != NULL) {
		delete dev_finder->index_queue;
	}
}

void Delete_NSYNC(DEVICE_FINDER* dev_finder) {
	Integer _temp;
	Client* c_temp = dev_finder->client;
	dev_finder->client = NULL;
	log_dbg("device size " + to_string(dev_finder->p_d_size));
	for (int i = 0; i < dev_finder->p_d_size; i++) {
		log_dbg("device " + to_string(i));
		if (Check_Device_Node(dev_finder->previous_dev[i]) && dev_finder->previous_dev[i]->type != _GARBAGE) {
			log_dbg("accessing device " + to_string(i));
			//stop all running threads
			log_dbg("running = false device  " + to_string(i));
			if (dev_finder->previous_dev[i]->running) {
				dev_finder->previous_dev[i]->running = false;
			}
			log_dbg("joining thread device  " + to_string(i));
			log_dbg("deleting thread device  " + to_string(i));
			if (dev_finder->previous_dev[i]->thr != NULL) {
				log_dbg("joining");
				dev_finder->previous_dev[i]->thr->join();
				log_dbg("deleting");
				delete dev_finder->previous_dev[i]->thr;
				log_dbg("NULLing");
				dev_finder->previous_dev[i]->thr = NULL;
			}
		}
	}
	log_dbg("did we get here");
	_temp.data = _NIL; //stop the thread
	c_temp->Send(_temp.bytes, 4);
	c_temp->CloseConnection();
	delete c_temp;
}

void Initialize_Device_Finder(DEVICE_FINDER* dev_finder, int w, int h, int type_id, EVENT* event_status) {
	dev_finder->type_id = type_id;

	//local device list
	dev_finder->current_dev = new DEVICE_NODE*[MAX_DEV];
	dev_finder->c_d_size = 0;

	//initializing device lists
	dev_finder->previous_dev = new DEVICE_NODE*[MAX_DEV];
	dev_finder->p_d_size = 0;

	dev_finder->previous_dev = new DEVICE_NODE*[MAX_DEV];
	dev_finder->p_d_size = 0;

	for (int i = 0; i < MAX_DEV; i++) dev_finder->current_dev[i] = new DEVICE_NODE();
	for (int i = 0; i < MAX_DEV; i++) dev_finder->previous_dev[i] = new DEVICE_NODE();

	dev_finder->index_queue = NULL;
	if (type_id == DFID_RANA || type_id == DFID_IRIS) {
		dev_finder->index_queue = new INDEX_QUEUE(0, MAX_DEV);
	}

	dev_finder->mouse        = new MOUSE*[MAX_DEV];
	dev_finder->keyboard     = new KEYBOARD*[MAX_DEV];

	for (int i = 0; i < MAX_DEV; i++) {
		dev_finder->mouse[i] = NULL;
		dev_finder->keyboard[i] = NULL;
	}

	dev_finder->w = w;
	dev_finder->h = h;

	dev_finder->client = NULL;
	dev_finder->Event_Status = event_status;

	DEVICE_FINDER::locked = false;
}

int Create_New_Keyboard(string& str2, int port) {
	DIR *dp;
    dirent *dirp;
	int fd, ret;
    uinput_user_dev uidev;
    char sysfs_device_name[16];

    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0) {
        log_err("couldnt open file!!");
        return -1;
    }

    ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
    for (int aux = KEY_ESC; aux <= KEY_RFKILL; aux++) ioctl(fd, UI_SET_KEYBIT, aux);

    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, string(string("vkbd") + to_string(port)).c_str());
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;

    ret = write(fd, &uidev, sizeof(uidev));
	if (ret < 0) {
		log_err("couldnt write to /dev/uinput");
		return -1;
	}
    ret = ioctl(fd, UI_DEV_CREATE);
	if (ret < 0) {
		log_err("couldnt create device");
		return -1;
	}
    ret = ioctl(fd, UI_GET_SYSNAME(sizeof(uidev.name)), uidev.name);
	if (ret < 0) {
		log_err("couldnt get sysname");
		return -1;
	}

    if ((dp = opendir(string(string("/sys/devices/virtual/input/") + string(uidev.name)).c_str())) == NULL) {
        log_err("Error opening input directory " + to_string(errno) + " " + string(uidev.name));
        exit(-1); //unrecoverable error
    }

	while ((dirp = readdir(dp)) != NULL) {
    	if (string(dirp->d_name).find("event") == 0) {
    		break;
    	}
    }
    closedir(dp);

    str2 = string("/dev/input/") + dirp->d_name;
    return fd;
}

int Create_New_Mouse(string& str2, int port, Mouse_Info mouse_info) {
	int fd;
	libevdev *dev;
	libevdev_uinput *uidev;

	dev = libevdev_new();
	libevdev_set_name(dev, mouse_info.name);
	libevdev_set_id_product(dev, mouse_info.product);
	libevdev_set_id_vendor(dev, mouse_info.vendor);
	libevdev_set_id_bustype(dev, mouse_info.busid);
	libevdev_set_id_version(dev, mouse_info.version);

	libevdev_enable_event_type(dev, EV_SYN);

	for (int aux = INPUT_PROP_POINTER; aux < INPUT_PROP_MAX; aux++) {
		if (mouse_info.properties[aux]) {
			libevdev_enable_property(dev, aux);
		}
	}

	if (mouse_info.key[0] != -1) {
		libevdev_enable_event_type(dev, EV_KEY);
    	for (int aux = KEY_RESERVED; aux < KEY_MAX; aux++) {
    		if (mouse_info.key[aux]) {
				libevdev_enable_event_code(dev, EV_KEY, aux, NULL);
    		}
    	}
    }

	if (mouse_info.rel[0] != -1) {
		libevdev_enable_event_type(dev, EV_REL);
    	for (int aux = REL_X; aux < REL_MAX; aux++) {
    		if (mouse_info.rel[aux]) {
				libevdev_enable_event_code(dev, EV_REL, aux, NULL);
    		}
    	}
    }

	if (mouse_info.abs[0] != -1) {
		libevdev_enable_event_type(dev, EV_ABS);
    	for (int aux = ABS_X; aux < ABS_MAX; aux++) {
    		if (mouse_info.abs[aux]) {
				libevdev_enable_event_code(dev, EV_ABS, aux, (void*)&mouse_info.abs_info[aux]);
    		}
    	}
    }

	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0) {
        log_err("couldnt open file!!");
        return -1;
    }

	libevdev_uinput_create_from_device(dev, fd, &uidev);

	str2 = string(libevdev_uinput_get_devnode(uidev));
	return fd;
}

void Receive_Data(int _fd, int port, string path, bool* complete) {
	if (path == "") {
		log_err("cannot listen to blank device");
		return;
	}
	Server* s = new Server();
	Integer _temp;
	int _size;
	char* buffer = new char[sizeof(input_event)];
	int loopss = 100;

	log_dbg("listening on " + to_string(port));

	while (!s->Listen(port)) {
		//cout << "failed to listen on " << port << endl;
	}

	log_dbg("successful bind " + to_string(port));


	int fd = open(path.c_str(), O_RDWR);
	while (fd == -1 && loopss > 0) {
		sleep(1);
		fd = open(path.c_str(), O_RDWR);
		if (fd == -1){
			cout << "couldn't open file..." << path << " " << strerror(errno) << " " << loopss << endl;
		}
		loopss--;
	}
	if (loopss <= 0) {
		cout << "giving up on file..." << path << " " << strerror(errno) << endl;
		delete buffer;
		s->CloseConnection();
		//ioctl(_fd, UI_DEV_DESTROY); ALEC/KEVIN I am 99% sure I need this but for some reason program crashes when this is called. Please advise
		//close(_fd);
		return;
	}

	log_dbg("Receiveing data from " + path);
	if (!s->Receive(_temp.bytes, 4)) _temp.data = _STOP;
	while (!*complete && _temp.data == _DATA) {
		if (!s->Receive(buffer, sizeof(input_event))) break;
		//cout << "received data" << endl;
		//gettimeofday (&       ((input_event*)buffer)[0].time   , NULL);
		write(fd, buffer, sizeof(input_event));
		//cout << "wrote data " << errno << endl;
		if (!s->Receive(_temp.bytes, 4)) break;
	}
	log_dbg("Done receiveing data from " + path);

	delete buffer;

	close (fd);
	s->CloseConnection();
	ioctl(_fd, UI_DEV_DESTROY);
	close(_fd);

	log_dbg("closed out " + path);
	*complete = true;
}

int get_recv_index(Recv_node* recv_nodes) {
	for (int i = 0; i < MAX_DEV; i++) {
		if (recv_nodes[i].complete) {
			if (recv_nodes[i].thr) {
				recv_nodes[i].thr->join();
				delete recv_nodes[i].thr;
				recv_nodes[i].thr = NULL;
			}
			recv_nodes[i].complete = false;
			return i;
		}
	}
	log_err("device recv threads are full");
}

void Device_Thread(int IRIS_ID, int type_id, volatile bool* running) {
	log_dbg("IRIS_ID: " + to_string(IRIS_ID));
	Server* server = new Server();
	Integer int_;
	int temp_id;
	int dev;
	char path[PATH_MAX];
	string str, str2;
	Mouse_Info mouse_info;
	struct timespec wait = {0};
	wait.tv_sec = 0;
	wait.tv_nsec = 100000000;
	Recv_node* recv_nodes = new Recv_node[MAX_DEV];
	int temp_node_index;

	for (int i = 0; i < MAX_DEV; i++) {
		recv_nodes[i].thr = NULL;
		recv_nodes[i].complete = true;
	}

	int temp_port;
	if (type_id == DFID_IRIS) {
		temp_port = PORT_RANA + (IRIS_ID * 10000);
	} else if (type_id == DFID_THEMIS) {
		temp_port = PORT_IRIS + (IRIS_ID * 10000);
	}

	log_dbg("preparing to listen on " +  to_string(temp_port));

	if (!server->Listen(temp_port)) {
		log_err("failed on " + to_string(temp_port));
		return;
	}

	while (true) {
		//keyboard/mouse
		if (!*running) {
			log_dbg("exiting");
			goto exit_loop;
		}
		if (!server->ReceiveNonBlocking(int_.bytes, 4)){
			nanosleep(&wait, (struct timespec *)NULL);
			continue;
		}
		dev = int_.data;
    	switch (dev) {
    		case _KEYBOARD:
    			server->Receive(int_.bytes, 4); //port
				temp_id = Create_New_Keyboard(str, int_.data);
				log_dbg("Received keyboard " + str + " at port " + to_string(int_.data));
				break;
			case _MOUSE:
				server->Receive(int_.bytes, 4); //port
				server->Receive((char*)&mouse_info, sizeof(Mouse_Info)); //port
				temp_id = Create_New_Mouse(str, int_.data, mouse_info);
				log_dbg("Received mouse " + str + " at port " + to_string(int_.data));
				break;
			case _NIL:
				cout << "hey exit already" << endl;
				goto exit_loop;
    	}
    	log_dbg("adding file " + str);

		temp_node_index = get_recv_index(recv_nodes);
		recv_nodes[temp_node_index].thr = new thread(Receive_Data, temp_id, int_.data, str, &recv_nodes[temp_node_index].complete);
	}

	exit_loop:

	cout << "exiting" << endl;

	log_dbg("joining recv threads");
	for (int i = 0; i < MAX_DEV; i++) {
		if (recv_nodes[i].thr) {
			recv_nodes[i].complete = true;
			recv_nodes[i].thr->join();
			delete recv_nodes[i].thr;
		}
	}

	server->CloseConnection();
}
