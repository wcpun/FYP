#ifndef _CRYPTO_H__
#define _CPYPTO_H__

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/md5.h>

#define OFFSET (-10)

void displayMD5(unsigned char *digest);
void displayCiper(unsigned char* ciper,int ciper_len);
void displayDecrypt(unsigned char* ciper,int len_cipher,unsigned char* key, 
						unsigned char* iv);
void genKey(unsigned char* buffer, int size, unsigned char* key);
void genIV(unsigned char* buffer, int size, unsigned char* iv);
int encrypt(unsigned char* buffer, int size, unsigned char* key,
				unsigned char* iv, unsigned char* ciphertext);
int decrypt(unsigned char* ciphertext, int len_cipher, unsigned char* key, 
				unsigned char* iv, unsigned char* decryptext);

#endif