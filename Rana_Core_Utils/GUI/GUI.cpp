#include "GUI.h"

SCREEN_DIM GUI::screen_dim;
TEXTURE_MANAGER GUI::texture_manager;

void Initialize_GUI(GUI* gui, string font_path) {
	log_dbg("initializing gui");
	gui->nk_backend = new NK_GEN;

	Initialize_NK_GEN(gui->nk_backend);

	GUI::screen_dim			= Get_Screen_Dimensions_NK_GEN(gui->nk_backend);

	uint32_t* font_heights	= new uint32_t[GUI_TOTAL_FONTS] {
		uint32_t(GUI::screen_dim.h * 0.02),
	 	uint32_t(GUI::screen_dim.h * 0.025),
		uint32_t(GUI::screen_dim.h * 0.03)
	};

	Load_Fonts_NK_GEN(
	gui->nk_backend, font_path, font_heights, GUI_TOTAL_FONTS);
	Load_All_Textures_TEXTURE_MANAGER(&GUI::texture_manager);

	Set_Style_Default_GUI(gui);

	delete [] font_heights;
}

void Set_Font(GUI* gui, int font_index) {
	Set_Font_NK_GEN(gui->nk_backend, font_index);
}

struct TEXTURE* Load_Image_GUI(
string filename, uint32_t width, uint32_t height) {
	return Load_Texture_TEXTURE_MANAGER(
	&GUI::texture_manager, filename, width, height);
}

void Delete_GUI(GUI* gui) {
	log_dbg("deleting gui");

	Delete_NK_GEN(gui->nk_backend);
	delete gui->nk_backend;

	Free_All_Textures_TEXTURE_MANAGER(&GUI::texture_manager);

	log_dbg("done deleting gui");
}

void Handle_Input_GUI(
GUI* gui, Queue<MOUSE_EVENT_T*>* m_events, Queue<KEYBOARD_EVENT_T*>* k_events) {
	nk_input_begin(gui->nk_backend->NK_Context);
	// Mouse

	for (int i = m_events->size(); i > 0; i--) {
		MOUSE_EVENT_T* m_event = NULL;
		m_events->pop(m_event);
		if (m_event->clicked) {
	    	nk_input_button(
			gui->nk_backend->NK_Context, NK_BUTTON_LEFT, m_event->x, m_event->y,
			m_event->state);

			log_dbg(
			"mouse clicked at " + to_string(m_event->x) + " " +
			to_string(m_event->y));
		} else {
			if (m_event->state == MOUSE_ABS_COORD) {
				nk_input_motion(
				gui->nk_backend->NK_Context, m_event->x, m_event->y);
			} else {
				MOUSE::Current_X += m_event->x;
				MOUSE::Current_Y += m_event->y;
				if (MOUSE::Current_X > GUI::screen_dim.sw) {
					MOUSE::Current_X = GUI::screen_dim.sw;
				} else if (MOUSE::Current_X < 0) {
					MOUSE::Current_X = 0;
				}
				if (MOUSE::Current_Y > GUI::screen_dim.h) {
					MOUSE::Current_Y = GUI::screen_dim.h;
				} else if (MOUSE::Current_Y < 0) {
					MOUSE::Current_Y = 0;
				}
				nk_input_motion(
				gui->nk_backend->NK_Context, MOUSE::Current_X,
				MOUSE::Current_Y);
			}
		}

		delete m_event;
	}

	// Keyboard
	for (int i = k_events->size(); i > 0; i--) {
		KEYBOARD_EVENT_T* k_event = NULL;
		k_events->pop(k_event);

		switch(k_event->code) {
			case KEY_LEFTSHIFT:
			case KEY_RIGHTSHIFT:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_SHIFT, k_event->value);
				switch(k_event->value){
					case 0: // Key Release
						KEYBOARD::Shift = false;
						break;
					case 1: // Key Press
					case 2: // Auto Repeat
						KEYBOARD::Shift = true;
						break;
					default:
						break;
				}
				break;
			case KEY_CAPSLOCK:
				switch(k_event->value){
					case 0: // Key Release
						KEYBOARD::Caps_Lock = false;
						break;
					case 1: // Key Press
					case 2: // Auto Repeat
						KEYBOARD::Caps_Lock = true;
						break;
					default:
						break;
				}
				break;
			case KEY_BACKSPACE:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_BACKSPACE, k_event->value);
				break;
			case KEY_UP:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_UP, k_event->value);
				break;
			case KEY_DOWN:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_DOWN, k_event->value);
				break;
			case KEY_LEFT:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_LEFT, k_event->value);
				break;
			case KEY_RIGHT:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_RIGHT, k_event->value);
				break;
			case KEY_ENTER:
				nk_input_key(
				gui->nk_backend->NK_Context, NK_KEY_ENTER, k_event->value);
				break;
			default:
				if(k_event->code <= 111){
					switch(k_event->value){
						case 0:
							break;
						case 1: // Key Press
						case 2: // Auto Repeat
							char key_value;
							if(KEYBOARD::Caps_Lock){
								if (KEYBOARD::Shift) {
									key_value = KEYBOARD::Keys[k_event->code];
								} else {
									key_value =
									KEYBOARD::Keys_Shifted[k_event->code];
								}
							} else {
								if (KEYBOARD::Shift) {
									key_value =
									KEYBOARD::Keys_Shifted[k_event->code];
								} else {
									key_value = KEYBOARD::Keys[k_event->code];
								}
							}
							if(key_value > 0){
								nk_input_char(
								gui->nk_backend->NK_Context, key_value);
							}
							break;
						default:
							break;
					}
				}
				break;
		}
		delete k_event;
	}

	nk_input_end(gui->nk_backend->NK_Context);
}

void Render_Nuklear_GUI(GUI* gui) {
	Render_NK_GEN(gui->nk_backend);
}

void Set_Style_Default_GUI(GUI* gui) {
	gui->nk_backend->NK_Context->style.window.fixed_background.type =
	NK_STYLE_ITEM_COLOR;

	gui->nk_backend->NK_Context->style.window.fixed_background.data.color =
	nk_rgba(0x25,0x25,0x25,0xff);

	gui->nk_backend->NK_Context->style.button.normal = nk_style_item_color(
	nk_rgb(0x98,0x98,0x98));

	gui->nk_backend->NK_Context->style.button.hover = nk_style_item_color(
	nk_rgb(0x90,0x90,0x90));

	gui->nk_backend->NK_Context->style.button.active = nk_style_item_color(
	nk_rgb(0x98,0x98,0x98));

	gui->nk_backend->NK_Context->style.button.text_normal = nk_rgb(
	0x25,0x25,0x25);

	gui->nk_backend->NK_Context->style.button.text_hover = nk_rgb(
	0x25,0x25,0x25);

	gui->nk_backend->NK_Context->style.button.text_active = nk_rgb(
	0x25,0x25,0x25);

	gui->nk_backend->NK_Context->style.button.padding = nk_vec2(0,0);
	gui->nk_backend->NK_Context->style.button.border = 0;

	gui->nk_backend->NK_Context->style.window.padding = nk_vec2(0,0);
	gui->nk_backend->NK_Context->style.window.spacing = nk_vec2(0,0);
	gui->nk_backend->NK_Context->style.window.scrollbar_size = nk_vec2(10,0);
	gui->nk_backend->NK_Context->style.window.min_size = nk_vec2(0,0);
	gui->nk_backend->NK_Context->style.window.border = 0;
	gui->nk_backend->NK_Context->style.window.group_border = 0;

	gui->nk_backend->NK_Context->style.window.header.padding = nk_vec2(0,0);
	gui->nk_backend->NK_Context->style.window.header.label_padding = nk_vec2(
	0,0);

	gui->nk_backend->NK_Context->style.window.header.spacing = nk_vec2(0,0);

	gui->nk_backend->NK_Context->style.text.color = nk_rgb(0xc1,0xc0,0xc0);

	gui->nk_backend->NK_Context->style.edit.normal = nk_style_item_color(
	nk_rgb(0xcc,0xcc,0xcb));

	gui->nk_backend->NK_Context->style.edit.hover = nk_style_item_color(
	nk_rgb(0xcc,0xcc,0xcb));

	gui->nk_backend->NK_Context->style.edit.active = nk_style_item_color(
	nk_rgb(0xcc,0xcc,0xcb));

	gui->nk_backend->NK_Context->style.edit.cursor_text_normal = nk_rgb(
	0x0,0x0,0x0);

	gui->nk_backend->NK_Context->style.edit.cursor_text_hover = nk_rgb(
	0x0,0x0,0x0);

	gui->nk_backend->NK_Context->style.edit.text_normal = nk_rgb(
	0x25,0x25,0x25);

	gui->nk_backend->NK_Context->style.edit.text_hover = nk_rgb(0x25,0x25,0x25);
	gui->nk_backend->NK_Context->style.edit.text_active = nk_rgb(
	0x25,0x25,0x25);

	gui->nk_backend->NK_Context->style.edit.selected_normal = nk_rgb(
	0x25,0x25,0x25);

	gui->nk_backend->NK_Context->style.edit.selected_text_normal = nk_rgb(
	0x25,0x25,0x25);
}
