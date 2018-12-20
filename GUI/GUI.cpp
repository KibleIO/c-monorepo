#define NK_IMPLEMENTATION

#include "GUI.h"


static float Font_Get_Text_Width(nk_handle handle, float height, const char* text, int length) {
	BAKED_GLYPH* glyphs = (BAKED_GLYPH*)handle.ptr;
	int _x = 0;
	for (int i = 0; i < length; i++) {
		_x += glyphs[int(text[i]) - 32].Advance;
	}
	return _x;
}

void Initialize_GUI_Themis(GUI* gui, int display_id) {
	gui->Display_ID = display_id;
	gui->NK_Context				= NULL;
	gui->Font					= NULL;
	gui->FontNK					= NULL;
	gui->Graphics_Handle_Buffer	= NULL;
	gui->Graphics_Handle		= NULL;
}

void Initialize_GUI(GUI* gui, int width, int height, string font_path, char* frame_buffer) {
	gui->Width				= width;
	gui->Height				= height;
	gui->Frame_Resolution	= width * height;
	gui->NK_Context			= new nk_context;
	gui->Font				= new FONT;
	gui->FontNK				= new nk_user_font;

	if (frame_buffer) {
		gui->Graphics_Handle_Buffer		  = NULL;
		gui->Graphics_Handle              = new GRAPHICS;
		Initialize_GRAPHICS(gui->Graphics_Handle, frame_buffer, gui->Width, gui->Height);
	} else {
		gui->Graphics_Handle_Buffer       = new char[(gui->Frame_Resolution + 2) * 4];
		gui->Graphics_Handle              = new GRAPHICS;
		Initialize_GRAPHICS(gui->Graphics_Handle, gui->Graphics_Handle_Buffer, gui->Width, gui->Height);
	}
	gui->Graphics_Handle->Transparent = true;

	float text_height = int((gui->Height * 3) / 100);
	Initialize_Font(gui->Font, font_path.c_str(), text_height);

	gui->FontNK->userdata.ptr = gui->Font->Baked_glyphs;
	gui->FontNK->height = text_height;
	gui->FontNK->width = Font_Get_Text_Width;
	nk_init_default(gui->NK_Context, gui->FontNK);

	gui->NK_Context->style.window.fixed_background.type = NK_STYLE_ITEM_COLOR;
	gui->NK_Context->style.window.fixed_background.data.color = nk_rgba(0x25,0x25,0x25,0xff);

	gui->NK_Context->style.button.normal = nk_style_item_color(nk_rgb(0x98,0x98,0x98));
	gui->NK_Context->style.button.hover = nk_style_item_color(nk_rgb(0x90,0x90,0x90));
	gui->NK_Context->style.button.active = nk_style_item_color(nk_rgb(0x98,0x98,0x98));
	gui->NK_Context->style.button.text_normal = nk_rgb(0x25,0x25,0x25);
	gui->NK_Context->style.button.text_hover = nk_rgb(0x25,0x25,0x25);
	gui->NK_Context->style.button.text_active = nk_rgb(0x25,0x25,0x25);

	gui->NK_Context->style.button.padding = nk_vec2(0,0);
	gui->NK_Context->style.button.border = 0;

	gui->NK_Context->style.window.padding = nk_vec2(0,0);
	gui->NK_Context->style.window.spacing = nk_vec2(0,0);
	gui->NK_Context->style.window.scrollbar_size = nk_vec2(10,0);
	gui->NK_Context->style.window.min_size = nk_vec2(0,0);
	gui->NK_Context->style.window.border = 0;
	gui->NK_Context->style.window.group_border = 0;

	gui->NK_Context->style.window.header.padding = nk_vec2(0,0);
	gui->NK_Context->style.window.header.label_padding = nk_vec2(0,0);
	gui->NK_Context->style.window.header.spacing = nk_vec2(0,0);

	gui->NK_Context->style.text.color = nk_rgb(0xc1,0xc0,0xc0);

	gui->NK_Context->style.edit.normal = nk_style_item_color(nk_rgb(0xcc,0xcc,0xcb));
	gui->NK_Context->style.edit.hover = nk_style_item_color(nk_rgb(0xcc,0xcc,0xcb));
	gui->NK_Context->style.edit.active = nk_style_item_color(nk_rgb(0xcc,0xcc,0xcb));

	gui->NK_Context->style.edit.cursor_text_normal = nk_rgb(0x0,0x0,0x0);
	gui->NK_Context->style.edit.cursor_text_hover = nk_rgb(0x0,0x0,0x0);
	gui->NK_Context->style.edit.text_normal = nk_rgb(0x25,0x25,0x25);
	gui->NK_Context->style.edit.text_hover = nk_rgb(0x25,0x25,0x25);
	gui->NK_Context->style.edit.text_active = nk_rgb(0x25,0x25,0x25);
	gui->NK_Context->style.edit.selected_normal = nk_rgb(0x25,0x25,0x25);
	gui->NK_Context->style.edit.selected_text_normal = nk_rgb(0x25,0x25,0x25);
}

void Delete_GUI(GUI* gui) {
	if (gui->NK_Context) {
		log_dbg("deleting nk context");
		nk_free(gui->NK_Context);
		delete gui->NK_Context;
	}
	if (gui->Font) {
		log_dbg("deleting font");
		Delete_Font(gui->Font);
		delete gui->Font;
	}
	if (gui->FontNK) {
		log_dbg("deleting fontNK");
		delete gui->FontNK;
	}
	if (gui->Graphics_Handle_Buffer) {
		log_dbg("deleting graphics handle buffer");
		delete gui->Graphics_Handle_Buffer;
	}
	if (gui->Graphics_Handle) {
		log_dbg("deleting graphics handle");
		delete gui->Graphics_Handle;
	}
}

void Draw_Text(GUI* gui, GRAPHICS* graphics, const struct nk_command* command) {
	const struct nk_command_text* t = (const struct nk_command_text*)command;
	string str = string(t->string);
	int _x = t->x;
	int _y = t->y + gui->Font->Baseline;

	unsigned char fg[] = {t->foreground.b,t->foreground.g,t->foreground.r,t->foreground.a};
	unsigned char bg[] = {t->background.r,t->background.g,t->background.b,t->background.a};
	if (((int*)fg)[0] == ((int*)bg)[0]) ((int*)bg)[0] = 0xff989898;
	unsigned char result[] = {0,0,0,0xff};

	union {
		unsigned char bytes[4];
		int data;
	} kk;

	for (int i = 0; i < str.length(); i++) {
		BAKED_GLYPH *c = &gui->Font->Baked_glyphs[int(str[i]) - 32];

		for (int x = 0; x < c->W; x++){
			for (int y = 0; y < c->H; y++){
				unsigned int alpha = c->Bitmap[(y*c->W)+x];
				unsigned int inv_alpha = 255 - alpha;
				result[0] = (unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
				result[1] = (unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
				result[2] = (unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
				result[3] = (unsigned char)((alpha * fg[3] + inv_alpha * bg[3]) >> 8);
				memcpy(kk.bytes, result, 4);
				DrawPoint_GRAPHICS(graphics, _x + x + c->Xoff, _y + y + c->Yoff, kk.data);
			}
		}
		_x += c->Advance;
	}
}

void Draw_Text(GUI* gui, GRAPHICS* graphics, string str, int _x, int _y, unsigned char* fg, unsigned char* back) {
	unsigned char result[] = {0,0,0,0xff};

	union {
		unsigned char bytes[4];
		int data;
	} kk;

	//unsigned char fg[] = {0x98,0x98,0x98,0xFF};
	//unsigned char bg[] = {0x25,0x25,0x25,0xFF};

	for (int i = 0; i < str.length(); i++) {
		BAKED_GLYPH *c = &gui->Font->Baked_glyphs[int(str[i]) - 32];

		for (int x = 0; x < c->W; x++){
			for (int y = 0; y < c->H; y++){
				unsigned int alpha = c->Bitmap[(y*c->W)+x];
				unsigned int inv_alpha = 255 - alpha;
				unsigned char* bg = (unsigned char*)&(((int*)back)[gui->Width * (_y + y + c->Yoff) + (_x + x)]);
				result[0] = (unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);
				result[1] = (unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);
				result[2] = (unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);
				memcpy(kk.bytes, result, 4);
				DrawPoint_GRAPHICS(graphics, _x + x + c->Xoff, _y + y + c->Yoff, kk.data);
			}
		}
		_x += c->Advance;
	}
}

void Handle_Input_GUI(GUI* gui, MOUSE** mouse, KEYBOARD** keyboard, int len) {
	nk_input_begin (gui->NK_Context);
	// Mouse

	for (int k = 0; k < len; k++) {
		if (mouse[k] != NULL) {
			for (int i = mouse[k]->Mouse_Event_Stack.getLength(); i > 0; i--) {
				libinput_event_pointer* lep;
				MOUSE_EVENT_ELEMENT* element = (MOUSE_EVENT_ELEMENT*)mouse[k]->Mouse_Event_Stack.GetElement(0);
				switch(libinput_event_get_type(element->Event)) {
					case LIBINPUT_EVENT_POINTER_MOTION:
	                    lep = libinput_event_get_pointer_event(element->Event);
						mouse[k]->Current_X += libinput_event_pointer_get_dx_unaccelerated(lep) * mouse[k]->Sensitivity;
						if (mouse[k]->Current_X > mouse[k]->Maximum_X) {
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
							log_dbg("left mouse clicked " + to_string(mouse[k]->Current_X) + " " + to_string(mouse[k]->Current_Y));
	                		nk_input_button(gui->NK_Context, NK_BUTTON_LEFT,   mouse[k]->Current_X, mouse[k]->Current_Y, libinput_event_pointer_get_button_state(lep));
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
			nk_input_motion(gui->NK_Context, mouse[k]->Current_X, mouse[k]->Current_Y);
		}
	}

	// Keyboard
	for (int k = 0; k < len; k++) {
		if (keyboard[k] != NULL) {
			for(int i = keyboard[k]->Device->Event_Stack.getLength(); i > 0; i--){
				EVENT_ELEMENT* element = (EVENT_ELEMENT*)keyboard[k]->Device->Event_Stack.GetElement(0);
				switch(element->Event.type){
					case EV_SYN:
					case EV_REL:
						break;
					case EV_KEY:
						switch(element->Event.code){
							case KEY_LEFTSHIFT:
							case KEY_RIGHTSHIFT:
								nk_input_key(gui->NK_Context, NK_KEY_SHIFT, element->Event.value);
								switch(element->Event.value){
									case 0: // Key Release
										keyboard[k]->Shift = false;
										break;
									case 1: // Key Press
									case 2: // Auto Repeat
										keyboard[k]->Shift = true;
										break;
									default:
										//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
										break;
								}
								break;
							case KEY_CAPSLOCK:
								switch(element->Event.value){
									case 0: // Key Release
										keyboard[k]->Caps_Lock = false;
										break;
									case 1: // Key Press
									case 2: // Auto Repeat
										keyboard[k]->Caps_Lock = true;
										break;
									default:
										//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_BACKSPACE:
								nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, element->Event.value);
								break;
							case KEY_UP:
								nk_input_key(gui->NK_Context, NK_KEY_UP,        element->Event.value);
								break;
							case KEY_DOWN:
								nk_input_key(gui->NK_Context, NK_KEY_DOWN,      element->Event.value);
								break;
							case KEY_LEFT:
								nk_input_key(gui->NK_Context, NK_KEY_LEFT,      element->Event.value);
								break;
							case KEY_RIGHT:
								nk_input_key(gui->NK_Context, NK_KEY_RIGHT,     element->Event.value);
								break;
							case KEY_ENTER:
								nk_input_key(gui->NK_Context, NK_KEY_ENTER,     element->Event.value);
								break;
							default:
								if(element->Event.code <= 111){
									switch(element->Event.value){
										case 0:
											break;
										case 1: // Key Press
										case 2: // Auto Repeat
											char key_value;
											if(keyboard[k]->Caps_Lock){
												if (keyboard[k]->Shift) {
													key_value = lower_key_strings[element->Event.code];
												} else {
													key_value = upper_key_strings[element->Event.code];
												}
											} else {
												if (keyboard[k]->Shift) {
													key_value = upper_key_strings[element->Event.code];
												} else {
													key_value = lower_key_strings[element->Event.code];
												}
											}
											if(key_value > 0){
												nk_input_char(gui->NK_Context, key_value);
											}
											break;
										default:
											//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
											break;
									}
								}
								break;
						}
						break;
					case EV_ABS:
					case EV_MSC:
					case EV_SW:
					case EV_LED:
					case EV_SND:
					case EV_REP:
					case EV_FF:
					case EV_PWR:
					case EV_FF_STATUS:
					case EV_MAX:
					case EV_CNT:
						break;
					default:
						//Write_Notice(string("@Listen_Keyboard() Unexpected event type: ") + to_string(element->Event.type) + " (code: " + to_string(element->Event.code) + " and value: " + to_string(element->Event.value) + ")");
						break;
				}
				keyboard[k]->Device->Event_Stack.Remove(element);
				delete element;
			}
		}
	}
	nk_input_end(gui->NK_Context);
}
void Render_Nuklear_GUI(GUI* gui) {
	const struct nk_command* command;
	Integer color;

	nk_foreach(command, gui->NK_Context) {
		switch (command->type) {
			case NK_COMMAND_NOP: break;
			case NK_COMMAND_SCISSOR: {
				const struct nk_command_scissor* s =(const struct nk_command_scissor*)command;
				Set_Clip_GRAPHICS(gui->Graphics_Handle, s->x, s->y, s->w, s->h);
				break;
			}
			case NK_COMMAND_LINE: {
				const struct nk_command_line* l = (const struct nk_command_line*)command;
				color.bytes[0] = l->color.r;
				color.bytes[1] = l->color.g;
				color.bytes[2] = l->color.b;
				color.bytes[3] = l->color.a;
				DrawLine_GRAPHICS(gui->Graphics_Handle, l->begin.x, l->begin.y, l->end.x, l->end.y, color.data);
				break;
			}
			case NK_COMMAND_RECT: {
				const struct nk_command_rect* r = (const struct nk_command_rect*)command;
				color.bytes[0] = r->color.r;
				color.bytes[1] = r->color.g;
				color.bytes[2] = r->color.b;
				color.bytes[3] = r->color.a;
				drawThickRect(gui->Graphics_Handle, r->x, r->y, r->w, r->h, r->line_thickness, color.data);
				break;
			}
			case NK_COMMAND_RECT_FILLED: {
				const struct nk_command_rect_filled* r = (const struct nk_command_rect_filled*)command;
				color.bytes[0] = r->color.r;
				color.bytes[1] = r->color.g;
				color.bytes[2] = r->color.b;
				color.bytes[3] = r->color.a;
				//fillRoundedRect(gui->Graphics_Handle, r->x, r->y, r->w, r->h, r->rounding, color.data);
				FillSquare_GRAPHICS(gui->Graphics_Handle, r->x, r->y, r->w, r->h, color.data);
				break;
			}
			case NK_COMMAND_CIRCLE: {
				const struct nk_command_circle* c = (const struct nk_command_circle*)command;
				color.bytes[0] = c->color.r;
				color.bytes[1] = c->color.g;
				color.bytes[2] = c->color.b;
				color.bytes[3] = c->color.a;
				DrawCircle_GRAPHICS(gui->Graphics_Handle, c->x + (c->w / 2), c->y + (c->w / 2), c->w / 2, color.data);
				break;
			}
			case NK_COMMAND_CIRCLE_FILLED: {
				const struct nk_command_circle_filled* c = (const struct nk_command_circle_filled*)command;
				color.bytes[0] = c->color.r;
				color.bytes[1] = c->color.g;
				color.bytes[2] = c->color.b;
				color.bytes[3] = c->color.a;
				FillCircle_GRAPHICS(gui->Graphics_Handle, c->x + (c->w / 2), c->y + (c->w / 2), c->w / 2, color.data);
				break;
			}
			case NK_COMMAND_TRIANGLE: {
				const struct nk_command_triangle* t = (const struct nk_command_triangle*)command;
				break;
			}
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled* t = (const struct nk_command_triangle_filled*)command;
				color.bytes[0] = t->color.r;
				color.bytes[1] = t->color.g;
				color.bytes[2] = t->color.b;
				color.bytes[3] = t->color.a;
				Polygon tri;
				tri.addPoint(t->a.x, t->a.y);
				tri.addPoint(t->b.x, t->b.y);
				tri.addPoint(t->c.x, t->c.y);
				FillPolygon_GRAPHICS(gui->Graphics_Handle, tri, color.data);
				break;
			}
			case NK_COMMAND_POLYGON: {
				const struct nk_command_polygon* p = (const struct nk_command_polygon*)command;
				break;
			}
			case NK_COMMAND_POLYGON_FILLED: {
				const struct nk_command_polygon_filled* p = (const struct nk_command_polygon_filled*)command;
				break;
			}
			case NK_COMMAND_POLYLINE: {
				break;
			}
			case NK_COMMAND_TEXT: {
				const struct nk_command_text* t = (const struct nk_command_text*)command;

				color.bytes[0] = t->foreground.r;
				color.bytes[1] = t->foreground.g;
				color.bytes[2] = t->foreground.b;
				color.bytes[3] = t->foreground.a;

				Draw_Text(gui, gui->Graphics_Handle, command);

				//Draw_Text(gui, gui->Graphics_Handle, string(t->string), t->x, t->y + gui->Font->Baseline, (unsigned char*) color.bytes, (unsigned char*) gui->Graphics_Handle->buffer_i);
				break;
			}
			case NK_COMMAND_CURVE:
			case NK_COMMAND_RECT_MULTI_COLOR:
			case NK_COMMAND_IMAGE: {
				const struct nk_command_image* image = (const struct nk_command_image*)command;
				Draw_BMP((BMP*)image->img.handle.ptr, gui->Graphics_Handle, image->x, image->y);
				break;
			}
			case NK_COMMAND_ARC:
			case NK_COMMAND_ARC_FILLED:
			case NK_COMMAND_CUSTOM:
			default:
				break;
		}
	}
	nk_clear(gui->NK_Context);
	Set_Clip_GRAPHICS(gui->Graphics_Handle, -1, -1, -1, -1); // sets clip to full 0, 0, width, height
}

void Render_Mouse_GUI(GUI* gui, double c_x, double c_y) {
	for (int x = 0; x < 11; x++) {
		for (int y = 0; y < 11; y++) {
			int hg = mousie[x][y];
			if (hg == 0x03ffffff) {
				DrawPoint_GRAPHICS(gui->Graphics_Handle, (int) (c_x + 0.5) + x, (int) (c_y + 0.5) + y, 0xffffffff);
			} else if (hg == 0x03000000) {
				DrawPoint_GRAPHICS(gui->Graphics_Handle, (int) (c_x + 0.5) + x, (int) (c_y + 0.5) + y, 0xff000000);
			}
		}
	}
}

void Render_Mouse_GUI(GUI* gui, GRAPHICS* Graphics_Handle, double c_x, double c_y) {
	for (int x = 0; x < 11; x++) {
		for (int y = 0; y < 11; y++) {
			int hg = mousie[x][y];
			if (hg == 0x03ffffff) {
				DrawPoint_GRAPHICS(Graphics_Handle, (int) (c_x + 0.5) + x, (int) (c_y + 0.5) + y, 0xffffffff);
			} else if (hg == 0x03000000) {
				DrawPoint_GRAPHICS(Graphics_Handle, (int) (c_x + 0.5) + x, (int) (c_y + 0.5) + y, 0xff000000);
			}
		}
	}
}

void Render_GUI(GUI* gui, char* output_buffer) {
	//char* swapper;
	copy(gui->Graphics_Handle_Buffer, gui->Graphics_Handle_Buffer + (gui->Frame_Resolution * 4), output_buffer);
	//swapper = gui->Graphics_Handle_Buffer;
	//gui->Graphics_Handle_Buffer = *output_buffer;
	//*output_buffer = swapper;
}
