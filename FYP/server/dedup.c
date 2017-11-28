#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dedup.h"

dedupObj_t initDedupObj()
{
	dedupObj_t dedupObj;
	if ((dedupObj.filetable=createFileTable())==NULL)
		printf("ERROR: Cannot Init FileTable.\n");
	if ((dedupObj.pointertable=createPointerTable())==NULL)
		printf("ERROR: Cannot Init PointerTable.\n");
	if ((dedupObj.chunklist=createChunkList())==NULL)
		printf("ERROR: Cannot Init ChunkList.\n");
	return dedupObj;
}

void displayDedupObj(dedupObj_t dedupObj)
{
	printf("\n");
	displayFileTable(dedupObj.filetable);
	printf("\n");
	displayPointerTable(dedupObj.pointertable);
	printf("\n");
	displayChunkList(dedupObj.chunklist);
	printf("\n");
}

void freeDedupObj(dedupObj_t *dedupObj)
{
	freeFileTable(dedupObj->filetable);
	freePointerTablle(dedupObj->pointertable);
	freeChunkList(dedupObj->chunklist);
}

int dedup(dedupObj_t *dedupObj, Meta_t meta, Code_t code, Input_t input)
{

	char chunkName[10];
	unsigned int chunkID=hashchunkID(input.ciper);
	
	printf("chunkID: %u\n",chunkID);
	sprintf(chunkName,"%u",chunkID);
	if (lookupFile(dedupObj->filetable,meta.fileName) == NULL){
		// file does not in filetable then insert filenode and the 1st chunk
		// wirte the 1st chunk in data folder
		insertFileNode(dedupObj->filetable,meta.fileName,meta.fileSize,chunkID);
		insertPointerNode(dedupObj->pointertable,chunkID,code.key,code.iv);
		insertChunkNode(dedupObj->chunklist,meta.fileName,chunkID,input.ciperSize,
			chunkName,input.chunkSize);
		writeChunk(chunkName,input.ciper,input.ciperSize);
	}
	else{
		// file is in filetable
		if (lookupPointer(dedupObj->pointertable,chunkID) != NULL){
			// the chunk is not unique only insert it to chunklist
			insertChunkNode(dedupObj->chunklist,meta.fileName,chunkID,input.ciperSize,
			chunkName,input.chunkSize);
		}
		else{
			// the chunk is unique insert it to pointertable and chunklist
			// wirte chunk in data folder
			insertPointerNode(dedupObj->pointertable,chunkID,code.key,code.iv);
			insertChunkNode(dedupObj->chunklist,meta.fileName,chunkID,input.ciperSize,
			chunkName,input.chunkSize);
			writeChunk(chunkName,input.ciper,input.ciperSize);
		}
	}

	return 1;
}



int restore(int fd, dedupObj_t dedupObj, char* fileName)
{
	unsigned int chunkID;
	char* chunk_buffer;


	FileNode *filenode;
	PointerNode *pointernode;
	ChunkNode *chunknode;

	if ((filenode=lookupFile(dedupObj.filetable,fileName))==NULL){
		// file does not exist
		return -1;
	}
	else{
		// file exists then first find the 1st chunk and its chunklist
		chunkID=filenode->firstChunkID;
		chunknode=lookupChunk(dedupObj.chunklist,fileName);

		// for every chunk in chunklist
		while(chunknode!=NULL){
			// read chunk first		
			chunk_buffer=(char*)malloc(sizeof(char)*chunknode->ciperSize);
			readChunk(chunknode->chunkName,chunknode->ciperSize,chunk_buffer);
			chunkID=chunknode->chunkID;
			// find its key and iv
			pointernode=lookupPointer(dedupObj.pointertable,chunkID);
			// send chunk finally
			SendChunk(fd,chunk_buffer,chunknode->ciperSize,pointernode->key,
						pointernode->iv);
			chunknode=chunknode->next;
			free(chunk_buffer);
		}
	}
	return 1;
}


void readChunk(char* chunkName, int chunkSize, char* buffer){
	char fileName[50];
	strcpy(fileName,"data/");
	strncat(fileName,chunkName,strlen(chunkName));

	FILE *fp = fopen(fileName,"rb");
	int ret = fread(buffer,chunkSize,1,fp);
	if (ret < 0)
		printf("ERROR: Cannot Read File.\n");
	fclose(fp);
}
void writeChunk(char* chunkName, unsigned char* chunk, int chunkSize){
	char fileName[50];
	strcpy(fileName,"data/");
	strncat(fileName,chunkName,strlen(chunkName));

	FILE *fp = fopen(fileName,"wb");
	int ret = fwrite(chunk,chunkSize,1,fp);
	if (ret < 0)
		printf("ERROR: Cannot Write File.\n");
	fclose(fp);
}