#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "chunk.h"

// constant for chunking
#define BUFFER_SIZE (1024*1024)
#define INDEX_LIST_SIZE 256

void usage(char* s)
{
	printf("usage: ./client [filename] [action]\n"
				   "[filename]: full path of the file.\n"
				   "[action]: [-u] upload [-d] download.\n");
	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 3) usage(NULL);

	char* opt = argv[2];
	char* fileName = argv[1];

	Meta_t meta;

	if (strncmp(opt,"-u",2) == 0){
		unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char)*BUFFER_SIZE);
		int *indexList = (int*)malloc(sizeof(int)*INDEX_LIST_SIZE);
		int numOfCut;
		meta = initMeta(fileName);
		// DEBUG(meta.fileName,str);

		FILE *fin = fopen(fileName,"r");
		int ret = fread(buffer,1,meta.fileSize,fin);
		chunking(buffer,ret,indexList,&numOfCut,VAR_SIZE_T);
		// DEBUG(numOfCut,int);
		// displayChunk(buffer,ret,indexList,numOfCut);
		unsigned char *chunk, *ciper;
		int offset=0;
		int len_chunk=0,len_ciper=0;
		for (int i=0; i<=numOfCut; i++){
			len_chunk = i < numOfCut ? indexList[i]-offset : ret-indexList[numOfCut-1];
			chunk = (unsigned char*)malloc(sizeof(char)*len_chunk);
			len_ciper = ((len_chunk+AES_BLOCK_SIZE)/AES_BLOCK_SIZE)*AES_BLOCK_SIZE;
			ciper = (unsigned char*)malloc(sizeof(char)*len_ciper);
			memcpy(chunk,buffer+offset,len_chunk);
			Code_t code = initCode(chunk,len_chunk,i);
			// DEBUG(len_chunk,int);
			int len_ciper = encrypt(chunk,len_chunk,code.key,code.iv,ciper);
			Input_t input = initInput(ciper,len_ciper,len_chunk,i);
			// displayCiper(input.ciper,input.ciperSize);
			offset = i < numOfCut ? indexList[i] : 0;
			upload(meta,code,input);
			// displayDecrypt(input.ciper,input.ciperSize,code.key,code.iv);
			// dedup(&dedupObj,meta,code,input);
			free(ciper);
			free(chunk);
		}
		fclose(fin);
		free(buffer);
		free(indexList);
	}
	if (strncmp(opt,"-d",2) == 0){
		meta.fileName=fileName;
		meta.fileNameSize=strlen(fileName);
		meta.fileSize=0;
		download(meta);
	}
	if (strncmp(opt,"-v",2) == 0){
		verify();
	}

	return 0;
}