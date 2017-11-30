#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "client.h"
#include "chunk.h"
#include "../common/debug.h"

struct timeval tv1, tv2;

// constant for chunking
#define BUFFER_SIZE (1024*1024*16)
#define INDEX_LIST_SIZE 1024

void printTime(struct timeval tv1, struct timeval tv2)
{
	printf ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));
}

void usage(char* s)
{
	printf("usage: ./client [filename] [action]\n"
				   "[filename]: full path of the file.\n"
				   "[action]: [-u] upload [-d] download.\n");
	exit(1);
}

void storage(char* fileName)
{
		unsigned char *buffer = (unsigned char*)malloc(sizeof(unsigned char)*BUFFER_SIZE);
		int *indexList = (int*)malloc(sizeof(int)*INDEX_LIST_SIZE);
		int numOfCut;
		Meta_t meta = initMeta(fileName);
		// debug("%s",meta.fileName);

		FILE *fin = fopen(fileName,"rb");
		int ret = fread(buffer,1,meta.fileSize,fin);
		chunking(buffer,ret,indexList,&numOfCut,VAR_SIZE_T);
		DEBUG(numOfCut,int);
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
			// debug("%d",len_chunk);
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
		free(buffer);
		free(indexList);
}


void retrieval(char* fileName)
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

	// debug("%s",meta.fileName);
	// debug("%d",meta.fileNameSize);
	meta.fileSize=0;
	download(meta);
}

int main(int argc, char* argv[])
{
	if (argc != 3) usage(NULL);

	char* opt = argv[2];
	char* fileName = argv[1];

	if (strncmp(opt,"-u",2) == 0){
		gettimeofday(&tv1, NULL);
		storage(fileName);
		gettimeofday(&tv2, NULL);
		printTime(tv1,tv2);
	}
	else if (strncmp(opt,"-d",2) == 0){
		gettimeofday(&tv1, NULL);
		retrieval(fileName);
		gettimeofday(&tv2, NULL);
		printTime(tv1,tv2);
	}
	else verify();

	return 0;
}

