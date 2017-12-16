#ifndef THEMIS_CPP_
#define THEMIS_CPP_

#include "Themis.h"

Themis::Themis() {
	compressed_send_buffer = NULL;
	handleKeyboard = NULL;
	handleMouse = NULL;
	time = 0;
	render = true;
	arrLength = -1;
	swap_render = false;
	swapper = false;
}

Themis::~Themis() {
	if (handleKeyboard != NULL) handleKeyboard->join();
	if (handleMouse != NULL) handleMouse->join();
}

void Themis::Init(int argc, char **argv) {
	server = new Server();
	int port = atoi(argv[1]);
	cout << "listening on: " << port << endl;
	if (!server->Listen(port)) {
		cout << "Failed on " << port << ". Please try again." << endl;
		return;
	}
	mouse_s = new Server();
	int port1 = atoi(argv[2]);
    if (!mouse_s->Listen(port1)) {
            cout << "Failed on " << port1 << ". Please try again." << endl;
            return;
    }
	keyboard_s = new Server();
	int port2 = atoi(argv[3]);
    if (!keyboard_s->Listen(port2)) {
            cout << "Failed on " << port2 << ". Please try again." << endl;
            return;
    }
	cout << "connected!!" << endl;
	server->Receive(screen_width.bytes, 4);
	server->Receive(screen_height.bytes, 4);
	arrLength = screen_width.data * screen_height.data * 4;
	
	//alec
	//server1 = new MEMORY_SERVER(2, arrLength);
	//server1->Update();
	
	

	compressed_send_buffer = new char[arrLength];
	history = new char[arrLength + sizeof(unsigned long) * 2];
	image = new Buffer(arrLength + sizeof(unsigned long) * 2);
	_send = new Buffer(arrLength * 2);
	final = new Buffer(arrLength + sizeof(unsigned long) * 2);
	_send->out(0);
	final->out(0);

	graphics_handle = new Graphics(image->main, screen_width.data, screen_height.data, screen_width.data, screen_height.data);
	mouse_stack = new DynArr();
    keyboard_stack = new DynArr();
	handleKeyboard = new thread(&Themis::keyboard, this);
	handleMouse = new thread(&Themis::mouse, this);

	new thread(&Themis::Render, this);
}

Graphics* Themis::GetRender() {
	if (swap_render) {
		//swapping desktop rendering buffer
		//cout << "swapping out graphics handle" << endl;
		graphics_handle->Swapbuffers(image->back);
		//cout << "heyyyyyy I did it!" << endl;
    	image->swap();
		swap_render = false;
	}
	
	//server1->Update();
	//cout << "Updated" << endl;
	//graphics_handle->Swapbuffers((char*)server1->Write->Bytes);

	return graphics_handle;
}

void Themis::Render() {
	Timer t, t1;
	char c[] = {0};
	while (true) {
		t.Start();
		//cout << "here1" << endl;
		prepare = new thread(&Themis::PrepareSendBuffer, this);
		t1.Start();
		server->Receive(c, 1);
		//cout << "here2" << endl;
		if (c[0] == 1) {
			send_int.data = _send->size_main;
			server->Send(send_int.bytes, 4);
			
			if (send_int.data != 0) {
				compressedSize.data = LZ4_compress(_send->main, compressed_send_buffer, _send->size_main);

				server->Send(compressedSize.bytes, 4);

        		server->Send(compressed_send_buffer, compressedSize.data);
			}
			swapper = true;
		} else {
			//filler data to keep Iris in shape
			server->Send(send_int.bytes, 4);
		}
		//cout << "here3" << endl;
		//cout << "Send time: " << t1.Stop() << endl;
		prepare->join();

		SwapBuffers();
		delete prepare;
		time++;
		//cout << "here4" << endl;
		//cout << "Timer: " << t.Stop() << endl;
	}
}

void Themis::PrepareSendBuffer() {
	
	//cout << "starting prepare send buffer" << endl;
	
	//Timer t, t2;
	//t.Start();
	swap_render = true;
	while (swap_render) {}
	
	//cout << "swapped out render" << endl;
	//cout << "wait time: " << t.Stop() << endl;
	//t2.Start();
	//copy(image->back, image->back + arrLength, final->main);
	/*
	MEMORY_CLIENT *latest = server1->Get_Latest();
	if (client1 != latest) {
		client1 = latest;
		//saving the historice buffer
		swap = history;
		history = final->main;
		final->main = swap;
		
		//final->main = client1->read->bytes;
		
		swap = (char*) client1->Read->Bytes;
		client1->Read->Bytes = (unsigned char*) final->main;
		final->main = swap;
		
		client1->Release();
	} else {
		latest->Release();
	}
	*/
	//saving the historice buffer
	swap = history;
	history = final->main;
	final->main = swap;
	
	//final->main = client1->read->bytes;
	
	//swap = (char*) client1->Read->Bytes;
	//client1->Read->Bytes = (unsigned char*) final->main;
	//final->main = swap;
	
	//client1->Release();
	
	//swapping newest image buffer with newest comparison buffer
	swap = image->back;
	image->back = final->main;
	final->main = swap;
	
	//cout << "finished swapping!" << endl;
	//t2.Start();
	//int hel = 0;
	//while (hel < 10000000) {
	//	hel += 1;
	//	cout << "";
	//}
	_send->size_back = Encode_Difference_File_spec((unsigned char*)_send->back, (unsigned char*)final->back, (unsigned char*)final->main, (unsigned int) arrLength, 200);
	//cout << "processing time niggadawdwadwdadwaadw: " << t2.Stop() << endl;
	
	
	
	//cout << "processing time nigga: " << t2.Stop() << endl;
	
	//cout << "running" << endl;
}

void Themis::SwapBuffers() {
	//swap out the send buffers
    _send->swap();

    //swap history buffers
	if (swapper) {
		swap = history;
		history = final->back;
		final->back = swap;
		swapper = false;
	}
}

void Themis::mouse() {
	MouseInfo *mouseInfo;
	while (true) {
		mouseInfo = new MouseInfo; //potential memory leak!
        mouse_s->Receive(mouseInfo->x.bytes, 4);
        mouse_s->Receive(mouseInfo->y.bytes, 4);
        mouse_s->Receive(mouseInfo->button.bytes, 4);
		mouse_stack->Add(mouseInfo);
	}
}

void Themis::keyboard() {
	KeyboardInfo *keyboardInfo;
    while (true) {
		keyboardInfo = new KeyboardInfo; //potential memory leak!
        keyboard_s->Receive(&keyboardInfo->c, 1);
        keyboard_s->Receive(keyboardInfo->code.bytes, 4);
        keyboard_s->Receive(keyboardInfo->state.bytes, 4);
        keyboard_stack->Add(keyboardInfo);
	}
}

#endif /* THEMIS_CPP_ */
