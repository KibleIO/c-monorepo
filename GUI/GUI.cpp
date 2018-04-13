#define STB_TRUETYPE_IMPLEMENTATION
#define NK_IMPLEMENTATION
#include "GUI.h"

// TRY TO REMOVE THESE
nk_font* font_14;

/*************************************************/
//	stb_truetype code

int advances[95];
stbtt_fontinfo font_stb;

static float Font_Get_Text_Width(nk_handle handle, float height, const char* text, int length){
	int _x = 0;
	for (int i = 0; i < length; i++) {
		_x += advances[int(text[i]) - 32]; // Add check for oob
	}
	return _x;
}

int Font_Get_Text_Height(GUI* gui, const char* text, int length){
	int _y = 0;
	for (int i = 0; i < length; i++) {
		if (gui->baked_glyphs[int(text[i]) - 32].h > _y) {
			_y = gui->baked_glyphs[int(text[i] - 32)].h;
		}
	}
	return _y;
}
// END: TRY TO REMOVE

void Initialize_GUI       (GUI* gui, MOUSE** mouse, KEYBOARD** keyboard, int* num_dev, unsigned int width, unsigned int height, unsigned int frame_count) {
	gui->Mouse            = mouse;
	gui->Keyboard         = keyboard;
	gui->num_dev          = num_dev;
	gui->Width            = width;
	gui->Height           = height;
	gui->Frame_Resolution = width * height;
	gui->NK_Context       = new nk_context();

	/*************************************************/
	//	FONT SETUP

	//STB_ttf font set up
	float         scale;
	float         text_height = 20;
	int           ascent;
	int           baseline;
	int           c_w;
	int           c_h;
	int           c_xoff;
	int           c_yoff;
	int           advance;
	int           lsb;
	unsigned char ttf_buffer[1<<20];

	text_height = 20;
	fread(ttf_buffer, 1, 1<<20, fopen("/root/IRIS/myriad.ttf", "rb"));
	stbtt_InitFont(&font_stb, ttf_buffer,0);
	scale = stbtt_ScaleForPixelHeight(&font_stb, text_height);
	stbtt_GetFontVMetrics(&font_stb, &ascent, 0, 0);
	gui->baseline = (int) ascent * scale;

	//Bake stb_font
	for (int i = 0; i < 95; i++)
	{
		gui->baked_glyphs[i].bmp  = stbtt_GetCodepointBitmap(&font_stb,scale,scale, 32 + i, &c_w, &c_h, &c_xoff, &c_yoff);
		gui->baked_glyphs[i].w    = c_w;
		gui->baked_glyphs[i].h    = c_h;
		gui->baked_glyphs[i].xoff = c_xoff;
		gui->baked_glyphs[i].yoff = c_yoff;
		stbtt_GetCodepointHMetrics(&font_stb, char(32 + i), &advance, &lsb);
		advances[i] = advance * scale;
		gui->baked_glyphs[i].advance = advances[i];
	}

	//Nuklear font setup
	nk_font_atlas* atlas;
	nk_font_atlas_init_default(atlas);
	nk_font_atlas_begin(atlas);
	font_14 = nk_font_atlas_add_from_file(atlas, "/root/IRIS/myriad.ttf", text_height, 0);
	nk_font_atlas_end(atlas, nk_handle_id(font_14->texture.id), NULL);
	font_14->handle.width    = Font_Get_Text_Width;
	nk_init_default(gui->NK_Context, &font_14->handle);

	/*************************************************/
	//	GUI Setup

	Initialize_Frames_GUI(gui, frame_count);
}
GUI* Construct_GUI        (MOUSE** mouse, KEYBOARD** keyboard, int* num_dev, unsigned int width, unsigned int height, unsigned int frame_count)          {
	GUI* gui = new GUI();
	Initialize_GUI(gui, mouse, keyboard, num_dev, width, height, frame_count);
	return gui;
}
void Delete_GUI           (GUI* gui)                                                                                                     {
	delete gui->NK_Context;
	Delete_Frames_GUI(gui);
}

// Isolate to a FRAME_MANAGER struct
void Initialize_Frames_GUI(GUI* gui, unsigned int frame_count)                                                                           {
	gui->Frame_Count            = frame_count;
	gui->Frames                 = new FRAME       [frame_count];
	gui->Delta_Buffers_Reserved = new char*       [frame_count];
	gui->Delta_Buffers          = new char*       [frame_count];
	gui->History_Buffers        = new char*       [frame_count];
	gui->Delta_Lengths          = new unsigned int[frame_count];
	for (int frame_index = 0; frame_index < frame_count; frame_index++){
		Initialize_Frame(gui->Frames + frame_index, gui->Frame_Resolution);
		gui->History_Buffers       [frame_index] = (char*)gui->Frames[frame_index].Buffer;
		gui->Delta_Buffers_Reserved[frame_index] = new char[(gui->Frame_Resolution * sizeof(int)) + SIZE_OF_DELTA_HEADER];
		gui->Delta_Buffers         [frame_index] = gui->Delta_Buffers_Reserved[frame_index];
	}
}
void Delete_Frames_GUI    (GUI* gui)                                                                                                     {
	for (int frame_index = 0; frame_index < gui->Frame_Count; frame_index++){
		Delete_Frame(gui->Frames + frame_index);
		delete gui->Delta_Buffers_Reserved[frame_index];
	}
	delete gui->Frames;
	delete gui->Delta_Buffers_Reserved;
	delete gui->Delta_Buffers;
	delete gui->History_Buffers;
	delete gui->Delta_Lengths;
	gui->Frame_Count = 0;
}
void Refresh_Frames_GUI   (GUI* gui, unsigned int frame_count)                                                                           {
	Delete_Frames_GUI    (gui);
	Initialize_Frames_GUI(gui, frame_count);
}
void Refresh_Frames_GUI   (GUI* gui)                                                                                                     {
	Refresh_Frames_GUI(gui, gui->Frame_Count);
}

//
void Draw_Text(GUI* gui, Graphics* graphics, const struct nk_command_text* text_command){
	Baked_Glyph* baked_glyph
	unsigned int forecolor;
	unsigned int alpha;
	string       str;
	int          _x;
	int          _y;

	forecolor = (text_command->foreground.r << 16) + (text_command->foreground.g << 8) + text_command->foreground.b;
	str       = string(text_command->string);
	_x        = text_command->x;
	_y        = text_command->y + (Font_Get_Text_Height(gui, str.c_str(), str.length()) >> 1); // + gui->baseline - (t->h / 2);

	switch ((unsigned char)text_command->foreground.a){
		case 0xFF: {
			for (int i = 0; i < str.length(); i++) {
				baked_glyph = &gui->baked_glyphs[int(str[i]) - 32];
				for (int x = 0; x < baked_glyph->w; x++) {
					for (int y = 0; y < baked_glyph->h; y++) {
						alpha = baked_glyph->bmp[(y * baked_glyph->w) + x];
						switch (alpha) {
							case 0xFF:{
								graphics->drawPoint(
									_x + x + baked_glyph->xoff,
									_y + y + baked_glyph->yoff,
									forecolor + (alpha << 24));
								break;
							}
							case 0x00:{
								break;
							}
							default:{
								graphics->Draw_Transparent_Point(
									_x + x + baked_glyph->xoff,
									_y + y + baked_glyph->yoff,
									forecolor + (alpha << 24));
								break;
							}
						}
					}
				}
				_x += baked_glyph->advance;
			}
			break;
		}
		case 0x00: {
			break;
		}
		default: {
			for (int i = 0; i < str.length(); i++) {
				baked_glyph = &gui->baked_glyphs[int(str[i]) - 32];
				for (int x = 0; x < baked_glyph->w; x++) {
					for (int y = 0; y < baked_glyph->h; y++) {
						alpha = ((((unsigned int)text_command->foreground.a) * ((unsigned int)baked_glyph->bmp[(y * baked_glyph->w) + x])) + 127) / 255;
						// alpha = (((((unsigned int)text_command->foreground.a) + 1) * (((unsigned int)baked_glyph->bmp[(y * baked_glyph->w) + x]) + 1)) + 128) >> 8; // May be faster, test me
						switch (alpha) {
							case 0xFF:{
								graphics->drawPoint(
									_x + x + baked_glyph->xoff,
									_y + y + baked_glyph->yoff,
									forecolor + (alpha << 24));
								break;
							}
							case 0x00:{
								break;
							}
							default:{
								graphics->Draw_Transparent_Point(
									_x + x + baked_glyph->xoff,
									_y + y + baked_glyph->yoff,
									forecolor + (alpha << 24));
								break;
							}
						}
					}
				}
				_x += baked_glyph->advance;
			}
			break;
		}
	}
}

void Apply_GUI            (GUI* gui, Graphics* graphics)                                                                    {
	//int* buffer = new int[gui->Frame_Resolution + 2];
	//fill(buffer, buffer + gui->Frame_Resolution + 2, 0x00000000);
	//Graphics* graphics = new Graphics((char*)buffer, gui->Width, gui->Height, gui->Width, gui->Height);

	const struct nk_command* command;
	nk_foreach(command, gui->NK_Context) {
		switch (command->type) {
			case NK_COMMAND_NOP:
				break;
			case NK_COMMAND_SCISSOR:         {
				const struct nk_command_scissor* s =(const struct nk_command_scissor*)command;
				graphics->setClip(s->x, s->y, s->w, s->h);
				break;
			}
			case NK_COMMAND_LINE:            {
				const struct nk_command_line* l = (const struct nk_command_line*)command;
				graphics->drawLine(l->begin.x, l->begin.y, l->end.x, l->end.y, l->color.r + (l->color.g << 8) + (l->color.b << 16));
				break;
			}
			case NK_COMMAND_RECT:            {
				const struct nk_command_rect* rectangle = (const struct nk_command_rect*)command;
				graphics->drawSquare(
					rectangle->x,
					rectangle->y,
					rectangle->w,
					rectangle->h,
					rectangle->color.r + (rectangle->color.g << 8) + (rectangle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_RECT_FILLED:     {
				const struct nk_command_rect_filled* rectangle = (const struct nk_command_rect_filled*)command;
				graphics->fillSquare(
					rectangle->x,
					rectangle->y,
					rectangle->w,
					rectangle->h,
					rectangle->color.r + (rectangle->color.g << 8) + (rectangle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_CIRCLE:          {
				const struct nk_command_circle* circle = (const struct nk_command_circle*)command;
				/*
				graphics->drawCircle(
					circle->x + (circle->w / 2),
					circle->y + (circle->w / 2),
					circle->w / 2,
					circle->color.r + (circle->color.g << 8) + (circle->color.b << 16) + (0xff << 24));
				*/
				break;
			}
			case NK_COMMAND_CIRCLE_FILLED:   {
				const struct nk_command_circle_filled* circle = (const struct nk_command_circle_filled*)command;
				graphics->fillCircle(
					circle->x + (circle->w / 2),
					circle->y + (circle->w / 2),
					circle->w / 2,
					circle->color.r + (circle->color.g << 8) + (circle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_TRIANGLE:        {
				const struct nk_command_triangle* triangle = (const struct nk_command_triangle*)command;
				break;
			}
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled* triangle = (const struct nk_command_triangle_filled*)command;
				Polygon tri;
				tri.addPoint(triangle->a.x, triangle->a.y);
				tri.addPoint(triangle->b.x, triangle->b.y);
				tri.addPoint(triangle->c.x, triangle->c.y);
				graphics->fillPolygon(
					tri, triangle->color.r + (triangle->color.g << 8) + (triangle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_POLYGON:         {
				const struct nk_command_polygon* polygon = (const struct nk_command_polygon*)command;
				break;
			}
			case NK_COMMAND_POLYGON_FILLED:  {
				const struct nk_command_polygon_filled* polygon = (const struct nk_command_polygon_filled*)command;
				break;
			}
			case NK_COMMAND_POLYLINE:        {
				break;
			}
			case NK_COMMAND_TEXT:            {
				Draw_Text(gui, graphics, command);
				break;
			}
			case NK_COMMAND_CURVE:
			case NK_COMMAND_RECT_MULTI_COLOR:
			case NK_COMMAND_IMAGE:
			case NK_COMMAND_ARC:
			case NK_COMMAND_ARC_FILLED:
			case NK_COMMAND_CUSTOM:
			default:
				break;
		}
	}
	nk_clear(gui->NK_Context);
}
void Apply_GUI            (GUI* gui, FRAME* frame, Graphics* graphics)                                                                    {
	// This can't be efficient...
	int* delta_buffer = new int[gui->Frame_Resolution + SIZE_OF_DELTA_HEADER]; // leak!
	int  delta_length = Encode_Difference_File_spec(
		(unsigned char*)delta_buffer,
		(unsigned char*)frame->Buffer,
		(unsigned char*)graphics->buffer_i,
		gui->Frame_Resolution * sizeof(unsigned int));
	if (delta_length > 0){
		Damage_Frame(frame, (char*)delta_buffer, (unsigned int)delta_length);
	} else {
		delete delta_buffer;
	}
	//delete graphics;
	//delete buffer;
}
void Apply_GUI            (GUI* gui, FRAME* frame)                                                                                       {
	int* buffer = new int[gui->Frame_Resolution + 2];
	fill(buffer, buffer + gui->Frame_Resolution + 2, 0x00000000);
	Graphics* graphics = new Graphics((char*)buffer, gui->Width, gui->Height, gui->Width, gui->Height);

	const struct nk_command* command;
	nk_foreach(command, gui->NK_Context) {
		switch (command->type) {
			case NK_COMMAND_NOP:
				break;
			case NK_COMMAND_SCISSOR:         {
				const struct nk_command_scissor* s =(const struct nk_command_scissor*)command;
				graphics->setClip(s->x, s->y, s->w, s->h);
				break;
			}
			case NK_COMMAND_LINE:            {
				const struct nk_command_line* l = (const struct nk_command_line*)command;
				graphics->drawLine(l->begin.x, l->begin.y, l->end.x, l->end.y, l->color.r + (l->color.g << 8) + (l->color.b << 16));
				break;
			}
			case NK_COMMAND_RECT:            {
				const struct nk_command_rect* rectangle = (const struct nk_command_rect*)command;
				graphics->drawSquare(
					rectangle->x,
					rectangle->y,
					rectangle->w,
					rectangle->h,
					rectangle->color.r + (rectangle->color.g << 8) + (rectangle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_RECT_FILLED:     {
				const struct nk_command_rect_filled* rectangle = (const struct nk_command_rect_filled*)command;
				graphics->fillSquare(
					rectangle->x,
					rectangle->y,
					rectangle->w,
					rectangle->h,
					rectangle->color.r + (rectangle->color.g << 8) + (rectangle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_CIRCLE:          {
				const struct nk_command_circle* circle = (const struct nk_command_circle*)command;
				graphics->drawCircle(
					circle->x + (circle->w / 2),
					circle->y + (circle->w / 2),
					circle->w / 2,
					circle->color.r + (circle->color.g << 8) + (circle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_CIRCLE_FILLED:   {
				const struct nk_command_circle_filled* circle = (const struct nk_command_circle_filled*)command;
				graphics->fillCircle(
					circle->x + (circle->w / 2),
					circle->y + (circle->w / 2),
					circle->w / 2,
					circle->color.r + (circle->color.g << 8) + (circle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_TRIANGLE:        {
				const struct nk_command_triangle* triangle = (const struct nk_command_triangle*)command;
				break;
			}
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled* triangle = (const struct nk_command_triangle_filled*)command;
				Polygon tri;
				tri.addPoint(triangle->a.x, triangle->a.y);
				tri.addPoint(triangle->b.x, triangle->b.y);
				tri.addPoint(triangle->c.x, triangle->c.y);
				graphics->fillPolygon(
					tri, triangle->color.r + (triangle->color.g << 8) + (triangle->color.b << 16) + (0xff << 24));
				break;
			}
			case NK_COMMAND_POLYGON:         {
				const struct nk_command_polygon* polygon = (const struct nk_command_polygon*)command;
				break;
			}
			case NK_COMMAND_POLYGON_FILLED:  {
				const struct nk_command_polygon_filled* polygon = (const struct nk_command_polygon_filled*)command;
				break;
			}
			case NK_COMMAND_POLYLINE:        {
				break;
			}
			case NK_COMMAND_TEXT:            {
				Draw_Text(gui, graphics, command);
				break;
			}
			case NK_COMMAND_CURVE:
			case NK_COMMAND_RECT_MULTI_COLOR:
			case NK_COMMAND_IMAGE:
			case NK_COMMAND_ARC:
			case NK_COMMAND_ARC_FILLED:
			case NK_COMMAND_CUSTOM:
			default:
				break;
		}
	}
	nk_clear(gui->NK_Context);

	int* delta_buffer = new int[gui->Frame_Resolution + SIZE_OF_DELTA_HEADER];
	int  delta_length = Encode_Difference_File_spec(
		(unsigned char*)delta_buffer,
		(unsigned char*)frame->Buffer,
		(unsigned char*)buffer,
		gui->Frame_Resolution * sizeof(unsigned int));
	if (delta_length > 0) {
		Damage_Frame(frame, (char*)delta_buffer, (unsigned int)delta_length);
	} else {
		delete delta_buffer;
	}
	delete graphics;
	delete buffer;
}
void Apply_GUI            (GUI* gui, unsigned int frame_index)                                                                           {
	if (frame_index < gui->Frame_Count){
		Apply_GUI(gui, gui->Frames + frame_index);
	} else {
		nk_clear(gui->NK_Context);
	}
}

void Apply_Mouse_GUI      (GUI* gui, FRAME* frame)                                                                                       {
	int* buffer = new int[gui->Frame_Resolution + 2];
	fill(buffer, buffer + gui->Frame_Resolution + 2, 0x00000000);
	Graphics* graphics = new Graphics((char*)buffer, gui->Width, gui->Height, gui->Width, gui->Height);

	int  mousie[][11] = {
		{0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000},
		{0x03000000,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03FFFFFF,0x03000000},
		{0x03000000,0x03FFFFFF,0x03FFFFFF,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000,0x03000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000,0x00000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000,0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000,0x00000000,0x03000000,0x03FFFFFF,0x03000000,0x00000000},
		{0x03000000,0x03FFFFFF,0x03000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x03000000,0x03FFFFFF,0x03000000},
		{0x03000000,0x03000000,0x03000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x03000000,0x03000000}
	};
	for (int x = 0; x < 11; x++) {
		for (int y = 0; y < 11; y++) {
			switch (mousie[x][y]) {
				case 0x03ffffff:
					graphics->drawPoint(gui->Current_X + x, gui->Current_Y + y, 0xffffffff);
					break;
				case 0x03000000:
					graphics->drawPoint(gui->Current_X + x, gui->Current_Y + y, 0xff000000);
					break;
			}
		}
	}

	int* delta_buffer = new int[gui->Frame_Resolution + SIZE_OF_DELTA_HEADER];
	int  delta_length = Encode_Difference_File_spec(
		(unsigned char*)delta_buffer,
		(unsigned char*)frame->Buffer,
		(unsigned char*)buffer,
		(unsigned int  )gui->Frame_Resolution * sizeof(unsigned int));
	if (delta_length > 0){
		Damage_Frame(frame, (char*)delta_buffer, (unsigned int)delta_length);
	} else {
		delete delta_buffer;
	}
	delete graphics;
	delete buffer;
}
void Apply_Mouse_GUI      (GUI* gui, unsigned int frame_index)                                                                           {
	if (frame_index < gui->Frame_Count){
		Apply_Mouse_GUI(gui, gui->Frames + frame_index);
	}
}

int  Render_GUI           (GUI* gui, char** delta_output)                                                                                {
	int damage_length;
	switch (gui->Frame_Count){
		case 0:
			damage_length = 0;
			break;
		case 1: {
			damage_length = Render_Frame(gui->Frames, delta_output);
			break;
		}
		default: {
			int* delta_counts;
			int  frame_index;
			for (frame_index = 0; frame_index < gui->Frame_Count; frame_index++) {
				gui->Delta_Lengths[frame_index] = Render_Frame(gui->Frames + frame_index, &gui->Delta_Buffers[frame_index]);
			}

			delta_counts  = count_num_diff(gui->Delta_Buffers, (int*)gui->Delta_Lengths, (int)gui->Frame_Count);
			damage_length = black_box(gui->Delta_Buffers, delta_counts, gui->History_Buffers, gui->Frame_Count, *delta_output);

			for (frame_index = 0; frame_index < gui->Frame_Count; frame_index++){
				if (gui->Delta_Buffers[frame_index] != gui->Delta_Buffers_Reserved[frame_index]) {
					delete gui->Delta_Buffers[frame_index];
					gui->Delta_Buffers[frame_index] = gui->Delta_Buffers_Reserved[frame_index];
				}
			}
			delete delta_counts;
			break;
		}
	}
	return damage_length;
}

void Handle_Mouse_GUI     (GUI* gui)                                                                                                     {
	MOUSE* mouse;
	for (int k = 0; k < *gui->num_dev; k++) {
		if (gui->Mouse[k] != NULL) {
			mouse = gui->Mouse[k];
			for (int i = mouse->Mouse_Event_Stack.getLength(); i > 0; i--) {
				libinput_event_pointer* lep;
				MOUSE_EVENT_ELEMENT* element = (MOUSE_EVENT_ELEMENT*)mouse->Mouse_Event_Stack.GetElement(0);
				switch(libinput_event_get_type(element->Event)) {
					case LIBINPUT_EVENT_POINTER_MOTION:
	                    lep = libinput_event_get_pointer_event(element->Event);
						gui->Current_X += libinput_event_pointer_get_dx_unaccelerated(lep) * mouse->Sensitivity;
						if(gui->Current_X > mouse->Maximum_X) {
							gui->Current_X = mouse->Maximum_X;
						} else if (gui->Current_X < mouse->Minimum_X) {
							gui->Current_X = mouse->Minimum_X;
						}
						gui->Current_Y += libinput_event_pointer_get_dy_unaccelerated(lep) * mouse->Sensitivity;
						if (gui->Current_Y > mouse->Maximum_Y) {
							gui->Current_Y = mouse->Maximum_Y;
						} else if(gui->Current_Y < mouse->Minimum_Y) {
							gui->Current_Y = mouse->Minimum_Y;
						}
	                    break;
	                case LIBINPUT_EVENT_POINTER_BUTTON:
	                	lep = libinput_event_get_pointer_event(element->Event);
	                	if (libinput_event_pointer_get_button(lep) == BTN_LEFT) {
	                		nk_input_button(gui->NK_Context, NK_BUTTON_LEFT, gui->Current_X, gui->Current_Y, libinput_event_pointer_get_button_state(lep));
	                	}
	                	break;
					default:
						Write_Notice(string("Unexpected mouse type ") + to_string(libinput_event_get_type(element->Event)));
						break;
				}
				libinput_event_destroy(element->Event);
				mouse->Mouse_Event_Stack.Remove(element);
				delete element;
			}
			nk_input_motion(gui->NK_Context, gui->Current_X, gui->Current_Y);
		}
	}
}

void Handle_Keyboard_GUI  (GUI* gui){
	KEYBOARD* keyboard;
	for (int k = 0; k < *gui->num_dev; k++) {
		if (gui->Keyboard[k] != NULL) {
			keyboard = gui->Keyboard[k];
			for (int i = keyboard->Device->Event_Stack.getLength(); i > 0; i--){
				EVENT_ELEMENT* element = (EVENT_ELEMENT*)keyboard->Device->Event_Stack.GetElement(0);
				switch (element->Event.type){
					case EV_SYN:
					case EV_REL:
						break;
					case EV_KEY:
						switch (element->Event.code){
							case KEY_LEFTSHIFT:
							case KEY_RIGHTSHIFT:
								nk_input_key(gui->NK_Context, NK_KEY_SHIFT, element->Event.value);
								switch (element->Event.value){
									case 0: // Key Release
										keyboard->Shift = false;
										break;
									case 1: // Key Press
									case 2: // Auto Repeat
										keyboard->Shift = true;
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_LEFTSHIFT or KEY_RIGHTSHIFT");
										break;
								}
								break;
							case KEY_CAPSLOCK:
								switch (element->Event.value){
									case 0: // Key Release
										break;
									case 1: // Key Press
										keyboard->Caps_Lock = !keyboard->Caps_Lock;
										break;
									case 2: // Auto Repeat
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_BACKSPACE:
								switch (element->Event.value){
									case 0: // Key Release
										nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 0);
										break;
									case 1: // Key Press
										nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 1);
										break;
									case 2: // Auto Repeat
										nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 0);
										nk_input_key(gui->NK_Context, NK_KEY_BACKSPACE, 1);
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_UP:
								switch (element->Event.value){
									case 0: // Key Release
										nk_input_key(gui->NK_Context, NK_KEY_UP, 0);
										break;
									case 1: // Key Press
										nk_input_key(gui->NK_Context, NK_KEY_UP, 1);
										break;
									case 2: // Auto Repeat
										nk_input_key(gui->NK_Context, NK_KEY_UP, 0);
										nk_input_key(gui->NK_Context, NK_KEY_UP, 1);
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_DOWN:
								switch (element->Event.value){
									case 0: // Key Release
										nk_input_key(gui->NK_Context, NK_KEY_DOWN, 0);
										break;
									case 1: // Key Press
										nk_input_key(gui->NK_Context, NK_KEY_DOWN, 1);
										break;
									case 2: // Auto Repeat
										nk_input_key(gui->NK_Context, NK_KEY_DOWN, 0);
										nk_input_key(gui->NK_Context, NK_KEY_DOWN, 1);
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_LEFT:
								switch (element->Event.value){
									case 0: // Key Release
										nk_input_key(gui->NK_Context, NK_KEY_LEFT, 0);
										break;
									case 1: // Key Press
										nk_input_key(gui->NK_Context, NK_KEY_LEFT, 1);
										break;
									case 2: // Auto Repeat
										nk_input_key(gui->NK_Context, NK_KEY_LEFT, 0);
										nk_input_key(gui->NK_Context, NK_KEY_LEFT, 1);
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_RIGHT:
								switch (element->Event.value){
									case 0: // Key Release
										nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 0);
										break;
									case 1: // Key Press
										nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 1);
										break;
									case 2: // Auto Repeat
										nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 0);
										nk_input_key(gui->NK_Context, NK_KEY_RIGHT, 1);
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							case KEY_ENTER:
								switch (element->Event.value){
									case 0: // Key Release
										nk_input_key(gui->NK_Context, NK_KEY_ENTER, 0);
										break;
									case 1: // Key Press
										nk_input_key(gui->NK_Context, NK_KEY_ENTER, 1);
										break;
									case 2: // Auto Repeat
										nk_input_key(gui->NK_Context, NK_KEY_ENTER, 0);
										nk_input_key(gui->NK_Context, NK_KEY_ENTER, 1);
										break;
									default:
										Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
										break;
								}
								break;
							default:
								if (element->Event.code <= 111){
									switch (element->Event.value){
										case 0:
											break;
										case 1: // Key Press
										case 2: // Auto Repeat
											char key_value;
											if (keyboard->Shift) {
												key_value = keyboard->Keys_Shifted[element->Event.code];
											} else {
												key_value = keyboard->Keys        [element->Event.code];
											}
											if (key_value > 0) {
												if (keyboard->Caps_Lock) {
													if (key_value >= 'A' && key_value <= 'Z') {
														key_value += 32;
													} else if (key_value >= 'a' && key_value <= 'z') {
														key_value -= 32;
													}
												}
												nk_input_char(gui->NK_Context, key_value);
											}
											break;
										default:
											Write_Notice(string("@Listen_Keyboard() Unhandled event value: ") + to_string(element->Event.value) + " for type: " + to_string(element->Event.type) + " and code: KEY_CAPSLOCK");
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
						Write_Notice(string("@Listen_Keyboard() Unexpected event type: ") + to_string(element->Event.type) + " (code: " + to_string(element->Event.code) + " and value: " + to_string(element->Event.value) + ")");
						break;
				}
				keyboard->Device->Event_Stack.Remove(element);
				delete element;
			}
		}
	}
}
