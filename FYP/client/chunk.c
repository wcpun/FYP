#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chunk.h"
#include "../common/debug.h"

void chunking(unsigned char *buff, int size, int *indexList, int *num, int type)
{
	if (type == FIX_SIZE_T)
		fixSizeChunking(buff, size, indexList, num);
	if (type == VAR_SIZE_T)
		varSizeChunking(buff, size, indexList, num);
}


void fixSizeChunking(unsigned char *buff, int size, int *indexList, int *num)
{
	int chunkEndIndex;
	(*num) = 0;
	chunkEndIndex = -1 + AVG_CHUNK_SIZE;

	while (chunkEndIndex < size){
		indexList[(*num)] = chunkEndIndex;
		chunkEndIndex = indexList[(*num)] + AVG_CHUNK_SIZE;
		(*num)++;
	}

	if (((*num) == 0) || (((*num) > 0) && (indexList[(*num)-1] != size-1))){
		indexList[(*num)] = size - 1;
		(*num)++;
	}
}

void varSizeChunking(unsigned char *buff, int size, int *indexList, int *num)
{
	int chunkEndIndex = 0;
	unsigned char max = buff[0];
	(*num) = 0;
	int offset = 0;
	while (chunkEndIndex < size){
		if (buff[chunkEndIndex] >= max ){
			if (chunkEndIndex> WIN_SIZE+offset){
				indexList[(*num)] = chunkEndIndex;
				offset = chunkEndIndex;
				max=0;
				(*num)++;
			}
			max = buff[chunkEndIndex];
		}
		chunkEndIndex++;
	}
}


void displayChunk(unsigned char *buff, int size, int *indexList, int num)
{
	char* sub;
	int offset = 0;
	int lenSub = 0;
	for (int i=0; i<=num; i++){
		lenSub = i < num ? indexList[i]-offset : size-indexList[num-1];
		sub = (char*)malloc(sizeof(char)*(lenSub+1));
		memcpy(sub,buff+offset,lenSub);
		sub[lenSub]='\0';
		printf("Str[%d]: %s\n", i, sub);
		free(sub);
		offset = i < num ? indexList[i] : 0;
	}
}