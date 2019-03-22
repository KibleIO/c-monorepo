#ifndef ENCRYPTION_PROFILE_H_
#define ENCRYPTION_PROFILE_H_

#include <string>
#include <fstream>
#include <Utilities/LOGGING.h>
#include <sodium.h>

#define ENCRYPTION_PROFILE_PATH "/opt/enc"

using namespace std;

struct ENCRYPTION_PROFILE {
	uint8_t*    master_nonce;
	uint8_t*    master_key;
	uint8_t*    poly1305_nonce;
	uint8_t*    poly1305_key;
	uint8_t*	entropy_pool;
	uint8_t*	generichash_buffer;
	uint32_t	original_pool_size;
	uint32_t	entropy_pool_index;
	string		identifier; //just a name to be used internally. mostly ignored
	bool		saved;
};

bool Initialize_ENCRYPTION_PROFILE					(
ENCRYPTION_PROFILE*, string, int32_t);
bool Initialize_ENCRYPTION_PROFILE					(
ENCRYPTION_PROFILE*, uint8_t*, int32_t, string);
bool Load_Master_Key_And_Nonce_ENCRYPTION_PROFILE	(
ENCRYPTION_PROFILE*);
bool Load_Poly1305_Key_And_Nonce_ENCRYPTION_PROFILE	(
ENCRYPTION_PROFILE*);
bool Get_Entropy_ENCRYPTION_PROFILE					(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t);
bool Update_ENCRYPTION_PROFILE						(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t);
bool Encrypt_Data_ENCRYPTION_PROFILE				(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*);
bool Encrypt_Data_ENCRYPTION_PROFILE				(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*, uint8_t*);
bool Decrypt_Data_ENCRYPTION_PROFILE				(
ENCRYPTION_PROFILE*, uint8_t*,
uint32_t, uint8_t*);
bool Decrypt_Data_ENCRYPTION_PROFILE				(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*, uint8_t*);
bool Generate_Auth_Code_ENCRYPTION_PROFILE			(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*, uint8_t*);
bool Generate_Auth_Code_ENCRYPTION_PROFILE			(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*);
bool Authenticate_Auth_Code_ENCRYPTION_PROFILE		(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*);
bool Authenticate_Auth_Code_ENCRYPTION_PROFILE		(
ENCRYPTION_PROFILE*, uint8_t*, uint32_t, uint8_t*, uint8_t*);
void Delete_ENCRYPTION_PROFILE						(
ENCRYPTION_PROFILE*);

#endif
