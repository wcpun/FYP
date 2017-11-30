#ifndef _FS_H__
#define _FS_H__

void readChunk(char* chunkName, int chunkSize, char* buffer);
void writeChunk(char* chunkName, unsigned char* chunk, int chunkSize);

#endif