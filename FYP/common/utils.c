#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void print_unsigned_char(unsigned char *buffer, int len)
{
	for (int i=0; i<len; i++){
		printf(buffer[i] < 0x10 ? "0%1x" : "%2x", buffer[i]);
	}
}

unsigned int hashfile(char* fileName)
{
	unsigned int hashval;
	hashval=0;
	for (;*fileName!='\0';fileName++)
		hashval=*fileName+(hashval<<5)-hashval;
	return hashval%FILE_TABLE_SIZE;
}

unsigned int hashchunkID(unsigned char* key)
{
	unsigned int hashval;
	hashval=0;
	for (;*key!='\0';key++)
		hashval=*key+(hashval<<5)-hashval;
	return hashval%POINTER_TABLE_SIZE;

}


FileTable_t* createFileTable()
{
	FileTable_t *filetable;
	if ((filetable = malloc(sizeof(FileTable_t)))==NULL)
		return NULL;
	if ((filetable->table = malloc(sizeof(FileNode*)*FILE_TABLE_SIZE))==NULL)
		return NULL;
	for (int i=0; i<FILE_TABLE_SIZE;i++)
		filetable->table[i]=NULL;
	filetable->size=FILE_TABLE_SIZE;
	return filetable;
}

FileNode* lookupFile(FileTable_t *filetable, char* fileName)
{
	FileNode *tmp;
	unsigned int hashval = hashfile(fileName);
	if ((tmp=filetable->table[hashval]) != NULL)
		return tmp;
	return NULL;
}

int insertFileNode(FileTable_t *filetable, char* fileName, int fileSize,
						unsigned int firstChunkID)
{
	if (filetable==NULL)
		return -1;
	if (filetable->table==NULL)
		return -1;
	FileNode *tmp = (FileNode*)malloc(sizeof(FileNode));
	tmp->fileName=strdup(fileName);
	tmp->fileSize=fileSize;
	tmp->firstChunkID=firstChunkID;	
	unsigned int hashval = hashfile(fileName);
	// printf("hash: %u.\n",hashval);	
	filetable->table[hashval]=tmp;
	return 1;
}

int removeFileNode(FileTable_t *filetable, char* fileName)
{
	if (filetable==NULL || filetable->table==NULL)
		return -1;
	unsigned int hashval = hashfile(fileName);
	if (filetable->table[hashval]==NULL)
		return -1;
	free(filetable->table[hashval]->fileName);
	filetable->table[hashval]=NULL;
	return 1;
}

void freeFileTable(FileTable_t *filetable)
{
	if (filetable==NULL) return;
	for (int i=0; i<filetable->size; i++){
		if (filetable->table[i] != NULL){
			free(filetable->table[i]->fileName);
			free(filetable->table[i]);
			filetable->table[i]=NULL;
		}
	}
	free(filetable->table);
	free(filetable);
}

void displayFileTable(FileTable_t *filetable)
{	
	FileNode *tmp;
	if (filetable==NULL || filetable->table==NULL)
		return;
	printf("File Table with Size %d.\n",filetable->size);
	for (int i=0; i<filetable->size; i++){
		printf("File[%d]: ",i);
		tmp=filetable->table[i];
		if (tmp == NULL) {
			printf("NULL.\n");
			continue;
		}
		printf("FileName: %s ",tmp->fileName);
		printf("FileSize: %d ",tmp->fileSize);
		printf("FirstChunkID: %u\n", tmp->firstChunkID);
	}
	free(tmp);
}


PointerTable_t* createPointerTable()
{
	PointerTable_t *pointertable;
	if ((pointertable = malloc(sizeof(PointerTable_t)))==NULL)
		return NULL;
	if ((pointertable->table = malloc(sizeof(PointerNode*)*POINTER_TABLE_SIZE))==NULL)
		return NULL;
	for (int i=0; i<FILE_TABLE_SIZE;i++)
		pointertable->table[i]=NULL;
	pointertable->size=POINTER_TABLE_SIZE;
	return pointertable;
}

PointerNode* lookupPointer(PointerTable_t *pointertable, unsigned int chunkID)
{
	PointerNode *tmp;
	if ((tmp=pointertable->table[chunkID]) != NULL)
		return tmp;
	return NULL;	
}


int insertPointerNode(PointerTable_t *pointertable, unsigned int chunkID,
							unsigned char* key, unsigned char* iv)
{
	if (pointertable==NULL)
		return -1;
	if (pointertable->table==NULL)
		return -1;
	PointerNode *tmp = (PointerNode*)malloc(sizeof(PointerNode));
	for (int i=0; i<16; i++){
		tmp->key[i]=key[i];
		tmp->iv[i]=iv[i];
	}
	pointertable->table[chunkID]=tmp;
	return 1;
}

void displayPointerTable(PointerTable_t *pointertable)
{
	PointerNode *tmp;
	if (pointertable==NULL || pointertable->table==NULL)
		return;
	printf("Pointer Table with Size %d.\n",pointertable->size);
	for (int i=0; i<pointertable->size; i++){
		printf("Chunk[%d]: ",i);
		tmp=pointertable->table[i];
		if (tmp == NULL) {
			printf("NULL.\n");
			continue;
		}
		printf("key: ");
		print_unsigned_char(tmp->key,16);
		printf(" iv: ");
		print_unsigned_char(tmp->iv,16);
		printf("\n");
	}
	free(tmp);
}

int removePointerNode(PointerTable_t *pointertable, unsigned int chunkID)
{
	if (pointertable==NULL || pointertable->table==NULL)
		return -1;
	if (pointertable->table[chunkID]==NULL)
		return -1;
	pointertable->table[chunkID]=NULL;
	return 1;
}


void freePointerTablle(PointerTable_t *pointertable)
{
	if (pointertable==NULL) return;
	for (int i=0; i<pointertable->size; i++){
		if (pointertable->table[i] != NULL){
			free(pointertable->table[i]);
			pointertable->table[i]=NULL;
		}
	}
	free(pointertable->table);
	free(pointertable);
}


ChunkList_t* createChunkList()
{
	ChunkList_t *chunklist;
	if ((chunklist = malloc(sizeof(ChunkList_t)))==NULL)
		return NULL;
	if ((chunklist->list = malloc(sizeof(ChunkNode*)*CHUNK_LIST_TABLE_SIZE))==NULL)
		return NULL;
	for (int i=0; i<FILE_TABLE_SIZE;i++)
		chunklist->list[i]=NULL;
	chunklist->size=CHUNK_LIST_TABLE_SIZE;
	return chunklist;
}

ChunkNode* lookupChunk(ChunkList_t *chunklist, char* fileName)
{
	ChunkNode *tmp;
	unsigned int hashval = hashfile(fileName);
	if ((tmp=chunklist->list[hashval]) != NULL)
		return tmp;
	return NULL;
}

int insertChunkNode(ChunkList_t *chunklist, char* fileName, unsigned int chunkID, 
						int ciperSize, char* chunkName, int chunkSize)
{
	if (chunklist==NULL)
		return -1;
	if (chunklist->list==NULL)
		return -1;
	ChunkNode *tmp = (ChunkNode*)malloc(sizeof(ChunkNode));
	tmp->chunkID=chunkID;
	tmp->ciperSize=ciperSize;
	tmp->chunkName=strdup(chunkName);
	tmp->chunkSize=chunkSize;
	unsigned int hashval = hashfile(fileName);
	ChunkNode *prev = chunklist->list[hashval];
	if (prev==NULL){
		chunklist->list[hashval]=tmp;
		tmp->next=NULL;
	}
	else{
		while(prev->next!=NULL) 
			prev=prev->next;
		tmp->next=prev->next;
		prev->next=tmp;
	}
	return 1;
}

int removeChunkNode(ChunkList_t *chunklist, char* fileName, unsigned int chunkID)
{
	if (chunklist==NULL)
		return -1;
	if (chunklist->list==NULL)
		return -1;
	unsigned int hashval = hashfile(fileName);
	if (chunklist->list[hashval]==NULL)
		return -1;
	ChunkNode *tmp;
	if (chunklist->list[hashval]->chunkID == chunkID){
		tmp=chunklist->list[hashval];
		chunklist->list[hashval]=NULL;
		free(tmp);
		return 1;
	}
	while (chunklist->list[hashval]->next->chunkID != chunkID){
		chunklist->list[hashval]=chunklist->list[hashval]->next;
	}
	tmp=chunklist->list[hashval]->next;
	chunklist->list[hashval]->next=tmp->next;
	free(tmp);
	return 1;
}

void displayChunkList(ChunkList_t *chunklist)
{
	ChunkNode *tmp;
	if (chunklist==NULL || chunklist->list==NULL)
		return;
	printf("Chunk List with Size %d.\n",chunklist->size);
	for (int i=0; i<chunklist->size; i++){
		// if (chunklist->list[i] == NULL)
		// 	continue;
		printf("File[%d]: ",i);
		tmp=chunklist->list[i];
		while(tmp!=NULL){
			printf("%s-> ",tmp->chunkName);
			tmp=tmp->next;
		}
		printf("NULL\n");
	}
	free(tmp);
}

void freeChunkList(ChunkList_t *chunklist)
{
	ChunkNode *list,*tmp;
	if (chunklist==NULL) return;
	for (int i=0; i<chunklist->size; i++){
		list=chunklist->list[i];
		while(list!=NULL){
			tmp=list;
			list=list->next;
			free(tmp->chunkName);
			free(tmp);
		}
		free(list);
	}
	free(chunklist->list);
	free(chunklist);
}
