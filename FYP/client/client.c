#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "../common/debug.h"

void verify(){
	int serv_fd[NUM_DS];
	INIT_SER_FD(serv_fd);

	for (int i=0; i<NUM_DS; i++){
		VerifyConnection(serv_fd[i], CLIENT);
		close(serv_fd[i]);
	}
}

Meta_t initMeta(char* fileName)
{
	Meta_t meta;
	char tmp[50];

	sscanf(fileName,"%*[^/]/%s",tmp);
	int fileNameSize = strlen(tmp)+1;
	// tmp[fileNameSize]="\0";
	// printf("tmp: %s\n",tmp);

	meta.fileName = (char*)malloc(fileNameSize*sizeof(char));
	memcpy(meta.fileName,tmp,fileNameSize);
	meta.fileNameSize=fileNameSize;
	// meta.fileName[fileNameSize]="\0";

	// debug("%s",meta.fileNmae);
	// debug("%d",meta.fileNameSize);

	FILE *fin = fopen(fileName,"rb");
	fseek(fin,0,SEEK_END);
	int fileSize = ftell(fin);
	fseek(fin,0,SEEK_SET);
	meta.fileSize=fileSize;

	fclose(fin);
	return meta;
}


Code_t initCode(unsigned char* chunk, int chunkSize, int chunkID)
{
	Code_t code;
	genKey(chunk,chunkSize,code.key);
	genIV(chunk,chunkSize,code.iv);
	code.chunkID=chunkID;
	return code;
}


Input_t initInput(unsigned char* ciper, int ciperSize,int chunkSize, int chunkID)
{
	Input_t input;
	input.ciper=ciper;
	input.ciperSize=ciperSize;
	input.chunkSize=chunkSize;
	input.chunkID=chunkID;
	return input;
}

void upload(Meta_t meta, Code_t code, Input_t input)
{
	int fd_serv = ConnectServer(DS_PORT_1,DS_IP_1);
	SendInt(fd_serv,UPLOAD);
	SendInt(fd_serv,SENDMETA);
	SendMeta(fd_serv,meta);
	// debug("%s",meta.fileName);
	// debug("%d",meta.fileNameSize);
	// debug("%d",meta.fileSize);
	SendInt(fd_serv,SENDCODE);
	SendCode(fd_serv,code);
	SendInt(fd_serv,SENDDATA);
	SendInput(fd_serv,input);
	SendInt(fd_serv,END);
	close(fd_serv);
}

void download(Meta_t meta)
{
	Chunk_t chunk;
	int indicator, len_decryp;
	unsigned char* decryptext;
	FILE *fp = fopen(meta.fileName,"wb");

	int fd_serv = ConnectServer(DS_PORT_1,DS_IP_1);

	SendInt(fd_serv,DOWNLOAD);
	SendMeta(fd_serv,meta);
	while((indicator=RecvInt(fd_serv)) != END){
		if (indicator==FILENOTEXIST){
			fclose(fp);
			remove(meta.fileName);
			error("ERROR: File Not Exist!");
		}

		if (indicator==SENDCHUNK){
			chunk=RecvChunk(fd_serv);
			// decrypted chunk
	 		decryptext=(unsigned char*)malloc(sizeof(char)*chunk.ciperSize);
			len_decryp = decrypt((unsigned char*)chunk.ciper,chunk.ciperSize,
											chunk.key,chunk.iv,decryptext);
			// decryptext[len_decryp+1]='\0';
			// printf("%s\n",decryptext);
			fwrite(decryptext,len_decryp,1,fp);
			free(decryptext);
		}
	}
	fclose(fp);
}