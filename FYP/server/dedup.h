#ifndef _DEDUP_H__
#define _DEDUP_H__


#include "../common/utils.h"
#include "../common/tcp.h"

typedef struct{
	FileTable_t *filetable;
	PointerTable_t *pointertable;
	ChunkList_t *chunklist;
}dedupObj_t;

dedupObj_t initDedupObj();
void displayDedupObj(dedupObj_t dedupObj);
void freeDedupObj(dedupObj_t *dedupObj);

int dedup(dedupObj_t *dedupObj, Meta_t meta, Code_t code, Input_t input);
int restore(int fd, dedupObj_t dedupObj, char* fileName);


#endif