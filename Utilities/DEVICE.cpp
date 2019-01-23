#include "DEVICE.h"

void    Initialize_Device(DEVICE* device, char* device_path, EVENT* event_status){
	log_dbg("Initializing device " + string(device_path));
	device->Path = device_path;
	device->File = open(device->Path, O_RDONLY);
	device->Event_Status = event_status;
	if(device->File == -1) {
		log_dbg("Invalid path for device " + string(device_path));
		device->Listening      = false;
		device->Event_Listener = NULL;
	} else {
		log_dbg("Device " + string(device_path) + " initialized");
		device->Listening      = true;
		device->Event_Listener = new thread(Listen_Device, device);
	}
}
DEVICE* Construct_Device (char* device_path, EVENT* event_status)                {
	log_dbg("Constructing device " + string(device_path));
	DEVICE* device = new DEVICE();
	Initialize_Device(device, device_path, event_status);
	return device;
}
void    Delete_Device    (DEVICE* device)                   {
	log_dbg("Device " + string(device->Path) + " deleted");
	device->Listening = false;
	if(device->Event_Listener != NULL){
		device->Event_Listener->join();
		delete device->Event_Listener;
	}
	close(device->File);
	while (device->Event_Stack.size() > 0) {
		EVENT_ELEMENT* event;
		device->Event_Stack.pop(event);
		delete event;
	}
	delete device;
	device = NULL;
}

void    Listen_Device    (DEVICE* device)                   {
	log_dbg("Beginning to listen to device " + string(device->Path));
	while (device->Listening) {
		EVENT_ELEMENT* event_element = new EVENT_ELEMENT();
		if(read(device->File, &event_element->Event, sizeof(input_event)) != -1 &&
		   event_element->Event.type != EV_SYN                                  &&
		   device->Listening) {
			log_dbg("Adding to stack " + string(device->Path));
			device->Event_Stack.push(event_element);
			Set_Event(device->Event_Status);
		} else {
			delete event_element;
		}
	}
	log_dbg("Done listening to device " + string(device->Path));
}
