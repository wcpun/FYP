#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/debug.h"

void readChunk(char* chunkName, int chunkSize, char* buffer){
	char fileName[50];
	strcpy(fileName,"data/");
	strncat(fileName,chunkName,strlen(chunkName));

	FILE *fp = fopen(fileName,"rb");
	int ret = fread(buffer,chunkSize,1,fp);
	if (ret < 0)
		error("ERROR: Cannot Read File.\n");
	fclose(fp);
}
void writeChunk(char* chunkName, unsigned char* chunk, int chunkSize){
	char fileName[50];
	strcpy(fileName,"data/");
	strncat(fileName,chunkName,strlen(chunkName));

	FILE *fp = fopen(fileName,"wb");
	int ret = fwrite(chunk,chunkSize,1,fp);
	if (ret < 0)
		error("ERROR: Cannot Write File.\n");
	fclose(fp);
}