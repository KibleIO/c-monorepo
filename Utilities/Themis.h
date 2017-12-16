#ifndef THEMIS_H_
#define THEMIS_H_

#include <system_error>
#include <thread>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <functional>
#include <Utilities/lz4.h>
#include <Utilities/Integer.h>
#include <Utilities/Timer.h>
#include <Networking/Server.h>
#include <Graphics/Graphics.h>
#include <Input/MouseInfo.h>
#include <Input/KeyboardInfo.h>
#include "Buffer.h"
#include "Difference.h"
#include "DynArr.h"
#include "MemorySharer.h"

using namespace std;

class Themis {
	private:
		int arrLength;
		Buffer *image, *_send, *final;
		Server *server, *mouse_s, *keyboard_s;
		Integer compressedSize, send_int;
		thread *handleMouse, *handleKeyboard, *prepare;
		char *compressed_send_buffer;
		char *swap;
		char *history;
		MEMORY_SERVER *server1;
		MEMORY_CLIENT *client1;
		bool swapper;
		void SwapBuffers();
		void PrepareSendBuffer();
		void handleImage();
		void mouse();
		void keyboard();
	public:
		DynArr *mouse_stack;
		DynArr *keyboard_stack;
		Graphics *graphics_handle;
		Integer screen_width, screen_height;
		int time;
		bool render;
		volatile bool swap_render;
		Themis();
		~Themis();
		void Init(int, char **);
		void Sender();
		void Render();
		Graphics* GetRender();
};

#endif /* THEMIS_H_ */
