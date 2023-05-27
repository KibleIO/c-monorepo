#ifndef MOUSE_SERVER_H_
#define MOUSE_SERVER_H_

#include <Utilities/ASSERT.h>
#include <Utilities/KCONTEXT.h>
#include <Utilities/MOUSE.h>
#include <Utilities/C_ARRAY.h>
#include <controller/server/SERVICE_SERVER.h>

struct MOUSE_SERVER : public SERVICE_SERVER{
	KCONTEXT *ctx;
	SOCKET_SERVER socket_server;

	bool Initialize(KCONTEXT*, SERVICE_SERVER_REGISTRY*) override;
	void Delete() override;
};

#endif
