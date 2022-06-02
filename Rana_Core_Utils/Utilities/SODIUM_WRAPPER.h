#ifndef _SODIUM_WRAPPER_H_
#define _SODIUM_WRAPPER_H_

// is SODIUM_STATIC required? idk
#define SODIUM_STATIC
#include <sodium.h>
#include <iostream>
#include <Utilities/KCONTEXT.h>
#include <Utilities/UTILS.h>

using namespace std;

#define LOCAL_KEY_NAME "local.key"

#define LOCAL_KEY_SIZE crypto_sign_PUBLICKEYBYTES + \
	crypto_sign_SECRETKEYBYTES + crypto_sign_PUBLICKEYBYTES

#define SODIUM_WRAPPER_SERVER_TYPE 0
#define SODIUM_WRAPPER_CLIENT_TYPE 1
#define SODIUM_WRAPPER_INTERACTIVE_CREATE_KEY_TYPE 2

//may the lord have mercy on yee who enter here...

/*
https://pastebin.com/PVsP8PyQ
https://pastebin.com/zpy4g8QW
*/

struct SODIUM_WRAPPER {
	KCONTEXT *ctx;
	bool initialized;
	bool partner_ephemeral_pub_key_initialized;
	bool session_key_generated;
	int type;

	//these are the genesis keys
	unsigned char local_pub_key[crypto_sign_PUBLICKEYBYTES];
	unsigned char local_priv_key[crypto_sign_SECRETKEYBYTES];
	unsigned char partner_pub_key[crypto_sign_PUBLICKEYBYTES];

	unsigned char local_rx_key[crypto_kx_SESSIONKEYBYTES];
	unsigned char local_tx_key[crypto_kx_SESSIONKEYBYTES];
	unsigned char local_ephemeral_pub_key[crypto_kx_PUBLICKEYBYTES];
	unsigned char local_ephemeral_priv_key[crypto_kx_SECRETKEYBYTES];
	unsigned char partner_ephemeral_pub_key[crypto_kx_PUBLICKEYBYTES];

	unsigned char signed_partner_ephemeral_pub_key[crypto_sign_BYTES +
		crypto_kx_PUBLICKEYBYTES];
	int signed_partner_ephemeral_pub_key_size;

	unsigned char signed_local_ephemeral_pub_key[crypto_sign_BYTES +
		crypto_kx_PUBLICKEYBYTES];
	int signed_local_ephemeral_pub_key_size;

	unsigned char nonce_rx[crypto_secretbox_NONCEBYTES];
	unsigned char nonce_tx[crypto_secretbox_NONCEBYTES];
};

bool Initialize_SODIUM_WRAPPER(SODIUM_WRAPPER*, KCONTEXT*, int);
bool Check_Partner_Ephemeral_Pub_Key_SODIUM_WRAPPER(SODIUM_WRAPPER*, char*,
	long long);
bool Sign_Local_Pub_Key_SODIUM_WRAPPER(SODIUM_WRAPPER*);
bool Generate_Session_Key_SODIUM_WRAPPER(SODIUM_WRAPPER*);
int Encrypt_Message_SODIUM_WRAPPER(SODIUM_WRAPPER*, char*, int, char*);
int Decrypt_Message_SODIUM_WRAPPER(SODIUM_WRAPPER*, char*, int, char*);
void Delete_SODIUM_WRAPPER(SODIUM_WRAPPER*);

bool Interactive_Create_Key_Step_1_SODIUM_WRAPPER(SODIUM_WRAPPER*);
bool Interactive_Create_Key_Step_2_SODIUM_WRAPPER(SODIUM_WRAPPER*);

#endif
