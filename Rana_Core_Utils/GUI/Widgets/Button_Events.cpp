#include "Button_Events.h"
using namespace std;

/*
BUTTON_CLICKED_EVENT_DATA
-------------------------
*/

BUTTON_CLICKED_EVENT_DATA* Button_Clicked_Event_Data(void* ui_ptr,
uint8_t owner_id, uint8_t button_id) {
	BUTTON_CLICKED_EVENT_DATA* data = new BUTTON_CLICKED_EVENT_DATA;
	data->ui = ui_ptr;
	data->invokerID = owner_id;
	data->buttonID = button_id;
	return data;
}

void Update_Screen(void* uint8_t_screen_id_ptr,
void* BUTTON_CLICKED_EVENT_DATA_ptr) {
	uint8_t* screen_id = (uint8_t*)uint8_t_screen_id_ptr;
	BUTTON_CLICKED_EVENT_DATA* event_data =
		(BUTTON_CLICKED_EVENT_DATA*)BUTTON_CLICKED_EVENT_DATA_ptr;
	*screen_id = event_data->invokerID + event_data->buttonID;
	cout << "Current screen id: " << (int)(*screen_id) << endl;
}
