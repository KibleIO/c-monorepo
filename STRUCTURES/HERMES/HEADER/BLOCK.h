#ifndef HERMES_DEVICE_BLOCK_H_
#define HERMES_DEVICE_BLOCK_H_

#include "BLOCK_DESCRIPTOR.h"

#define HERMES_DEVICE_PROTOCOL_HTTP       0;
#define HERMES_DEVICE_PROTOCOL_IP         1;
#define HERMES_DEVICE_PROTOCOL_TCP_CLIENT 2;
#define HERMES_DEVICE_PROTOCOL_TCP_SERVER 3;
#define HERMES_DEVICE_PROTOCOL_UDP        4;

struct IPV4_ADDRESS
{
	unsigned char Octets[4];
}
struct IPV6_ADDRESS
{
	unsigned char Octets[16];
}
struct MAC_ADDRESS
{
	unsigned char Octets[6];
}

struct HERMES_DEVICE_BLOCK
{
	IPV4_ADDRESS   IPv4_Address;
	IPV6_ADDRESS   IPv6_Address;
	MAC_ADDRESS    MAC_Address;
	unsigned short Port;
	unsigned int   Protocol;
};

const struct BLOCK_DESCRIPTOR DEFAULT_HERMES_DEVICE_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_HERMES_DEVICE,
	BLOCK_OPTION_SEND,
	BLOCK_LATENCY_MODERATE
};

#endif // HERMES_DEVICE_BLOCK_H_
