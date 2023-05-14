#ifndef  AUDIO_SERVER_H_
#define  AUDIO_SERVER_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Hermes/SERVER.h>
#include <iostream>
#include <thread>
#include <Utilities/LOGGING.h>
#include <Utilities/TIMER.h>
#include <Utilities/UTILS.h>
#include <Utilities/C_ARRAY.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#ifdef __linux__
#include <alsa/asoundlib.h>
#include <opus/opus.h>
#endif
#ifdef _WIN64
#include "../Network/UDP_Client.h"
#include "opus.h"
#pragma comment(lib, "opus.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "Rana_Core_Utils.lib")
#endif

using namespace std;

#define AS_FRAME_SIZE 960	// periode_size / ((bits / 8) * Channels)
#define AS_PERIOD_SIZE 960
#define AS_SAMPLE_RATE 48000
#define AS_CHANNELS 2
#define AS_APPLICATION OPUS_APPLICATION_AUDIO
#define AS_BITRATE 64000
#define AS_MAX_FRAME_SIZE (6 * 960) // 6*960
#define AS_MAX_PACKET_SIZE (3 * 1276) // 3*1276

#define MAX_SO_PACKETSIZE 1154
#define AUDIO_PORT 6660
#define AUDIO_DEV "hw:0,1"
//OMG OMG someone audit this
#define MAX_OPUS_OUTPUT_SIZE 270

using namespace std;

struct AUDIO_SERVER {
	opus_int16*	Raw;
	uint8_t*	Compressed;
	uint8_t*	PCM_bytes;
	uint8_t*	PCM_bytes_back;
	uint8_t*	swapper;
	int rc;

	SERVER*		S;

	OpusEncoder* encoder;

	uint32_t			s_rate;
	uint8_t				running;

	thread* thr;
	C_ARRAY audio_arr;
	int audio_count;
	char temp_buffer[AS_MAX_FRAME_SIZE];

	pa_simple *s;

	#ifdef _WIN64
	UDP_CLIENT* client;
	int32_t	Send_size;
	int32_t	f_size;
	#endif
	#ifdef __linux__
	snd_pcm_t*	handle;
	snd_pcm_uframes_t	f_size;
	snd_pcm_uframes_t 	p_size;
	// uint32_t format;
	#endif
};

bool Initialize_AUDIO_SERVER(AUDIO_SERVER* server);
void Connect_AUDIO_SERVER(AUDIO_SERVER* server, SERVER* s);
void Disconnect_AUDIO_SERVER(AUDIO_SERVER* server);
void Delete_AUDIO_SERVER(AUDIO_SERVER* server);

#endif
