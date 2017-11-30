#ifndef _SERVER_H__
#define _SERVER_H__
#include <pthread.h>
#include <signal.h>

#include "../common/tcp.h"
#include "dedup.h"

typedef struct{
	int fd;
	int flag;
}VerifyArgs;

typedef struct{
	int fd;
	dedupObj_t *dedupObj;
}UploadArgs, DownloadArgs;


void* ThreadVerify(void* VerifyArgs);
void* ThreadUpload(void* UploadArgs);
void* ThreadDownload(void* DownloadArgs);

void Sigint_handler(int sig);

void RunServer(int lsd);


#endif