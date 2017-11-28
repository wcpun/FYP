#ifndef _CLIENT_H__
#define _CLIENT_H__

#include "../common/tcp.h"
#include "conf.h"
#include "crypto.h"

#define INIT_SER_FD(var) do{	\
	var[0] = ConnectServer(DS_PORT_1, DS_IP_1); \
	var[1] = ConnectServer(DS_PORT_2, DS_IP_2); \
	var[2] = ConnectServer(DS_PORT_3, DS_IP_3); \
}while(0)	\


void verify();
Meta_t initMeta(char* fileNmae);
Code_t initCode(unsigned char* chunk, int chunkSize, int chunkID);
Input_t initInput(unsigned char* ciper, int ciperSize,int chunkSize, int chunkID);
void upload(Meta_t meta, Code_t code, Input_t input);
void download(Meta_t meta);

#endif