#ifndef _UTILS_H__
#define _UTILS_H__

#define FILE_TABLE_SIZE 128
#define POINTER_TABLE_SIZE (2*1024)
#define CHUNK_LIST_TABLE_SIZE 128

typedef struct{
	char* fileName;
	int fileNameSize;
	int fileSize;
}Meta_t;

typedef struct{
	unsigned char key[16];
	unsigned char iv[16];
	int chunkID;
}Code_t;

typedef struct{
	unsigned char* ciper;
	int ciperSize;
	//  for debug
	int chunkSize;
	int chunkID;
}Input_t;

typedef struct{
	unsigned char* ciper;
	unsigned char key[16];
	unsigned char iv[16];
	int ciperSize;
}Chunk_t;

typedef struct{ 
	char* fileName;
	int fileSize;
	unsigned int firstChunkID;
}FileNode;

typedef struct{
	unsigned char key[16];
	unsigned char iv[16];
}PointerNode;

typedef struct ChunkNode{
	unsigned int chunkID;
	int ciperSize;
	char* chunkName;
	// for debug
	int chunkSize;
	struct ChunkNode* next;
}ChunkNode;


typedef struct{
	int size;
	FileNode **table;
}FileTable_t;

typedef struct{
	int size;
	PointerNode **table;
}PointerTable_t;

typedef struct{
	int size;
	ChunkNode **list;
}ChunkList_t;

unsigned int hashfile(char* fileName);
unsigned int hashchunkID(unsigned char* key);

FileTable_t* createFileTable();
PointerTable_t* createPointerTable();
ChunkList_t* createChunkList();

FileNode* lookupFile(FileTable_t *filetable, char* fileName);
PointerNode* lookupPointer(PointerTable_t *pointertable, unsigned int chunkID);
ChunkNode* lookupChunk(ChunkList_t *chunklist, char* fileName);

int insertFileNode(FileTable_t *filetable, char* fileName, int fileSize,
						unsigned int firstChunkID);
int insertPointerNode(PointerTable_t *pointertable, unsigned int chunkID,
 							unsigned char* key, unsigned char* iv);
int insertChunkNode(ChunkList_t *chunklist, char* fileName, unsigned int chunkID, 
						int ciperSize, char* chunkName, int chunkSize);

int removeFileNode(FileTable_t *filetable, char* fileName);
int removePointerNode(PointerTable_t *pointertable, unsigned int chunkID);
int removeChunkNode(ChunkList_t *chunklist, char* fileName, unsigned int chunkID);

void freeFileTable(FileTable_t *filetable);
void freePointerTablle(PointerTable_t *pointertable);
void freeChunkList(ChunkList_t *chunklist);

void displayFileTable(FileTable_t *filetable);
void displayPointerTable(PointerTable_t *pointertable);
void displayChunkList(ChunkList_t *chunklist);

void print_unsigned_char(unsigned char *buffer, int len);

#endif