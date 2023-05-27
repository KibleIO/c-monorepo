#ifndef KEYBOARD_SERVER_H_
#define KEYBOARD_SERVER_H_

#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/KEYBOARD.h>
#include <Utilities/C_ARRAY.h>
#include <controller/server/SERVICE_SERVER.h>

struct KEYBOARD_SERVER : public SERVICE_SERVER{
	KCONTEXT *ctx;
	SOCKET_SERVER socket_server;
	
	bool Initialize(KCONTEXT*, SERVICE_SERVER_REGISTRY*) override;
	void Delete() override;
};

#endif
