#include "FRAME.h"

void   Initialize_Frame(FRAME* frame, unsigned int frame_resolution)                {
	frame->Update_Stack = new DynArr();
	frame->Buffer       = new int[frame_resolution + 2];
	fill(frame->Buffer, frame->Buffer + frame_resolution + 2, 0x00000000); // Zero out trailing bytes for use by delta algorithm
}
FRAME* Construct_Frame (unsigned int frame_resolution)                              {
	FRAME* frame = new FRAME();
	Initialize_Frame(frame, frame_resolution);
	return frame;
}
void   Delete_Frame    (FRAME* frame)                                               {
	delete frame->Update_Stack;
	delete frame->Buffer;
}

void   Damage_Frame    (FRAME* frame, char* delta_buffer, unsigned int delta_length) {
	//cout << "@FRAME::Render_Frame() START" << endl;
	BUFFER_ELEMENT* buffer_element = new BUFFER_ELEMENT();
	buffer_element->Buffer         = delta_buffer;
	buffer_element->Length         = delta_length;
	frame->Update_Stack->Add(buffer_element);
	//cout << "@FRAME::Damage_Frame() END" << endl;
}

int    Render_Frame    (FRAME* frame, char** delta_output)                          {
	//cout << "@FRAME::Render_Frame() START" << endl;
	int damage_length;
	int update_count = frame->Update_Stack->getLength();
	switch (update_count){
		case 0:
			damage_length = 0;
			break;
		case 1: {
			
			BUFFER_ELEMENT* element = (BUFFER_ELEMENT*)frame->Update_Stack->GetElement(0);
			*delta_output           = (char*)element->Buffer;
			damage_length           = (int)  element->Length;
			frame->Update_Stack->Remove(element);
			delete element;
			break;
		}
		default: {
			BUFFER_ELEMENT* element;
			int*            delta_counts;
			
			char**          delta_buffers   = new char*[update_count];
			int*            delta_lengths   = new int  [update_count];

			for (int i = update_count - 1; i >= 0; i--) {
			//for (int i = 0; i < update_count; i++) {
				element            = (BUFFER_ELEMENT*)frame->Update_Stack->GetElement(0);
				delta_buffers  [i] = (char*)element->Buffer;
				delta_lengths  [i] = (int)  element->Length;
				
				frame->Update_Stack->Remove(element);
				delete element; // Might do in DynArr
			}

			delta_counts  = count_num_diff(delta_buffers, delta_lengths, update_count);
			damage_length = black_box(delta_buffers, delta_counts, update_count, *delta_output);
			
			delete delta_counts;
			for (int i = 0; i < update_count; i++) {
				delete delta_buffers[i];
			}
			delete delta_buffers;
			delete delta_lengths;
			break;
		}
	}

	// Update Historic Buffer
	Decode_Difference_File(
		(unsigned char*)*delta_output,
		damage_length,
		(unsigned char*)frame->Buffer);
	//cout << "@FRAME::Render_Frame() END" << endl;
	return damage_length;
}