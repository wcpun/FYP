#include <stdio.h>
#include <stdlib.h>
#include "crypto.h"


void displayMD5(unsigned char *digest)
{
	for (int i=0; i<MD5_DIGEST_LENGTH; i++){
		printf(digest[i] < 0x10 ? "0%1x" : "%2x", digest[i]);
	}
	printf("\n");
}

void displayCiper(unsigned char* ciper,int ciper_len)
{
	printf("Cipertext of length %d:\n", ciper_len);
	BIO_dump_fp(stdout,(const char*)ciper,ciper_len);
}

void displayDecrypt(unsigned char* ciper,int len_cipher,unsigned char* key, 
						unsigned char* iv)
{
	unsigned char* decryptext = (unsigned char*)malloc(sizeof(char)*(len_cipher));
	int len_decryp = decrypt(ciper,len_cipher,key,iv,decryptext);
	printf("len_decryp: %d\n", len_decryp);
	decryptext[len_decryp]='\0';
	printf("Decryted: %s\n", decryptext);
	free(decryptext);
}

void genKey(unsigned char* buffer, int size, unsigned char* key)
{
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context,buffer,size);
	MD5_Final(key,&context);
	// printf("Buffer: %s\t", buffer);
	// printf("key: ");
	// displayMD5(key);
}

void genIV(unsigned char* buffer, int size, unsigned char* iv)
{

	char tmp[size];
	for (int i=0; i<size; i++)tmp[i]=buffer[i]-OFFSET;
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context,tmp,size);
	MD5_Final(iv,&context);
	// printf("Buffer: %s\t", buffer);
	// printf("iv: ");
	// displayMD5(iv);	
}

int encrypt(unsigned char* buffer, int size, unsigned char* key,
				unsigned char* iv, unsigned char* ciphertext)
{
	int len_update,len_final,len_cipher=0;
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx,EVP_aes_128_cbc(),NULL,key,iv);
	EVP_EncryptUpdate(ctx,ciphertext,&len_update,buffer,size);
	EVP_EncryptFinal_ex(ctx,ciphertext+len_update,&len_final);
	EVP_CIPHER_CTX_free(ctx);
	len_cipher=len_update+len_final;
	return len_cipher;
}

int decrypt(unsigned char* ciphertext, int len_cipher, unsigned char* key, 
				unsigned char* iv, unsigned char* decryptext)
{
	int len_update,len_final,len_decrypt=0;
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx,EVP_aes_128_cbc(),NULL,key,iv);
	EVP_DecryptUpdate(ctx,decryptext,&len_update,ciphertext,len_cipher);
	EVP_DecryptFinal_ex(ctx,decryptext+len_update,&len_final);
	EVP_CIPHER_CTX_free(ctx);
	len_decrypt=len_update+len_final;
	return len_decrypt;
}