#include "DEVICE.h"

void    Initialize_Device(DEVICE* device, char* device_path){
	device->Path = device_path;
	device->File = 0;
	device->File = open(device->Path, O_RDONLY);
	if(device->File == -1) {
		device->Listening      = false;
		device->Event_Listener = NULL;
		Write_Error(string("NEW:: Failed to open device '") + device->Path + "' - " + strerror(errno));
	} else {
		device->Listening      = true;
		device->Event_Listener = new thread(Listen_Device, device);
	}
}
DEVICE* Construct_Device (char* device_path)                {
	DEVICE* device = new DEVICE();
	Initialize_Device(device, device_path);
	return device;
}
void    Delete_Device    (DEVICE* device)                   {
	device->Listening = false;
	if(device->Event_Listener != NULL){
		device->Event_Listener->join();
		delete device->Event_Listener;
	}
	close(device->File);
	delete device;
}
void    Listen_Device    (DEVICE* device)                   {
	while (device->Listening) {
		EVENT_ELEMENT* event_element = new EVENT_ELEMENT();
		if(read(device->File, &event_element->Event, sizeof(input_event)) != -1 &&
		   event_element->Event.type != EV_SYN                                  &&
		   device->Listening) {
			device->Event_Stack.Add(event_element);
		} else {
			delete event_element;
		}
	}
}