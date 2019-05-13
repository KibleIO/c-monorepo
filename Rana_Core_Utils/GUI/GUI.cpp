#define NK_IMPLEMENTATION

#include "GUI.h"

static float Font_Get_Text_Width(nk_handle handle, float height, const char* text, int length) {
	(void)height;
	BAKED_GLYPH* glyphs = (BAKED_GLYPH*)handle.ptr;
	int _x = 0;
	for (int i = 0; i < length; i++) {
		_x += glyphs[int(text[i]) - 32].Advance;
	}
	return _x;
}

void Initialize_GUI_Themis(GUI* gui, int display_id) {
	gui->Display_ID = display_id;
	gui->NK_Context	= NULL;

	gui->fonts = NULL;
	gui->fontHeights = NULL;

	gui->Graphics_Handle_Buffer	= NULL;
	gui->Graphics_Handle		= NULL;
}

void Initialize_GUI(GUI* gui, int width, int height, string font_path, char* frame_buffer) {
	gui->Width				= width;
	gui->Height				= height;
	gui->Frame_Resolution	= width * height;
	gui->NK_Context			= new nk_context;

	gui->fonts = new GUI_FONT[GUI_TOTAL_FONTS];
	gui->fontHeights = new int[GUI_TOTAL_FONTS] {
		int(gui->Height * 0.02),
	 	int(gui->Height * 0.025),
		int(gui->Height * 0.03)
	};

	gui->BakedBmp = false;

	if (frame_buffer) {
		gui->Graphics_Handle_Buffer		  	= NULL;
		gui->Graphics_Handle              = new GRAPHICS;
		Initialize_GRAPHICS(gui->Graphics_Handle, frame_buffer, gui->Width, gui->Height);
	} else {
		gui->Graphics_Handle_Buffer       = new char[(gui->Frame_Resolution + 2) * 4];
		gui->Graphics_Handle              = new GRAPHICS;
		Initialize_GRAPHICS(gui->Graphics_Handle, gui->Graphics_Handle_Buffer, gui->Width, gui->Height);
	}
	gui->Graphics_Handle->Transparent = true;

	// Initialize each of the gui fonts
	for(int i = 0; i < GUI_TOTAL_FONTS; i++) {
		Initialize_GUI_Font(&gui->fonts[i], gui->fontHeights[i], font_path.c_str());
	}
	// Initialize the nk context with the default font size
	nk_init_default(gui->NK_Context, &gui->fonts[GUI_FONT_DEFAULT_SIZE].nkFont);
	Set_Font(gui, GUI_FONT_DEFAULT_SIZE);
	Set_GUI_Style_Default(gui);
}

void Pair_Fonts(nk_user_font* nkFont, FONT* userFont, float height, const char* font_path) {
	Initialize_Font(userFont, font_path, height);
	nkFont->userdata.ptr = userFont->Baked_glyphs;
	nkFont->height = height;
	nkFont->width = Font_Get_Text_Width;
}

void Initialize_GUI_Font(GUI_FONT* gfont, int height, const char* fname) {
	Initialize_Font(&gfont->userFont, fname, height);
	gfont->nkFont.userdata.ptr = gfont->userFont.Baked_glyphs;
	gfont->nkFont.height = height;
	gfont->nkFont.width = Font_Get_Text_Width;
}

void Set_Font(GUI* gui, int font_index) {
	if(font_index >= 0 && font_index < GUI_TOTAL_FONTS) {
		gui->currentFont = font_index;
		nk_style_set_font(gui->NK_Context, &gui->fonts[font_index].nkFont);
	}
}

void Delete_GUI(GUI* gui) {
	log_dbg("deleting gui");
	if (gui->NK_Context) {
		log_dbg("deleting nk context");
		nk_free(gui->NK_Context);
		delete gui->NK_Context;
	}
	if (gui->fonts) {
		log_dbg("deleting gui fonts");
		for (int i = 0; i < GUI_TOTAL_FONTS; i++) {
			Delete_Font(&gui->fonts[i].userFont);
		}
		delete [] gui->fonts;
	}
	if (gui->fontHeights) {
		log_dbg("deleting font heights");
		delete [] gui->fontHeights;
	}
	if (gui->Graphics_Handle_Buffer) {
		log_dbg("deleting graphics handle buffer");
		delete [] gui->Graphics_Handle_Buffer;
	}
	if (gui->Graphics_Handle) {
		log_dbg("deleting graphics handle");
		Delete_GRAPHICS(gui->Graphics_Handle);
		delete gui->Graphics_Handle;
	}
	log_dbg("done deleting gui");
}

void Draw_Text(GUI* gui, GRAPHICS* graphics, const struct nk_command_text* command) {
	const struct nk_command_text* t = (const struct nk_command_text*)command;
	string str = string(t->string);
	int _x = t->x;
	int _y = t->y + gui->fonts[gui->currentFont].userFont.Baseline;

	unsigned char fg[] = {t->foreground.b,t->foreground.g,t->foreground.r,t->foreground.a};
	unsigned char bg[] = {t->background.r,t->background.g,t->background.b,t->background.a};
	int* ifg = (int*)fg;
	int* ibg = (int*)bg;
	if (ifg[0] == ibg[0]) {
		ibg[0] = 0xff989898;
	}
	unsigned char result[] = {0,0,0,0xff};

	union {
		unsigned char bytes[4];
		int data;
	} kk;

	for (unsigned int i = 0; i < str.length(); i++) {
		BAKED_GLYPH* c = &((BAKED_GLYPH*)command->font->userdata.ptr)[int(str[i]) - 32];//&gui->Font->Baked_glyphs[int(str[i]) - 32];

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

	for (unsigned int i = 0; i < str.length(); i++) {
		// TEMP
		// Change to current index in array of fonts
		BAKED_GLYPH *c = &gui->fonts[gui->currentFont].userFont.Baked_glyphs[int(str[i]) - 32];

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

void Handle_Input_GUI(GUI* gui, Queue<MOUSE_EVENT_T*>* m_events, Queue<KEYBOARD_EVENT_T*>* k_events) {
	nk_input_begin (gui->NK_Context);
	// Mouse

	for (int i = m_events->size(); i > 0; i--) {
		MOUSE_EVENT_T* m_event = NULL;
		m_events->pop(m_event);
		if (m_event->clicked) {
	    	nk_input_button(gui->NK_Context, NK_BUTTON_LEFT, m_event->x, m_event->y, m_event->state);
			log_dbg("mouse clicked at " + to_string(m_event->x) + " " + to_string(m_event->y));
		} else {
			if (m_event->state == MOUSE_ABS_COORD) {
				nk_input_motion(gui->NK_Context, m_event->x, m_event->y);
			} else {
				MOUSE::Current_X += m_event->x;
				MOUSE::Current_Y += m_event->y;
				if (MOUSE::Current_X > gui->Width) {
					MOUSE::Current_X = gui->Width;
				} else if (MOUSE::Current_X < 0) {
					MOUSE::Current_X = 0;
				}
				if (MOUSE::Current_Y > gui->Height) {
					MOUSE::Current_Y = gui->Height;
				} else if (MOUSE::Current_Y < 0) {
					MOUSE::Current_Y = 0;
				}
				nk_input_motion(gui->NK_Context, MOUSE::Current_X, MOUSE::Current_Y);
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
				nk_input_key(gui->NK_Context, NK_KEY_SHIFT, k_event->value);
				switch(k_event->value){
					case 0: // Key Release
						KEYBOARD::Shift = false;
						break;
					case 1: // Key Press
					case 2: // Auto Repeat
						KEYBOARD::Shift = true;
						break;
					default:
						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
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
						//Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
						break;
				}
				break;
			case KEY_BACKSPACE:
				nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, k_event->value);
				break;
			case KEY_UP:
				nk_input_key(gui->NK_Context, NK_KEY_UP,        k_event->value);
				break;
			case KEY_DOWN:
				nk_input_key(gui->NK_Context, NK_KEY_DOWN,      k_event->value);
				break;
			case KEY_LEFT:
				nk_input_key(gui->NK_Context, NK_KEY_LEFT,      k_event->value);
				break;
			case KEY_RIGHT:
				nk_input_key(gui->NK_Context, NK_KEY_RIGHT,     k_event->value);
				break;
			case KEY_ENTER:
				nk_input_key(gui->NK_Context, NK_KEY_ENTER,     k_event->value);
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
									key_value = KEYBOARD::Keys_Shifted[k_event->code];
								}
							} else {
								if (KEYBOARD::Shift) {
									key_value = KEYBOARD::Keys_Shifted[k_event->code];
								} else {
									key_value = KEYBOARD::Keys[k_event->code];
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
		delete k_event;
	}

	nk_input_end(gui->NK_Context);
}

void Render_Nuklear_GUI(GUI* gui) {
	const struct nk_command* command;

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

				DrawLine_GRAPHICS(
				gui->Graphics_Handle, l->begin.x, l->begin.y, l->end.x,
				l->end.y, l->color.a << 24 | l->color.r << 16 | l->color.g << 8 |
				l->color.b);

				break;
			}
			case NK_COMMAND_RECT: {
				const struct nk_command_rect* r = (const struct nk_command_rect*)command;

				DrawThickRect_GRAPHICS(
				gui->Graphics_Handle, r->x, r->y, r->w, r->h, r->line_thickness,
				r->color.a << 24 | r->color.r << 16 | r->color.g << 8 |
				r->color.b);

				break;
			}
			case NK_COMMAND_RECT_FILLED: {
				const struct nk_command_rect_filled* r = (const struct nk_command_rect_filled*)command;

				FillRoundedRect_GRAPHICS(
				gui->Graphics_Handle, r->x, r->y, r->w, r->h, r->rounding,
				r->color.a << 24 | r->color.b << 16 | r->color.g << 8 |
				r->color.r);

				break;
			}
			case NK_COMMAND_CIRCLE: {
				const struct nk_command_circle* c = (const struct nk_command_circle*)command;

				DrawCircle_GRAPHICS(
				gui->Graphics_Handle, c->x + (c->w / 2), c->y + (c->w / 2),
				c->w / 2, c->color.a << 24 | c->color.r << 16 | c->color.g << 8 |
				c->color.b);

				break;
			}
			case NK_COMMAND_CIRCLE_FILLED: {
				const struct nk_command_circle_filled* c = (const struct nk_command_circle_filled*)command;

				FillCircle_GRAPHICS(
				gui->Graphics_Handle, c->x + (c->w / 2), c->y + (c->w / 2),
				c->w / 2, c->color.a << 24 | c->color.r << 16 | c->color.g << 8 |
				c->color.b);

				break;
			}
			case NK_COMMAND_TRIANGLE: {
				const struct nk_command_triangle* t = (const struct nk_command_triangle*)command;
				(void)t;
				break;
			}
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled* t = (const struct nk_command_triangle_filled*)command;
				POLYGON tri;
				tri.addPoint(t->a.x, t->a.y);
				tri.addPoint(t->b.x, t->b.y);
				tri.addPoint(t->c.x, t->c.y);

				FillPolygon_GRAPHICS(
				gui->Graphics_Handle, tri, t->color.a << 24 | t->color.r << 16 | t->color.g << 8 |
				t->color.b);

				break;
			}
			case NK_COMMAND_POLYGON: {
				const struct nk_command_polygon* p = (const struct nk_command_polygon*)command;
				(void)p;
				break;
			}
			case NK_COMMAND_POLYGON_FILLED: {
				const struct nk_command_polygon_filled* p = (const struct nk_command_polygon_filled*)command;
				(void)p;
				break;
			}
			case NK_COMMAND_POLYLINE: {
				break;
			}
			case NK_COMMAND_TEXT: {
				const struct nk_command_text* t = (const struct nk_command_text*)command;

				Draw_Text(
				gui, gui->Graphics_Handle, t);

				break;
			}
			case NK_COMMAND_CURVE:
			case NK_COMMAND_RECT_MULTI_COLOR:
			case NK_COMMAND_IMAGE: {
				const struct nk_command_image* image = (const struct nk_command_image*)command;
				log_tmp("image " + ((BMP*)image->img.handle.ptr)->name);
				if (!gui->BakedBmp) {
					Draw_BMP((BMP*)image->img.handle.ptr, gui->Graphics_Handle, image->x, image->y);
				}
				log_tmp("done image");
				break;
			}
			case NK_COMMAND_ARC:
			case NK_COMMAND_ARC_FILLED:
			case NK_COMMAND_CUSTOM:
			default:
				break;
		}
	}

	log_tmp("nk clear");
	nk_clear(gui->NK_Context);
	log_tmp("set clip");
	Set_Clip_GRAPHICS(gui->Graphics_Handle, -1, -1, -1, -1); // sets clip to full 0, 0, width, height
}

void Render_GUI(GUI* gui, char* output_buffer) {
	//char* swapper;
	copy(gui->Graphics_Handle_Buffer, gui->Graphics_Handle_Buffer + (gui->Frame_Resolution * 4), output_buffer);
	//swapper = gui->Graphics_Handle_Buffer;
	//gui->Graphics_Handle_Buffer = *output_buffer;
	//*output_buffer = swapper;
}

void Set_GUI_Style_Default(GUI* gui) {
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
