#ifndef  OPUS_PA_H_
#define  OPUS_PA_H_

/*

What's up with that weird name? Well, this is an opus encoder and pulse audio
client combination... so OPUS_PA is a great name

*/

#include <iostream>
#include <thread>
#include <Utilities/LOGGING.h>
#include <Utilities/TIMER.h>
#include <Utilities/UTILS.h>
#include <Utilities/C_ARRAY.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <opus/opus.h>

#define AS_FRAME_SIZE 960	// periode_size / ((bits / 8) * Channels)
#define AS_PERIOD_SIZE 960
#define AS_SAMPLE_RATE 48000
#define AS_CHANNELS 2
#define AS_BITRATE 64000
#define AS_MAX_FRAME_SIZE (6 * 960) // 6*960

#define MAX_SO_PACKETSIZE 1154
#define AUDIO_PORT 6660
#define AUDIO_DEV "hw:0,1"

struct OPUS_PA {
	OpusEncoder *encoder;
	pa_simple *pa_server;

	uint8_t	running;
	thread* thr;
	int audio_count;
};

bool Initialize_OPUS_PA(OPUS_PA* server);
int Encode_Buffer_OPUS_PA(OPUS_PA*, char*, char*);
void Get_Audio_OPUS_PA(OPUS_PA*, char*);
void Delete_OPUS_PA(OPUS_PA* server);

#endif
