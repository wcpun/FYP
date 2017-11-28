#ifndef __CHUNK_H__
#define __CHUNK_H__

#define FIX_SIZE_T 0
#define VAR_SIZE_T 1
#define WIN_SIZE 1024
#define AVG_CHUNK_SIZE 1024

void chunking(unsigned char *buff, int size, int *indexList, int *num, int type);
void fixSizeChunking(unsigned char *buff, int size, int *indexList, int *num);
void varSizeChunking(unsigned char *buff, int size, int *indexList, int *num);
void displayChunk(unsigned char *buff, int size, int *indexList, int num);

#endif